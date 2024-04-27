#pragma once

#include "app_config.h"
//
#include "marty_cpp/src_normalization.h"

#include "umba/transliteration.h"
#include "umba/id_gen.h"


//
#include <stack>
#include <string>
#include <utility>

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
inline
bool isInsertCommand(std::string line)
{
    umba::string_plus::trim(line);
    return umba::string_plus::starts_with(line, ("#!insert"));
}

//----------------------------------------------------------------------------
inline
bool isTocCommand(std::string line)
{
    umba::string_plus::trim(line);
    return umba::string_plus::starts_with(line, ("#!toc"));
}

//----------------------------------------------------------------------------
inline
bool isSingleLineComment(std::string line)
{
    umba::string_plus::trim(line);
    return umba::string_plus::starts_with(line, ("#//"));
}

//----------------------------------------------------------------------------
inline
bool isListingCommand(std::string line)
{
    umba::string_plus::trim(line);
    return umba::string_plus::starts_with(line, ("```")) || umba::string_plus::starts_with(line, ("~~~"));
}

//----------------------------------------------------------------------------
inline
bool isMetaStartCommand(std::string line)
{
    umba::string_plus::trim(line);
    return umba::string_plus::starts_with(line, ("---"));
}

//----------------------------------------------------------------------------
inline
bool isMetaEndCommand(std::string line)
{
    umba::string_plus::trim(line);
    return umba::string_plus::starts_with(line, ("---")) || umba::string_plus::starts_with(line, ("..."));
}

//----------------------------------------------------------------------------
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
bool hasIdentifierChars(const std::string str)
{
    for(char ch: str)
    {
        if (ch>='a' && ch<='z')
            return true;
        if (ch>='A' && ch<='Z')
            return true;
        if (ch>='0' && ch<='9')
            return true;
        if (ch=='_')
            return true;
    }

    return false;
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

    if (!hasIdentifierChars(line))
        line.clear();

    return true;
}

//----------------------------------------------------------------------------
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

    auto isTargetFragmentTagOpened = [&]()
    {
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


    std::size_t lineIdx = 0;

    for(; lineIdx!=lines.size(); ++lineIdx)
    {
        auto l = lines[lineIdx]; 

        if (!isCodeTagLine(l, tagPrefix))
        {
            // Строка обычная, не тэговая
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

//----------------------------------------------------------------------------
std::string processMdFile(const AppConfig &appCfg, std::string fileText, const std::string &curFilename);

//----------------------------------------------------------------------------
//! LineHandler: bool handler(LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
template<typename LineHandler> inline
std::vector<std::string> processLines(const AppConfig &appCfg, const std::vector<std::string> &lines, LineHandler handler)
{
    std::vector<std::string> resLines; resLines.reserve(lines.size());

    if (lines.empty())
        return resLines;

    std::size_t lastLineIdx = lines.size()-1;

    PreprocessorParsingState state = PreprocessorParsingState::normal;

    //for(auto line: lines)
    for(std::size_t idx=0u; idx!=lines.size(); ++idx)
    {
        std::string line = lines[idx];

        if (state==PreprocessorParsingState::listing) // listing mode
        {
            if (isListingCommand(line))
            {
                state = PreprocessorParsingState::normal;
                if (handler(LineHandlerEvent::listingEnd, resLines, line, idx, lastLineIdx))
                {
                    resLines.emplace_back(line);
                }
            }
            else
            {
                if (handler(LineHandlerEvent::listingLine, resLines, line, idx, lastLineIdx))
                {
                    resLines.emplace_back(line);
                }
            }
        }

        else if (state==PreprocessorParsingState::meta) // meta mode
        {
            if (isMetaEndCommand(line))
            {
                state = PreprocessorParsingState::normal;
                if (handler(LineHandlerEvent::metaEnd, resLines, line, idx, lastLineIdx))
                {
                    resLines.emplace_back(line);
                }
            }
            else
            {
                if (handler(LineHandlerEvent::metaLine, resLines, line, idx, lastLineIdx))
                {
                    resLines.emplace_back(line);
                }
            }
        }

        else if (state==PreprocessorParsingState::normal) // normal mode
        {
            if (isSingleLineComment(line))
            {
                // Пропускаем коменты
                continue;
            }

            if (isListingCommand(line))
            {
                state = PreprocessorParsingState::listing;
                if (handler(LineHandlerEvent::listingStart, resLines, line, idx, lastLineIdx))
                {
                    resLines.emplace_back(line);
                }
                continue;
            }

            if (isMetaStartCommand(line) && idx==0) // metadata allowed only at the beginning of the file
            {
                state = PreprocessorParsingState::meta;
                if (handler(LineHandlerEvent::metaStart, resLines, line, idx, lastLineIdx))
                {
                    resLines.emplace_back(line);
                }
                continue;
            }

            if (isInsertCommand(line))
            {
                if (handler(LineHandlerEvent::insertCommand, resLines, line, idx, lastLineIdx))
                {
                    resLines.emplace_back(line);
                }
                continue;
            }

            if (isTocCommand(line))
            {
                if (handler(LineHandlerEvent::tocCommand, resLines, line, idx, lastLineIdx))
                {
                    resLines.emplace_back(line);
                }
                continue;
            }

            if (isHeaderCommand(line))
            {
                if (handler(LineHandlerEvent::headerCommand, resLines, line, idx, lastLineIdx))
                {
                    resLines.emplace_back(line);
                }
                continue;
            }

            if (handler(LineHandlerEvent::normalLine, resLines, line, idx, lastLineIdx))
            {
                resLines.emplace_back(line);
            }
        }

        else
        {
            // Something goes wrong
        }
    }

    #if 0
    bool inListing = false;

    for(auto line: lines)
    {
        if (inListing)
        {
            if (isListingCommand(line))
            {
                inListing = false;
                if (handler(LineHandlerEvent::listingEnd, resLines, line))
                {
                    resLines.emplace_back(line);
                }
            }
            else
            {
                if (handler(LineHandlerEvent::listingLine, resLines, line))
                {
                    resLines.emplace_back(line);
                }
            }
        }

        else if (isSingleLineComment(line))
        {
            // Пропускаем коменты
        }

        else // normal mode
        {
            if (isListingCommand(line))
            {
                inListing = true;
                if (handler(LineHandlerEvent::listingStart, resLines, line))
                {
                    resLines.emplace_back(line);
                }
                continue;
            }

            if (isInsertCommand(line))
            {
                if (handler(LineHandlerEvent::insertCommand, resLines, line))
                {
                    resLines.emplace_back(line);
                }
                continue;
            }

            if (isTocCommand(line))
            {
                if (handler(LineHandlerEvent::tocCommand, resLines, line))
                {
                    resLines.emplace_back(line);
                }
                continue;
            }

            if (isHeaderCommand(line))
            {
                // std::string lCopy = line;
                // if (handler(lCopy))
                // {
                //     resLines.emplace_back(lCopy);
                // }
                if (handler(LineHandlerEvent::headerCommand, resLines, line))
                {
                    resLines.emplace_back(line);
                }
                continue;
            }

            if (handler(LineHandlerEvent::normalLine, resLines, line))
            {
                resLines.emplace_back(line);
            }
        }
    }
    #endif

    return resLines;
}

//----------------------------------------------------------------------------
template<typename HeaderLineHandler> inline
std::vector<std::string> processHeaderLines(const AppConfig &appCfg, const std::vector<std::string> &lines, HeaderLineHandler headerHandler)
{
    auto handler = [&](LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
    {
        if (event!=LineHandlerEvent::headerCommand)
        {
            return true;
        }

        if (headerHandler(line))
        {
            //resLines.emplace_back(line);
            return true;
        }

        return false;
    };

    return processLines(appCfg, lines, handler);

    // std::vector<std::string> resLines; resLines.reserve(lines.size());
    //  
    // bool inListing = false;
    //  
    // for(const auto &line: lines)
    // {
    //     if (inListing)
    //     {
    //         if (isListingCommand(line))
    //         {
    //             inListing = false;
    //         }
    //  
    //         resLines.emplace_back(line);
    //     }
    //  
    //     else if (isSingleLineComment(line))
    //     {
    //         // Пропускаем коменты
    //     }
    //  
    //     else // normal mode
    //     {
    //         if (isListingCommand(line))
    //         {
    //             inListing = true;
    //             resLines.emplace_back(line);
    //             continue;
    //         }
    //  
    //         if (isInsertCommand(line))
    //         {
    //             resLines.emplace_back(line);
    //             continue;
    //         }
    //  
    //         if (isTocCommand(line))
    //         {
    //             resLines.emplace_back(line);
    //             continue;
    //         }
    //  
    //         if (isHeaderCommand(line))
    //         {
    //             std::string lCopy = line;
    //             if (handler(lCopy))
    //             {
    //                 resLines.emplace_back(lCopy);
    //             }
    //             continue;
    //         }
    //  
    //         resLines.emplace_back(line);
    //     }
    // }
    //  
    // return resLines;
}

//----------------------------------------------------------------------------
inline
bool splitHeaderLine(const std::string &line, std::string &levelStr, std::string &headerText)
{
    std::size_t hashPos = 0;
    while(hashPos!=line.size() && line[hashPos]!='#' )
    {
        ++hashPos;
    }

    if (hashPos==line.size())
        return false;

    std::size_t nextPos = hashPos;
    while(nextPos!=line.size() && line[nextPos]=='#' )
    {
        ++nextPos;
    }

    levelStr.assign(line, hashPos, nextPos-hashPos);

    while(nextPos!=line.size() && line[nextPos]==' ' )
    {
        ++nextPos;
    }

    headerText.assign(line, nextPos, line.npos);

    umba::string_plus::rtrim(headerText, [](char ch) { return ch=='#'; } );
    umba::string_plus::rtrim(headerText);

    umba::string_plus::rtrim(levelStr);

    return true;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> raiseHeaders(const AppConfig &appCfg, const std::vector<std::string> &lines, int raiseVal)
{
    // Ограничиваем изменение разумной величиной
    if (raiseVal>3)
        raiseVal = 3;
    if (raiseVal<-3)
        raiseVal = -3;

    if (!raiseVal)
         return lines;

    auto raiseHeader = [&](std::string &line) -> bool
    {
        std::string levelStr;
        std::string headerText;
        
        if (!splitHeaderLine(line, levelStr, headerText))
            return true;

        std::size_t newHeaderSize = levelStr.size();

        int rv = raiseVal;

        if (rv<0)
        {
            rv = -rv;
            newHeaderSize += (std::size_t)rv;
        }
        else
        {
            if (rv<=(int)newHeaderSize)
            {
                newHeaderSize -= (std::size_t)rv;
            }
            else
            {
                newHeaderSize = 0;
            }
        }

        if (newHeaderSize==0)
           newHeaderSize = 1;

        line = std::string(newHeaderSize, '#') + std::string(1u, ' ') + headerText;

        return true;
    };

    return processHeaderLines(appCfg, lines, raiseHeader);
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> generateSecionNumbers(const AppConfig &appCfg, const std::vector<std::string> &lines)
{
    int sectionCounters[16] = { 0 }; // Не более 16 уровней секций

    auto generateSectionNumber = [&](std::size_t lvl)
    {
        std::string resStr;

        if (lvl>16)
            lvl = 16;

        for(std::size_t i=0; i!=lvl; ++i)
        {
            resStr += std::to_string(sectionCounters[i] /* +1 */ );
            resStr += ".";
        }
    
        return resStr;
    };

    auto processSectionNumber = [&](std::string &line) -> bool
    {
        std::string levelStr;
        std::string headerText;
        
        if (!splitHeaderLine(line, levelStr, headerText))
            return true;
    
        if (levelStr.empty())
            return true;

        std::size_t curSectionLevel = levelStr.size();
        if (!curSectionLevel || curSectionLevel>=16u)
            return true;

        ++sectionCounters[curSectionLevel-1];

        line = levelStr + std::string(1u,' ') + generateSectionNumber(curSectionLevel) + std::string(1u,' ') + headerText;
        for( /* ++curSectionLevel */ ; curSectionLevel!=16u; ++curSectionLevel)
        {
            sectionCounters[curSectionLevel] = 0;
        }

        return true;
    };

    return processHeaderLines(appCfg, lines, processSectionNumber);
}


//----------------------------------------------------------------------------
inline
bool isSectionNumber(const std::string &str)
{
    for(auto ch: str)
    {
        if (ch>='0' && ch<='9')
           continue;
        if (ch=='.')
           continue;

        return false;
    }

    return true;
}

//----------------------------------------------------------------------------
inline
std::string generateSectionIdImpl(const AppConfig &appCfg, std::string secText)
{
    if (appCfg.targetRenderer==TargetRenderer::github)
    {
    }
    else if (appCfg.targetRenderer==TargetRenderer::doxygen)
    {
        auto spacePos = secText.find(' ');
        if (spacePos!=secText.npos)
        {
            if (isSectionNumber(std::string(secText, 0, spacePos)))
            {
                secText.erase(0, spacePos+1);
            }
        }
    }
    else
    {
    }

    umba::string_plus::rtrim(secText);

    if (appCfg.targetRenderer==TargetRenderer::github)
    {
        return umba::generateIdFromText_forGitHub(secText);
    }
    else if (appCfg.targetRenderer==TargetRenderer::doxygen)
    {
        return umba::generateIdFromText_generic(secText, '-');
    }
    else
    {
        return std::string();
    }
}

//----------------------------------------------------------------------------
inline
std::string generateSectionId(const AppConfig &appCfg, const std::string secLine, std::size_t *pLevel=0, std::string *pHeaderText=0)
{
    if (pLevel)
    {
        *pLevel = 0;
    }

    std::string levelStr;
    std::string headerText;

    if (!splitHeaderLine(secLine, levelStr, headerText))
        return std::string();

    if (pHeaderText)
    {
        *pHeaderText = headerText;
    }

    if (headerText.empty())
        return std::string();

    if (headerText.back()=='}') // already has id?
        return std::string();

    // if (appCfg.targetRenderer!=TargetRenderer::doxygen)
    //     return std::string(); // Нужно только для доксигена

    if (pLevel)
    {
        *pLevel = levelStr.size();
    }

    if (headerText.back()==']' && appCfg.targetRenderer==TargetRenderer::doxygen)
    {
        // У нас есть идентификаторы в квадратных скобках, по ним мы генерим якоря (только doxygen)
        std::size_t idx = headerText.size();
        for(; idx!=0 && headerText[idx-1]!='['; --idx) {}

        if (idx==0)
        {
            return generateSectionIdImpl(appCfg, headerText);
        }

        std::string takenId = std::string(headerText, idx, headerText.size()-idx-1);
        return generateSectionIdImpl(appCfg, takenId);
    }
    else
    {
        return  generateSectionIdImpl(appCfg, headerText);
    }

}
//----------------------------------------------------------------------------
inline
std::vector<std::string> generateSectionIds(const AppConfig &appCfg, const std::vector<std::string> &lines)
{
    std::unordered_map<std::string, std::size_t> usedIds;

    auto processSectionHeader = [&](std::string &line) -> bool
    {
        // std::string levelStr;
        // std::string headerText;
        //  
        // if (!splitHeaderLine(line, levelStr, headerText))
        //     return true;
        //  
        // if (headerText.empty())
        //     return true;
        //  
        // if (headerText.back()=='}') // already has id?
        //     return true;
        //  
        // if (appCfg.targetRenderer!=TargetRenderer::doxygen)
        //     return true; // Нужо только для доксигена

        std::size_t headerLevel = 0;
        std::string headerText;
        std::string id = generateSectionId(appCfg, line, &headerLevel, &headerText);
        if (headerLevel==0 || id.empty())
            return true;

        // if (headerText.back()==']')
        // {
        //     // У нас есть идентификаторы в квадратных скобках, по ним мы генерим якоря
        //     std::size_t idx = headerText.size();
        //     for(; idx!=0 && headerText[idx-1]!='['; --idx) {}
        //  
        //     if (idx==0)
        //         return true; // Открывающая '[' не найдена
        //  
        //     std::string takenId = std::string(headerText, idx, headerText.size()-idx-1);
        //     id = umba::generateIdFromText_generic(takenId, '-');
        // }
        // else
        // {
        //     id = umba::generateIdFromText_generic(headerText, '-');
        // }

        ++usedIds[id];

        if (usedIds[id]>1)
        {
            auto n = usedIds[id];
            id.append(1,'-');
            id.append(std::to_string(n));
        }

        line = std::string(headerLevel, '#') + std::string(1u,' ') + headerText + std::string(" {#") + id + std::string("}");

        return true;
    };

    return processHeaderLines(appCfg, lines, processSectionHeader);
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> generateTocLines(const AppConfig &appCfg, const std::vector<std::string> &lines)
{
    std::unordered_map<std::string, std::size_t> usedIds;

    std::vector<std::string> tocLines;

    auto processSectionHeader = [&](std::string &line) -> bool
    {
        std::string levelStr;
        std::string headerText;
    
        if (!splitHeaderLine(line, levelStr, headerText))
            return false;

        std::size_t headerLevel = 0;
        std::string id = generateSectionId(appCfg, line, &headerLevel);

        if (!headerLevel)
            return false; // нам не надо в целевой вектор ничего добавлять

        std::string tocLine = std::string(headerLevel*2u /* +2u */ , ' ');
        tocLine.append("- ");

        if (id.empty())
        {
            // Generate simple text entry
            tocLine.append(headerText);
            tocLines.emplace_back(tocLine);
            return false;
        }

        ++usedIds[id];

        if (usedIds[id]>1)
        {
            auto n = usedIds[id];
            id.append(1,'-');
            id.append(std::to_string(n));
        }

        tocLine.append("[");
        tocLine.append(headerText);
        tocLine.append("](#");
        tocLine.append(id);
        tocLine.append(")");
        tocLines.emplace_back(tocLine);

        return false;
    };

    processHeaderLines(appCfg, lines, processSectionHeader);

    return tocLines;
}

//----------------------------------------------------------------------------
// //----------------------------------------------------------------------------
// template<typename HeaderLineHandler> inline
// std::vector<std::string> processHeaderLines(const AppConfig &appCfg, const std::vector<std::string> &lines, HeaderLineHandler headerHandler)
// {
//     auto handler = [&](LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
//     {
//         if (event!=LineHandlerEvent::headerCommand)
//         {
//             return true;
//         }
//  
//         if (headerHandler(line))
//         {
//             //resLines.emplace_back(line);
//             return true;
//         }
//  
//         return false;
//     };
//  
//     return processLines(appCfg, lines, handler);

//----------------------------------------------------------------------------
std::vector<std::string> processMdFileLines(const AppConfig &appCfg, const std::vector<std::string> &lines, const std::string &curFilename, const std::unordered_set<std::string> &alreadyIncludedDocs);

//----------------------------------------------------------------------------
template <typename SetType> inline
SetType updatedSet(const SetType &s, typename SetType::key_type k, bool bAddKey /*else remove*/)
{
    SetType sCopy = s;
    if (bAddKey)
    {
        sCopy.insert(k);
    }
    else
    {
        sCopy.erase(k);
    }

    return sCopy;
}

//----------------------------------------------------------------------------
template<typename VectorType> inline
void vectorPushBack(VectorType &pushBackTo, const VectorType &pushFrom)
{
    pushBackTo.insert( pushBackTo.end(), pushFrom.begin(), pushFrom.end() );
}

//----------------------------------------------------------------------------
inline
bool insertDoc( const AppConfig          &appCfg
              , std::vector<std::string> &resLines
              , const std::string        &insertCommandLine
              , const std::string        &curFilename
              , const std::string        &docFile
              , const std::unordered_set<SnippetOptions>      &snippetFlagsOptions
              , const std::unordered_map<SnippetOptions, int> &intOptions
              , const std::unordered_set<std::string> &alreadyIncludedDocs
              )
{
    std::string foundFullFilename;
    std::string foundFileText;
    auto findRes = appCfg.findDocFileByIncludedFromFilename(docFile, foundFullFilename, foundFileText, curFilename);
    if (!findRes) // document not found
    {
        if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::fail))
        {
            return false; // сфейли
        }
    }

    // файл может включаться под разными именами, через всякие .. или ., слэши опять же разные
    std::string foundFullFilenameCanonical = umba::filename::makeCanonical( foundFullFilename, '/' );

    if (alreadyIncludedDocs.find(foundFullFilenameCanonical)!=alreadyIncludedDocs.end())
    {
        return true; // делаем вид, что всё хорошо, а на самом деле предотвратили рекурсивное подключение одного и того же файла
    }

    
    std::vector<std::string> docLines = marty_cpp::splitToLinesSimple(foundFileText);
    //std::unordered_set<std::string> alreadyIncludedDocsCopy = alreadyIncludedDocs;
    //alreadyIncludedDocsCopy.insert(foundFullFilename);
    std::vector<std::string> processedDocLines = processMdFileLines(appCfg, docLines, foundFullFilename, updatedSet(alreadyIncludedDocs, foundFullFilenameCanonical, true /* bAddKey */ ) /* alreadyIncludedDocsCopy */ );
    
    //TODO: !!! extract meta info here

    std::unordered_map<SnippetOptions, int>::const_iterator raiseOptIt = intOptions.find(SnippetOptions::raise);
    if (raiseOptIt!=intOptions.end() && raiseOptIt->second!=0)
    {
        processedDocLines = raiseHeaders(appCfg, processedDocLines, raiseOptIt->second);
    }
    
    resLines.insert(resLines.end(), processedDocLines.begin(), processedDocLines.end());
    
    return true; // всё хорошо
    
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> prepareSnippetLines( const AppConfig                &appCfg
                                            , std::vector<std::string>       lines
                                            , std::string                    snippetFilename
                                            , std::size_t                    firstLineIdx
                                            , bool                           bTrimLeft
                                            , bool                           trimArround
                                            , bool                           addLineNumbers
                                            , bool                           addFilename
                                            , bool                           addFilenameOnly
                                            , bool                           addFilenameLineNumber
                                            )
{
    lines = trimLeadingSpaces(lines, bTrimLeft);

    if (trimArround)
    {
        std::vector<std::string>::const_iterator itNonEmptyFirst = lines.begin();
        for(
           ; itNonEmptyFirst!=lines.end()
           ; ++itNonEmptyFirst, ++firstLineIdx
           )
        {
            auto l = *itNonEmptyFirst;
            umba::string_plus::trim(l);
            if (!l.empty())
                break;
        }
    
        // Удаляем пустые строки в начале блока
        lines.erase(lines.begin(), itNonEmptyFirst);
    
    
        std::vector<std::string>::const_iterator itNonEmptyLast = lines.begin();
        for(std::vector<std::string>::const_iterator it=itNonEmptyLast; it!=lines.end(); ++it)
        {
            auto l = *it;
            umba::string_plus::trim(l);
            if (!l.empty())
            {
                itNonEmptyLast = it;
            }
        }
    
        if (itNonEmptyLast!=lines.end())
        {
            ++itNonEmptyLast;
            lines.erase(itNonEmptyLast, lines.end());
        }
    }

    std::size_t filenameLineNo = firstLineIdx+1;

    if (addLineNumbers)
    {
        std::size_t lastLineIdx = firstLineIdx + lines.size();
        ++firstLineIdx;
    
        std::size_t numDigits = 0;
        std::size_t lastLineIdxRest = lastLineIdx;
        while(lastLineIdxRest>0)
        {
            ++numDigits;
            lastLineIdxRest /= 10u;
        }
    
        for( auto &l : lines)
        {
            std::string lineNoStr = std::to_string(firstLineIdx++);
            std::string fullLineNoStr = std::string(numDigits-lineNoStr.size(), ' ');
            fullLineNoStr.append(lineNoStr);
            fullLineNoStr.append(1u, ':');
            fullLineNoStr.append(1u, ' ');
            l = fullLineNoStr + l;
        }
    }

    std::vector<std::string> resLines;

    std::string lang = appCfg.getLangByFilename(snippetFilename);

    if (addFilename)
    {
        std::string filename = umba::filename::normalizePathSeparators(snippetFilename, '/');
        if (addFilenameOnly)
        {
            filename = umba::filename::getFileName(filename);
        }
        if (addFilenameLineNumber)
        {
            filename.append(1u,':');
            filename.append(std::to_string(filenameLineNo));
        }

        resLines.emplace_back(filename); // !!! Какое-то оформление надо
    }

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
    vectorPushBack(resLines, lines);
    resLines.emplace_back(lstEnd);
    
    return resLines;
}

//----------------------------------------------------------------------------
//! Возвращает true, если всё хорошо и исходную строку не надо вставлять
inline
bool insertSnippet( const AppConfig          &appCfg
                  , std::vector<std::string> &resLines
                  , const std::string        &insertCommandLine
                  , const std::string        &curFilename
                  , const std::string        &snippetFile
                  , const std::string        &snippetTag
                  , const std::unordered_set<SnippetOptions>      &snippetFlagsOptions
                  , const std::unordered_map<SnippetOptions, int> &intOptions
                  )
{
    // bool fTrimLeading = testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::trimLeft);
    bool fTrimLeft              = testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::trimLeft)      ;
    bool fTrimArround           = testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::trimArround)   ;
    bool fAddLineNumbers        = testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::lineNo)        ;
    bool fAddFilename           = testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::filename)      ;
    bool fAddFilenameOnly       = !testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::path)         ;
    bool fAddFilenameLineNumber = testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::filenameLineNo);


    std::string foundFullFilename;
    std::string foundFileText;
    auto findRes = appCfg.findSamplesFile(snippetFile, foundFullFilename, foundFileText /* , curFilename */ );
    if (!findRes)
    {
        bool noFail = !testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::fail);
        // если noFail, возвращаем true, что не включит оригинальную строку в результат для сигнализации автору об ошибке
        return noFail;
    }

    std::vector<std::string> snippetsFileLines = marty_cpp::splitToLinesSimple(foundFileText);
    std::vector<std::string> insertLines; insertLines.reserve(snippetsFileLines.size());
    std::size_t firstLineIdx = 0;
    
    std::string lang = appCfg.getLangByFilename(foundFullFilename);

    if (snippetTag.empty()) // Вставляем файл целиком
    {
        std::vector<std::string>
        listingLines = prepareSnippetLines( appCfg, snippetsFileLines
                                          , snippetFile, 0u // firstLineIdx
                                          , fTrimLeft
                                          , fTrimArround
                                          , fAddLineNumbers
                                          , fAddFilename
                                          , fAddFilenameOnly
                                          , fAddFilenameLineNumber
                                          );
        vectorPushBack(resLines, listingLines); // вставляем листинг целиком, prepareSnippetLines уже всё оформлекние сделал
        return true; // всё хорошо, не включит исходную строку
    }

    std::size_t startLineNo = 0;
    std::size_t endLineNo   = 0;
    if (isCodeTagLinesRange(snippetTag, startLineNo, endLineNo))
    {
        std::vector<std::string> insertSnippetLines;
        firstLineIdx = startLineNo;
        for(std::size_t idx=startLineNo; idx!=endLineNo; ++idx)
        {
            if (idx>=snippetsFileLines.size())
                break;
            insertSnippetLines.emplace_back(snippetsFileLines[idx]);
        }

        std::vector<std::string>
        listingLines = prepareSnippetLines( appCfg, insertSnippetLines
                                          , snippetFile, firstLineIdx
                                          , fTrimLeft
                                          , fTrimArround
                                          , fAddLineNumbers
                                          , fAddFilename
                                          , fAddFilenameOnly
                                          , fAddFilenameLineNumber
                                          );
        vectorPushBack(resLines, listingLines); // вставляем листинг целиком, prepareSnippetLines уже всё оформлекние сделал
        return true; // всё хорошо, не включит исходную строку
    }

    std::string snippetTagPrefix;
    if (!lang.empty())
    {
        snippetTagPrefix = appCfg.getLangCutPrefix(lang);
    }

    if (snippetTagPrefix.empty()) // Не знаем, как искать тэг - нет информации по тому, какой префикс используется для тэгов сниппетов в данном языке
    {
        return false; // Поэтому просто ошибка, исходная строка будет включена
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

    std::vector<std::string> insertSnippetLines = extractCodeFragment(snippetsFileLines, firstLineIdx, snippetTag, snippetTagPrefix, listingNestedTagsMode, 4u /* tabSize */ );

    std::vector<std::string>
    listingLines = prepareSnippetLines( appCfg, insertSnippetLines
                                      , snippetFile, firstLineIdx
                                      , fTrimLeft
                                      , fTrimArround
                                      , fAddLineNumbers
                                      , fAddFilename
                                      , fAddFilenameOnly
                                      , fAddFilenameLineNumber
                                      );
    vectorPushBack(resLines, listingLines); // вставляем листинг целиком, prepareSnippetLines уже всё оформлекние сделал
    return true; // всё хорошо, не включит исходную строку
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> processMdFileLines(const AppConfig &appCfg, const std::vector<std::string> &lines, const std::string &curFilename, const std::unordered_set<std::string> &alreadyIncludedDocs )
{

    auto handler = [&](LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
    {
        if (event!=LineHandlerEvent::insertCommand)
        {
            return true;
        }
 
        auto snippetFlagsOptions = appCfg.snippetOptions;
        std::unordered_map<SnippetOptions, int> intOptions;
        std::string snippetFile;
        std::string snippetTag ;
        SnippetOptionsParsingResult parseRes = parseSnippetInsertionCommandLine( snippetFlagsOptions, intOptions, appCfg.conditionVars
                                                                               , line, snippetFile, snippetTag
                                                                               );
        if (parseRes==SnippetOptionsParsingResult::okButCondition)
            return false; // prevent insertion

        if (parseRes==SnippetOptionsParsingResult::fail)
            return true; // insert source line when fail

        // SnippetOptionsParsingResult::ok

        if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::snippetOptions))
        {
            resLines.emplace_back(serializeSnippetOptions(snippetFlagsOptions, intOptions));
        }

        if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::doc))
        {
            // если фейл, и insertDoc возвращает false, то возвращаем true для вставки текущей строки, пусть автор документа разбирается,
            // в чем он накосячил, увидев такой выхлоп в виде заголовка с '!'
            // Если fail-опция не установлена, то не выводим ничего
            // По умолчанию в конфигах .options - установлена
            // Но если мы хотим тихо ничего не делать при обломе поиска подключаемого файла, то надо явно указать no-fail
            return !insertDoc( appCfg, resLines, line // insertCommandLine
                             , curFilename
                             , snippetFile // docFile
                             , snippetFlagsOptions
                             , intOptions
                             , alreadyIncludedDocs
                             );
        }
        else
        {
            return !insertSnippet( appCfg, resLines, line // insertCommandLine
                                 , curFilename
                                 , snippetFile
                                 , snippetTag
                                 , snippetFlagsOptions
                                 , intOptions
                                 );
        }

    };
 
    return processLines(appCfg, lines, handler);
}

//----------------------------------------------------------------------------
inline
std::string processMdFile(const AppConfig &appCfg, std::string fileText, const std::string &curFilename)
{
    //fileText = marty_cpp::normalizeCrLfToLf(fileText);
    std::vector<std::string> lines = marty_cpp::splitToLinesSimple(fileText);

    auto resLines = processMdFileLines(appCfg, lines, curFilename, std::unordered_set<std::string>()/*alreadyIncludedDocs*/);

    if (appCfg.testProcessingOption(ProcessingOptions::numericSections))
    {
        resLines = generateSecionNumbers(appCfg, resLines);
    }

    bool generateSecIds = false;
    if ((appCfg.targetRenderer==TargetRenderer::doxygen && (appCfg.testProcessingOption(ProcessingOptions::generateToc) || appCfg.testProcessingOption(ProcessingOptions::generateSectionId) ) ) )
    {
        // Генерировать идентификаторы секций нужно, если у нас целевой рендерер - доксиген, и явно задано генерировать ID секций, или генерировать "Содержание"
        // Для генерации "Содержания" для гитхаба ID секций генерировать не нужно
        generateSecIds = true;
    }

    // Тут надо сгенерировать "Содержание"
    if (appCfg.testProcessingOption(ProcessingOptions::generateToc))
    {
        // Пока просто в начало пихаем
        std::vector<std::string> tocLines = generateTocLines(appCfg, resLines);
        std::vector<std::string> tmpLines = tocLines;
        tmpLines.insert(tmpLines.end(), resLines.begin(), resLines.end());
        std::swap(tmpLines, resLines);
    }

    if (generateSecIds)
    {
        resLines = generateSectionIds(appCfg, resLines);
    }

    // std::unordered_set<ProcessingOptions>                 processingOptions;
    // TargetRenderer                                        targetRenderer = TargetRenderer::github;

    return marty_cpp::mergeLines(resLines, appCfg.outputLinefeed, true  /* addTrailingNewLine */ );
}



// SnippetOptionsParsingResult parseSnippetInsertionCommandLine( std::unordered_set<SnippetOptions>       &snippetFlagsOptions
//                                                             , std::unordered_map<SnippetOptions, int>  &snippetIntOptions
//                                                             , const std::unordered_map<std::string, std::string> &condVars
//                                                             , std::string line, std::string &snippetFile, std::string &snippetTag
//                                                             )
