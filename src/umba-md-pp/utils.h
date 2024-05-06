/*! \file
    \brief Разные утилитки
 */

#include "umba/filename.h"
#include "umba/filesys.h"
#include "umba/string_plus.h"
#include "umba/transliteration.h"
#include "tr/tr.h"

//
#include <string>



inline
std::string escapeCommandLineAegument(const std::string &str)
{
    std::string res; res.reserve(str.size());
}

inline
bool findProjectOptionsFile(const std::string &mdFile, std::string &foundOptionsFile)
{
    return umba::cli_tool_helpers::findProjectOptionsFile(mdFile, foundOptionsFile, std::vector<std::string>{".md-pp.options", ".umba-md-pp.options"});
}

inline
bool findProjectOptionsFile(const std::wstring &mdFile, std::wstring &foundOptionsFile)
{
    return umba::cli_tool_helpers::findProjectOptionsFile(mdFile, foundOptionsFile, std::vector<std::wstring>{L".md-pp.options", L".umba-md-pp.options"});
}



inline
std::string findGoxygenLanguageByLangTag(std::string langTag)
{
    auto trNullErrHandler = marty_tr::makeErrReportHandler([](marty_tr::MsgNotFound what, const std::string& msg, const std::string& catId, const std::string& langId)
	{
	}
	);
  
    auto autoRestoreTrErrHandler = mtr::AutoRestoreErrReportHandler(mtr::tr_set_err_handler(&trNullErrHandler));
    auto autoEmptyMsgNotExist    = mtr::AutoEmptyMsgNotExist(mtr::tr_set_empty_msg_not_exist(true));

    if (mtr::tr_has_msg(langTag, "doxygen-language", "en"))
    {
        return mtr::tr(langTag, "doxygen-language", "en");
    }

    return "English";

}

inline
std::string findLangTagByString(std::string strLang)
{

	auto trNullErrHandler = marty_tr::makeErrReportHandler([](marty_tr::MsgNotFound what, const std::string& msg, const std::string& catId, const std::string& langId)
	{
	}
	);
  
    auto autoRestoreTrErrHandler = mtr::AutoRestoreErrReportHandler(mtr::tr_set_err_handler(&trNullErrHandler));
    auto autoEmptyMsgNotExist    = mtr::AutoEmptyMsgNotExist(mtr::tr_set_empty_msg_not_exist(true));

    umba::string_plus::trim(strLang);
  
    std::string langTag = mtr::getLocaleLanguageTag(strLang);
  
    if (!langTag.empty())
    {
        return langTag;
    }

    std::string langNaturalName    ;
    std::string locationNaturalName;
    umba::string_plus::split_to_pair(strLang, langNaturalName, locationNaturalName, '/');
    umba::string_plus::trim(langNaturalName    );
    umba::string_plus::trim(locationNaturalName);

    langNaturalName      = umba::transliterate(langNaturalName);
    langNaturalName      = marty_cpp::toLower (langNaturalName);

    locationNaturalName  = umba::transliterate(locationNaturalName);
    locationNaturalName  = marty_cpp::toLower (locationNaturalName);


    std::string foundCanonicalLangName    ;
    std::string foundCanonicalLocationName;


    // natural-language-to-canonical
    // natural-location-to-canonical
    // language-location-to-lang-tag


    if (!mtr::tr_has_msg(langNaturalName, "natural-language-to-canonical", "en"))
    {
        return std::string();
    }

    foundCanonicalLangName = mtr::tr(langNaturalName, "natural-language-to-canonical", "en");


    if (mtr::tr_has_msg(locationNaturalName, "natural-location-to-canonical", "en"))
    {
        foundCanonicalLocationName = mtr::tr(locationNaturalName, "natural-location-to-canonical", "en");
    }

    std::string canonicalLangLocation = foundCanonicalLangName;
    if (!foundCanonicalLocationName.empty())
    {
        canonicalLangLocation.append(1,'/');
        canonicalLangLocation.append(foundCanonicalLocationName);
    }

    if (mtr::tr_has_msg(canonicalLangLocation, "language-location-to-lang-tag", "en"))
    {
        return mtr::tr(canonicalLangLocation, "language-location-to-lang-tag", "en");
    }

    if (mtr::tr_has_msg(foundCanonicalLangName, "language-location-to-lang-tag", "en"))
    {
        return mtr::tr(foundCanonicalLangName, "language-location-to-lang-tag", "en");
    }

    return std::string();

#if 0

    // Как найти каноничное название языка?

    /*
      Для каждого известного языка zz-ZZ:

          1) Получить список переводов названий языка в категории "marty-tr/language-name" (при langTag zz-ZZ)
             пройтись по списку. Если перевод совпадает (а лучше транлитерация и в нижнем регистре), то мы нашли каноническое название языка (на английском)
          2) Получить список переводов названий стран в категории "marty-tr/language-location" (при langTag zz-ZZ)
             пройтись по списку. Если перевод совпадает (а лучше транлитерация и в нижнем регистре), то мы нашли каноническое название страны (на английском)
          3) Пройтись по сету идентификаторов языков, где язык и страна совпадут, то такой и LangTag.
             Если не нашли, то проходимся также, но без сравнения без страны
    */

    static const std::string langNameCatId = "marty-tr/language-name";
    static const std::string langLocCatId  = "marty-tr/language-location";

    std::string langNaturalName    ;
    std::string locationNaturalName;
    umba::string_plus::split_to_pair(strLang, langNaturalName, locationNaturalName, '/');
    umba::string_plus::trim(langNaturalName    );
    umba::string_plus::trim(locationNaturalName);

    std::string langNaturalNameForCmp     = langNaturalName    ;
    std::string locationNaturalNameForCmp = locationNaturalName;

    if (useTransiteration)
    {
        langNaturalNameForCmp      = umba::transliterate(langNaturalNameForCmp);
        langNaturalNameForCmp      = marty_cpp::toLower (langNaturalNameForCmp);

        locationNaturalNameForCmp  = umba::transliterate(locationNaturalNameForCmp);
        locationNaturalNameForCmp  = marty_cpp::toLower (locationNaturalNameForCmp);
    }


    std::string foundCanonicalLangNaturalName    ;
    std::string foundCanonicalLocationNaturalName;


    std::map<std::string, mtr::StringLocaleInfo> locales = mtr::getStringLocaleInfoMap();

    std::map<std::string, mtr::StringLocaleInfo>::const_iterator lcIt = locales.begin();
    for(; lcIt!=locales.end(); ++lcIt)
    {
        // if (!foundCanonicalLangNaturalName.empty() && !foundCanonicalLocationNaturalName.empty())
        //     break;

        const std::string &langTag = lcIt->first;

        if (foundCanonicalLangNaturalName.empty())
        {
	        std::vector<std::string> canonicalLangNameMsgids = mtr::tr_get_msgids(langNameCatId, langTag);
	
	        for(const auto &canonicalLangNameMsgid: canonicalLangNameMsgids)
	        {
	            if (!mtr::tr_has_msg(canonicalLangNameMsgid, langNameCatId, langTag))
	            {
	                continue;
	            }

                auto valForCompare = mtr::tr(canonicalLangNameMsgid, langNameCatId, langTag);
			    if (useTransiteration)
			    {
			        valForCompare  = umba::transliterate(valForCompare);
			        valForCompare  = marty_cpp::toLower (valForCompare);
                }

                if (langNaturalNameForCmp==valForCompare)
                {
                    foundCanonicalLangNaturalName = canonicalLangNameMsgid;
                    break;
                }
	        }
        } // if (foundCanonicalLangNaturalName.empty())


        if (foundCanonicalLocationNaturalName.empty())
        {
	        std::vector<std::string> canonicalLocationNameMsgids = mtr::tr_get_msgids(langLocCatId, langTag);
	
	        for(const auto &canonicalLocationNameMsgid: canonicalLocationNameMsgids)
	        {
                if (canonicalLocationNameMsgid=="-" || canonicalLocationNameMsgid=="_")
                {
	                continue;
	            }

	            if (!mtr::tr_has_msg(canonicalLocationNameMsgid, langLocCatId, langTag))
	            {
	                continue;
	            }

                auto valForCompare = mtr::tr(canonicalLocationNameMsgid, langLocCatId, langTag);
			    if (useTransiteration)
			    {
			        valForCompare  = umba::transliterate(valForCompare);
			        valForCompare  = marty_cpp::toLower (valForCompare);
                }

                if (locationNaturalNameForCmp==valForCompare)
                {
                    foundCanonicalLocationNaturalName = canonicalLocationNameMsgid;
                    break;
                }
	        }
        } // if (foundCanonicalLocationNaturalName.empty())

        //tr_enumerate_msgids(THandler handler, std::string catId, std::string langId)
    }

    lcIt = locales.begin();
    for(; lcIt!=locales.end(); ++lcIt)
    {
        const std::string &langTag  = lcIt->first;
        const std::string &lang     = lcIt->second.lang    ;
        const std::string &location = lcIt->second.location;

        if (foundCanonicalLangNaturalName==lang && foundCanonicalLocationNaturalName==location && !location.empty())
        {
            return langTag;
        }
    }


    lcIt = locales.begin();
    for(; lcIt!=locales.end(); ++lcIt)
    {
        const std::string &langTag  = lcIt->first;
        const std::string &lang     = lcIt->second.lang    ;
        const std::string &location = lcIt->second.location;

        if (foundCanonicalLangNaturalName==lang)
        {
            return langTag;
        }
    }


    return std::string();
#endif

}




