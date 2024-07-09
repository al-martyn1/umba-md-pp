/*! \file
    \brief Помогалки для YAML/JSON
 */
#pragma once


//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::string generateDocMetadata(const AppConfig<FilenameStringType> &appCfg, Document &doc)
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
template<typename FilenameStringType> inline
void parseDocumentMetadata(const AppConfig<FilenameStringType> &appCfg, Document &doc)
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

                //LOG_MSG_OPT << "parseDocumentMetadata: " << strKey << ": " << strVal << "\n";

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

//----------------------------------------------------------------------------


