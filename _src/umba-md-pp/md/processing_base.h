#pragma once

#include "base_includes.h"


namespace umba {
namespace md {


//----------------------------------------------------------------------------
//! LineHandler: bool handler(LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
template<typename LineHandler, typename FilenameStringType> inline
std::vector<std::string> processLines(const AppConfig<FilenameStringType> &appCfg, Document &doc, const FilenameStringType &curFilename, const std::vector<std::string> &lines, LineHandler handler)
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
            MdPpTag foundMdPpTagType = MdPpTag::invalid;
            auto it = umba::md::tryParseLineToHtmlTag(mdHtmlTag, line.begin(), line.end(), foundMdPpTagType);
            if (foundMdPpTagType!=MdPpTag::invalid && !mdHtmlTag.isCloseTag())
            {
                tagLineExtraParse(appCfg, mdHtmlTag, foundMdPpTagType, it, line.end());

                std::vector<std::string> tagLines;

                bool externFile = false;

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
                        externFile = true;
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
                        if (isSingleLineComment(line))
                            continue;
                        // auto it2 = 
                        umba::md::tryParseLineToHtmlTag(mdHtmlTagEnd, line.begin(), line.end(), foundEndTagType);
                        if (foundMdPpTagType==foundEndTagType && mdHtmlTagEnd.isCloseTag())
                            break;
                        tagLines.emplace_back(lines[idx]);
                    }

                    if (idx==lines.size())
                        --idx;
                }

                if (externFile && foundMdPpTagType!=MdPpTag::invalid)
                {
                    // Тут надо выцепить мету из строк внешнего файла 
                    auto commentPrefix = getTagLinesCommentStart(foundMdPpTagType);
                    if (!commentPrefix.empty())
                    {
                        auto metaLines = extractMetaLinesFromDocument(tagLines, commentPrefix);
                        if (!metaLines.empty())
                        {
                            std::string metadataText = marty_cpp::mergeLines(metaLines, marty_cpp::ELinefeedType::lf, true  /* addTrailingNewLine */ );
                            doc.collectedMetadataTexts.emplace_back(metadataText);
                        }
                    }
                }

                tagLinesProcess(appCfg, doc, mdHtmlTag, foundMdPpTagType, curFilename, tagLines, resLines);

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
                    //resLines.emplace_back(std::string());
                    resLines.emplace_back(line);
                    //resLines.emplace_back(std::string());
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

            if (isMetaCommand(line))
            {
                if (handler(LineHandlerEvent::metaCommand, resLines, line, idx, lastLineIdx))
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




} // namespace md
} // namespace umba

