/*! \file
    \brief Утилита umba-md-pp-view - рендерер маркдауна, использующий Doxygen и локальный просмотрщик RTF-файлов
 */

// C:\Users\mag\AppData\Local\Temp\.umba-md-pp-view\

// Должна быть первой
#include "umba/umba.h"
//---

//#-sort
#include "umba/simple_formatter.h"
#include "umba/char_writers.h"
//#+sort

#include "umba/debug_helpers.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
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
#include "umba/utf8.h"
//
#include "snippet_options.h"
#include "rtf_workarounds.h"

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
#include "extern_tools.h"



// #define TRY_UNICODE_VIEWER


#if defined(UMBA_MD_PP_VIEW_CONSOLE)

    #if defined(TRY_UNICODE_VIEWER)
        #undef TRY_UNICODE_VIEWER
    #endif

#endif


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


#ifdef TRY_UNICODE_VIEWER
    using  FilenameStringType = std::wstring;
#else
    using  FilenameStringType = std::string;
#endif


FilenameStringType               inputFilename;
FilenameStringType               outputFilename;

#include "log.h"
//
#include "umba/cmd_line.h"
//

AppConfig<FilenameStringType> appConfig;

// Парсер параметров командной строки
#include "arg_parser.h"

//
FilenameStringType curFile;
unsigned lineNo = 0;

#include "processing.h"



#if defined(_MSC_VER) && !(defined(CMAKE_INTDIR) || defined(CMAKE_CFG_INTDIR) || defined(CMAKE_BINARY_DIR))
    #pragma comment(lib, "yaml-cpp.lib")
#endif



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


/*
    У нас есть такие ситуации:

    1) У нас неюникодное оконное приложение, собирается под MSVC. Есть
       - WinMain
       - main_impl

    2) У нас юникодное оконное приложение, собирается под MSVC. Есть
       - wWinMain
       - wmain_impl

    3) У нас неюникодное оконное приложение, собирается под GCC. Есть
       - WinMain
       - main_impl

    4) У нас юникодное оконное приложение, собирается под GCC. Есть
       - wWinMain
       - wmain_impl

    5) У нас неюникодное консольное приложение, собирается под MSVC. Есть
       - main

    6) У нас юникодное консольное приложение, собирается под MSVC. Есть
       - wmain

    7) У нас неюникодное консольное приложение, собирается под GCC. Есть
       - WinMain
       - main_impl

    8) У нас юникодное консольное приложение, собирается под GCC. Есть
       - wWinMain
       - wmain_impl

    Итого:

                        UNICODE             UNICODE             UNICODE             UNICODE
              Windows   Windows   Windows   Windows   Console   Console   Console   Console
              MSVC      MSVC      GCC       GCC       MSVC      MSVC      GCC       GCC

WinMain         +                   +                                       +
wWinMain                  +                   +                                       +
main_impl       +                   +                                       +
wmain_impl                +                   +                                       +
main                                                    +
wmain                                                             +


main/wmain - нужны только для MSVC/Console

Вопрос: обходной путь через жопу с использованием WinMain/wWinMain нужен только для GCC или для всех, кроме MSVC?


 */


#if defined(_MSC_VER) && defined(UMBA_MD_PP_VIEW_CONSOLE)

    #define UMBA_MD_PP_VIEW_NEED_MAIN_IMPL
    #define UMBA_MD_PP_VIEW_NEED_MAIN

#endif

#if defined(__GNUC__) || !defined(UMBA_MD_PP_VIEW_CONSOLE)

    #define UMBA_MD_PP_VIEW_NEED_MAIN_IMPL
    #define UMBA_MD_PP_VIEW_NEED_WINMAIN

#endif



//#if defined(UMBA_MD_PP_VIEW_CONSOLE)
#if defined(UMBA_MD_PP_VIEW_NEED_MAIN_IMPL)

    #ifdef TRY_UNICODE_VIEWER

        int wmain_impl(int argc, wchar_t* argv[])

    #else

        int main_impl(int argc, char* argv[])

    #endif

#endif
{

    marty_tr::tr_set_err_handler(&trErrHandler);
    marty_tr::tr_init_all_translations(tr_get_translations_json());

    // std::cout << underwood::tr("Hello world!") << "\n";
    // std::cout << underwood::tr("Hello non-translated world!") << "\n";


    using namespace umba::omanip;


    auto argsParser = umba::command_line::makeArgsParser( ArgParser<FilenameStringType>()
                                                        , CommandLineOptionCollector()
                                                        , argc, argv
                                                        , umba::program_location::getProgramLocation
                                                            ( argc, argv
                                                            , false // useUserFolder = false
                                                            //, "" // overrideExeName
                                                            )
                                                        );

    //programLocationInfo = argsParser.programLocationInfo;

    // Force set CLI arguments while running under debugger

    if (umba::isDebuggerPresent())
    {

        std::string cwd = umba::filesys::getCurrentDirectory<std::string>();
        std::cout << "Working Dir: " << cwd << "\n";

        #if (defined(WIN32) || defined(_WIN32))

            std::string rootPath;

            if (winhelpers::isProcessHasParentOneOf({"devenv"}))
            {
                // По умолчанию студия задаёт текущим каталогом На  уровень выше от того, где лежит бинарник
                rootPath = umba::filename::makeCanonical(umba::filename::appendPath<std::string>(cwd, "..\\..\\..\\"));
                //argsParser.args.push_back("--batch-output-root=D:/temp/mdpp-test");
            }
            else if (winhelpers::isProcessHasParentOneOf({"code"}))
            {
                // По умолчанию VSCode задаёт текущим каталогом тот, где лежит бинарник
                rootPath = umba::filename::makeCanonical(umba::filename::appendPath<std::string>(cwd, "..\\..\\..\\..\\"));
                //argsParser.args.push_back("--batch-output-root=C:/work/temp/mdpp-test");

            }
            else
            {
                //rootPath = umba::filename::makeCanonical(umba::filename::appendPath<std::string>(cwd, "..\\..\\..\\"));
            }

            //#endif

            rootPath = umba::filename::appendPathSepCopy(rootPath);

        #endif


        argsParser.args.clear();

        // argsParser.args.push_back("@" + rootPath + "conf/conf/umba-md-pp.options");
        //
        // argsParser.args.push_back("--overwrite");
        // argsParser.args.push_back("--add-examples-path="+rootPath+"/tests/snippets");
        // argsParser.args.push_back("--add-examples-path="+rootPath+"/tests/..");
        // argsParser.args.push_back(rootPath + "tests\\test03.md_");
        // // F:\_github\umba-tools\umba-md-pp

        //argsParser.args.push_back("--register-view-handler");

        #if defined(UMBA_MD_PP_VIEW)
            // argsParser.args.push_back("--register-view-handler");
        #else

        #endif

        //argsParser.args.push_back("C:\\work\\github\\umba-tools\\umba-md-pp\\README.md_");

        argsParser.args.push_back(rootPath + "/README.md_");


    }


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



    //unsigned errCount = 0;

    // if (!argsParser.quet)
    // {
    //     umbaLogStreamMsg << "Processing: "<<inputFilename<<"\n";
    // }

    std::string inputFileText;
    //if (!umba::filesys::readFile(inputFilename, inputFileText))
    if (!AppConfig<FilenameStringType>::readInputFile(inputFilename, inputFileText))
    {
        LOG_ERR_OPT << umba::formatMessage("failed to read input file: '$(fileName)'")
                                          .arg("fileName",umba::toUtf8(inputFilename))
                                          .toString()
                    << "\n";
        //errCount++;
        return 2;
    }

    UMBA_USED(lineNo);

    appConfig.setStrictPathFromFilename(inputFilename);



    //std::string
    curFile = umba::toUtf8(inputFilename); // = fileName;
    //unsigned lineNo = 0;


    appConfig.checkFixRenderingTargetName(false /* forView */ );
    std::vector<std::string> foundOptionsFiles;
    findProjectOptionsFiles(curFile, appConfig.renderingTargetName, foundOptionsFiles);
    if (!foundOptionsFiles.empty())
    {
        appConfig.setStrictPathFromFilename(foundOptionsFiles[0]); // рестрикции задаем по самому первому (верхнему в файловой иерархии) файлу
        for(const auto& projectOptionsFile: foundOptionsFiles)
        {
            appConfig.pushSamplesPaths();
            argsParser.pushOptionsFileName(projectOptionsFile);
            argsParser.parseOptionsFile(projectOptionsFile);
            argsParser.popOptionsFileName();
            appConfig.popSamplesPathsAndInsertNewAtFront();
        }
    }


    // Необходимо для нормальной генерации доксигеном RTF'а
    appConfig.targetRenderer = TargetRenderer::doxygen;
    appConfig.targetFormat   = TargetFormat::rtf;
    appConfig.updateProcessingOptions("title");
    appConfig.updateProcessingOptions("convert-github-alerts");
    appConfig.copyImageFiles    = true;
    appConfig.flattenImageLinks = true;
    appConfig.graphVizOptions.dpi = 240;
    appConfig.graphVizOptions.targetFormat = GraphVizTargetFormat::png;
    appConfig.graphVizOptions.showLabels   = false;
    //appConfig.graphVizOptions.targetFormat = GraphVizTargetFormat::emf;

    appConfig.singleModeInOutPathsDifferent = true; // Без проверок говорим, что входной и выходной файл - в разных каталогах (выходной в сгенерённом TEMP'е - так что ок)

    bOverwrite = true;


    // bool
    // regRes = regShellExtentionHandlerApplication(L"_umba-app", L"open", L"umba %1");
    // std::cout << "Register res: " << (regRes ? true : false) << "\n";
    //
    // regRes = regShellExtentionHandlerForExt(L"_umba-app", {L"md_", L"md", L"markdown"});
    // std::cout << "Register res: " << (regRes ? true : false) << "\n";

    appConfig.checkAdjustDocNumericLevels();
    appConfig.checkTargetFormat();


    try
    {
        // if (!argsParser.quet)
        // {
        //     std::string printableOutputFilename;
        //     umba::utfToStringTypeHelper(printableOutputFilename, outputFilename);
        //     umbaLogStreamMsg << "Writting output to: "<<printableOutputFilename<<"\n";
        // }

        // umba::cli_tool_helpers::writeOutput( outputFilename, outputFileType
        //                                    , encoding::ToUtf8(), encoding::FromUtf8()
        //                                    , resText, std::string() // bomData
        //                                    , true /* fromFile */, true /* utfSource */ , bOverwrite
        //                                    );

        FilenameStringType tempPath;
        if (!createTempFolder(tempPath, inputFilename /* inputFileText */ , umba::string_plus::make_string<FilenameStringType>("umba-md-pp-view")))
        {
            std::ostringstream oss;
            oss << "Can't view MD file.\n";
            oss << "Failed to create temp folder: " << tempPath << "\n";
            #if defined(WIN32) || defined(_WIN32)
            oss << "Error code: " << (unsigned)GetLastError();
            #endif
            showErrorMessageBox(oss.str());
            return 3;
        }

        FilenameStringType mdTempFile              = umba::filename::appendPath(tempPath, umba::string_plus::make_string<FilenameStringType>("document.md"));
        FilenameStringType doxygenConfigTempFile   = umba::filename::appendPath(tempPath, umba::string_plus::make_string<FilenameStringType>("Doxyfile"));
        FilenameStringType doxygenRtfCfgTempFile   = umba::filename::appendPath(tempPath, umba::string_plus::make_string<FilenameStringType>("doxygen_rtf.cfg"));

        FilenameStringType doxygenOutTempFolder    = umba::filename::appendPath(tempPath, umba::string_plus::make_string<FilenameStringType>("doxy"));
        umba::filesys::createDirectory(doxygenOutTempFolder);
        FilenameStringType doxygenRtfOutTempFolder = umba::filename::appendPath(doxygenOutTempFolder, umba::string_plus::make_string<FilenameStringType>("rtf"));
        umba::filesys::createDirectory(doxygenRtfOutTempFolder);

        appConfig.graphVizOptions.savePath         = umba::string_plus::make_string<std::string>(tempPath);

        FilenameStringType generatedRtfFile        = umba::filename::appendPath(doxygenRtfOutTempFolder, umba::string_plus::make_string<FilenameStringType>("refman.rtf"));


        Document doc;
        std::string resText  = processMdFile(appConfig, inputFileText, inputFilename, doc);
        std::string docTitle = doc.getDocumentTitleAny();

        FilenameStringType finalFilename           = generateFinalFilenameFromTitle(docTitle, appConfig.testProcessingOption(ProcessingOptions::transliterateGeneratedFilenames));


        if (finalFilename.empty())
            finalFilename = umba::filename::getName(inputFilename);
        FilenameStringType finalFilenameWithExt    = umba::filename::appendExt(finalFilename, umba::string_plus::make_string<FilenameStringType>("rtf"));
        FilenameStringType fullFinalFilename       = umba::filename::appendPath(doxygenRtfOutTempFolder, finalFilenameWithExt);

        if (!argsParser.quet)
        {
            std::cout << "Doc Title: " << docTitle << "\n";
            std::cout << "Generated final file name: " << finalFilename << "\n";
            std::cout << "Generated full final file name: " << fullFinalFilename << "\n";
            std::cout << "Known languages: " << appConfig.getAllLangFileExtentions() << "\n";
        }

        // umba::cli_tool_helpers::writeOutput( mdTempFile, umba::cli_tool_helpers::IoFileType::regularFile // outputFileType
        //                                    , encoding::ToUtf8(), encoding::FromUtf8()
        //                                    , resText, std::string() // bomData
        //                                    , true /* fromFile */, true /* utfSource */ , true // bOverwrite
        //                                    );
        if (!umba::filesys::writeFile(mdTempFile, resText, true /* overwrite */ ))
        {
            showErrorMessageBox("Failed to write file MD temp file: " + mdTempFile);
            return 4;
        }


        auto &infoLog = argsParser.quet ? umbaLogStreamNul : umbaLogStreamMsg;
        std::map<std::string, ImageFileForCopyInfo> imagesToCopy;
        addImageFilesForCopying( imagesToCopy, inputFilename, mdTempFile, doc.imageFiles);
        copyDocumentImageFiles(infoLog, imagesToCopy, bOverwrite);


        std::string doxyfileData   = generateDoxyfile(appConfig, doc, mdTempFile);
        std::string doxyRtfCfgData = generateDoxygenRtfCfg(appConfig, doc);

        // umba::cli_tool_helpers::writeOutput( doxygenConfigTempFile, umba::cli_tool_helpers::IoFileType::regularFile // outputFileType
        //                                    , encoding::ToUtf8(), encoding::FromUtf8()
        //                                    , doxyfileData, std::string() // bomData
        //                                    , true /* fromFile */, true /* utfSource */ , true // bOverwrite
        //                                    );
        if (!umba::filesys::writeFile(doxygenConfigTempFile, doxyfileData, true /* overwrite */ ))
        {
            showErrorMessageBox("Failed to write Doxyfile temp file: " + doxygenConfigTempFile);
            return 5;
        }


        // umba::cli_tool_helpers::writeOutput( doxygenRtfCfgTempFile, umba::cli_tool_helpers::IoFileType::regularFile // outputFileType
        //                                    , encoding::ToUtf8(), encoding::FromUtf8()
        //                                    , doxyRtfCfgData, std::string() // bomData
        //                                    , true /* fromFile */, true /* utfSource */ , true // bOverwrite
        //                                    );
        if (!umba::filesys::writeFile(doxygenRtfCfgTempFile, doxyRtfCfgData, true /* overwrite */ ))
        {
            showErrorMessageBox("Failed to write doxygen RTF CFG temp file: " + doxygenRtfCfgTempFile);
            return 6;
        }


        if (!umba::filesys::setCurrentDirectory(tempPath))
        {
            showErrorMessageBox("Failed to change current directory. Can't view MD file.");
            return 7;
        }

        std::wstring wDoxygenExeName = findDoxygenExecutableName<std::wstring>(appConfig.dontLookupForDoxygen);
        std::string  doxygenExeName  = umba::toUtf8(wDoxygenExeName);
        //std::string  doxygenExeNameEscaped  = escapeCommandLineArgument(doxygenExeName);

        #if defined(UMBA_MD_PP_VIEW_CONSOLE)

        {
            std::cout << "Found Doxygen: " << doxygenExeName << "\n";
            std::string  doxygenExeNameEscaped  = escapeCommandLineArgument(doxygenExeName);
            std::cout << "Escaped      : " << doxygenExeNameEscaped << "\n";
        }

        #endif

        std::string callingDoxygenErrMsg;
        //auto systemRes = system(doxygenExeNameEscaped.c_str());
        auto systemRes = safeSystemFunction(&callingDoxygenErrMsg, doxygenExeName);
        if (systemRes<0)
        {
            showErrorMessageBox("Failed to execute '" + doxygenExeName + "': " + callingDoxygenErrMsg);
            return 8;
        }

        if (!umba::filesys::isFileReadable(generatedRtfFile))
        {
            showErrorMessageBox("Can't read generated file '" + generatedRtfFile + "'");
            return 9;
        }

        //!!! Fix RTF here
        if (!rtfEmbedImagesWorkaround(generatedRtfFile))
        {
            showErrorMessageBox("Failed to embed images");
            return 10;
        }


        FilenameStringType generatedRtfFileCanonical  = umba::filename::makeCanonical(generatedRtfFile);
        FilenameStringType fullFinalFilenameCanonical = umba::filename::makeCanonical(fullFinalFilename);

        // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefileexa

        if (appConfig.viewerCopyToSourceLocation)
        {
            std::string targetPath = umba::filename::getPath(inputFilename);
            std::string targetName = umba::filename::getName(inputFilename);
            std::string targetExt  = umba::filename::getExt(generatedRtfFile);

            if (appConfig.viewerCopyFilenameDocTitle)
            {
                targetName = finalFilename;
            }

            fullFinalFilenameCanonical = umba::filename::makeCanonical(umba::filename::appendPath(targetPath, umba::filename::appendExt(targetName, targetExt)));
        }

        if (!MoveFileExA( generatedRtfFileCanonical.c_str(), fullFinalFilenameCanonical.c_str()
                        , MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH
                        )
           )
        {
            showErrorMessageBox("Failed to create final file"); // !!! Это вылезает, если предыдущая версия файла уже открыта в ворде
            return 11;
        }

        ShellExecuteA( 0 // HWND
                     , "open"
                     , fullFinalFilenameCanonical.c_str()
                     , 0 // lpParameters
                     , 0 // lpDirectory
                     , SW_NORMAL
                     );

    } // try
    catch(const std::runtime_error &e)
    {

        showErrorMessageBox(e.what());
        return 12;

        // LOG_ERR_OPT << e.what() << "\n";
        // return 1;
    }

    return 0;
}


#if defined(UMBA_MD_PP_VIEW_NEED_MAIN)

    #ifdef TRY_UNICODE_VIEWER

        int wmain(int argc, wchar_t* argv[])
        {
            return wmain_impl(argc, argv);
        }

    #else

        int main(int argc, char* argv[])
        {
            return main_impl(argc, argv);
        }

    #endif

#endif


//#if (defined(WIN32) || defined(_WIN32)) && defined(__GNUC__)
//#if defined(UMBA_MD_PP_VIEW_CONSOLE)
#if defined(UMBA_MD_PP_VIEW_NEED_WINMAIN)

    //#error "444"

    // Fix for MinGW problem - https://sourceforge.net/p/mingw-w64/bugs/942/
    // https://github.com/brechtsanders/winlibs_mingw/issues/106
    // https://stackoverflow.com/questions/74999026/is-there-the-commandlinetoargva-function-in-windows-c-c-vs-2022


    #include <winsock2.h>
    #include <windows.h>
    #include <shellapi.h>

    #ifdef TRY_UNICODE_VIEWER
    int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
    #else
    int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
    #endif
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


        #ifdef TRY_UNICODE_VIEWER

            return wmain_impl(nArgs, wargv);

        #else

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

        return main_impl(nArgs, argv);

        #endif

    }

//#endif

#else

//#error "777"

#endif

