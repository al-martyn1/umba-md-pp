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


template<typename StringType>
StringType tryMakeOutputFilenameFromInput(StringType name)
{
    name = umba::filename::makeCanonical(name);

    auto path           = umba::filename::getPath(name);
    auto fileNameNoPath = umba::filename::getFileName(name);

    if (umba::string_plus::starts_with_and_strip(fileNameNoPath, umba::string_plus::make_string<StringType>(".")))
    {
        // Если имя файла начинается с точки - убрали точку и зашибись
        return umba::filename::appendPath(path, fileNameNoPath);
    }

    auto pathFile = umba::filename::getPathFile(name);
    auto ext      = umba::filename::getFileExtention(name);
    if (umba::string_plus::starts_with_and_strip(ext, umba::string_plus::make_string<StringType>("_")))
    {
        // Если расширение файла начинается с подчеркивания - убрали его и зашибись
        return umba::filename::appendExtention(pathFile, ext);
    }

    if (umba::string_plus::ends_with_and_strip(ext, umba::string_plus::make_string<StringType>("_")))
    {
        // Если расширение файла заканчивается с подчеркиванием - убрали его и зашибись
        return umba::filename::appendExtention(pathFile, ext);
    }

    return StringType();

}

inline
std::string escapeCommandLineArgument(const std::string &str)
{
    // http://learn.microsoft.com/en-us/cpp/c-language/parsing-c-command-line-arguments?view=msvc-170

    bool needEscape = false;
    if (str.find_first_of(" \"")!=str.npos)
       needEscape = true;

    if (!needEscape)
        return str;

    std::string res; res.reserve(str.size());
    res.append(1, '\"');
    for(auto ch : str)
    {
        if (ch=='\"')
            res.append(2, '\"');
        else
            res.append(1, ch);
    }
    res.append(1, '\"');

    return res;
}

inline
std::wstring escapeCommandLineArgument(const std::wstring &str)
{
    return umba::fromUtf8(escapeCommandLineArgument(umba::toUtf8(str)));
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




