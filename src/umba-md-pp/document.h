#pragma once

#include "app_config.h"
#include "umba/text_utils.h"
//
#include <string>
#include <vector>
#include <unordered_map>


struct SectionInfo
{
    unsigned        sectionLevel = 0;
    std::string     originalTitle   ;
    std::string     sectionNumber   ; //
    std::string     fullTitle       ; // with section number
    std::string     sectionTargetId ;

}; // struct SectionInfo



struct Document
{
    std::vector<std::string>                                     collectedMetadataTexts; // from all processed documents, each string is an \n multiline text
    std::unordered_map<std::string, std::vector<std::string> >   tagsData;

    std::vector<std::string>                                     lines   ; //!< MD lines
    std::vector<std::string>                                     tocLines; //!< MD lines

    std::map<std::string, std::vector<SectionInfo> >             sectionInfos; //!< Инфа о секциях. OrgTitle->Info. Вектор - потому, что могут быть дубликаты, а мультимап я чёт не любли и не использую

    std::string                                                  titleFromText; //!< From document text extracted title

    std::unordered_map<std::string, std::string>                 imageFiles; // src -> dst

    std::vector<std::string> getTocLines() const
    {
        return umba::text_utils::textStripCommonIndentCopy(tocLines, [](char ch){ return ch==' '; });
    }

    template<typename AppConfigType>
    std::vector<std::string> getDocumentMetatagsMarkdown(const AppConfigType &appCfg, std::vector<std::string> metaTags=std::vector<std::string>()) const
    {
        if (metaTags.empty())
            metaTags = appCfg.documentMetaTagsList;

        std::vector<std::string> resLines;

        bool headerAdded = false;

        std::vector<std::string>::const_iterator mtIt = metaTags.begin();
        for(; mtIt!=metaTags.end(); ++mtIt)
        {
            std::string tagSerializedName = appCfg.serializeMetaTag(*mtIt); //!!! Тут надо подставить локализованное имя
            if (tagSerializedName.empty())
                continue;

            std::unordered_map<std::string, std::vector<std::string> >::const_iterator tit = tagsData.find(appCfg.makeCanonicalMetaTag(*mtIt));
            if (tit==tagsData.end())
                continue;
    
            if (tit->second.empty())
                continue;
         
            const std::vector<std::string> &tagData = tit->second;
    
            // Тут уже однозначно будет вставка
            if (!headerAdded)
            {
                //!!! Тут надо добавить что-то типа хидера, или break line
                // Или - не надо?

                headerAdded = true;
            }

            resLines.emplace_back(tagSerializedName + ": Bla-bla");
            //!!! Доделать

        }

        return resLines;
    }

    //  
    //     // Имя тэга не пустое, вектор со значениями также не пуст, надо что-то выдать
    //     ++metaTagsAddedCout;
    //  
    //     emitter << YAML::Key << tagSerializedName;
    //  
    //     MetaTagType metaTagType = appCfg.getMetaTagType(*mtIt);
    //  
    //     if (metaTagType==MetaTagType::textFirst) /* Simple text, allowed multiple definitions, but only first value is applied */
    //     {
    //         emitter << YAML::Value << tagData.front();
    //     }
    //     else if (metaTagType==MetaTagType::textReplace) /* Simple text, allowed multiple definitions, but only last value is applied */
    //     {
    //         emitter << YAML::Value << tagData.back();
    //     }
    //     else if (metaTagType==MetaTagType::textMerge) /* Text fragments will be merged to paras */
    //     {
    //         auto text = umba::string_plus::merge< std::string, std::vector<std::string>::const_iterator >( tagData.begin(), tagData.end(), std::string("\n\n") );
    //         emitter << YAML::Value << text;
    //     }
    //     else if (metaTagType==MetaTagType::list || metaTagType==MetaTagType::commaList)
    //     {
    //         emitter << YAML::BeginSeq;
    //         for(auto tv : tagData)
    //         {
    //             emitter << tv;
    //         }
    //         emitter << YAML::EndSeq;
    //     }
    //     else if (metaTagType==MetaTagType::set || metaTagType==MetaTagType::commaSet)
    //     {
    //         std::set<std::string> s;
    //         for(auto tv : tagData)
    //         {
    //             s.insert(tv);
    //         }
    //  
    //         emitter << YAML::BeginSeq;
    //         for(auto sv : s)
    //         {
    //             emitter << sv;
    //         }
    //         emitter << YAML::EndSeq;
    //  
    //     }
    //     else
    //     {
    //         emitter << std::string();
    //     }
    //  
    //     //    emitter << YAML::EndSeq;
    //  







    template<typename FilenameStringType>
    bool getMetaTagValueAsText(const AppConfig<FilenameStringType> &appCfg, std::string tag, std::string listDelimiter, std::string &tagText) const
    {
        tag = appCfg.makeCanonicalMetaTag(tag);

        std::unordered_map<std::string, std::vector<std::string> >::const_iterator tit = tagsData.find(tag);
        if (tit==tagsData.end())
            return false;

        const std::vector<std::string> &tagData = tit->second;
        if (tagData.empty())
            return false;

        MetaTagType metaTagType = appCfg.getMetaTagType(tag);

        if (metaTagType==MetaTagType::textFirst) /* Simple text, allowed multiple definitions, but only first value is applied */
        {
            tagText = tagData.front();
            return true;
        }
        else if (metaTagType==MetaTagType::textReplace) /* Simple text, allowed multiple definitions, but only last value is applied */
        {
            tagText = tagData.back();
            return true;
        }
        else if (metaTagType==MetaTagType::textMerge) /* Text fragments will be merged to paras */
        {
            //tagText = umba::string_plus::merge< std::string, std::vector<std::string>::const_iterator >( tagData.begin(), tagData.end(), std::string(" ") );
            tagText = umba::string_plus::merge< std::string, std::vector<std::string>::const_iterator >( tagData.begin(), tagData.end(), std::string("\n\n") );
            return true;
        }
        else if (metaTagType==MetaTagType::list || metaTagType==MetaTagType::commaList)
        {
            tagText = umba::string_plus::merge< std::string, std::vector<std::string>::const_iterator >( tagData.begin(), tagData.end(), listDelimiter );
            return true;
        }
        else if (metaTagType==MetaTagType::set || metaTagType==MetaTagType::commaSet)
        {
            std::set<std::string> s;
            for(auto tv : tagData)
            {
                s.insert(tv);
            }

            std::vector<std::string> v; v.reserve(s.size());
            for(auto sv : s)
            {
                v.emplace_back(sv);;
            }

            tagText = umba::string_plus::merge< std::string, std::vector<std::string>::const_iterator >( v.begin(), v.end(), listDelimiter );
            return true;

        }
        else
        {
            return false;
        }

    }

    template<typename FilenameStringType>
    std::string getMetaTagValueAsText(const AppConfig<FilenameStringType> &appCfg, std::string tag, std::string listDelimiter) const
    {
        std::string tagText;
        if (!getMetaTagValueAsText(appCfg, tag, listDelimiter, tagText))
            return std::string();
        return tagText;
    }

    template<typename FilenameStringType>
    bool getMetaTagValueAsTextLines(const AppConfig<FilenameStringType> &appCfg, std::string tag, std::string listDelimiter, std::vector<std::string> &tagTextLines) const
    {
        std::string tagText;
        if (!getMetaTagValueAsText(appCfg, tag, listDelimiter, tagText))
            return false;
        tagTextLines = marty_cpp::splitToLinesSimple(tagText, false, '\n');
        return true;
    }

    template<typename FilenameStringType>
    std::vector<std::string> getMetaTagValueAsTextLines(const AppConfig<FilenameStringType> &appCfg, std::string tag, std::string listDelimiter) const
    {
        std::vector<std::string> tagTextLines;
        if (!getMetaTagValueAsTextLines(appCfg, tag, listDelimiter, tagTextLines))
            return std::vector<std::string>();
        return tagTextLines;
    }

    template<typename FilenameStringType>
    bool getMetaTagValueAsSingleLineText(const AppConfig<FilenameStringType> &appCfg, std::string tag, std::string listDelimiter, std::string &tagText) const
    {
        if (!getMetaTagValueAsText(appCfg, tag, listDelimiter, tagText))
            return false;

        std::vector<std::string> lines = splitAndTrimAndSkipEmpty(tagText, '\n');
        tagText = umba::string_plus::merge<std::string, std::vector<std::string>::const_iterator>( lines.begin(), lines.end(), ' ' );
        umba::string_plus::trim(tagText);

        return true;
    }

    template<typename FilenameStringType>
    std::string getMetaTagValueAsSingleLineText(const AppConfig<FilenameStringType> &appCfg, std::string tag, std::string listDelimiter) const
    {
        std::string tagText;
        if (!getMetaTagValueAsSingleLineText(appCfg, tag, listDelimiter, tagText))
            return std::string();
        return tagText;
    }

    std::string getDocumentTitleFromMeta() const
    {
        std::unordered_map<std::string, std::vector<std::string> >::const_iterator tit = tagsData.find("title");
        if (tit==tagsData.end())
            return std::string();

        const std::vector<std::string> &allTakenTitles = tit->second;
        if (allTakenTitles.empty() || allTakenTitles[0].empty())
            return std::string();

        std::vector<std::string> titleSplitted = splitAndTrimAndSkipEmpty(allTakenTitles[0], '\n');
        // auto returnArg = [](const std::string &str) { return str; };
        // auto res = umba::string_plus::merge<std::string, std::vector<std::string>::const_iterator, decltype(returnArg) >( titleSplitted.begin(), titleSplitted.end(), ' ', returnArg );
        // auto returnArg = [](const std::string &str) { return str; };
        auto res = umba::string_plus::merge<std::string, std::vector<std::string>::const_iterator>( titleSplitted.begin(), titleSplitted.end(), ' ' );
        umba::string_plus::trim(res);

        return res;
    }

    std::string getDocumentTitleAny() const
    {
        std::string title = getDocumentTitleFromMeta();
        if (title.empty())
        {
            title = titleFromText;
        }

        umba::string_plus::trim(title);

        return title;
    }

    bool findSectionInfo(const std::string &orgTitle, SectionInfo &secInfo, std::size_t *pNumFoundSections=0) const
    {
        std::map<std::string, std::vector<SectionInfo> >::const_iterator it = sectionInfos.find(orgTitle);
        if (it==sectionInfos.end() || it->second.empty())
        {
            return false;
        }

        secInfo = it->second[0];

        if (pNumFoundSections)
        {
            *pNumFoundSections = it->second.size();
        }

        return true;
    }

    template<typename FilenameStringType>
    std::string getDocumentLanguage(const AppConfig<FilenameStringType> &appCfg) const
    {
        if (!appCfg.documentForceLanguage.empty())
        {
            return appCfg.documentForceLanguage;
        }

        std::string lang;
        if (getMetaTagValueAsSingleLineText(appCfg, "lanuage", ",", lang))
        {
            return lang;
        }

        if (!appCfg.documentDefaultLanguage.empty())
        {
            return appCfg.documentDefaultLanguage;
        }

        return "en-US";

    }

}; // struct Document


