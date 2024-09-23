#pragma once

#include "app_config.h"
#include "document.h"
//
#include "marty_cpp/src_normalization.h"

#include "umba/transliteration.h"
#include "umba/id_gen.h"
#include "umba/container_utility.h"
#include "umba/parse_utils.h"
#include "umba/null_inserter.h"
#include "umba/macros.h"
#include "umba/macro_helpers.h"
//
#include "marty_yaml_toml_json/json_utils.h"
#include "marty_yaml_toml_json/yaml_json.h"
#include "marty_yaml_toml_json/yaml_utils.h"

//
#include "umba_md_processing_utils.h"
//
#include "processing_code_utils.h"
//
#include "processing_section_utils.h"

#include "processing_utils.h"
//
#include "yaml-cpp/yaml.h"
//
#include "enum_hash.h"
//
#include "md_pp_html_csv.h"
#include "md_pp_html_graph.h"
//
#include <stack>
#include <string>
#include <utility>
#include <sstream>
#include <utility>
#include <algorithm>
#include <iomanip>
#include <unordered_map>
#include <functional>


//
#include "yaml_json_document_helpers.h"
#include "md-pp-macro_helpers.h"
//
#include "config.h"


//----------------------------------------------------------------------------
template<typename FilenameStringType>
using TagLineExtraParser = std::function<std::string::const_iterator (const AppConfig<FilenameStringType>&, umba::html::HtmlTag&, MdPpTag, std::string::const_iterator, std::string::const_iterator)>;

template<typename FilenameStringType>
using TagLineExtraParsersMap = std::unordered_map<MdPpTag, TagLineExtraParser<FilenameStringType>, EnumClassHash >;

template<typename FilenameStringType>
TagLineExtraParsersMap<FilenameStringType> makeTagLineExtraParsersMap()
{
    //TODO: !!! Набить парсеры для graph/csv
    TagLineExtraParsersMap<FilenameStringType> m;

    m[MdPpTag::graph] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parsePossibleFilenameAndTextToGraphTag(mdHtmlTag, b, e);
                        };

    return m;
}

template<typename FilenameStringType>
const TagLineExtraParsersMap<FilenameStringType>& getTagLineExtraParsersMap()
{
    static auto m = makeTagLineExtraParsersMap<FilenameStringType>();
    return m;
}

template<typename FilenameStringType>
std::string::const_iterator tagLineExtraParse(const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
{
    auto &m = const_cast< TagLineExtraParsersMap<FilenameStringType>& >(getTagLineExtraParsersMap<FilenameStringType>());
    auto it = m.find(tagType);
    if (it==m.end())
        return e;

    return it->second(appCfg, mdHtmlTag, tagType, b, e);
}

//----------------------------------------------------------------------------
template<typename FilenameStringType>
using TagLinesProcessor = std::function<void (const AppConfig<FilenameStringType>&, umba::html::HtmlTag&, MdPpTag, const FilenameStringType&, const std::vector<std::string>&, std::vector<std::string>&)>;

template<typename FilenameStringType>
using TagLinesProcessorsMap = std::unordered_map<MdPpTag, TagLinesProcessor<FilenameStringType>, EnumClassHash >;

template<typename FilenameStringType>
TagLinesProcessorsMap<FilenameStringType> makeTagLinesProcessorsMap()
{
    //TODO: !!! Набить процессоры для graph/csv
    TagLinesProcessorsMap<FilenameStringType> m;

    m[MdPpTag::graph] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processGraphLines(appCfg, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };
    return m;
}

template<typename FilenameStringType>
const TagLinesProcessorsMap<FilenameStringType>& getTagLinesProcessorsMap()
{
    static auto m = makeTagLinesProcessorsMap<FilenameStringType>();
    return m;
}

template<typename FilenameStringType>
void tagLinesProcess(const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
{
    auto &m = const_cast< TagLinesProcessorsMap<FilenameStringType>& >(getTagLinesProcessorsMap<FilenameStringType>());
    auto it = m.find(tagType);
    if (it==m.end())
        return;

    it->second(appCfg, mdHtmlTag, tagType, docFilename, tagLines, resLines);
}


//----------------------------------------------------------------------------


//MdPpTag foundTagType = MdPpTag::invalid;


//----------------------------------------------------------------------------
template<typename FilenameStringType>
std::string processMdFile(const AppConfig<FilenameStringType> &appCfg, std::string fileText, const FilenameStringType &curFilename, Document &resDoc);

//----------------------------------------------------------------------------
//! LineHandler: bool handler(LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
template<typename LineHandler, typename FilenameStringType> inline
std::vector<std::string> processLines(const AppConfig<FilenameStringType> &appCfg, const FilenameStringType &curFilename, const std::vector<std::string> &lines, LineHandler handler)
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


        else if (state==PreprocessorParsingState::comment) // comment mode
        {
            if (isMultiLineCommentEnd(line))
                state = PreprocessorParsingState::normal; // Больше ничего не делаем, строка всё ещё от комента
        }


        else if (state==PreprocessorParsingState::normal) // normal mode
        {
            if (isSingleLineComment(line))
            {
                // Пропускаем коменты
                continue;
            }

            if (isMultiLineCommentStart(line))
            {
                // Пропускаем многострочные коменты
                state = PreprocessorParsingState::comment;
                continue;
            }

            umba::html::HtmlTag mdHtmlTag;
            MdPpTag foundTagType = MdPpTag::invalid;
            auto it = umba::md::tryParseLineToHtmlTag(mdHtmlTag, line.begin(), line.end(), foundTagType);
            if (foundTagType!=MdPpTag::invalid && !mdHtmlTag.isCloseTag())
            {
                tagLineExtraParse(appCfg, mdHtmlTag, foundTagType, it, line.end());

                std::vector<std::string> tagLines;

                if (mdHtmlTag.hasAttr("file"))
                {
                    // read file here
                    std::string foundFullFilename;
                    std::string foundFileText;
                    auto findRes = appCfg.findDocFileByIncludedFromFilename(mdHtmlTag.getAttrValue("file"), foundFullFilename, foundFileText, curFilename);
                    if (!findRes) // document not found
                    {
                    }
                    else
                    {
                        tagLines = marty_cpp::splitToLinesSimple(foundFileText);
                    }
                }
                else
                {
                    // collect lines here
                    for(++idx; idx!=lines.size(); ++idx)
                    {
                        umba::html::HtmlTag mdHtmlTagEnd;
                        MdPpTag foundEndTagType = MdPpTag::invalid;
                        line = lines[idx];
                        auto it2 = umba::md::tryParseLineToHtmlTag(mdHtmlTagEnd, line.begin(), line.end(), foundEndTagType);
                        if (foundTagType==foundEndTagType && mdHtmlTagEnd.isCloseTag())
                            break;
                        tagLines.emplace_back(lines[idx]);
                    }

                    if (idx==lines.size())
                        --idx;
                }

                tagLinesProcess(appCfg, mdHtmlTag, foundTagType, curFilename, tagLines, resLines);

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

    std::string finalLine;
    handler(LineHandlerEvent::documentEnd, resLines, finalLine, 0 /* idx */ , lastLineIdx);

    return resLines;
}

//----------------------------------------------------------------------------
// std::vector<std::string> collectMetadataLines(const AppConfig &appCfg, const std::vector<std::string> &lines, std::vector<std::string> &metadataLines)
// {
//     auto handler = [&](LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
//     {
//         if (event==LineHandlerEvent::metaStart || event==LineHandlerEvent::metaEnd)
//         {
//             return false; // prevent to add this line to result lines
//         }
//         else if (event==LineHandlerEvent::metaLine)
//         {
//             metadataLines.emplace_back(line);
//             return false; // prevent to add this line to result lines
//         }
//         else
//         {
//             return true; // allow line to be added to output
//         }
//     };
//
//     return processLines(appCfg, lines, handler);
// }
//
// //----------------------------------------------------------------------------
// std::vector<std::string> collectMetadataToText(const AppConfig &appCfg, const std::vector<std::string> &lines, std::string &metadataText)
// {
//     std::vector<std::string> metadataLines;
//     std::vector<std::string> resLines = collectMetadataLines(appCfg, lines, metadataLines);
//
//     metadataText = marty_cpp::mergeLines(resLines, marty_cpp::ELinefeedType::lf, true  /* addTrailingNewLine */ );
//
//     return resLines;
// }

//----------------------------------------------------------------------------
template<typename HeaderLineHandler, typename FilenameStringType> inline
std::vector<std::string> processHeaderLines(const AppConfig<FilenameStringType> &appCfg, const FilenameStringType &curFilename, const std::vector<std::string> &lines, HeaderLineHandler headerHandler)
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

    return processLines(appCfg, curFilename, lines, handler);

}

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::vector<std::string> raiseHeaders(const AppConfig<FilenameStringType> &appCfg, const FilenameStringType &curFilename, const std::vector<std::string> &lines, int raiseVal)
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

    return processHeaderLines(appCfg, curFilename, lines, raiseHeader);
}

//----------------------------------------------------------------------------
#if 0
inline
std::vector<std::string> generateSectionIds(const AppConfig &appCfg, const std::vector<std::string> &lines)
{
    std::unordered_map<std::string, std::size_t> usedIds;

    auto processSectionHeader = [&](std::string &line) -> bool
    {
        std::size_t headerLevel = 0;
        std::string headerText;

        std::string id = generateSectionId(appCfg, line, &headerLevel, &headerText);
        if (headerLevel==0 || id.empty())
            return true;

        ++usedIds[id];

        if (usedIds[id]>1)
        {
            auto n = usedIds[id];
            id.append(1,'-');
            id.append(std::to_string(n-1));
        }

        line = std::string(headerLevel, '#') + std::string(1u,' ') + headerText + std::string(" {#") + id + std::string("}");

        return true;
    };

    return processHeaderLines(appCfg, lines, processSectionHeader);
}
#endif
//----------------------------------------------------------------------------
    // resLines = generateSecionsExtra( appCfg, resLines, docTo
                                   // , true // update doc info
                                   // , true // update title
    //                                , appCfg.testProcessingOption(ProcessingOptions::numericSections) // нужно или нет реально генерить номера секций
    //                                );

    // std::vector<std::string> tocLines;

//----------------------------------------------------------------------------
const std::size_t maxSectionLevelsTotal = 64;

//----------------------------------------------------------------------------
inline
std::string generateSectionNumberImpl(std::size_t lvl, const int *sectionCounters)
{
    std::string resStr;

    if (lvl>maxSectionLevelsTotal)
        lvl = maxSectionLevelsTotal;

    for(std::size_t i=0; i!=lvl; ++i)
    {
        // resStr += std::to_string(sectionCounters[i] /* +1 */ );
        // resStr += ".";
        if (!resStr.empty())
            resStr += ".";
        resStr += std::to_string(sectionCounters[i] /* +1 */ );
    }

    return resStr;
}

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::vector<std::string> generateSecionsExtra( const AppConfig<FilenameStringType> &appCfg
                                             , const FilenameStringType            &curFilename
                                             , const std::vector<std::string>      &lines
                                             , Document                            &docTo
                                             , bool                                updateDocInfo
                                             , bool                                updateHeader
                                             , bool                                numerateSections
                                             , bool                                idWithSecNumber
                                             , bool                                addIdToHeader
                                             )
{

    int sectionCounters[maxSectionLevelsTotal] = { 0 }; // Не более maxSectionLevelsTotal уровней секций
    std::unordered_map<std::string, std::size_t> usedIds;

    auto generateSectionNumber = [&](std::size_t lvl) { return generateSectionNumberImpl(lvl, &sectionCounters[0]); };

    auto processSectionNumber = [&](std::string &line) -> bool
    {

        std::string levelStr;
        std::string headerText;

        if (!splitHeaderLine(line, levelStr, headerText))
            return true;

        if (levelStr.empty())
            return true;

        //std::size_t headerLevel = levelStr.size();

        std::size_t curSectionLevel = levelStr.size();
        if (!curSectionLevel || curSectionLevel>=maxSectionLevelsTotal)
            return true;

        ++sectionCounters[curSectionLevel-1];

        umba::string_plus::trim(headerText);

        SectionInfo secInfo;
        secInfo.sectionLevel    = curSectionLevel;
        secInfo.originalTitle   = headerText     ;
        secInfo.fullTitle       = headerText     ;
        if (numerateSections)
        {
            if (appCfg.numSecMaxLevel==0 || curSectionLevel<=(std::size_t)appCfg.numSecMaxLevel)
            {
                secInfo.sectionNumber   = generateSectionNumber(curSectionLevel);
                secInfo.fullTitle       = secInfo.sectionNumber + std::string(1u,' ') + headerText;
            }
        }

        //idWithSecNumber
        //std::string id = generateSectionId(appCfg, line, &headerLevel, &headerText);
        std::string id = generateSectionIdImpl(appCfg, idWithSecNumber ? secInfo.fullTitle : secInfo.originalTitle);

        ++usedIds[id];

        if (usedIds[id]>1)
        {
            auto n = usedIds[id];
            id.append(1,'-');
            id.append(std::to_string(n-1));
        }

        secInfo.sectionTargetId = id;

        std::string newTitleStr = levelStr + std::string(1u,' ') + secInfo.fullTitle;

        if (addIdToHeader)
        {
            newTitleStr += std::string(" {#") + secInfo.sectionTargetId + std::string("}");
        }

        if (updateHeader)
        {
            line = newTitleStr;
        }

        if (updateDocInfo)
        {
            if (appCfg.tocMaxLevel==0 || curSectionLevel<=(std::size_t)appCfg.tocMaxLevel)
            {
                std::string tocLine = std::string(curSectionLevel*2u /* +2u */ , ' ');
                tocLine.append("- ");
                tocLine.append("[");
                tocLine.append(secInfo.fullTitle);
                tocLine.append("](#");
                tocLine.append(secInfo.sectionTargetId);
                tocLine.append(")");

                docTo.tocLines.emplace_back(tocLine);
            }

            docTo.sectionInfos[secInfo.originalTitle].emplace_back(secInfo);
        }


        // line = levelStr + std::string(1u,' ') + generateSectionNumber(curSectionLevel) + std::string(1u,' ') + headerText;
        for( /* ++curSectionLevel */ ; curSectionLevel!=maxSectionLevelsTotal; ++curSectionLevel)
        {
            sectionCounters[curSectionLevel] = 0;
        }

        return true;
    };

    return processHeaderLines(appCfg, curFilename, lines, processSectionNumber);
}



//----------------------------------------------------------------------------
#if 0
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
            id.append(std::to_string(n-1));
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
#endif


// //! LineHandler: bool handler(LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
// template<typename LineHandler, typename FilenameStringType> inline
// std::vector<std::string> processLines(const AppConfig<FilenameStringType> &appCfg, const std::vector<std::string> &lines, LineHandler handler)
// {

//----------------------------------------------------------------------------
inline
void checkPrintLineIfContainsPngExt(LineHandlerEvent event, const std::string &line)
{
    UMBA_USED(event);
    UMBA_USED(line);

    #if defined(LOG_PROCESSING_PROCESS_TEXT_LINES_SIMPLE_PRINT_PNG_LINES)
    std::string::size_type pos = line.find(".png");
    if (pos!=line.npos)
    {
        std::cout << "Found PNG in line: " << line << "; line type: " << enum_serialize(event) << "\n";
    }
    #endif
}




//----------------------------------------------------------------------------
// For substitutions|data collection on text lines
template<typename LineHandler, typename FilenameStringType> inline
std::vector<std::string> processTextLinesSimple(const AppConfig<FilenameStringType> &appCfg, const FilenameStringType &curFilename, const std::vector<std::string> &lines, LineHandler simpleHandler)
{
    auto processLinesHandler = [&](LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
    {
        switch(event)
        {
            case LineHandlerEvent::normalLine:    checkPrintLineIfContainsPngExt(event, line);
                                                  line = simpleHandler(line);
                                                  return true;

            case LineHandlerEvent::documentEnd:   checkPrintLineIfContainsPngExt(event, line);
                                                  return true;
            case LineHandlerEvent::listingLine:   checkPrintLineIfContainsPngExt(event, line);
                                                  return true;
            case LineHandlerEvent::listingStart:  checkPrintLineIfContainsPngExt(event, line);
                                                  return true;
            case LineHandlerEvent::listingEnd:    checkPrintLineIfContainsPngExt(event, line);
                                                  return true;
            case LineHandlerEvent::insertCommand: checkPrintLineIfContainsPngExt(event, line);
                                                  return true;
            case LineHandlerEvent::tocCommand:    checkPrintLineIfContainsPngExt(event, line);
                                                  return true;
            case LineHandlerEvent::headerCommand: checkPrintLineIfContainsPngExt(event, line);
                                                  return true;
            case LineHandlerEvent::metaLine:      checkPrintLineIfContainsPngExt(event, line);
                                                  return true;
            case LineHandlerEvent::metaStart:     checkPrintLineIfContainsPngExt(event, line);
                                                  return true;
            case LineHandlerEvent::metaEnd:       checkPrintLineIfContainsPngExt(event, line);
                                                  return true;
            //case LineHandlerEvent:::
        }
        return true;
    };

    return processLines(appCfg, curFilename, lines, processLinesHandler);
}

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::vector<std::string> extractImageLinks( const AppConfig<FilenameStringType>  &appCfg
                                         , Document                              &docTo
                                         , const std::vector<std::string>        &lines
                                         , const std::string                     &curFilename         // currently processed file
                                         )
{
    #if defined(LOG_PROCESSING_PROCESS_TEXT_LINES_SIMPLE_PRINT_PNG_LINES)
    std::cout << "--- extractImageLinks\n";
    #endif

    //TODO: !!! Надо бы ещё принудительно флэттенизировать урлы, которые выходят за пределы каталога с документом
    const bool flattenImageLinks = appConfig.getEffectiveFlattenImageLinksOption(); // flattenImageLinks && appConfig.copyImageFiles;
    //UMBA_USED(flattenImageLinks);

    auto urlHandler = [&](std::string url, bool bImage)
    {
        if (bImage && !umba::md::isUrlAbsolute(url) && !umba::md::isUrlAbsoluteHostPath(url))
        {
            #if defined(LOG_PROCESSING_EXTRACT_IMAGE_LINKS)
            std::cout << "Extracted link: " << url << "\n";
            #endif
            //docTo.imageFiles.insert( umba::filename::makeCanonicalForCompare(url, '/', std::string("."), std::string(".."), true  /* keepLeadingParents */ ) );
            auto srcUrl = umba::filename::makeCanonical(url, '/', std::string("."), std::string(".."), true  /* keepLeadingParents */ );

            if (flattenImageLinks)
                url = umba::filename::flattenPath(srcUrl);

            docTo.imageFiles[srcUrl] = url;
        }

        return url;
    };

    return
    processTextLinesSimple( appCfg, curFilename, lines
                          , [&](const std::string &line)
                            {
                                //TODO: !!! Надо бы сделать какой-то null_insert_iterator и null_inserter

                                #if defined(LOG_PROCESSING_PROCESS_TEXT_LINES_SIMPLE_PRINT_PNG_LINES)
                                std::string::size_type pos = line.find(".png");
                                if (pos!=line.npos)
                                {
                                    std::cout << ""; // "Found PNG in line: " << line << "; line type: " << enum_serialize(event) << "\n";
                                }
                                #endif

                                if (flattenImageLinks)
                                {
                                    std::string res;
                                    umba::md::transformMarkdownLinksUrlString(std::back_inserter(res), line.begin(), line.end(), urlHandler);
                                    return res;
                                }
                                else
                                {
                                    umba::md::transformMarkdownLinksUrlString(umba::null_inserter(), line.begin(), line.end(), urlHandler);
                                    return line;
                                }
                            }
                          );
}

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::vector<std::string> stripLocalLinksExtentions( const AppConfig<FilenameStringType>           &appCfg
                                                  , Document                                      &docTo
                                                  , const std::vector<std::string>                &lines
                                                  , const std::string                             &curFilename         // currently processed file
                                                  )
{
    if (!appConfig.stripExtentions)
        return lines;

    const bool stripExtentions = appConfig.stripExtentions;
    //UMBA_USED(flattenImageLinks);

    auto urlHandler = [&](std::string url, bool bImage)
    {
        if (bImage || umba::md::isUrlAbsolute(url))
            return url;

        std::string urlPath;
        std::string urlTag ;
        umba::md::splitUrlToPathAndTag(url, urlPath, urlTag);

        if (stripExtentions)
        {
            urlPath = umba::filename::getPathFile(urlPath);
        }

        return umba::md::mergeUrlFromPathAndTag(urlPath, urlTag);
    };

    return
    processTextLinesSimple( appCfg, curFilename, lines
                          , [&](const std::string &line)
                            {
                                std::string res;
                                umba::md::transformMarkdownLinksUrlString(std::back_inserter(res), line.begin(), line.end(), urlHandler);
                                return res;
                            }
                          );
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct CoutPrinter
{
    std::string    indent;

    std::string operator()(const std::string &str) const
    {
        std::cout << indent << str << "\n";

        return str;
    }

}; // struct CoutPrinter

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
struct UmbaMdLinksUrlCoutPrinter
{

    std::string    &filePathIncludedFrom;
    std::string    &filePathIncluded    ;
    std::string    indent;

    std::string operator()(const std::string &line) const
    {
        std::string res; res.reserve(line.size());

        auto urlHandler = [&](std::string url, bool bImage)
        {
            std::string includedFullname = url;
            std::string urlOrg = url;

            std::cout << indent << (bImage?"!":" ") << url;

            if (!umba::md::isUrlAbsolute(url) && !umba::md::isUrlAbsoluteHostPath(url))
            {

                // // Тут надо процессить имя файла - оно относительное
                // includedFullname = umba::filename::makeCanonical(umba::filename::appendPath(filePathIncluded, url));
                //
                // std::string newRelName = url;
                //
                // if (umba::filename::isSubPathName(filePathIncludedFrom, includedFullname, &newRelName))
                // {
                //     newRelName = umba::filename::makeCanonical(newRelName, '/', std::string("."), std::string(".."), true /* keepLeadingParents */ );
                // }
                // else if (umba::filename::isSubPathName(filePathIncluded, filePathIncludedFrom, &newRelName))
                // {
                //     newRelName = umba::filename::normalizePathSeparators(newRelName, '/');
                //     std::vector<std::string> parts = umba::string_plus::split(newRelName, '/', true /* skipEmpty */ );
                //
                //     //newRelName = umba::string_plus::merge(std::vector<std::string>(parts.size(), ".."), '/');
                //     newRelName = umba::filename::appendPath(umba::string_plus::merge(std::vector<std::string>(parts.size(), ".."), '/'), umba::filename::getFileName(url));
                // }
                //
                // std::swap(newRelName, url);

                //std::string relName = url;
                // результат можно не проверять, мы заранее присвоили

    // StringType makeCanonical( StringType fileName
    //                         , typename StringType::value_type pathSep  = umba::filename::getNativePathSep<typename StringType::value_type>()
    //                         , const StringType &currentDirAlias        = umba::filename::getNativeCurrentDirAlias<StringType>()
    //                         , const StringType &parentDirAlias         = umba::filename::getNativeParentDirAlias<StringType>()
    //                         , bool keepLeadingParents                  = false
    //                         );

                umba::filename::makeRelPath( url
                                           , filePathIncludedFrom
                                           , umba::filename::appendPath( filePathIncluded, url, '/')
                                           , '/'
                                           , std::string(".")
                                           , std::string("..")
                                           , true // keepLeadingParents
                                           , true // tryReverseRelPath
                                           );
            }

            if (includedFullname!=url)
            {
                std::cout << " (" << includedFullname << ")";
            }

            // std::cout << "\n";
            //std::cout << indent << "New link/img filename: " << url << "\n";
            std::cout << ", New link/img filename: " << url << "\n";

            return urlOrg;
        };

        //std::cout << indent << str << "\n";

        umba::md::transformMarkdownLinksUrlString(std::back_inserter(res), line.begin(), line.end(), urlHandler);
        return res;

    }

}; // struct UmbaMdLinksUrlCoutPrinter


// inline
// void testTransformMarkdownLinksUrlString(const std::string &input)
// {
//     std::string res; res.reserve(input.size());
//
//     auto handler = [](std::string url)
//     {
//         std::cout << "URL: " << url << "\n";
//         if (isUrlAbsolute(url))
//         {
//         }
//         else if (isUrlAbsoluteHostPath(url))
//         {
//         }
//         else // path reletive to current document
//         {
//             url = "../../" + url; // Для теста добавляем два уровня вверх
//         }
//
//         return makeUrlPathCanonical(url);
//     };
//
//
//     transformMarkdownLinksUrlString(std::back_inserter(res), input.begin(), input.end(), handler);
//
//     //std::cout << "testTransformMarkdownText:";
//     std::cout << "In : " << input << "\n";
//     std::cout << "Out: " << res << "\n";
//     std::cout << "\n";
// }





//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::vector<std::string> parseMarkdownFileLines( const AppConfig<FilenameStringType> &appCfg
                                               , Document &docTo
                                               , const std::vector<std::string> &lines
                                               , const FilenameStringType &curFilename
                                               , const std::unordered_set<FilenameStringType> &alreadyIncludedDocs
                                               );

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
bool insertDoc( const AppConfig<FilenameStringType>           &appCfg
              , Document                                      &docTo
              , std::vector<std::string>                      &resLines
              , const std::string                             &insertCommandLine
              , const std::string                             &curFilename         // currently processed file
              , const std::string                             &docFile             // file for insertion
              , const std::unordered_set<SnippetOptions>      &snippetFlagsOptions
              , const std::unordered_map<SnippetOptions, int> &intOptions
              , const std::unordered_set<std::string>         &alreadyIncludedDocs
              )
{
    std::string foundFullFilename;
    std::string foundFileText;
    auto findRes = appCfg.findDocFileByIncludedFromFilename(docFile, foundFullFilename, foundFileText, curFilename);
    if (!findRes) // document not found
    {
        if (umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::fail))
        {
            makeShureEmptyLine(resLines);
            resLines.emplace_back("!!! File not found in: " + appCfg.getSamplesPathsAsMergedString(umba::string_plus::make_string<FilenameStringType>(", ")));
            return false; // сфейлили
        }

        return true; // делаем вид, что всё хорошо
    }

    // файл может включаться под разными именами, через всякие .. или ., слэши опять же разные
    std::string foundFullFilenameCanonical = umba::filename::makeCanonical( foundFullFilename, '/' );

    if (alreadyIncludedDocs.find(foundFullFilenameCanonical)!=alreadyIncludedDocs.end())
    {
        return true; // делаем вид, что всё хорошо, а на самом деле предотвратили рекурсивное подключение одного и того же файла
    }

    if (appCfg.checkIsInsertRestricted(foundFullFilename))
    {
        makeShureEmptyLine(resLines);
        resLines.emplace_back("!!! Doc insertion is restricted");
        return false; // сфейли
    }
    //bool checkIsInsertRestricted(const std::string &fName) const

    std::vector<std::string> docLines = marty_cpp::splitToLinesSimple(foundFileText);
    std::vector<std::string> processedDocLines = parseMarkdownFileLines( appCfg, docTo, docLines, foundFullFilename
                                                                       , umba::updatedSet(alreadyIncludedDocs
                                                                       , foundFullFilenameCanonical, true /* bAddKey */ ) /* alreadyIncludedDocsCopy */
                                                                       );



    std::unordered_map<SnippetOptions, int>::const_iterator raiseOptIt = intOptions.find(SnippetOptions::raise);
    if (raiseOptIt!=intOptions.end() && raiseOptIt->second!=0)
    {
        processedDocLines = raiseHeaders(appCfg, curFilename, processedDocLines, raiseOptIt->second);
    }

    auto curFilePath      = umba::filename::getPath(curFilename);
    auto includedFilePath = umba::filename::getPath(foundFullFilename);

#if defined(LOG_DOC_INSERTIONS_AND_REFS)

    std::cout << "\n";
    std::cout << "Document: " << curFilename << ", inserting: " << docFile << ", found: " << foundFullFilename << "\n";
    std::cout << "Current file path : " << curFilePath << "\n";
    std::cout << "Included file path: " << includedFilePath << "\n";

    processedDocLines = processTextLinesSimple(appCfg, processedDocLines, UmbaMdLinksUrlCoutPrinter{curFilePath, includedFilePath, "    "});
#endif


    auto urlRebaseHandler = [&](std::string url, bool bImage)
    {
        std::string urlPath;
        std::string urlTag ;
        umba::md::splitUrlToPathAndTag(url, urlPath, urlTag);


        if (!umba::md::isUrlAbsolute(urlPath) && !umba::md::isUrlAbsoluteHostPath(urlPath))
        {
            // если URL ссылки относительный (относительно включаемого файла),
            // то его надо переделать на относительный относительно того файла, куда включаем

            umba::filename::makeRelPath( urlPath
                                       , curFilePath
                                       , umba::filename::appendPath( includedFilePath, url, '/')
                                       , '/'
                                       , std::string(".")
                                       , std::string("..")
                                       , true // keepLeadingParents
                                       , true // tryReverseRelPath
                                       );
        }

        // Замена поддерживаемых input расширений на целевые (для любых ссылок, даже для абсолютных)
        if (!bImage)
        {
            std::string ext = umba::filename::getExt(urlPath);
            if (appCfg.isSupportedSourceExtention(ext))
            {
                // if (appConfig.stripExtentions)
                // {
                //     urlPath = umba::filename::getPathFile(urlPath);
                // }
                // else
                {
                    std::string targetExt = appConfig.getTargetFileExtention(ext);
                    urlPath               = umba::filename::appendExt(umba::filename::getPathFile(urlPath), targetExt);
                }
            }
        }

        return umba::md::mergeUrlFromPathAndTag(urlPath, urlTag);
    };

    #if defined(LOG_PROCESSING_TRANSFORM_BEFORE_AFTER)
    std::cout << "--- Transform\n";
    #endif

    processedDocLines = processTextLinesSimple( appCfg, curFilename, processedDocLines
                                              , [&](const std::string &line)
                                                {
                                                    #if defined(LOG_PROCESSING_TRANSFORM_BEFORE_AFTER)
                                                    std::cout << "Transform, before: " << line << "\n"; // << std::flush;
                                                    #endif

                                                    std::string res;
                                                    umba::md::transformMarkdownLinksUrlString(std::back_inserter(res), line.begin(), line.end(), urlRebaseHandler);

                                                    #if defined(LOG_PROCESSING_TRANSFORM_BEFORE_AFTER)
                                                    std::cout << "Transform, after : " << res << "\n";
                                                    #endif

                                                    return res;
                                                }
                                              );

    makeShureEmptyLine(resLines);
    //resLines.insert(resLines.end(), processedDocLines.begin(), processedDocLines.end());
    umba::vectorPushBack(resLines, processedDocLines);

    return true; // всё хорошо

}

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::vector<std::string> prepareSnippetLines( const AppConfig<FilenameStringType>            &appCfg
                                            , std::vector<std::string>   lines
                                            , std::string                snippetFilename
                                            , std::size_t                firstLineIdx
                                            , bool                       bTrimLeft
                                            , bool                       trimArround
                                            , bool                       addLineNumbers
                                            , bool                       addFilename
                                            , bool                       addFilenameOnly
                                            , bool                       addFilenameLineNumber
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
    umba::vectorPushBack(resLines, lines);
    resLines.emplace_back(lstEnd);

    return resLines;
}

//----------------------------------------------------------------------------
//! Возвращает true, если всё хорошо и исходную строку не надо вставлять
template<typename FilenameStringType> inline
bool insertSnippet( const AppConfig<FilenameStringType>          &appCfg
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
    bool fTrimLeft              =  umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::trimLeft)      ;
    bool fTrimArround           =  umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::trimArround)   ;
    bool fAddLineNumbers        =  umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::lineNo)        ;
    bool fAddFilename           =  umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::filename)      ;
    bool fAddFilenameOnly       = !umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::path)         ;
    bool fAddFilenameLineNumber =  umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::filenameLineNo);

    bool noFail = !umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::fail);

    std::string foundFullFilename;
    std::string foundFileText;
    auto findRes = appCfg.findSamplesFile(snippetFile, foundFullFilename, foundFileText /* , curFilename */ );
    if (!findRes)
    {
        // если noFail, возвращаем true, что не включит оригинальную строку в результат для сигнализации автору об ошибке
        if (!noFail)
        {
            makeShureEmptyLine(resLines);
            //resLines.emplace_back("!!! File not found");
            resLines.emplace_back("!!! File not found in: " + appCfg.getSamplesPathsAsMergedString(umba::string_plus::make_string<FilenameStringType>(", ")));
        }
        return noFail;
    }

    // Сниппеты не рестриктим, они и так по явно заданным папкам только ищутся
    // if (appCfg.checkIsInsertRestricted(fName))
    // {
    //     makeShureEmptyLine(resLines);
    //     resLines.emplace_back("!!! Insertion is restricted");
    //     return false; // сфейли
    // }


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
        makeShureEmptyLine(resLines);
        umba::vectorPushBack(resLines, listingLines); // вставляем листинг целиком, prepareSnippetLines уже всё оформлекние сделал
        return true; // всё хорошо, не включит исходную строку
    }


    umba::md::SnippetTagInfo snippetTagInfo = umba::md::parseSnippetTag(snippetTag);
    if (!snippetTagInfo.isValid())
    {
        resLines.emplace_back("!!! Invalid tag string: '" + snippetTag + "'");
        return false;
    }

    ListingNestedTagsMode listingNestedTagsMode = ListingNestedTagsMode::remove;

    if (umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::keepCutTags))
    {
        listingNestedTagsMode = ListingNestedTagsMode::keep;
    }
    else if (umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::lineNo))
    {
        listingNestedTagsMode = ListingNestedTagsMode::emptyLine;
    }

    //std::vector<std::string> snippetLines = extractCodeFragmentBySnippetTag(appCfg.languageOptionsDatabase.getLanguageOptions(lang), lang, snippetsFileLines, firstLineIdx, snippetTag, listingNestedTagsMode, 0, 4u /* tabSize */ );
    std::vector<std::string> snippetLines = umba::md::extractCodeFragmentBySnippetTagInfo( appCfg.languageOptionsDatabase.getLanguageOptions(lang)
                                                                                         , lang
                                                                                         , snippetsFileLines
                                                                                         , snippetTagInfo
                                                                                         , firstLineIdx
                                                                                         , listingNestedTagsMode
                                                                                         , 4u // tabSize
                                                                                         );
    // Если snippetLines пуст и firstLineIdx==-1, то это ошибка
    std::vector<std::string>
    listingLines = prepareSnippetLines( appCfg, snippetLines
                                      , snippetFile, firstLineIdx
                                      , fTrimLeft
                                      , fTrimArround
                                      , fAddLineNumbers
                                      , fAddFilename
                                      , fAddFilenameOnly
                                      , fAddFilenameLineNumber
                                      );
    makeShureEmptyLine(resLines);
    umba::vectorPushBack(resLines, listingLines); // вставляем листинг целиком, prepareSnippetLines уже всё оформление сделал
    return true; // всё хорошо, не включит исходную строку
}

//----------------------------------------------------------------------------

// std::vector<std::string> collectMetadataLines(const AppConfig &appCfg, const std::vector<std::string> &lines, std::vector<std::string> &metadataLines)
// {
//     auto handler = [&](LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
//     {
//         if (event==LineHandlerEvent::metaStart || event==LineHandlerEvent::metaEnd)
//         {
//             return false; // prevent to add this line to result lines
//         }
//         else if (event==LineHandlerEvent::metaLine)
//         {
//             metadataLines.emplace_back(line);
//             return false; // prevent to add this line to result lines
//         }
//         else
//         {
//             return true; // allow line to be added to output
//         }
//     };
//
//     return processLines(appCfg, lines, handler);
// }
//
// //----------------------------------------------------------------------------
// std::vector<std::string> collectMetadataToText(const AppConfig &appCfg, const std::vector<std::string> &lines, std::string &metadataText)
// {
//     std::vector<std::string> metadataLines;
//     std::vector<std::string> resLines = collectMetadataLines(appCfg, lines, metadataLines);
//
//     metadataText = marty_cpp::mergeLines(resLines, marty_cpp::ELinefeedType::lf, true  /* addTrailingNewLine */ );
//
//     return resLines;
// }

template<typename FilenameStringType> inline
std::vector<std::string> parseMarkdownFileLines( const AppConfig<FilenameStringType> &appCfg
                                               , Document                            &docTo
                                               , const std::vector<std::string>      &lines
                                               , const FilenameStringType            &curFilename
                                               , const std::unordered_set<FilenameStringType> &alreadyIncludedDocs
                                               )
{
    std::vector<std::string> metadataLines;

    auto handler = [&](LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
    {
        if (event==LineHandlerEvent::documentEnd)
        {
            if (!metadataLines.empty())
            {
                std::string metadataText = marty_cpp::mergeLines(metadataLines, marty_cpp::ELinefeedType::lf, true  /* addTrailingNewLine */ );
                docTo.collectedMetadataTexts.emplace_back(metadataText);
                metadataLines.clear();
            }
        }

        else if (event==LineHandlerEvent::metaStart)
        {
            return false; // prevent to add this line to result lines
        }
        else if (event==LineHandlerEvent::metaEnd)
        {
            if (!metadataLines.empty())
            {
                std::string metadataText = marty_cpp::mergeLines(metadataLines, marty_cpp::ELinefeedType::lf, true  /* addTrailingNewLine */ );
                docTo.collectedMetadataTexts.emplace_back(metadataText);
                metadataLines.clear();
            }
            return false; // prevent to add this line to result lines
        }
        else if (event==LineHandlerEvent::metaLine)
        {
            metadataLines.emplace_back(line);
            return false; // prevent to add this line to result lines
        }

        if (event==LineHandlerEvent::headerCommand)
        {
            std::string levelStr;
            std::string headerText;

            if (!splitHeaderLine(line, levelStr, headerText))
                return true;

            if (levelStr.empty())
                return true;

            if (!docTo.titleFromText.empty())
                return true;

            docTo.titleFromText = headerText;

            return true;
        }

        if (event==LineHandlerEvent::normalLine)
        {
            if (docTo.titleFromText.empty())
            {
                std::string tmpStr = line;
                umba::string_plus::trim(tmpStr);
                if (tmpStr.size()>10)
                    docTo.titleFromText = tmpStr;
            }

        }


        if (event!=LineHandlerEvent::insertCommand)
        {
            return true;
        }

        auto snippetFlagsOptions = appCfg.snippetOptions;
        std::unordered_map<SnippetOptions, int> intOptions;
        std::string snippetFile;
        std::string snippetTag ;
        SnippetOptionsParsingResult parseRes = umba::md::parseSnippetInsertionCommandLine( snippetFlagsOptions, intOptions, appCfg.conditionVars
                                                                                         , line, snippetFile, snippetTag
                                                                                         );
        if (parseRes==SnippetOptionsParsingResult::okButCondition)
            return false; // prevent insertion

        if (parseRes==SnippetOptionsParsingResult::fail)
        {
            makeShureEmptyLine(resLines);
            resLines.emplace_back("!!! Options parsing error");
            makeShureEmptyLine(resLines);
            resLines.emplace_back(umba::md::serializeSnippetOptions(snippetFlagsOptions, intOptions));
            return true; // insert source line when fail
        }
        // SnippetOptionsParsingResult::ok

        if (umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::snippetOptions))
        {
            makeShureEmptyLine(resLines);
            resLines.emplace_back(umba::md::serializeSnippetOptions(snippetFlagsOptions, intOptions));
        }

        //snippetFile
        {
            using namespace umba::macros;
            snippetFile = substMacros( snippetFile
                                     , umba::md::MacroTextFromMapOrEnvRef(appCfg.conditionVars, true /* envAllowed */ )
                                     , smf_KeepUnknownVars // | smf_uppercaseNames // !!! Надо заморачиваться с регистром? Если надо, то тогда при добавлении всё в upper case и кондишены надо подправить
                                     );
        }

        if (umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::doc))
        {
            // если фейл, и insertDoc возвращает false, то возвращаем true для вставки текущей строки, пусть автор документа разбирается,
            // в чем он накосячил, увидев такой выхлоп в виде заголовка с '!'
            // Если fail-опция не установлена, то не выводим ничего
            // По умолчанию в конфигах .options - установлена
            // Но если мы хотим тихо ничего не делать при обломе поиска подключаемого файла, то надо явно указать no-fail
            return !insertDoc( appCfg, docTo, resLines, line // insertCommandLine
                             , curFilename
                             , snippetFile // file to insert
                             , snippetFlagsOptions
                             , intOptions
                             , alreadyIncludedDocs
                             );
        }
        else
        {
            return !insertSnippet( appCfg, resLines, line // insertCommandLine
                                 , curFilename
                                 , snippetFile            // file from wich code fragment will be cutted
                                 , snippetTag
                                 , snippetFlagsOptions
                                 , intOptions
                                 );
        }

    };

    return processLines(appCfg, curFilename, lines, handler);
}

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
//
// }

// LineHandlerEvent::tocCommand

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::vector<std::string> processTocCommands(const AppConfig<FilenameStringType> &appCfg, const FilenameStringType &curFilename, Document &doc, const std::vector<std::string> &lines, bool &tocAdded)
{
    std::size_t numTocFound = 0;

    tocAdded = false;

    auto handler = [&](LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
    {
        if (event!=LineHandlerEvent::tocCommand)
        {
            return true;
        }

        if (numTocFound==0)
        {
            if (appCfg.testProcessingOption(ProcessingOptions::generateToc))
            {
                makeShureEmptyLine(resLines);
                umba::vectorPushBack(resLines, doc.tocLines);
                makeShureEmptyLine(resLines);
                tocAdded = true;
            }
        }

        ++numTocFound;

        return false;
    };

    return processLines(appCfg, curFilename, lines, handler);
}

//----------------------------------------------------------------------------
// Ищем закрывающий символ с учетом возможной вложенности, предполагается, что первый открывающий уже пройден
inline
std::string::size_type findPairedChar(const std::string &line, std::string::size_type pos, const char chOpen, const char chClose)
{
    if (pos>=line.size())
        return line.npos;

    std::size_t openCount = 1;

    for(; pos!=line.size(); ++pos)
    {
        char ch = line[pos];

        if (ch==chOpen)
        {
            ++openCount;
            continue;
        }

        if (ch==chClose)
        {
            --openCount;
            if (openCount==0)
                return pos;
        }
    }

    return line.npos;
}

//----------------------------------------------------------------------------
// https://www.markdownguide.org/basic-syntax/#escaping-characters
//----------------------------------------------------------------------------
// Tables
// https://www.markdownguide.org/hacks/#table-formatting
// https://docs.github.com/en/get-started/writing-on-github/working-with-advanced-formatting/organizing-information-with-tables

//----------------------------------------------------------------------------
// enum class TransformMarkdownNormalTextEvent
// {
//
// };

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::string updateInDocRefs(const AppConfig<FilenameStringType> &appCfg, const FilenameStringType &curFilename, Document &doc, const std::string &line)
{
    // Идея по ссылкам на разделы. Указываем в них просто текст заголовка - [Какой-то текст](#Просто полный текст заголовка)
    // Также для каждого заголовка можно задать частично или полностью квалифицированную ссылку. Потом распарсим текст на предмет
    // ссылок и сделаем замену.
    //
    // В теле текста линка можно задать просто [$] - тогда будет вставлен текст заголовка, если задать [#] - тогда будет вставлен
    // только номер заголовка, если есть нумерация, или его текст, если нумерации нет, [#$] - номер и текст заголовка, если есть нумерация,
    // или только текст, если нумерации нет.

    #if defined(LOG_PROCESSING_UPDINDOCREFS_BEFORE_AFTER)
    std::cout << "UpdInDocRefs before: " << line << "\n";
    #endif

    std::string resLine; resLine.reserve(line.size());

    bool prevIsImageMark = false;

    std::string::size_type pos = 0;
    for(; pos!=line.size(); )
    {
        char ch = line[pos];

        if (ch!='[' && !prevIsImageMark)
        {
            if (ch=='!')
               prevIsImageMark = true;
            else
               prevIsImageMark = false;
            resLine.append(1,ch);
            ++pos;
            continue;
        }

        prevIsImageMark = false;

        auto closePos = findPairedChar(line, pos+1, '[', ']');
        if (closePos==line.npos) // ничего не нашли, надо тупо остаток строки вместе с текущим символом '[' перекинуть в результат и вернуть его
        {
            resLine.append(line, pos, line.npos);
            return resLine;
        }

        std::string linkText = std::string(line, pos+1, closePos-pos-1);

        pos = closePos+1;
        if (pos==line.size() || line[pos]!='(')
        {
            resLine.append(1,'[');
            resLine.append(linkText);
            resLine.append(1,']');
            if (pos==line.size())
                return resLine;
            else
                continue;
        }


        auto roundClosePos = findPairedChar(line, pos+1, '(', ')');
        if (roundClosePos==line.npos)
        {
            resLine.append(1,'[');
            resLine.append(linkText);
            resLine.append(1,']');
            resLine.append(line, pos, line.npos);
            return resLine;
        }

        if ((pos+1)==line.npos || line[pos+1]!='#') // ссылка не локальная, значит, не наш вариант. Специально отделил от предыдущего if'а
        {
            resLine.append(1,'[');
            resLine.append(linkText);
            resLine.append(1,']');
            resLine.append(line, pos, line.npos);
            return resLine;
        }

        // std::string linkRef = std::string(line, pos+1, roundClosePos-pos-1); // не убирать, тут с решеткой, которую надо всё равно удалять
        std::string linkRef = std::string(line, pos+2, roundClosePos-pos-2); // тут мы копируем без решетки

        pos = roundClosePos+1;

        // process link here
        //resLine.append("[]()");

        std::string linkTextTrimmed = linkText;
        umba::string_plus::trim(linkTextTrimmed);

        std::string linkRefTrimmed = linkRef;
        umba::string_plus::trim(linkRefTrimmed);
        // umba::string_plus::ltrim(linkRefTrimmed, [](char ch) { return ch=='#'; } ); // убираем решетки слева


        SectionInfo secInfo;
        if (!doc.findSectionInfo(linkRefTrimmed, secInfo))
        {
            resLine.append(1,'[');
            resLine.append(linkText);
            resLine.append(1,']');
            resLine.append(1,'(');
            resLine.append(linkRef);
            resLine.append(1,')');
        }

        if (linkTextTrimmed.empty() || linkTextTrimmed=="$")
        {
            linkTextTrimmed = secInfo.originalTitle;
        }
        else if (linkTextTrimmed=="#")
        {
            if (secInfo.sectionNumber.empty())
                linkTextTrimmed = secInfo.originalTitle;
            else
                linkTextTrimmed = secInfo.sectionNumber;
        }
        else if (linkTextTrimmed=="#$")
        {
            linkTextTrimmed = secInfo.fullTitle;
        }

        resLine.append(1,'[');
        resLine.append(linkTextTrimmed);
        resLine.append(1,']');
        resLine.append(1,'(');
        resLine.append(1,'#');
        resLine.append(secInfo.sectionTargetId);
        resLine.append(1,')');

    }

    #if defined(LOG_PROCESSING_UPDINDOCREFS_BEFORE_AFTER)
    std::cout << "UpdInDocRefs after: " << resLine << "\n";
    #endif

    return resLine;
}

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::vector<std::string> updateInDocRefs(const AppConfig<FilenameStringType> &appCfg, const FilenameStringType &curFilename, Document &doc, const std::vector<std::string> &lines)
{
    auto handler = [&](LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
    {
        if (event!=LineHandlerEvent::normalLine)
        {
            return true;
        }

        line = updateInDocRefs(appCfg, curFilename, doc, line);
        return true;
    };

    return processLines(appCfg, curFilename, lines, handler);
}

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::string processMdFile(const AppConfig<FilenameStringType> &appCfg, std::string fileText, const FilenameStringType &curFilename, Document &resDoc)
{
    //fileText = marty_cpp::normalizeCrLfToLf(fileText);
    std::vector<std::string> lines = marty_cpp::splitToLinesSimple(fileText);

    Document doc;
    std::vector<std::string> resLines = parseMarkdownFileLines(appCfg, doc, lines, curFilename, std::unordered_set<FilenameStringType>()/*alreadyIncludedDocs*/);

    // Генерировать идентификаторы секций нужно, если у нас целевой рендерер - доксиген, и явно задано генерировать ID секций, или генерировать "Содержание"
    // Для генерации "Содержания" для гитхаба ID секций генерировать не нужно

    // Или, если целевой рендерер доксиген - нам всегда нужны идентификатры секций, чтобы на них ссылаться.
    bool generateSecIds = false;
    if ( appCfg.targetRenderer==TargetRenderer::doxygen
      // && ( appCfg.testProcessingOption(ProcessingOptions::generateToc)
      //   || appCfg.testProcessingOption(ProcessingOptions::generateSectionId)
      //    )
       )
    {
        generateSecIds = true;
    }

    //bool needSpecialIdInSectionHeader = appCfg.targetRenderer==TargetRenderer::doxygen;
    bool generateIdWithSectionNumber  = appCfg.targetRenderer==TargetRenderer::github
                                     || appCfg.targetRenderer==TargetRenderer::gitlab
                                      ;

    resLines = generateSecionsExtra( appCfg, curFilename, resLines, doc
                                   , true // update doc info
                                   , true // update header
                                   , appCfg.testProcessingOption(ProcessingOptions::numericSections) // нужно или нет реально генерить номера секций
                                   , generateIdWithSectionNumber
                                   , generateSecIds // needSpecialIdInSectionHeader
                                   );

    resLines = updateInDocRefs(appCfg, curFilename, doc, resLines);

    resLines = extractImageLinks(appCfg, doc, resLines, curFilename );
    resLines = stripLocalLinksExtentions(appCfg, doc, resLines, curFilename );


    bool tocAdded = false;
    resLines = processTocCommands(appCfg, curFilename, doc, resLines, tocAdded);

    if (!tocAdded && appCfg.testProcessingOption(ProcessingOptions::generateToc))
    {
        std::vector<std::string> tmpLines = doc.tocLines;
        //umba::vectorPushBack(tmpLines, doc.tocLines);
        makeShureEmptyLine(tmpLines);
        umba::vectorPushBack(tmpLines, resLines);
        std::swap(tmpLines, resLines);
    }

    parseDocumentMetadata(appCfg, doc);

    std::vector<std::string> metaDescriptionLines = doc.getMetaTagValueAsTextLines(appCfg, "description", std::string() /* listDelimiter */);

    std::string metaTitle = doc.getDocumentTitleFromMeta();
    if (appCfg.testProcessingOption(ProcessingOptions::title) && !metaTitle.empty())
    {
        std::vector<std::string> tmpLines;
        if (appCfg.testProcessingOption(ProcessingOptions::metaData))
        {
            tmpLines.emplace_back(std::string());
        }

        tmpLines.emplace_back(std::string("# ") + metaTitle);
        tmpLines.emplace_back(std::string());
        if (!metaDescriptionLines.empty())
        {
            umba::vectorPushBack(tmpLines, metaDescriptionLines);
            tmpLines.emplace_back(std::string());
        }

        umba::vectorPushBack(tmpLines, resLines);
        std::swap(tmpLines, resLines);
    }


    if (appCfg.testProcessingOption(ProcessingOptions::metaData))
    {
        // std::cout << "Write metadata\n";
        auto metadataText  = generateDocMetadata(appCfg, doc);
        auto metadataLines = marty_cpp::splitToLinesSimple(metadataText);
        std::vector<std::string> tmpLines;
        tmpLines.emplace_back(std::string("---"));
        umba::vectorPushBack(tmpLines, metadataLines);
        tmpLines.emplace_back(std::string("---"));
        umba::vectorPushBack(tmpLines, resLines);
        std::swap(tmpLines, resLines);
    }
    else
    {
        //std::cout << "Write metadata NOT turned ON\n";
    }
    //std::string generateDocMetadata(const AppConfig &appCfg, Document &doc)

    resDoc = doc;
    resDoc.lines = resLines;

    return marty_cpp::mergeLines(resLines, appCfg.outputLinefeed, true  /* addTrailingNewLine */ );
}


