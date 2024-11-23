/*! \file
    \brief Помогалки для YAML/JSON
 */
#pragma once

#include "md-pp-macro_helpers.h"

#include <algorithm>


//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::string generateDocMetadata(const AppConfig<FilenameStringType> &appCfg, Document &doc)
{
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;

    // emitter << YAML::Key << "var1" << YAML::Value << "value1";

    std::size_t metaTagsAddedCout = 0;

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
        ++metaTagsAddedCout;

        emitter << YAML::Key << tagSerializedName;

        MetaTagType metaTagType = appCfg.getMetaTagType(*mtIt);

        if (metaTagType==MetaTagType::textFirst || metaTagType==MetaTagType::rootOnly) /* Simple text, allowed multiple definitions, but only first value is applied */
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
        else if (metaTagType==MetaTagType::uniqueList || metaTagType==MetaTagType::commaUniqueList)
        {
            auto tmp = tagData;
            auto tmp2 = std::vector<std::string>(tmp.begin(), std::unique(tmp.begin(), tmp.end()));
            emitter << YAML::BeginSeq;
            for(auto tv : tmp2)
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

    if (!metaTagsAddedCout)
        return std::string();

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

        auto dotPos = resVal.find('.');
        if (dotPos!=resVal.npos)
        {
            umba::string_plus::rtrim(resVal, [](char ch) { return ch=='0'; } );
        }

        return true;
    }
    else
    {
        return false;
    }

}

//----------------------------------------------------------------------------
template<typename AppCfgT> inline
std::string pdmSubstMetaMacros(const AppCfgT &appCfg, const std::string &text, bool forceSubst)
{
    if (!appCfg.testProcessingOption(ProcessingOptions::metaDataSubst) || !forceSubst)
        return text;

    using namespace umba::macros;
    return substMacros( text
                      , umba::md::MacroTextFromMapOrEnvRef(appCfg.conditionVars, false /* !envAllowed */ )
                      , smf_KeepUnknownVars // | smf_uppercaseNames // !!! Надо заморачиваться с регистром? Если надо, то тогда при добавлении всё в upper case и кондишены надо подправить
                      );
}

//----------------------------------------------------------------------------
template<typename AppCfgT> inline
std::vector<std::string> pdmSubstMetaMacros(const AppCfgT &appCfg, const std::vector<std::string> &textVec, bool forceSubst)
{
    std::vector<std::string> res; res.reserve(textVec.size());
    for(const auto &text : textVec)
    {
        res.emplace_back(pdmSubstMetaMacros(appCfg, text, forceSubst));
    }

    return res;
}

//----------------------------------------------------------------------------
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

template<typename FilenameStringType> inline
void parseDocumentMetadata(const AppConfig<FilenameStringType> &appCfg, Document &doc)
{
    auto collectedMetadataTexts = doc.collectedMetadataTexts;

    // rootOnly - в collectedMetadataTexts при обработке корневого документа надо добавить
    // специальный метатэг ___root_document: 1 в виде текста,
    // а тут мы должны просто выцепить это значение и сделать пометку, а там видно будет

    // Тут мы делаем подстановку макросов, если она задана
    // Теперь так нельзя
    #if 0
    if ((appCfg.testProcessingOption(ProcessingOptions::metaDataSubst)))
    {
        using namespace umba::macros;

        for(auto &metaText : collectedMetadataTexts)
        {
            metaText = substMacros( metaText
                                  , umba::md::MacroTextFromMapOrEnvRef(appCfg.conditionVars, false /* !envAllowed */ )
                                  , smf_KeepUnknownVars // | smf_uppercaseNames // !!! Надо заморачиваться с регистром? Если надо, то тогда при добавлении всё в upper case и кондишены надо подправить
                                  );
        }
    }
    #endif


    for(const auto &metaText : collectedMetadataTexts)
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

        bool rootDocument = false;

        // Перво-наперво ищем признак корневого элемента
        for(auto el : j.items())
        {
            try
            {
                std::string strKey = el.key();
                if (strKey=="___root_document") // Проверяем только наличие тэга
                {
                    LOG_INFO("auto-url") << "Current document is root document\n";
                    rootDocument = true;
                    break;
                }
            }
            catch(...)
            {}
        }

        for(auto el : j.items())
        {
            try
            {
                std::string strKey = el.key();
                if (strKey=="___root_document")
                {
                    continue; // этот тэг технический, пропускаем
                }

                strKey = appCfg.makeCanonicalMetaTag(strKey);
                std::vector<std::string> &tagValsVec = doc.tagsData[strKey];

                MetaTagType tagType = appCfg.getMetaTagType(strKey);

                auto val = el.value();

                bool forceSubst = false;

                if (tagType==MetaTagType::rootOnly) // Одиночное значение, допустимое только в корневом документе?
                {
                    if (!rootDocument) // Найден не в корневом документе? Пропускаем
                        continue;

                    // тут надо сделать подстановку безусловно - rootOnly тэги типа URL могут всегда содержать макросы
                    // val - это не строка
                    // val = substMetaMacros(val, true);
                    forceSubst = true;
                }
                else
                {
                    // val - это не строка
                    //val = substMetaMacros(val, false);
                }
                
                //    std::string valStr = val;
                // //std::string strVal = el.value();
                //    LOG_MSG << "parseDocumentMetadata: " << strKey << ": " << valStr << "\n";

                nlohmann::detail::value_t valType = val.type();

                if ( valType==nlohmann::detail::value_t::object
                  || valType==nlohmann::detail::value_t::binary
                  || valType==nlohmann::detail::value_t::discarded
                   )
                    continue;

                if (valType==nlohmann::detail::value_t::array)
                {
                    // Элемент списка/массива не надо разбирать, даже если он commaList или commaSet
                    // Но добавлять надо
                    //else if (metaTagType==MetaTagType::uniqueList || metaTagType==MetaTagType::commaUniqueList)
                    if ( tagType!=MetaTagType::list 
                      && tagType!=MetaTagType::set
                      && tagType!=MetaTagType::commaList
                      && tagType!=MetaTagType::commaSet
                      && tagType!=MetaTagType::uniqueList
                      && tagType!=MetaTagType::commaUniqueList
                       )
                        continue;

                    for (auto vel : val.items())
                    {
                        auto jVel = vel.value();
                        std::string strVal;
                        if (!getJsonNodeTypeValueAsString(jVel, strVal))
                            continue;

                        // tagValsVec.emplace_back(strVal);
                        tagValsVec.emplace_back(pdmSubstMetaMacros(appCfg, strVal, forceSubst));
                    }

                    continue;
                }

                std::string strVal;
                if (!getJsonNodeTypeValueAsString(val, strVal))
                    continue;

                //LOG_MSG << "parseDocumentMetadata: " << strKey << ": " << strVal << "\n";

                if (tagType==MetaTagType::commaList || tagType==MetaTagType::commaSet || tagType==MetaTagType::commaUniqueList)
                {
                    // Если одиночная строка является commaList или commaSet, то надо разобрать
                    umba::vectorPushBack(tagValsVec, pdmSubstMetaMacros(appCfg, splitAndTrimAndSkipEmpty(strVal, ','), forceSubst));
                }
                else // MetaTagType::rootOnly элементы тоже сюда попадают
                {
                    tagValsVec.emplace_back(pdmSubstMetaMacros(appCfg, strVal, forceSubst));
                }

            }
            catch(...)
            {
                LOG_MSG << "parseDocumentMetadata exception\n";
            }

        }

    }

    if (appCfg.testProcessingOption(ProcessingOptions::autoUrl))
    {
        LOG_INFO("auto-url") << "Found 'auto-url' processing option\n";

        auto cvIt = appCfg.conditionVars.find("__DocumentBaseUrl");
        if (cvIt!=appCfg.conditionVars.end())
        {
            LOG_INFO("auto-url") << "Found '__DocumentBaseUrl' var\n";

            auto urlCanonicalTagName = appCfg.makeCanonicalMetaTag("url");
            LOG_INFO("auto-url") << "URL tag canonical name: '" << urlCanonicalTagName << "'\n";
            auto tgIt = doc.tagsData.find(urlCanonicalTagName);
            if (tgIt==doc.tagsData.end()) // тэга URL нету
            {
                LOG_INFO("auto-url") << "URL tag not taken in document, adding auto-generated value\n";

                // auto simpleSubst = [&](const std::string &text)
                // {
                //     using namespace umba::macros;
                //     return substMacros( text
                //                       , umba::md::MacroTextFromMapOrEnvRef(appCfg.conditionVars, false /* !envAllowed */ )
                //                       , smf_KeepUnknownVars // | smf_uppercaseNames // !!! Надо заморачиваться с регистром? Если надо, то тогда при добавлении всё в upper case и кондишены надо подправить
                //                       );
                // };

                auto
                tmpIt = appCfg.conditionVars.find("__DocumentBaseUrl");
                if (tmpIt==appCfg.conditionVars.end())
                {
                    LOG_INFO("auto-url") << "'__DocumentBaseUrl' variable not found\n";
                }
                else
                {
                    LOG_INFO("auto-url") << "Found '__DocumentBaseUrl' variable\n";
                }

                std::string baseUrl = tmpIt->second;
                if (baseUrl.empty())
                {
                    LOG_INFO("auto-url") << "'__DocumentBaseUrl' was found, but empty\n";
                }


                tmpIt = appCfg.conditionVars.find("__DocumentRelFileName");
                if (tmpIt==appCfg.conditionVars.end())
                {
                    LOG_INFO("auto-url") << "'__DocumentRelFileName' variable not found\n";
                }
                else
                {
                    LOG_INFO("auto-url") << "Found '__DocumentRelFileName' variable\n";
                }

                std::string documentRelFileName = tmpIt->second;
                if (documentRelFileName.empty())
                {
                    LOG_INFO("auto-url") << "'__DocumentRelFileName' was found, but empty\n";
                }

                if (baseUrl.empty() || documentRelFileName.empty())
                {
                    LOG_INFO("auto-url") << "URL tag not added automatically\n";
                }
                else
                {
                    std::string docUrl = umba::filename::appendPath(baseUrl, documentRelFileName, '/');
                    // doc.tagsData[urlCanonicalTagName].emplace_back(pdmSubstMetaMacros(appCfg, "$(__DocumentBaseUrl)/$(__DocumentRelFileName)", true /* forceSubst */ ));
                    doc.tagsData[urlCanonicalTagName].emplace_back(docUrl);
                }

            }
            else
            {
                LOG_INFO("auto-url") << "URL tag found in document, do nothing\n";
            }
        }
        else
        {
            LOG_INFO("auto-url") << "'__DocumentBaseUrl' var not found\n";
        }
    }
    // strKey = appCfg.makeCanonicalMetaTag(strKey);
    // std::vector<std::string> &tagValsVec = doc.tagsData[strKey];
    //  
    // MetaTagType tagType = appCfg.getMetaTagType(strKey);

}

//----------------------------------------------------------------------------


