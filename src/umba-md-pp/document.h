#pragma once

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


    std::string getDocumentTitleFromMeta() const
    {
        std::unordered_map<std::string, std::vector<std::string> >::const_iterator tit = tagsData.find("title");
        if (tit==tagsData.end())
            return std::string();

        const std::vector<std::string> &allTakenTitles = tit->second;
        if (allTakenTitles.empty() || allTakenTitles[0].empty())
            return std::string();

        std::vector<std::string> titleSplitted = marty_cpp::splitToLinesSimple(allTakenTitles[0], false, '\n');
        auto returnArg = [](const std::string &str) { return str; };
        auto res = umba::string_plus::merge<std::string, std::vector<std::string>::const_iterator, decltype(returnArg) >( titleSplitted.begin(), titleSplitted.end(), ' ', returnArg );
        umba::string_plus::trim(res);

        return res;
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


}; // struct Document


