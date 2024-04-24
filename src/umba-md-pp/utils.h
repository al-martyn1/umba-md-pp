#pragma once

#include "app_config.h"
//
#include "marty_cpp/src_normalization.h"

//
#include <stack>

inline
bool isInsertCommand(std::string line)
{
    umba::string_plus::trim(line);

    return umba::string_plus::starts_with(line, ("#!insert"));
}

inline
bool isListingCommand(std::string line)
{
    umba::string_plus::trim(line);

    return umba::string_plus::starts_with(line, ("```")) || umba::string_plus::starts_with(line, ("~~~"));
}

inline
bool isHeaderCommand(std::string line)
{
    umba::string_plus::trim(line);

    std::size_t idx = 0u;

    while(idx!=line.size() && line[idx]=='#') { ++idx; }

    return idx>0;
}


//----------------------------------------------------------------------------
inline
bool isCodeTagLinesRange(const std::string &fragmentTag, std::size_t &startLineNo, std::size_t &endLineNo /* last line number +1 */ )
{
    std::size_t dCnt = 0; // считаем дефисы

    for(auto ch: fragmentTag)
    {
        if (ch=='-')
        {
            ++dCnt;
            continue;
        }

        if (ch<'0' || ch>'9')
            return false; // какой-то левый символ
    }

    if (dCnt!=1)
    {
        return false; // неверное количество дефисов
    }

    auto numbersVec = marty_cpp::simple_string_split(fragmentTag, "-"); // делим по дефису

    if (numbersVec.size()!=2)
    {
        return false; // что-то пошло не так
    }

    std::size_t lFirst = 0;
    std::size_t lLast  = 0;

    try
    {
        lFirst = std::stoul(numbersVec[0]);
        lLast  = std::stoul(numbersVec[1]);
    }
    catch(...)
    {
        return false; // что-то пошло не так
    }

    if (lFirst>lLast)
    {
        std::swap(lFirst,lLast);
    }

    if (lFirst==0 || lLast==0)
    {
        return false; // нумерация строк человеческая - с единицы, если нет, то ошибка
    }

    // Уменьшаем до машинной нумерации
    --lFirst;
    --lLast;

    startLineNo = lFirst;
    endLineNo   = lLast+1;

    return true;
}

//----------------------------------------------------------------------------
inline
bool isCodeTagLine(std::string line, const std::string &tagPrefix)
{
    umba::string_plus::trim(line);

    return umba::string_plus::starts_with(line, tagPrefix);
}

//----------------------------------------------------------------------------
inline
bool extractCodeTagFromLine(std::string &line, const std::string &tagPrefix)
{
    auto lineCopy = line;
    umba::string_plus::trim(lineCopy);

    if (!umba::string_plus::starts_with_and_strip(lineCopy, tagPrefix))
        return false;

    umba::string_plus::trim(lineCopy);

    auto spacePos = lineCopy.find(' ');
    if (spacePos!=lineCopy.npos)
    {
        lineCopy.erase(spacePos, lineCopy.npos);
    }

    line = lineCopy;

    return true;
}

inline
bool extractCodeTagFromLine(std::string line, const std::string &tagPrefix, std::string &targetTag)
{
    if (!extractCodeTagFromLine(line, tagPrefix))
        return false;

    targetTag = line;

    return true;
}
//----------------------------------------------------------------------------
inline
std::vector<std::string> extractCodeFragment( std::vector<std::string>    lines
                                            , std::size_t                 &firstFoundLineIdx
                                            , const std::string           &targetFragmentTag
                                            , const std::string           &tagPrefix
                                            , ListingNestedTagsMode       listingNestedTagsMode
                                            , std::size_t                 tabSize=4u
                                            , bool                        trimLeadingSpaces_a = true
                                            )
{
    marty_cpp::expandTabsToSpaces(lines, tabSize);

    if (targetFragmentTag.empty() || lines.empty())
    {
        firstFoundLineIdx = 0;
        return lines;
    }

    std::vector<std::string> fragmentLines; fragmentLines.reserve(lines.size());

    //std::size_t startLineNo = 0;
    //std::size_t endLineNo   = 0;

    //bool startFound = false;
    //int tagLevel    = 0;


    auto addTagLine = [&](const std::string &line)
    {
        switch(listingNestedTagsMode)
        {
            case ListingNestedTagsMode::keep  :
                 fragmentLines.emplace_back(line);
                 break;
         
            case ListingNestedTagsMode::remove:
                 break;
         
            case ListingNestedTagsMode::empty :
                 fragmentLines.emplace_back(std::string());
                 break;

            case marty_simplesquirrel::ListingNestedTagsMode::invalid :
                 break;
        }
    };

    //std::stack<std::string>  openedTags;
    std::vector<std::string> openedTags;

    auto isClosingTag = [&](std::string tagName)
    {
        if (openedTags.empty())
            return false;

        if (tagName.empty())
            return true; // разрешаем закрывающим тэгам не иметь имени

        if (openedTags.back()==tagName)
            return true; // текущий тэг закрывает того, что на стеке - имя одно
    
        return false;
    };

    auto openCodeFragment = [&](std::string tagName)
    {
        openedTags.emplace_back(tagName);
    }

    auto closeCurTag = [&]()
    {
        if (openedTags.empty())
            return;
        openedTags.pop_back();
    };

    auto getTagLevel = [&]()
    {
        return openedTags.size();
    };

    auto isTargetFragmentTagOpened = [&]()
    {
        // if (openedTags.empty())
        //     return false;
        //  
        // return openedTags.top()targetFragmentTag;
        for(const auto &openedTag: openedTags)
        {
            if (openedTag==targetFragmentTag)
                return true;
        }

        return false;
    }

    auto isTargetFragmentTagOnTop = [&]()
    {
        if (openedTags.empty())
            return false;

        if (openedTags.back()==targetFragmentTag)
            return true; // искомый тэг на вершине
    
        return false;
    };

    // bool isClosingTag(std::string tagName)
    // void openCodeFragment(std::string tagName)
    // void closeCurTag()
    // std::size_t getTagLevel()
    // bool isTargetFragmentTagOpened() // открыт ли целевой фрагмент?


    std::size_t lineIdx = 0;
    //std::size_t endLineNo   = 0;

    //for(auto l: lines)
    for(; lineIdx!=lines.size(); ++lineIdx)
    {
        auto l = lines[lineIdx]; 

        if (!isCodeTagLine(l, tagPrefix))
        {
            // Строка обычная, не тэговая

            //if (startFound)
            //if (tagLevel!=0)
            if (isTargetFragmentTagOpened())
            {
                fragmentLines.emplace_back(l);
            }

            continue;
        }

        // Строка - тэговая
        std::string curTag;
        extractCodeTagFromLine(tag, tagPrefix, curTag);

        if (isClosingTag(curTag))
        {
            if (isTargetFragmentTagOnTop())
            {
                // Закрываем целевой тэг
                closeCurTag(); // на самом деле - насрать, всё равно из цикла выходим
                break;
            }
            else
            {
                // закрываем какой-то левый тэг
                closeCurTag();
                addTagLine(l);
                continue;
            }
        }

        // Если тэг не закрывающий, то он - открывающий

        if (curTag==targetFragmentTag)
        {
            firstFoundLineIdx = lineIdx+1; // начали со строки, которая следует за открывающим тэгом
        }

        // Если наш целевой тэг открыт, то надо добавить строчку, а тут идёт обработка вложенных тэгов
        if (isTargetFragmentTagOpened())
        {
            fragmentLines.emplace_back(l);
        }

        openCodeFragment(curTag);

        #if 0
        if (!tag.empty())
        {
            // открывающий тэг
            ++tagLevel;
            addTagLine(l);
        }
        else
        {
            // закрывающий тэг
            --tagLevel;
            if (tagLevel!=0) 
            {
                addTagLine(l);
            }
            else
            {
                // Тут, опа, закончился наш блок
                break;
            }
            
        }
        #endif

    }

    return fragmentLines;

    // MARTY_ARG_USED(targetFragmentTag);
    // MARTY_ARG_USED(tagPrefix);
    // MARTY_ARG_USED(listingNestedTagsMode);
    //  
    // return lfNormalizedText; // пока просто всё целиком возвращаем

    //StringType marty_cpp::stripTextTrailingSpaces(const StringType &str)
    //StringType stripLineTrailingSpaces(StringType str)
    //void stripLineTrailingSpaces(std::vector<StringType> &v)
}


std::string processMdFile(const AppConfig &appCfg, std::string fileText, const std::string &curFilename);

inline
std::vector<std::string> processMdFileLines(const AppConfig &appCfg, const std::vector<std::string> &lines, const std::string &curFilename, const std::unordered_set<std::string> &alreadyIncludedDocs=std::unordered_set<std::string>())
{
    std::vector<std::string> resLines; resLines.reserve(lines);
    bool inListing = false;

    for(auto line: lines)
    {
        if (inListing)
        {
            if (isListingCommand(line))
            {
                inListing = false;
            }

            resLines.emplace_back(line);
        }
        else // normal mode
        {
            if (isListingCommand(line))
            {
                inListing = true;
                resLines.emplace_back(line);
                continue;
            }

            if (isInsertCommand(line))
            {
                auto snippetFlagsOptions = appCfg.snippetOptions;
                std::unordered_map<SnippetOptions, int> intOptions;
                std::string snippetFile;
                std::string snippetTag ;
                SnippetOptionsParsingResult parseRes = parseSnippetInsertionCommandLine( snippetFlagsOptions, intOptions, appCfg.conditionVars
                                                                                       , line, snippetFile, snippetTag
                                                                                       );
                if (parseRes==SnippetOptionsParsingResult::okButCondition)
                {
                    // condition failed, simple skip it
                }
                else if (parseRes==SnippetOptionsParsingResult::fail)
                {
                    // optionally adding it to document
                    // Тестируем только те опции, которые были заданы из командной строки
                    // if (testFlagSnippetOption(appCfg.snippetOptions, SnippetOptions::fail))
                    // Или? Пока вставляем бузусловно
                    {
                        resLines.emplace_back(line);
                    }
                }
                else // process it unconditionally
                {
                    if (testFlagSnippetOption(appCfg.snippetOptions, SnippetOptions::doc))
                    {
                        // insert as document
                        // intOptions, SnippetOptions::raise

                        if (alreadyIncludedDocs.find(snippetFile)!=alreadyIncludedDocs.end())
                        {
                            // already included
                            // simple skip it? Or not?
                            resLines.emplace_back(line);
                            continue;
                        }

                        std::string foundFullFilename;
                        std::string foundFileText;
                        auto findRes = appCfg.findDocFileByIncludedFromFilename(snippetFile, foundFullFilename, foundFileText, curFilename);
                        if (!findRes)
                        {
                            // document not found
                            resLines.emplace_back(line);
                            continue;
                        }

                        std::vector<std::string> docLines = marty_cpp::splitToLinesSimple(foundFileText);
                        std::unordered_set<std::string> alreadyIncludedDocsCopy = alreadyIncludedDocs;
                        alreadyIncludedDocsCopy.insert(foundFullFilename);
                        std::vector<std::string> processedDocLines = processMdFileLines(appCfg, docLines, foundFullFilename, alreadyIncludedDocsCopy);

                        //TODO: !!! raise titles here

                        resLines.insert(resLines.end(), processedDocLines.begin(), processedDocLines.end());
                        
                        continue;

                    }
                    else // insert as code snippet 
                    {
                        std::string foundFullFilename;
                        std::string foundFileText;
                        auto findRes = appCfg.findSamplesFile(snippetFile, foundFullFilename, foundFileText /* , curFilename */ );
                        if (!findRes)
                        {
                            // snippets file not found
                            resLines.emplace_back(line);
                            continue;
                        }

                        std::vector<std::string> snippetsFileLines = marty_cpp::splitToLinesSimple(foundFileText);
                        std::vector<std::string> insertLines; insertLines.reserve(snippetsFileLines.size());
                        std::size_t firstLineIdx = 0;

                        std::string lang = appCfg.getLangByFilename(foundFullFilename);

                        if (snippetTag.empty())
                        {
                            // вставляем файл целиком
                            insertLines = snippetsFileLines;
                        }
                        else
                        {
                            // Тут надо искать конкретные строки, но пока вставляем всё, что есть
                            std::size_t startLineNo = 0;
                            std::size_t &endLineNo  = 0;
                            if (isCodeTagLinesRange(snippetTag, startLineNo, endLineNo))
                            {
                                firstLineIdx = startLineNo;
                                for(std::size_t idx=startLineNo; idx!=endLineNo; ++idx)
                                {
                                    if (idx>=snippetsFileLines.size())
                                        break;
                                    insertLines.emplace_back(snippetsFileLines[idx]);
                                }
                                
                            }
                            else // Тут надо искать по текстовому тэгу
                            {
                                std::string snippetTagPrefix;
                                if (!lang.empty())
                                {
                                    snippetTagPrefix = appCfg.getLangCutPrefix(lang);
                                }

                                if (snippetTagPrefix.empty())
                                {
                                    // Не знаем, как искать тэг - нет информации по тому, какой префикс используется для тэгов сниппетов в данном языке
                                    // Поэтому просто ошибка
                                    resLines.emplace_back(line);
                                    continue;
                                
                                }
                                
                            }

                            insertLines = snippetsFileLines;
                        }

                        
                    
                    }

    // noDoc        = 0x10F0 /*!< -doc */,
    // doc          = 0x10F1 /*!< +doc */,
    // raise        = 0x2011 /*!< Numeric option */
                
                }
            }
        
        }

    }


    return lines;
}


inline
std::string processMdFile(const AppConfig &appCfg, std::string fileText, const std::string &curFilename)
{
    //fileText = marty_cpp::normalizeCrLfToLf(fileText);
    std::vector<std::string> lines = marty_cpp::splitToLinesSimple(fileText);

    auto resLines = processMdFileLines(appCfg, lines, curFilename);

    return marty_cpp::mergeLines(resLines, appCfg.outputLinefeed, true  /* addTrailingNewLine */ );
}



// SnippetOptionsParsingResult parseSnippetInsertionCommandLine( std::unordered_set<SnippetOptions>       &snippetFlagsOptions
//                                                             , std::unordered_map<SnippetOptions, int>  &snippetIntOptions
//                                                             , const std::unordered_map<std::string, std::string> &condVars
//                                                             , std::string line, std::string &snippetFile, std::string &snippetTag
//                                                             )
