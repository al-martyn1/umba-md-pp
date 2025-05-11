#pragma once

#include "app_config.h"
//
#include "umba/container_utility.h"
#include "umba/text_utils.h"
#include "umba/string_plus.h"
#include "marty_tr/marty_tr.h"
//
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>


struct SectionInfo
{
    std::size_t     sectionLevel = 0;
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
        // см generateDocMetadata

        // getDocumentLanguage(const AppConfig<FilenameStringType> &appCfg) const

        if (metaTags.empty())
            metaTags = appCfg.documentMetaTagsList;

        // Добавлять ли в текст документа заголовок метасекции "Document Information"/"Информация о документе"
        bool addMetaTitle = appCfg.testProcessingOption(ProcessingOptions::documentMetaTitle);
        {
            std::vector<std::string> metaTagsTmp;
            for(auto t : metaTags)
            {
                auto optId = enum_deserialize(t, ProcessingOptions::invalid);
                if (optId==ProcessingOptions::noDocumentMetaTitle)
                {
                    addMetaTitle = false;
                }
                else if (optId==ProcessingOptions::documentMetaTitle)
                {
                    addMetaTitle = true;
                }
                else
                {
                    metaTagsTmp.emplace_back(t);
                }
            }

            swap(metaTags, metaTagsTmp);
        }

        std::string langStr = getDocumentLanguage(appCfg);
        std::string langId  = findLangTagByString(langStr);


        std::vector<std::string> resLines;

        bool headerAdded = false;

        std::vector<std::string>::const_iterator mtIt = metaTags.begin();
        for(; mtIt!=metaTags.end(); ++mtIt)
        {
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


            MetaTagType metaTagType = appCfg.getMetaTagType(*mtIt);

            std::string tagId  = *mtIt;
            if (tagData.size()>1 && (metaTagType==MetaTagType::list || metaTagType==MetaTagType::commaList || metaTagType==MetaTagType::uniqueList || metaTagType==MetaTagType::commaUniqueList || metaTagType==MetaTagType::set || metaTagType==MetaTagType::commaSet))
                tagId += "s";
            std::string tagSerializedName = marty_tr::tr(tagId, "metatag-titles", langId);
            //appCfg.serializeMetaTag(*mtIt); //!!! Тут надо подставить локализованное имя
            if (tagSerializedName.empty())
                continue;

            std::string tagPreparedName = "**" + tagSerializedName + "**:";
            
            if (metaTagType==MetaTagType::textFirst || metaTagType==MetaTagType::rootOnly) /* Simple text, allowed multiple definitions, but only first value is applied */
            {
                // emitter << YAML::Value << tagData.front();
                resLines.emplace_back(tagPreparedName + " " + tagData.front());
                resLines.emplace_back(std::string());
            }
            else if (metaTagType==MetaTagType::textReplace) /* Simple text, allowed multiple definitions, but only last value is applied */
            {
                // emitter << YAML::Value << tagData.back();
                resLines.emplace_back(tagPreparedName + " " + tagData.back());
                resLines.emplace_back(std::string());
            }
            else if (metaTagType==MetaTagType::textMerge) /* Text fragments will be merged to paras */
            {
                auto text = umba::string_plus::merge< std::string, std::vector<std::string>::const_iterator >( tagData.begin(), tagData.end(), std::string("\n\n") );
                //emitter << YAML::Value << text;
                resLines.emplace_back(tagPreparedName + " " + text);
                resLines.emplace_back(std::string());
            }
            else if (metaTagType==MetaTagType::list || metaTagType==MetaTagType::commaList)
            {
                resLines.emplace_back(tagPreparedName);
                for(auto tv : tagData)
                {
                    resLines.emplace_back("- " + tv);
                }
                resLines.emplace_back(std::string());
            }
            else if (metaTagType==MetaTagType::uniqueList || metaTagType==MetaTagType::commaUniqueList)
            {
                resLines.emplace_back(tagPreparedName);
                auto tmp = tagData;
                auto tmp2 = std::vector<std::string>(tmp.begin(), std::unique(tmp.begin(), tmp.end()));
                for(auto tv : tmp2)
                {
                    resLines.emplace_back("- " + tv);
                }
                resLines.emplace_back(std::string());
            }
            else if (metaTagType==MetaTagType::set || metaTagType==MetaTagType::commaSet)
            {
                resLines.emplace_back(tagPreparedName);
                std::set<std::string> s;
                for(auto tv : tagData)
                {
                    s.insert(tv);
                }

                for(auto sv : s)
                {
                    resLines.emplace_back("- " + sv);
                }
                resLines.emplace_back(std::string());
            }
            else
            {
                // emitter << std::string();
            }

        }

        if (addMetaTitle && !resLines.empty())
        {
            std::string title = marty_tr::tr("metatag-section-title", "metatag-titles", langId);
            std::vector<std::string> tmpLines;
            tmpLines.emplace_back("**" + title + "**");
            tmpLines.emplace_back(std::string());
            umba::vectorPushBack(tmpLines, resLines);
            swap(tmpLines, resLines);
        }

        return resLines;
    }

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
        else if (metaTagType==MetaTagType::uniqueList || metaTagType==MetaTagType::commaUniqueList)
        {
            auto tmp = tagData;
            auto tmp2 = std::vector<std::string>(tmp.begin(), std::unique(tmp.begin(), tmp.end()));
            tagText = umba::string_plus::merge< std::string, std::vector<std::string>::const_iterator >( tmp2.begin(), tmp2.end(), listDelimiter );
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

        std::vector<std::string> linesTmp = splitAndTrimAndSkipEmpty(tagText, '\n');
        tagText = umba::string_plus::merge<std::string, std::vector<std::string>::const_iterator>( linesTmp.begin(), linesTmp.end(), ' ' );
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
        if (getMetaTagValueAsSingleLineText(appCfg, "language", ",", lang))
        {
            return lang;
        }

        if (!appCfg.documentDefaultLanguage.empty())
        {
            return appCfg.documentDefaultLanguage;
        }

        return "en-US";

    }

    // template<typename FilenameStringType>
    // std::string getDocumentLanguage(const AppConfig<FilenameStringType> &appCfg) const
    // {
    //     if (!appCfg.documentForceLanguage.empty())
    //     {
    //         return appCfg.documentForceLanguage;
    //     }
    //  
    //     std::string lang;
    //     if (getMetaTagValueAsSingleLineText(appCfg, "lanuage", ",", lang))
    //     {
    //         return lang;
    //     }
    //  
    //     if (!appCfg.documentDefaultLanguage.empty())
    //     {
    //         return appCfg.documentDefaultLanguage;
    //     }
    //  
    //     return "en-US";
    //  
    // }


    


}; // struct Document


