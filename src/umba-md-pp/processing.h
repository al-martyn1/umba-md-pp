#pragma once

#include "app_config.h"
//
#include "marty_cpp/src_normalization.h"

//
#include <stack>
#include <string>

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
std::vector<std::string> trimLeadingSpaces(std::vector<std::string> lines, bool bTrim)
{
    std::size_t minLeadingSpaces = 4096u;
    for(auto &l : lines)
    {
        umba::string_plus::rtrim(l); // убираем конечные пробелы
        if (l.empty())
        {
            continue; // на пустых строках нет смысла париться
        }

        std::size_t i = 0u;
        for(; i!=l.size(); ++i)
        {
            if (l[i]!=' ')
                break;
        }

        // либо брякнулись, либо дошли до конца

        minLeadingSpaces = std::min(minLeadingSpaces, i);
    }

    if (!bTrim)
    {
        return lines;
    }

    if (!minLeadingSpaces)
    {
        return lines;
    }

    for(auto &l : lines)
    {
        if (l.empty())
        {
            continue; // на пустых строках нет смысла париться
        }

        if (l.size()<minLeadingSpaces)
        {
            continue;
        }

        l.erase(0, minLeadingSpaces);
    }

    return lines;
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
                                            //, bool                        trimLeadingSpaces_a = true
                                            )
{
    marty_cpp::expandTabsToSpaces(lines, tabSize);

    if (targetFragmentTag.empty() || lines.empty())
    {
        firstFoundLineIdx = 0;
        return lines;
    }

    std::vector<std::string> fragmentLines; fragmentLines.reserve(lines.size());


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

            case ListingNestedTagsMode::invalid :
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
    };

    auto closeCurTag = [&]()
    {
        if (openedTags.empty())
            return;
        openedTags.pop_back();
    };

    // auto getTagLevel = [&]()
    // {
    //     return openedTags.size();
    // };

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
    };

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
        extractCodeTagFromLine(l, tagPrefix, curTag);

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
        // Или он может быть пустым при пустом стеке
        if (curTag.empty())
        {
            continue; // Игнорим закрывающие тэги, когда не было открывающих
        }

        if (curTag==targetFragmentTag)
        {
            firstFoundLineIdx = lineIdx+1; // начали со строки, которая следует за открывающим тэгом
        }

        // Если наш целевой тэг открыт, то надо добавить строчку, а тут идёт обработка вложенных тэгов
        if (isTargetFragmentTagOpened())
        {
            //fragmentLines.emplace_back(l);
            addTagLine(l);
        }

        openCodeFragment(curTag);
    }

    return fragmentLines;
}


std::string processMdFile(const AppConfig &appCfg, std::string fileText, const std::string &curFilename);

inline
std::vector<std::string> processMdFileLines(const AppConfig &appCfg, const std::vector<std::string> &lines, const std::string &curFilename, const std::unordered_set<std::string> &alreadyIncludedDocs=std::unordered_set<std::string>())
{
    std::vector<std::string> resLines; resLines.reserve(lines.size());
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
                //resLines.emplace_back("!!! insert (1)");

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
                    //resLines.emplace_back("!!! insert (2)");
                }
                else if (parseRes==SnippetOptionsParsingResult::fail)
                {
                    //resLines.emplace_back("!!! insert (3)");

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
                    //resLines.emplace_back("!!! insert (4)");

                    if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::doc))
                    {
                        // insert as document
                        // intOptions, SnippetOptions::raise

                        //resLines.emplace_back("!!! insert (5)");

                        if (alreadyIncludedDocs.find(snippetFile)!=alreadyIncludedDocs.end())
                        {
                            //resLines.emplace_back("!!! insert (6)");

                            // already included
                            // simple skip it? Or not?
                            resLines.emplace_back(line);
                            continue;
                        }

                        //resLines.emplace_back("!!! insert (7)");

                        std::string foundFullFilename;
                        std::string foundFileText;
                        auto findRes = appCfg.findDocFileByIncludedFromFilename(snippetFile, foundFullFilename, foundFileText, curFilename);
                        if (!findRes)
                        {
                            //resLines.emplace_back("!!! insert (8)");
    
                            // document not found
                            // Если fail-опция не установлена, то не выводим ничего
                            // По умолчанию в конфигах .options - установлена
                            // Но если мы хотим тихо ничего не делать при обломе поиска подключаемого файла, то надо явно указать no-fail
                            if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::fail))
                            {
                                //resLines.emplace_back("!!! insert (9)");
                                resLines.emplace_back(line);
                            }
                            continue;
                        }

                        //resLines.emplace_back("!!! insert (10)");

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
                        //resLines.emplace_back("!!! insert (11)");

                        std::string foundFullFilename;
                        std::string foundFileText;
                        auto findRes = appCfg.findSamplesFile(snippetFile, foundFullFilename, foundFileText /* , curFilename */ );
                        if (!findRes)
                        {
                            //resLines.emplace_back("!!! insert (12)");
    
                            // snippets file not found
                            // Если fail-опция не установлена, то не выводим ничего
                            // По умолчанию в конфигах .options - установлена
                            // Но если мы хотим тихо ничего не делать при обломе поиска подключаемого файла, то надо явно указать no-fail
                            if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::fail))
                            {
                                //resLines.emplace_back("!!! insert (13)");
                                resLines.emplace_back(line);
                            }
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
                            std::size_t endLineNo   = 0;
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
                                    //resLines.emplace_back("!!! insert (14)");
            
                                    continue;
                                }

                                ListingNestedTagsMode listingNestedTagsMode = ListingNestedTagsMode::remove;

                                if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::keepCutTags))
                                {
                                    listingNestedTagsMode = ListingNestedTagsMode::keep;
                                }
                                else if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::lineNo))
                                {
                                    listingNestedTagsMode = ListingNestedTagsMode::emptyLine;
                                }

                                insertLines = extractCodeFragment(snippetsFileLines, firstLineIdx, snippetTag, snippetTagPrefix, listingNestedTagsMode, 4u /* tabSize */ );

                                //std::vector<std::string> trimLeadingSpaces(std::vector<std::string> lines, bool bTrim)
                                //trimLeadingSpaces(lines, trimLeadingSpaces_a)
                            }

                            // insertLines = snippetsFileLines;
                        }

                        // 
                        if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::trimLeft))
                        {
                            insertLines = trimLeadingSpaces(insertLines, true);
                        }


                        if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::trimArround))
                        {
                            //std::vector<std::string> newLines1; newLines1.reserve(insertLines.size());

                            std::vector<std::string>::const_iterator itNonEmptyFirst = insertLines.begin();
                            for(
                               ; itNonEmptyFirst!=insertLines.end()
                               ; ++itNonEmptyFirst, ++firstLineIdx
                               )
                            {
                                auto l = *itNonEmptyFirst;
                                umba::string_plus::trim(l);
                                if (!l.empty())
                                {
                                    //++itNonEmptyFirst;
                                    break;
                                }
                            }

                            // Удаляем пустые строки в начале блока
                            insertLines.erase(insertLines.begin(), itNonEmptyFirst);


                            std::vector<std::string>::const_iterator itNonEmptyLast = insertLines.begin();
                            for(std::vector<std::string>::const_iterator it=itNonEmptyLast; it!=insertLines.end(); ++it)
                            {
                                auto l = *it;
                                umba::string_plus::trim(l);
                                if (!l.empty())
                                {
                                    itNonEmptyLast = it;
                                }
                            }

                            if (itNonEmptyLast!=insertLines.end())
                            {
                                ++itNonEmptyLast;
                                insertLines.erase(itNonEmptyLast, insertLines.end());
                            }


                            // for(auto l: insertLines)
                            // {
                            //     umba::string_plus::trim(l);
                            //     if (l.empty())
                            //     {
                            //         ++firstLineIdx;
                            //         continue;
                            //     }
                            //  
                            //     newLines1.emplace_back(l);
                            // }
                            //  
                            //  
                            // insertLines = newLines1;
                        
                        }


                        std::string firstLineNoStr = std::to_string(firstLineIdx+1u);

                        if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::lineNo))
                        {
                            std::size_t lastLineIdx = firstLineIdx + insertLines.size();
                            ++firstLineIdx;
    
                            std::size_t numDigits = 0;
                            std::size_t lastLineIdxRest = lastLineIdx;
                            while(lastLineIdxRest>0)
                            {
                                ++numDigits;
                                lastLineIdxRest /= 10u;
                            }

                            for( auto &l : insertLines)
                            {
                                std::string lineNoStr = std::to_string(firstLineIdx++);
                                std::string fullLineNoStr = std::string(numDigits-lineNoStr.size(), ' ');
                                fullLineNoStr.append(lineNoStr);
                                fullLineNoStr.append(1u, ':');
                                fullLineNoStr.append(1u, ' ');
                                l = fullLineNoStr + l;
                            }
                        }

                        if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::snippetOptions))
                        {
                            std::string snippetOptionsLine = serializeSnippetOptions(snippetFlagsOptions, intOptions);
                            resLines.emplace_back("{" + snippetOptionsLine + "}");
                        }

                        if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::filename))
                        {
                            std::string filename = umba::filename::normalizePathSeparators(snippetFile, '/');
                            if (!testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::path))
                            {
                                filename = umba::filename::normalizePathSeparators(umba::filename::getFileName(snippetFile), '/');
                            }

                            if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::filenameLineNo))
                            {
                                filename.append(1u,':');
                                filename.append(firstLineNoStr);
                            }

                            resLines.emplace_back(filename); //TODO: !!! Нужно добавить обрамление
                        }

                        //std::string lang = appCfg.getLangByFilename(foundFullFilename);
                        std::string listingLangTag;
                        if (!lang.empty())
                        {
                            listingLangTag = appCfg.getLangListingTag(lang);
                        }

                        std::string lstStart = std::string(3u,'`');
                        std::string lstEnd   = lstStart;
                        if (!listingLangTag.empty())
                        {
                            lstStart.append(listingLangTag); //TODO: !!! Нужно добавить обрамление
                        }

                        resLines.emplace_back(lstStart);
                        resLines.insert(resLines.end(), insertLines.begin(), insertLines.end());
                        resLines.emplace_back(lstEnd);

                    } // end of insert as code snippet 
                
                }

            } // if (isInsertCommand(line))

            else // Some other conditions
            {
                resLines.emplace_back(line);
            }
        
        } // end normal mode

    }

    return resLines;
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
