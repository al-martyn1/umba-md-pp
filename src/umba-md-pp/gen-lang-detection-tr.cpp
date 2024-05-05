/*! \file
    \brief Утилита umba-md-pp - препроцессор для маркдауна
 */

// Должна быть первой
#include "umba/umba.h"
//---

//#-sort
#include "umba/simple_formatter.h"
#include "umba/char_writers.h"
//#+sort

#include "umba/debug_helpers.h"
#include "umba/time_service.h"

#include <iostream>
#include <iomanip>
#include <string>
// #include <filesystem>

#include "umba/debug_helpers.h"
#include "umba/string_plus.h"
#include "umba/program_location.h"
#include "umba/scope_exec.h"
#include "umba/macro_helpers.h"
#include "umba/macros.h"

#include "marty_cpp/marty_cpp.h"
#include "marty_cpp/sort_includes.h"
#include "marty_cpp/enums.h"
#include "marty_cpp/src_normalization.h"

#include "encoding/encoding.h"
#include "umba/cli_tool_helpers.h"
#include "umba/time_service.h"

#include "umba/filesys.h"
#include "umba/filename.h"
#include "umba/format_message.h"
#include "umba/id_gen.h"

#include "snippet_options.h"

#include "encoding/encoding.h"
#include "utils.h"

//
#include "marty_tr/marty_tr.h"
#include "tr/tr.h"
//

#include "marty_yaml_toml_json/json_utils.h"
#include "marty_yaml_toml_json/yaml_json.h"
#include "marty_yaml_toml_json/yaml_utils.h"



#if defined(WIN32) || defined(_WIN32)

    #define HAS_CLIPBOARD_SUPPORT 1
    #include "umba/clipboard_win32.h"

#endif


#include "app_config.h"
#include "viewer_utils.h"




umba::StdStreamCharWriter coutWriter(std::cout);
umba::StdStreamCharWriter cerrWriter(std::cerr);
umba::NulCharWriter       nulWriter;

umba::SimpleFormatter umbaLogStreamErr(&cerrWriter);
umba::SimpleFormatter umbaLogStreamMsg(&cerrWriter);
umba::SimpleFormatter umbaLogStreamNul(&nulWriter);

bool umbaLogGccFormat   = false; // true;
bool umbaLogSourceInfo  = false;

bool bOverwrite         = false;

unsigned jsonIndent     = 2;

std::string               inputFilename;
std::string               outputFilename;


#include "log.h"
//
#include "umba/cmd_line.h"
//

AppConfig<std::string> appConfig;

// Конфиг версии
#include "app_ver_config.h"
// Принтуем версию
#include "print_ver.h"
// Парсер параметров командной строки
#include "arg_parser.h"

//
std::string curFile;
unsigned lineNo = 0;

#include "processing.h"


static
auto trErrHandler = marty_tr::makeErrReportHandler([](marty_tr::MsgNotFound what, const std::string& msg, const std::string& catId, const std::string& langId)
{
    using umba::lout;
    using namespace umba::omanip;

    // std::cerr << "---\n";
    // std::cerr << "Translation not found(" << marty_tr::to_string(what) << "): [" << langId << ":" << catId << "] - " << msg << "\n";
    // std::cerr << "\n";

}
);


inline
std::string jsonEscape(const std::string &str)
{
    std::string res; res.reserve(str.size());

    for(auto ch: str)
    {
        if (ch=='\\')
        {
            res.append(2,'\\');
        }
        else if (ch=='\"')
        {
            res.append(1,'\\');
            res.append(1,'\"');
        }
        else
        {
            res.append(1,ch);
        }
    }

    return res;
}


int main(int argc, char* argv[])
{
    marty_tr::tr_set_err_handler(&trErrHandler);
    marty_tr::tr_init_all_translations(tr_get_translations_json());

    auto autoEmptyMsgNotExist    = mtr::AutoEmptyMsgNotExist(mtr::tr_set_empty_msg_not_exist(true));


    std::map<std::string, std::string> toCanonicalLangMap;
    std::map<std::string, std::string> toCanonicalLocationMap;
    std::map<std::string, std::string> toLangTagMap;



    static const std::string langNameCatId = "marty-tr/language-name";
    static const std::string langLocCatId  = "marty-tr/language-location";

    std::map<std::string, mtr::StringLocaleInfo> locales = mtr::getStringLocaleInfoMap();

    // std::cout << "Identifiers to canonical lang/location\n";


    std::map<std::string, mtr::StringLocaleInfo>::const_iterator lcIt = locales.begin();
    for(; lcIt!=locales.end(); ++lcIt)
    {
        const std::string &langTag = lcIt->first;

        std::string langLocation = lcIt->second.lang;
        if (!lcIt->second.location.empty() && lcIt->second.location!="-" && lcIt->second.location!="_")
        {
            langLocation.append(1,'/');
            langLocation.append(lcIt->second.location);
        }

        //std::cout << langTag << ":   " << langLocation << "\n";
        if (toLangTagMap.find(langLocation)==toLangTagMap.end())
        {
            toLangTagMap[langLocation] = langTag;
        }
        

    // std::string          lang    ;
    // std::string          location; // or type
    // unsigned             langId  ; // Windows Language Identifier (Language ID, which is a part of the Windows Language Code Identifier - LCID)
    // std::string          ltag    ; // Language tag (locale name), en-US etc...


        std::vector<std::string> canonicalLangNameMsgids = mtr::tr_get_msgids(langNameCatId, langTag);

        for(const auto &canonicalLangNameMsgid: canonicalLangNameMsgids)
        {
            if (canonicalLangNameMsgid=="-" || canonicalLangNameMsgid=="_")
            {
                continue;
            }

            if (!mtr::tr_has_msg(canonicalLangNameMsgid, langNameCatId, langTag))
            {
                continue;
            }

            auto valForCompare = mtr::tr(canonicalLangNameMsgid, langNameCatId, langTag);
            valForCompare  = umba::transliterate(valForCompare);
            valForCompare  = marty_cpp::toLower (valForCompare);

            if (toCanonicalLangMap.find(valForCompare)==toCanonicalLangMap.end())
            {
                toCanonicalLangMap[valForCompare] = canonicalLangNameMsgid;
            }
        }


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
	        valForCompare  = umba::transliterate(valForCompare);
	        valForCompare  = marty_cpp::toLower (valForCompare);
	
            if (toCanonicalLocationMap.find(valForCompare)==toCanonicalLocationMap.end())
            {
                toCanonicalLocationMap[valForCompare] = canonicalLocationNameMsgid;
            }
	    }

    }

    std::cout << "{\n\"0409\": {\n\"language-location-to-lang-tag\": {\n";

    bool 
    bFirst = true;
    std::map<std::string, std::string>::const_iterator
    it = toLangTagMap.begin();
    for(; it!=toLangTagMap.end(); ++it)
    {
        if (!bFirst)
            std::cout << ", ";
        else
            std::cout << "  ";

        std::cout << "\"" << jsonEscape(it->first) << "\": \"" << jsonEscape(it->second) << "\"\n";

        bFirst = false;
    }

    std::cout << "},\n";
    std::cout << "\"natural-language-to-canonical\": {\n";

    bFirst = true;
    it = toCanonicalLangMap.begin();
    for(; it!=toCanonicalLangMap.end(); ++it)
    {
        if (!bFirst)
            std::cout << ", ";
        else
            std::cout << "  ";

        std::cout << "\"" << jsonEscape(it->first) << "\": \"" << jsonEscape(it->second) << "\"\n";

        bFirst = false;
    }

    std::cout << "},\n";
    std::cout << "\"natural-location-to-canonical\": {\n";

    bFirst = true;
    it = toCanonicalLocationMap.begin();
    for(; it!=toCanonicalLocationMap.end(); ++it)
    {
        if (!bFirst)
            std::cout << ", ";
        else
            std::cout << "  ";

        std::cout << "\"" << jsonEscape(it->first) << "\": \"" << jsonEscape(it->second) << "\"\n";

        bFirst = false;
    }

    std::cout << "}\n"; // Конец группы
    std::cout << "}\n"; // Конец 0409
    std::cout << "}\n"; // Конец json

    return 0;
}


#if (defined(WIN32) || defined(_WIN32)) && defined(__GNUC__)

   // Fix for MinGW problem - https://sourceforge.net/p/mingw-w64/bugs/942/
   // https://github.com/brechtsanders/winlibs_mingw/issues/106
   // https://stackoverflow.com/questions/74999026/is-there-the-commandlinetoargva-function-in-windows-c-c-vs-2022


   #include <winsock2.h>
   #include <windows.h>
   #include <shellapi.h>

   int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
   {
       UMBA_USED(hInstance);
       UMBA_USED(hPrevInstance);
       UMBA_USED(lpCmdLine);
       UMBA_USED(nCmdShow);

       int nArgs = 0;
       wchar_t ** wargv = CommandLineToArgvW( GetCommandLineW(), &nArgs );
       if (!wargv)
       {
           return 1;
       }

       // Count the number of bytes necessary to store the UTF-8 versions of those strings
       int n = 0;
       for (int i = 0;  i < nArgs;  i++)
       {
         n += WideCharToMultiByte( CP_UTF8, 0, wargv[i], -1, NULL, 0, NULL, NULL ) + 1;
       }

       // Allocate the argv[] array + all the UTF-8 strings
       char **argv = (char**)new char*[( (nArgs + 1) * sizeof(char *) + n )];
       if (!argv)
       {
           return 1;
       }
       
       // Convert all wargv[] --> argv[]
       char * arg = (char *)&(argv[nArgs + 1]);
       for (int i = 0;  i < nArgs;  i++)
       {
         argv[i] = arg;
         arg += WideCharToMultiByte( CP_UTF8, 0, wargv[i], -1, arg, n, NULL, NULL ) + 1;
       }
       argv[nArgs] = NULL;

       return main(nArgs, argv);

   }



#endif


