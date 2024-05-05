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

    std::cerr << "---\n";
    std::cerr << "Translation not found(" << marty_tr::to_string(what) << "): [" << langId << ":" << catId << "] - " << msg << "\n";
    std::cerr << "\n";

}
);


int main(int argc, char* argv[])
{
    umba::time_service::init();

    marty_tr::tr_set_err_handler(&trErrHandler);
    marty_tr::tr_init_all_translations(tr_get_translations_json());


    std::cout << "Doxygen: " << umba::toUtf8(findDoxygenExecutableName()) << "\n";

    using namespace umba::omanip;


    auto argsParser = umba::command_line::makeArgsParser( ArgParser<std::string>()
                                                        , CommandLineOptionCollector()
                                                        , argc, argv
                                                        , umba::program_location::getProgramLocation
                                                            ( argc, argv
                                                            , false // useUserFolder = false
                                                            //, "" // overrideExeName
                                                            )
                                                        );

    // Force set CLI arguments while running under debugger
    if (umba::isDebuggerPresent())
    {
        #if (defined(WIN32) || defined(_WIN32))

            #if defined(__GNUC__)

                std::string rootPath = "..\\..\\..\\..\\..\\";

            #else // if

                std::string rootPath = "..\\";

            #endif

        #endif

        argsParser.args.clear();

        argsParser.args.push_back("@" + rootPath + "_distr_conf/conf/umba-md-pp.options");

        argsParser.args.push_back("--overwrite");
        argsParser.args.push_back("--set-insert-options=filename,path,filenameLineNo");
        argsParser.args.push_back("--add-examples-path=" + rootPath + "src;" + rootPath + "tests\\snippets");
        //argsParser.args.push_back("--set-insert-options=lineno,notrim,notag,fail");
        argsParser.args.push_back("--set-insert-options=filename,path,filenameLineNo,fail,snippet-options,trim-arround");
        argsParser.args.push_back("--processing-options=generate-toc,meta-data");
        argsParser.args.push_back("--serialize-meta-tags=title,descripion,author");
        argsParser.args.push_back("--target-renderer=doxygen");

        // argsParser.args.push_back("");
        // argsParser.args.push_back("");
        argsParser.args.push_back(rootPath + "tests\\test04.md_");
    }

    //programLocationInfo = argsParser.programLocationInfo;

    // try
    // {
        // Job completed - may be, --where option found
        if (argsParser.mustExit)
            return 0;
       
        // if (!argsParser.quet)
        // {
        //     printNameVersion();
        // }
       
        if (!argsParser.parseStdBuiltins())
            return 1;
        if (argsParser.mustExit)
            return 0;
       
        if (!argsParser.parse())
            return 1;
        if (argsParser.mustExit)
            return 0;
    // }
    // catch(const std::exception &e)
    // {
    //     LOG_ERR_OPT << e.what() << "\n";
    //     return -1;
    // }
    // catch(const std::exception &e)
    // {
    //     LOG_ERR_OPT << "command line arguments parsing error" << "\n";
    //     return -1;
    // }


    umba::cli_tool_helpers::IoFileType outputFileType = umba::cli_tool_helpers::IoFileType::nameEmpty;
    if (outputFilename.empty())
    {
        outputFilename = "STDOUT";
    }

    outputFileType = umba::cli_tool_helpers::detectFilenameType(outputFilename, false /* !bInput */);

    #if defined(WIN32) || defined(_WIN32)
    if (outputFileType==umba::cli_tool_helpers::IoFileType::clipboard)
    {
        LOG_ERR_OPT << "invalid output file name"
                    << "\n";
        return 1;
    }
    #endif

    //unsigned errCount = 0;

    if (!argsParser.quet)
    {
        umbaLogStreamMsg << "Processing: "<<inputFilename<<"\n";
    }

    std::string inputFileText;
    //if (!umba::filesys::readFile(inputFilename, inputFileText))
    if (!AppConfig<std::string>::readInputFile(inputFilename, inputFileText))
    {
        LOG_ERR_OPT << umba::formatMessage("failed to read input file: '$(fileName)'")
                                          .arg("fileName",inputFilename)
                                          .toString()
                    << "\n";
        //errCount++;
        return 1;
    }

    UMBA_USED(lineNo);

    appConfig.setStrictPathFromFilename(inputFilename);



    //std::string 
    curFile = inputFilename; // = fileName;
    //unsigned lineNo = 0;


    appConfig.checkAdjustDocNumericLevels();
    appConfig.checkTargetFormat();

    std::string projectOptionsFile;
    if (findProjectOptionsFile(inputFilename, projectOptionsFile))
    {
        appConfig.setStrictPathFromFilename(projectOptionsFile);
        argsParser.parseOptionsFile(projectOptionsFile);
    }


    std::string resText = processMdFile(appConfig, inputFileText, inputFilename);

    try
    {
        if (!argsParser.quet)
        {
            umbaLogStreamMsg << "Writting output to: "<<outputFilename<<"\n";
        }

        umba::cli_tool_helpers::writeOutput( outputFilename, outputFileType
                                           , encoding::ToUtf8(), encoding::FromUtf8()
                                           , resText, std::string() // bomData
                                           , true /* fromFile */, true /* utfSource */ , bOverwrite
                                           );
    } // try
    catch(const std::runtime_error &e)
    {
        LOG_ERR_OPT << e.what() << "\n";
        return 1;
    }



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


