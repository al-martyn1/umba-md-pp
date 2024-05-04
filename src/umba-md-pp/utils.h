/*! \file
    \brief Разные утилитки
 */

#include "umba/filename.h"
#include "umba/filesys.h"
#include "tr/tr.h"

//
#include <string>



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
std::string findLangTagByString(std::string strLang)
{

	// auto trNullErrHandler = marty_tr::makeErrReportHandler([](marty_tr::MsgNotFound what, const std::string& msg, const std::string& catId, const std::string& langId)
	// {
	// }
	// );
 //  
 //    auto autoRestoreTrErrHandler = mtr::AutoRestoreErrReportHandler(mtr::tr_set_err_handler(&trNullErrHandler));
 //  
 //    std::string langTag = mtr::getLocaleLanguageTag(strLang);
 //  
 //    if (!langTag.empty())
 //    {
 //        return langTag;
 //    }


    mtr::forEachLocaleInfo( [](const mtr::LocaleInfo li)
                            {
                                if (li.lang)
                                {
                                    std::cout << li.lang << "    ";
                                }
                                if (li.location)
                                {
                                    std::cout << li.location << "    ";
                                }
                                if (li.ltag)
                                {
                                    std::cout << li.ltag << "    ";
                                }
                                std::cout << "\n";
                            }
                          );


// struct LocaleInfo
// {
//     const char* const    lang    ;
//     const char* const    location; // or type
//     unsigned    const    langId  ; // Windows Language Identifier (Language ID, which is a part of the Windows Language Code Identifier - LCID)
//     const char* const    ltag    ; // Language tag (locale name), en-US etc...
// };
    

// "marty-tr/language-name"
// "marty-tr/language-location"

    return strLang;


}

//std::string getLocaleLanguageTag( std::string langTag, bool neutralAllowed = false )

// const LocaleInfo* getLocaleInfo( std::string langTag, bool neutralAllowed = false )

// std::string getLocaleLanguageTag( std::string langTag, bool neutralAllowed = false )

// template<typename THandler> inline void tr_enumerate_msgids(THandler handler, std::string catId, std::string langId)
// std::vector<std::string> tr_get_msgids(std::string catId, std::string langId)
// std::vector<std::string> tr_get_msgids(std::string catId)
// isNumericLanguageTag(const std::string &langTag)

// template<typename Handler> inline
// void forEachLocaleInfo(Handler handler)



