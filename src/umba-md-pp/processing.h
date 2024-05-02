#pragma once

#include "app_config.h"
#include "document.h"
//
#include "marty_cpp/src_normalization.h"

#include "umba/transliteration.h"
#include "umba/id_gen.h"
#include "umba/container_utility.h"

#include "marty_yaml_toml_json/json_utils.h"
#include "marty_yaml_toml_json/yaml_json.h"
#include "marty_yaml_toml_json/yaml_utils.h"

//
#include <yaml-cpp/yaml.h>
//
#include <stack>
#include <string>
#include <utility>
#include <sstream>

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
inline
std::string generateDocMetadata(const AppConfig &appCfg, Document &doc)
{
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;

    // emitter << YAML::Key << "var1" << YAML::Value << "value1";

    std::vector<std::string>::const_iterator mtIt = appCfg.metaTagsSerializeList.begin();
    for(; mtIt!=appCfg.metaTagsSerializeList.end(); ++mtIt)
    {
        std::string tagSerializedName = appCfg.serializeMetaTag(*mtIt);
        if (tagSerializedName.empty())
            continue;

        std::unordered_map<std::string, std::vector<std::string> >::const_iterator tit = doc.tagsData.find(appCfg.makeCanonicalMetaTag(*mtIt));
        if (tit==doc.tagsData.end())
            continue;

        if (tit->second.empty())
            continue;

        const std::vector<std::string> &tagData = tit->second;

        // Имя тэга не пустое, вектор со значениями также не пуст, надо что-то выдать

        emitter << YAML::Key << tagSerializedName;

        MetaTagType metaTagType = appCfg.getMetaTagType(*mtIt);

        if (metaTagType==MetaTagType::textFirst) /* Simple text, allowed multiple definitions, but only first value is applied */
        {
            emitter << YAML::Value << tagData.front();
        }
        else if (metaTagType==MetaTagType::textReplace) /* Simple text, allowed multiple definitions, but only last value is applied */
        {
            emitter << YAML::Value << tagData.back();
        }
        else if (metaTagType==MetaTagType::textMerge) /* Text fragments will be merged to paras */
        {
            auto text = umba::string_plus::merge< std::string, std::vector<std::string>::const_iterator >( tagData.begin(), tagData.end(), std::string("\n\n") );
            emitter << YAML::Value << text;
        }
        else if (metaTagType==MetaTagType::list || metaTagType==MetaTagType::commaList)
        {
            emitter << YAML::BeginSeq;
            for(auto tv : tagData)
            {
                emitter << tv;
            }
            emitter << YAML::EndSeq;
        }
        else if (metaTagType==MetaTagType::set || metaTagType==MetaTagType::commaSet)
        {
            std::set<std::string> s;
            for(auto tv : tagData)
            {
                s.insert(tv);
            }

            emitter << YAML::BeginSeq;
            for(auto sv : s)
            {
                emitter << sv;
            }
            emitter << YAML::EndSeq;

        }
        else
        {
            emitter << std::string();
        }

        //    emitter << YAML::EndSeq;

    }

// enum EMITTER_MANIP {
//   // general manipulators
//   Auto,
//   TagByKind,
//   Newline,
//  
//   // output character set
//   EmitNonAscii,
//   EscapeNonAscii,
//   EscapeAsJson,
//  
//   // string manipulators
//   // Auto, // duplicate
//   SingleQuoted,
//   DoubleQuoted,
//   Literal,
//  
//   // null manipulators
//   LowerNull,
//   UpperNull,
//   CamelNull,
//   TildeNull,
//  
//   // bool manipulators
//   YesNoBool,      // yes, no
//   TrueFalseBool,  // true, false
//   OnOffBool,      // on, off
//   UpperCase,      // TRUE, N
//   LowerCase,      // f, yes
//   CamelCase,      // No, Off
//   LongBool,       // yes, On
//   ShortBool,      // y, t
//  
//   // int manipulators
//   Dec,
//   Hex,
//   Oct,
//  
//   // document manipulators
//   BeginDoc,
//   EndDoc,
//  
//   // sequence manipulators
//   BeginSeq,
//   EndSeq,
//   Flow,
//   Block,
//  
//   // map manipulators
//   BeginMap,
//   EndMap,
//   Key,
//   Value,
//   // Flow, // duplicate
//   // Block, // duplicate
//   // Auto, // duplicate
//   LongKey
// };

    emitter << YAML::EndMap;

    // std::ofstream ofout(file);
    // ofout << emitter.c_str();

    std::ostringstream oss;

    oss << emitter.c_str();

    return oss.str();

}

//----------------------------------------------------------------------------

template<typename JsonNodeType>
bool getJsonNodeTypeValueAsString(const JsonNodeType &j, std::string &resVal)
{
    nlohmann::detail::value_t valType = j.type();
    if (valType==nlohmann::detail::value_t::string)
    {
        // https://github.com/nlohmann/json/issues/3827
        // https://en.cppreference.com/w/cpp/language/dependent_name#template_disambiguator
        resVal = j.template get<std::string>();
        return true;
    }
    else if (valType==nlohmann::detail::value_t::boolean)
    {
        auto bv = j.template get<bool>();
        resVal = bv ? "true" : "false";
        return true;
    }
    else if (valType==nlohmann::detail::value_t::number_integer)
    {
        resVal = std::to_string(j.template get<int>());
        return true;
    }
    else if (valType==nlohmann::detail::value_t::number_unsigned)
    {
        resVal = std::to_string(j.template get<unsigned>());
        return true;
    }
    else if (valType==nlohmann::detail::value_t::number_float)
    {
        resVal = std::to_string(j.template get<float>());
        return true;
    }
    else
    {
        return false;
    }

}

// enum class value_t : std::uint8_t
// {
//     null,             ///< null value
//     object,           ///< object (unordered set of name/value pairs)
//     array,            ///< array (ordered collection of values)
//     string,           ///< string value
//     boolean,          ///< boolean value
//     number_integer,   ///< number value (signed integer)
//     number_unsigned,  ///< number value (unsigned integer)
//     number_float,     ///< number value (floating-point)
//     binary,           ///< binary array (ordered collection of bytes)
//     discarded         ///< discarded by the parser callback function
// };



//----------------------------------------------------------------------------
inline
void parseDocumentMetadata(const AppConfig &appCfg, Document &doc)
{
    for(const auto &metaText : doc.collectedMetadataTexts)
    {
        std::string errMsg;
        std::string tmpJson;
        marty::json_utils::FileFormat detectedFormat = marty::json_utils::FileFormat::unknown;
        nlohmann::json j = marty::json_utils::parseJsonOrYaml( metaText, true /* allowComments */ , &errMsg, &tmpJson, &detectedFormat );
         
        if (detectedFormat==marty::json_utils::FileFormat::unknown)
            continue;

        nlohmann::detail::value_t jType = j.type();
        if (jType!=nlohmann::detail::value_t::object)
            continue;


        for (auto el : j.items())
        {
            try
            {
	            std::string strKey = el.key();
                strKey = appCfg.makeCanonicalMetaTag(strKey);
                std::vector<std::string> &tagValsVec = doc.tagsData[strKey];

                MetaTagType tagType = appCfg.getMetaTagType(strKey);


                auto val = el.value();
                //    std::string valStr = val;
                // //std::string strVal = el.value();
                //    LOG_MSG_OPT << "parseDocumentMetadata: " << strKey << ": " << valStr << "\n";

                nlohmann::detail::value_t valType = val.type();

                if ( valType==nlohmann::detail::value_t::object
                  || valType==nlohmann::detail::value_t::binary
                  || valType==nlohmann::detail::value_t::discarded
                   )
                    continue;

                if (valType==nlohmann::detail::value_t::array)
                {
                    if (tagType!=MetaTagType::list && tagType!=MetaTagType::set)
                        continue;

                    for (auto vel : val.items())
                    {
                        auto jVel = vel.value();
                        std::string strVal;
                        if (!getJsonNodeTypeValueAsString(jVel, strVal))
                            continue;

                        tagValsVec.emplace_back(strVal);
                    }

                    continue;
                }

                std::string strVal;
                if (!getJsonNodeTypeValueAsString(val, strVal))
                    continue;

                if (tagType==MetaTagType::commaList || tagType==MetaTagType::commaSet)
                {
                    umba::vectorPushBack(tagValsVec, marty_cpp::splitToLinesSimple(strVal, false, ','));
                }
                else
                {
                    tagValsVec.emplace_back(strVal);
                }

            }
            catch(...)
            {
                LOG_MSG_OPT << "parseDocumentMetadata exception\n";
            }

        }
    
    }


}
    // std::string errMsg;
    // std::string tmpJson;
    // marty::json_utils::FileFormat detectedFormat = marty::json_utils::FileFormat::unknown;
    // nlohmann::json j = marty::json_utils::parseJsonOrYaml( in, true /* allowComments */ , &errMsg, &tmpJson, &detectedFormat );
    //  
    // if (detectedFormat==marty::json_utils::FileFormat::unknown)
    // {
    //     std::cerr << testInputFileName << ": error: " << errMsg << std::endl;
    //     if (!tmpJson.empty())
    //     {
    //         std::cerr << "JSON:" << std::endl;
    //         std::cerr << tmpJson << std::endl;
    //     }
    //     
    //     return 1;
    // }

//----------------------------------------------------------------------------
inline
bool isInsertCommand(std::string line)
{
    umba::string_plus::trim(line);
    return (umba::string_plus::starts_with(line, ("#!insert")) || umba::string_plus::starts_with(line, ("#$insert")));
}

//----------------------------------------------------------------------------
inline
bool isTocCommand(std::string line)
{
    umba::string_plus::trim(line);
    return (umba::string_plus::starts_with(line, ("#!toc")) || umba::string_plus::starts_with(line, ("#$toc")));
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
bool isMultiLineCommentStart(std::string line)
{
    umba::string_plus::trim(line);
    return umba::string_plus::starts_with(line, ("#/*"));
}

//----------------------------------------------------------------------------
inline
bool isMultiLineCommentEnd(std::string line)
{
    umba::string_plus::trim(line);
    return umba::string_plus::ends_with(line, ("#*/"));
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
void makeShureEmptyLine(std::vector<std::string> &lines)
{
    if (lines.empty())
    {
        lines.emplace_back(std::string());
    }
    else
    {
        std::string lastLine = lines.back();
        umba::string_plus::trim(lastLine);
        if (!lastLine.empty())
        {
            lines.emplace_back(std::string());
        }
    }
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
bool isSectionNumberChar(char ch)
{
    if (ch>='0' && ch<='9')
        return true;

    if (ch=='.')
        return true;

    return false;
}

//----------------------------------------------------------------------------
inline
bool isSectionNumberStringHelper(const std::string &str)
{
    for(auto ch: str)
    {
        if (isSectionNumberChar(ch))
           continue;

        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
inline
bool isSectionNumber(const std::string &str, bool allowAppendixStyleNumbers=false)
{
    if (str.empty())
       return false;

    // Первый символ может быть латинской буквой
    if (allowAppendixStyleNumbers)
    {
        if (str[0]>='A' && str[0]<='Z' && isSectionNumberStringHelper(std::string(str, 1, std::string::npos)))
            return true;
    }

    if (isSectionNumberStringHelper(str))
        return true;

    return false;
}

//----------------------------------------------------------------------------
inline
bool isAppendixSectionNumber(const std::string &str)
{
    if (str.empty())
       return false;

    if (str[0]>='A' && str[0]<='Z')
        return true;

    return false;
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
        // Для доксигена зачем-то делаю исключение и чекаю-удаляю номер раздела
        // auto spacePos = secText.find(' ');
        // if (spacePos!=secText.npos)
        // {
        //     if (isSectionNumber(std::string(secText, 0, spacePos)))
        //     {
        //         secText.erase(0, spacePos+1);
        //     }
        // }
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

    // Доксигеновская тема, не завязываемся на неё
    // if (headerText.back()=='}') // already has id?
    //     return std::string();

    // if (appCfg.targetRenderer!=TargetRenderer::doxygen)
    //     return std::string(); // Нужно только для доксигена

    if (pLevel)
    {
        *pLevel = levelStr.size();
    }

    // Не делаем никаких исключений для 

    // if (headerText.back()==']' && appCfg.targetRenderer==TargetRenderer::doxygen)
    // {
    //     // У нас есть идентификаторы в квадратных скобках, по ним мы генерим якоря (только doxygen)
    //     std::size_t idx = headerText.size();
    //     for(; idx!=0 && headerText[idx-1]!='['; --idx) {}
    //  
    //     if (idx==0)
    //     {
    //         return generateSectionIdImpl(appCfg, headerText);
    //     }
    //  
    //     std::string takenId = std::string(headerText, idx, headerText.size()-idx-1);
    //     return generateSectionIdImpl(appCfg, takenId);
    // }
    // else
    {
        return generateSectionIdImpl(appCfg, headerText);
    }

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
inline
std::vector<std::string> generateSecionsExtra( const AppConfig                &appCfg
                                             , const std::vector<std::string> &lines
                                             , Document                       &docTo
                                             , bool                           updateDocInfo
                                             , bool                           updateHeader
                                             , bool                           numerateSections
                                             , bool                           idWithSecNumber
                                             , bool                           addIdToHeader
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

    return processHeaderLines(appCfg, lines, processSectionNumber);
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

//----------------------------------------------------------------------------
std::vector<std::string> parseMarkdownFileLines(const AppConfig &appCfg, Document &docTo, const std::vector<std::string> &lines, const std::string &curFilename, const std::unordered_set<std::string> &alreadyIncludedDocs);

//----------------------------------------------------------------------------
inline
bool insertDoc( const AppConfig          &appCfg
              , Document                 &docTo
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
            makeShureEmptyLine(resLines);
            resLines.emplace_back("!!! File not found");
            return false; // сфейли
        }
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
    
    //TODO: !!! extract meta info here

    std::unordered_map<SnippetOptions, int>::const_iterator raiseOptIt = intOptions.find(SnippetOptions::raise);
    if (raiseOptIt!=intOptions.end() && raiseOptIt->second!=0)
    {
        processedDocLines = raiseHeaders(appCfg, processedDocLines, raiseOptIt->second);
    }
    
    makeShureEmptyLine(resLines);
    //resLines.insert(resLines.end(), processedDocLines.begin(), processedDocLines.end());
    umba::vectorPushBack(resLines, processedDocLines);
    
    return true; // всё хорошо
    
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> prepareSnippetLines( const AppConfig            &appCfg
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
        if (!noFail)
        {
            makeShureEmptyLine(resLines);
            resLines.emplace_back("!!! File not found");
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
        makeShureEmptyLine(resLines);
        umba::vectorPushBack(resLines, listingLines); // вставляем листинг целиком, prepareSnippetLines уже всё оформлекние сделал
        return true; // всё хорошо, не включит исходную строку
    }

    std::string snippetTagPrefix;
    if (!lang.empty())
    {
        snippetTagPrefix = appCfg.getLangCutPrefix(lang);
    }

    if (snippetTagPrefix.empty()) // Не знаем, как искать тэг - нет информации по тому, какой префикс используется для тэгов сниппетов в данном языке
    {
        makeShureEmptyLine(resLines);
        resLines.emplace_back("!!! Unknown language, can't looking for tag");
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
    makeShureEmptyLine(resLines);
    umba::vectorPushBack(resLines, listingLines); // вставляем листинг целиком, prepareSnippetLines уже всё оформлекние сделал
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

inline
std::vector<std::string> parseMarkdownFileLines(const AppConfig &appCfg, Document &docTo, const std::vector<std::string> &lines, const std::string &curFilename, const std::unordered_set<std::string> &alreadyIncludedDocs )
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
        {
            makeShureEmptyLine(resLines);
            resLines.emplace_back("!!! Options parsing error");
            makeShureEmptyLine(resLines);
            resLines.emplace_back(serializeSnippetOptions(snippetFlagsOptions, intOptions));
            return true; // insert source line when fail
        }
        // SnippetOptionsParsingResult::ok

        if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::snippetOptions))
        {
            makeShureEmptyLine(resLines);
            resLines.emplace_back(serializeSnippetOptions(snippetFlagsOptions, intOptions));
        }

        if (testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::doc))
        {
            // если фейл, и insertDoc возвращает false, то возвращаем true для вставки текущей строки, пусть автор документа разбирается,
            // в чем он накосячил, увидев такой выхлоп в виде заголовка с '!'
            // Если fail-опция не установлена, то не выводим ничего
            // По умолчанию в конфигах .options - установлена
            // Но если мы хотим тихо ничего не делать при обломе поиска подключаемого файла, то надо явно указать no-fail
            return !insertDoc( appCfg, docTo, resLines, line // insertCommandLine
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
std::vector<std::string> processTocCommands(const AppConfig &appCfg, Document &doc, const std::vector<std::string> &lines, bool &tocAdded)
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

    return processLines(appCfg, lines, handler);
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
inline
std::string updateInDocRefs(const AppConfig &appCfg, Document &doc, const std::string &line)
{
    // Идея по ссылкам на разделы. Указываем в них просто текст заголовка - [Какой-то текст](#Просто полный текст заголовка)
    // Также для каждого заголовка можно задать частично или полностью квалифицированную ссылку. Потом распарсим текст на предмет 
    // ссылок и сделаем замену.
    //  
    // В теле текста линка можно задать просто [$] - тогда будет вставлен текст заголовка, если задать [#] - тогда будет вставлен 
    // только номер заголовка, если есть нумерация, или его текст, если нумерации нет, [#$] - номер и текст заголовка, если есть нумерация,
    // или только текст, если нумерации нет.

    std::string resLine; resLine.reserve(line.size());

    std::string::size_type pos = 0;
    for(; pos!=line.size(); )
    {
        char ch = line[pos];

        if (ch!='[')
        {
            resLine.append(1,ch);
            ++pos;
            continue;
        }

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

    return resLine;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> updateInDocRefs(const AppConfig &appCfg, Document &doc, const std::vector<std::string> &lines)
{
    auto handler = [&](LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
    {
        if (event!=LineHandlerEvent::normalLine)
        {
            return true;
        }

        line = updateInDocRefs(appCfg, doc, line);
        return true;
    };

    return processLines(appCfg, lines, handler);
}

//----------------------------------------------------------------------------
inline
std::string processMdFile(const AppConfig &appCfg, std::string fileText, const std::string &curFilename)
{
    //fileText = marty_cpp::normalizeCrLfToLf(fileText);
    std::vector<std::string> lines = marty_cpp::splitToLinesSimple(fileText);

    Document doc;
    auto resLines = parseMarkdownFileLines(appCfg, doc, lines, curFilename, std::unordered_set<std::string>()/*alreadyIncludedDocs*/);

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
    bool henerateIdWithSectionNumber  = appCfg.targetRenderer==TargetRenderer::github ;


    resLines = generateSecionsExtra( appCfg, resLines, doc
                                   , true // update doc info
                                   , true // update header
                                   , appCfg.testProcessingOption(ProcessingOptions::numericSections) // нужно или нет реально генерить номера секций
                                   , henerateIdWithSectionNumber
                                   , generateSecIds // needSpecialIdInSectionHeader
                                   );

    resLines = updateInDocRefs(appCfg, doc, resLines);


    bool tocAdded = false;
    resLines = processTocCommands(appCfg, doc, resLines, tocAdded);

    if (!tocAdded && appCfg.testProcessingOption(ProcessingOptions::generateToc))
    {
        std::vector<std::string> tmpLines = doc.tocLines;
        //umba::vectorPushBack(tmpLines, doc.tocLines);
        makeShureEmptyLine(tmpLines);
        umba::vectorPushBack(tmpLines, resLines);
        std::swap(tmpLines, resLines);
    }

    parseDocumentMetadata(appCfg, doc);


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
        umba::vectorPushBack(tmpLines, resLines);
        std::swap(tmpLines, resLines);
    }


    if (appCfg.testProcessingOption(ProcessingOptions::metaData))
    {
        //std::cout << "Write metadata\n";
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

    return marty_cpp::mergeLines(resLines, appCfg.outputLinefeed, true  /* addTrailingNewLine */ );
}


