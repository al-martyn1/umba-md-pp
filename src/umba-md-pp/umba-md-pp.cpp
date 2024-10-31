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

#include "umba/app_main.h"
#include "umba/debug_helpers.h"
#include "umba/time_service.h"

#include <iostream>
#include <iomanip>
#include <string>
// #include <filesystem>

#include "umba/debug_helpers.h"
#include "umba/shellapi.h"
#include "umba/string_plus.h"
#include "umba/program_location.h"
#include "umba/scope_exec.h"
#include "umba/macro_helpers.h"
#include "umba/macros.h"
#include "umba/info_log.h"
//
#include "marty_cpp/marty_cpp.h"
#include "marty_cpp/sort_includes.h"
#include "marty_cpp/enums.h"
#include "marty_cpp/src_normalization.h"

//
#include "marty_utf/utf.h"

//
#include "encoding/encoding.h"
#include "umba/cli_tool_helpers.h"
#include "umba/time_service.h"

#include "umba/filesys.h"
#include "umba/filename.h"
#include "umba/format_message.h"
#include "umba/id_gen.h"
//#include "umba/char_class.h"

#include "snippet_options.h"

#include "encoding/encoding.h"
#include "utils.h"
#include "batch_utils.h"
#include "umba_md_processing_utils.h"
#include "umba_md_html_utils.h"
#include "viewer_utils.h"
//findDoxygenExecutableName()

//
#include "marty_tr/marty_tr.h"
#include "tr/tr.h"
//

#include "marty_yaml_toml_json/json_utils.h"
#include "marty_yaml_toml_json/yaml_json.h"
#include "marty_yaml_toml_json/yaml_utils.h"
//
#include "app_config.h"
#include "viewer_utils.h"




umba::StdStreamCharWriter coutWriter(std::cout);
umba::StdStreamCharWriter cerrWriter(std::cerr);
umba::NulCharWriter       nulWriter;

umba::SimpleFormatter umbaLogStreamErr(&cerrWriter);
umba::SimpleFormatter umbaLogStreamMsg(&coutWriter);
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

// Парсер параметров командной строки
#include "arg_parser.h"

//
std::string curFile;
unsigned lineNo = 0;
//bool quet        = true;
bool verboseMode = false;

#include "processing.h"


umba::SimpleFormatter& getLogMsgStream(int  /* level */ )
{
    return verboseMode ? umbaLogStreamMsg : umbaLogStreamNul;
}


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

int safe_main(int argc, char* argv[]);

//int main(int argc, char* argv[])
UMBA_APP_MAIN()
{
    try
    {
        return safe_main(argc, argv);
    }
    catch(const std::exception &e)
    {
        LOG_ERR << "Exception found: " << e.what() << "\n";
    }
    catch(...)
    {
        LOG_ERR << "Exception found: " << "unknown exception" << "\n";
    }

    return 66;
}

int safe_main(int argc, char* argv[])
{

    // coutWriter();
    // cerrWriter();

    umba::time_service::init();

    marty_tr::tr_set_err_handler(&trErrHandler);
    marty_tr::tr_init_all_translations(tr_get_translations_json());


    //std::cout << "Doxygen: " << findDoxygenExecutableName() << "\n";

    using namespace umba::omanip;


    if (umba::isDebuggerPresent())
    {
        // Misc tests goes here

        #if 0

        // std::cout << (umba::CharClassUnderlyingType)(umba::CharClass::nonprintable | umba::CharClass::punctuation) << "\n";

        std::string doxygenExe = findDoxygenExecutableName();
        std::cout << "Doxygen: " << doxygenExe << "\n";

        std::wstring graphvizBinPath;
        if (umba::win32_utils::regQueryAppInstallLocationBin(std::wstring(L"Graphviz"), graphvizBinPath))
        {
        }
        std::cout << "Graphviz bin path: " << graphvizBinPath << "\n";


        #endif

        #if 0
        umba::md::testParseSingleTag(std::cout, "</tag/>", '/');
        umba::md::testParseSingleTag(std::cout, "<tag <", '<');
        umba::md::testParseSingleTag(std::cout, "<tag attr<", '<');
        umba::md::testParseSingleTag(std::cout, "<tag attr=<", '<');
        umba::md::testParseSingleTag(std::cout, "<tag attr=val<", '<');
        umba::md::testParseSingleTag(std::cout, "<", 'E');
        umba::md::testParseSingleTag(std::cout, "<tag>", '>');
        umba::md::testParseSingleTag(std::cout, "<tag attr=val />", '>');
        umba::md::testParseSingleTag(std::cout, "</tag>", '>');
        umba::md::testParseSingleTag(std::cout, "<tag attr />", '/');
        umba::md::testParseSingleTag(std::cout, "<tag attr= />", '/');
        umba::md::testParseSingleTag(std::cout, "<tag attr=val />", '/');
        #endif


        #if 0
        auto nonCanonicalName = std::string("../../some_file.txt");
        auto canonicalName1 = umba::filename::makeCanonical(nonCanonicalName, '/', std::string("."), std::string(".."), false);
        auto canonicalName2 = umba::filename::makeCanonical(nonCanonicalName, '/', std::string("."), std::string(".."), true );
        std::cout << nonCanonicalName << "\n";
        std::cout << canonicalName1 << "\n";
        std::cout << canonicalName2 << "\n";
        #endif


        #if 0
        umba::md::testTransformMarkdownText("Someting `in back`ticks");
        umba::md::testTransformMarkdownText("Someting ``in `double back``ticks");
        #endif

        #if 0
        umba::md::testTransformMarkdownLinksUrlString("[![Todo](doc/icons/todo-list-50.png) TODO](doc/todo.md)");
        umba::md::testTransformMarkdownLinksUrlString("[Test1](http://ya.ru\\test\\test.md?a=http://g.com\\some\\path#show)");
        umba::md::testTransformMarkdownLinksUrlString("[Te`st`2](http://ya.ru/test/test.md?a=http://g.com/some/path#show)");
        umba::md::testTransformMarkdownLinksUrlString("[Test1](  http://ya.ru\\test\\test.md?a=http://g.com#show)");
        umba::md::testTransformMarkdownLinksUrlString("[Te`st`2](http://ya.ru/test/test.md?a=http://g.com#show  )");
        umba::md::testTransformMarkdownLinksUrlString("[qwqwqw](\\test\\test.md)");
        umba::md::testTransformMarkdownLinksUrlString("[dcasc](/test/test.md)");
        umba::md::testTransformMarkdownLinksUrlString("[qwqwqw](  \\test\\test.md  )");
        umba::md::testTransformMarkdownLinksUrlString("[dcasc](/test/test md)");
        umba::md::testTransformMarkdownLinksUrlString("[dsc](test\\test.md)");
        umba::md::testTransformMarkdownLinksUrlString("[vfda](test/test.md)");
        #endif

        // NNN`inline\nvoid\ndoSomething`/`inline\nvoid\ndoSomeOther`-
        // NNNtag-
        // NNN-

        // NNN
        // {}
        // (-)
        // (N)
        // `inline\nvoid\ndoAnotherSomething`


        #if 0
        umba::md::testParseSnippetTag(std::cout, "125`inline\\nvoid\\ndoSomething`/`inline\\nvoid\\ndoSomeOther`");
        umba::md::testParseSnippetTag(std::cout, "125`inline\\nvoid\\ndoSomething`/`inline\\nvoid\\ndoSomeOther");
        umba::md::testParseSnippetTag(std::cout, "125tag");
        umba::md::testParseSnippetTag(std::cout, "125");

        umba::md::testParseSnippetTag(std::cout, "125`inline\\nvoid\\ndoSomething`/`inline\\nvoid\\ndoSomeOther`-125");
        umba::md::testParseSnippetTag(std::cout, "125tag-125");
        umba::md::testParseSnippetTag(std::cout, "125-125");

        umba::md::testParseSnippetTag(std::cout, "125`inline\\nvoid\\ndoSomething`/`inline\\nvoid\\ndoSomeOther`-{}");
        umba::md::testParseSnippetTag(std::cout, "125tag-{}");
        umba::md::testParseSnippetTag(std::cout, "125-{}");

        umba::md::testParseSnippetTag(std::cout, "125`inline\\nvoid\\ndoSomething`/`inline\\nvoid\\ndoSomeOther`-{");
        umba::md::testParseSnippetTag(std::cout, "125tag-{");
        umba::md::testParseSnippetTag(std::cout, "125-{");

        umba::md::testParseSnippetTag(std::cout, "125`inline\\nvoid\\ndoSomething`/`inline\\nvoid\\ndoSomeOther`-(-)");
        umba::md::testParseSnippetTag(std::cout, "125tag-(-)");
        umba::md::testParseSnippetTag(std::cout, "125-(-)");

        umba::md::testParseSnippetTag(std::cout, "125`inline\\nvoid\\ndoSomething`/`inline\\nvoid\\ndoSomeOther`-(-");
        umba::md::testParseSnippetTag(std::cout, "125tag-(-");
        umba::md::testParseSnippetTag(std::cout, "125-(-");

        umba::md::testParseSnippetTag(std::cout, "125`inline\\nvoid\\ndoSomething`/`inline\\nvoid\\ndoSomeOther`-(");
        umba::md::testParseSnippetTag(std::cout, "125tag-(");
        umba::md::testParseSnippetTag(std::cout, "125-(");

        umba::md::testParseSnippetTag(std::cout, "125`inline\\nvoid\\ndoSomething`/`inline\\nvoid\\ndoSomeOther`-(3)");
        umba::md::testParseSnippetTag(std::cout, "125tag-(3)");
        umba::md::testParseSnippetTag(std::cout, "125-(3)");

        umba::md::testParseSnippetTag(std::cout, "125`inline\\nvoid\\ndoSomething`/`inline\\nvoid\\ndoSomeOther`-(3");
        umba::md::testParseSnippetTag(std::cout, "125tag-(3");
        umba::md::testParseSnippetTag(std::cout, "125-(3");

        // umba::md::testParseSnippetTag(std::cout, "125`inline\\nvoid\\ndoSomething`/`inline\\nvoid\\ndoSomeOther`-");
        // umba::md::testParseSnippetTag(std::cout, "125tag-");
        // umba::md::testParseSnippetTag(std::cout, "125-");

        #endif

    }


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
        argsParser.args.clear();
        argsParser.args.push_back("--overwrite");

        std::string cwd;
        std::string rootPath = umba::shellapi::getDebugAppRootFolder(&cwd);
        std::cout << "Working Dir: " << cwd << "\n";
         
        //argsParser.args.push_back("@" + rootPath + "conf/conf/umba-md-pp.options");

        // argsParser.args.push_back("--set-insert-options=filename,path,filenameLineNo");
        // argsParser.args.push_back("--add-examples-path=" + rootPath + "src;" + rootPath + "tests\\snippets");
        // //argsParser.args.push_back("--set-insert-options=lineno,notrim,notag,fail");
        // argsParser.args.push_back("--set-insert-options=filename,path,filenameLineNo,fail,snippet-options,trim-arround");
        // argsParser.args.push_back("--processing-options=generate-toc,meta-data");
        // argsParser.args.push_back("--serialize-meta-tags=title,descripion,author");
        // argsParser.args.push_back("--target-renderer=doxygen");
        //
        // // argsParser.args.push_back("");
        // // argsParser.args.push_back("");
        // argsParser.args.push_back(rootPath + "tests\\test04.md_");

        //argsParser.args.push_back("--batch-scan="+rootPath);
        //argsParser.args.push_back("--batch-scan="+rootPath+"doc");
        //argsParser.args.push_back("--batch-scan-recurse="+rootPath+"tests");


        // argsParser.args.push_back("--md");
        // argsParser.args.push_back("--help");

        #if 0
        argsParser.args.push_back("--batch-scan-recurse="+rootPath+".");
        argsParser.args.push_back("--batch-exclude-dir=_libs,libs,_lib,lib,tests,test");

        argsParser.args.push_back("--batch-page-index-file=pages.md");
        argsParser.args.push_back("--batch-split-page-index-file");
        argsParser.args.push_back("--batch-exclude-files=*upper_inc.md*");

        argsParser.args.push_back("--batch-output-path=C:\\work\\temp\\mdpp-test");

        argsParser.args.push_back("--overwrite");
        argsParser.args.push_back("--copy-images");

        // argsParser.args.push_back("--add-examples-path="+rootPath+".\\doc");
        // argsParser.args.push_back("--add-examples-path="+rootPath+".\\src");
        // argsParser.args.push_back("--add-examples-path="+rootPath+".\\tests\\snippets");
        // argsParser.args.push_back("--add-examples-path="+rootPath+"\\_distr_conf");


        argsParser.args.push_back("--batch-exclude-dir=_libs,libs,_lib,lib,tests,test,rc,_generators,_distr_conf,src,.msvc2019,boost,icons");
        argsParser.args.push_back("--batch-exclude-files=*upper_inc.md*");
        argsParser.args.push_back("--batch-scan-recurse="+rootPath);

        argsParser.args.push_back("--graphviz-output-path="+rootPath+"\\doc\\generated_images");
        //argsParser.args.push_back("--graphviz-output-format=svg");


        //argsParser.args.push_back("");
        #endif

        argsParser.args.push_back("--processing-options=generate-toc");
        

        // F:\\_github\\umba-tools\\umba-md-pp
        #if defined(__has_include)
            #if __has_include("F:/_github/umba-tools/umba-md-pp/README.md_")
                argsParser.args.push_back("F:/_github/umba-tools/umba-md-pp/README.md_");
            #elif __has_include("C:/work/github/umba-tools/umba-md-pp/README.md_")
                argsParser.args.push_back("C:/work/github/umba-tools/umba-md-pp/README.md_");
            #endif
        #else
        #endif

        //argsParser.args.push_back("-q");

        // batch-exclude-files
        // batch-output-root


    } // if (umba::isDebuggerPresent())

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

        LOG_INFO("config") << "-----------------------------------------" << "\n";
        LOG_INFO("config") << "Processing builtin option files\n";
        if (!argsParser.parseStdBuiltins())
        {
            LOG_INFO("config") << "Error found in builtin option files\n";
            return 1;
        }
        LOG_INFO("config") << "-----------------------------------------" << "\n";

        if (argsParser.mustExit)
            return 0;

        LOG_INFO("config") << "-----------------------------------------" << "\n";
        LOG_INFO("config") << "Processing command line arguments\n";
        if (!argsParser.parse())
        {
            LOG_INFO("config") << "Error found while parsing command line arguments\n";
            return 1;
        }
        LOG_INFO("config") << "-----------------------------------------" << "\n";

        if (argsParser.mustExit)
            return 0;
    // }
    // catch(const std::exception &e)
    // {
    //     LOG_ERR << e.what() << "\n";
    //     return -1;
    // }
    // catch(const std::exception &e)
    // {
    //     LOG_ERR << "command line arguments parsing error" << "\n";
    //     return -1;
    // }

    if (!argsParser.quet  /* && !hasHelpOption */ )
    {
        //printNameVersion();
        //LOG_MSG<<"\n";
        umba::cli_tool_helpers::printNameVersion(umbaLogStreamMsg);
    }

    verboseMode = appConfig.verboseMode;


    if (appConfig.verboseMode)
    {
        umbaLogStreamMsg << "Snippets lookup paths:\n";
        for(auto p : appConfig.samplesPaths)
        {
            umbaLogStreamMsg << "    " << p << "\n";
        }
    }


    // umbaLogStreamMsg << "JAVA: " << appConfig.getJava() << "\n";
    // umbaLogStreamMsg << "PLANTUML: " << appConfig.getPlantUml() << "\n";

    //unsigned errCount = 0;

    auto &infoLog = argsParser.quet ? umbaLogStreamNul : umbaLogStreamMsg;
    std::map<std::string, ImageFileForCopyInfo> imagesToCopy;


    if (appConfig.isBatchMode())
    {
        if (!argsParser.quet  /* && !hasHelpOption */ )
        {
            umbaLogStreamMsg << "Batch mode\n";
        }

        std::vector<std::string>  foundFiles;
        std::vector<std::string>  foundFilesRootFolders;

        batchScanForFiles( appConfig.batchScanPaths
                         , appConfig.mdppExtentions
                         , appConfig.batchExcludeDirs
                         , appConfig.batchExcludeFilesMaskList
                         , infoLog
                         , foundFiles
                         , &foundFilesRootFolders
                         );

        if (foundFiles.empty())
        {
            umbaLogStreamMsg << "No files found\n";
            return 0;
        }



        if (foundFiles.size() != foundFilesRootFolders.size())
        {
            LOG_ERR << "Missmatch files and folders size, something goes wrong" << "\n";
            return 1;
        }


        // umba::info_log::printSectionHeader(infoLog, "Files for processing");
        // for(const auto &foundFile: foundFiles)
        // {
        //     infoLog << foundFile << "\n";
        // }

        if (!appConfig.clearGenerationCaches)
        {
            umba::info_log::printSectionHeader(infoLog, "Processing found files");
        }

        std::string commonLang        = "English";
        bool        commonLangChanged = false    ;

        std::vector<std::string>::const_iterator fileIt   = foundFiles.begin();
        std::vector<std::string>::const_iterator folderIt = foundFilesRootFolders.begin();

        //std::map<std::wstring, std::string> pageIndex;
        std::vector< std::pair<std::wstring, std::string> > pagesIndex;
        std::string calculatedCommonPath = umba::filename::makeCanonical(umba::filename::getPath(foundFiles.front()));

        std::vector<std::string> gitAddFiles; //gitAddBatchFileName
        bool useBatSyntax = appConfig.isGitAddBatchFileNameIsBatFile();


        for(; fileIt!=foundFiles.end() && folderIt!=foundFilesRootFolders.end(); ++fileIt, ++folderIt)
        {
            auto fileRootPath = *folderIt;
            auto fileFullName = *fileIt  ;
            auto fileRelName  = fileFullName;

            // Доп. проверка, если не совпало, то что-то очень пошло не так
            if (!umba::string_plus::starts_with_and_strip(fileRelName, fileRootPath))
            {
                LOG_ERR << "File '" << fileFullName << "' not in it's root folder '" << fileRootPath << "' - something goes wrong" << "\n";
                return 2;
            }

            umba::filename::stripLastPathSep(fileRootPath);
            umba::filename::stripFirstPathSep(fileRelName);

            if (!appConfig.clearGenerationCaches)
            {
                infoLog << "Processing file '" << fileRelName << "' from '" << fileRootPath << "' (" << fileFullName << ")";
            }

            #define UMBA_MD_PP_BATCH_MODE_PROCESSING_LOG_MULTILINE
            // #if defined(UMBA_MD_PP_BATCH_MODE_PROCESSING_LOG_MULTILINE)
            //     infoLog << "\n    Target: ";
            // #else
            //     infoLog << ", target: ";
            // #endif


            std::string ext       = umba::filename::getExt(fileFullName);
            std::string targetExt = appConfig.getTargetFileExtention(ext);
            std::string targetRelName;

            //std::string outputFileName;
            if (appConfig.batchOutputRoot.empty())
            {
                // Просто меняем расширение исходного файла
                targetRelName   = umba::filename::appendExt(umba::filename::getPathFile(fileFullName), targetExt);

                std::string targetPath = umba::filename::makeCanonical(umba::filename::getPath(targetRelName));
                std::string::const_iterator pit = targetPath.begin();
                std::string::iterator       cit = calculatedCommonPath.begin();
                for(; pit!=targetPath.end() && cit!=calculatedCommonPath.end() && *pit==*cit; ++pit, ++cit) {}
                if (cit!=calculatedCommonPath.end())
                {
                    calculatedCommonPath.erase(cit, calculatedCommonPath.end());
                }

                outputFilename = targetRelName;
            }
            else
            {
                // Присовывем в output
                targetRelName   = umba::filename::appendExt(umba::filename::getPathFile(fileRelName), targetExt);
                outputFilename  = umba::filename::appendPath(appConfig.batchOutputRoot, targetRelName);
            }

            // infoLog << "'" << outputFilename << "'" << "\n";

            //infoLog << "Processing file" foundFile << "\n";

            if (!argsParser.quet)
            {
                if (!appConfig.clearGenerationCaches)
                {
                    #if defined(UMBA_MD_PP_BATCH_MODE_PROCESSING_LOG_MULTILINE)
                        umbaLogStreamMsg << "\n    Writting output to: ";
                    #else
                        umbaLogStreamMsg << " - writting output to: ";
                    #endif
                    umbaLogStreamMsg << outputFilename << "\n";
                }
            }


            if (ext==targetExt)
            {
                //LOG_WARN_OPT("same-file") << "source file and target file has same extention, skip processing it (output file: '" << outputFilename << "')\n";
                LOG_WARN("same-file") << "source file and target file has same extention, skip processing it\n";
                continue;
            }


            std::string inputFileText;
            if (!AppConfig<std::string>::readInputFile(*fileIt, inputFileText))
            {
                LOG_ERR << umba::formatMessage("failed to read input file: '$(fileName)'")
                                                  .arg("fileName",*fileIt)
                                                  .toString()
                            << "\n";
                return 3;
            }

            appConfig.setStrictPathFromFilename(*fileIt); // Пока так

            curFile = *fileIt;

            appConfig.checkFixRenderingTargetName(false /* forView */ );
            std::vector<std::string> foundOptionsFiles;
            findProjectOptionsFiles(curFile, appConfig.renderingTargetName, foundOptionsFiles);
            auto appConfigForFile = applyProjectOptionsFiles(appConfig, argsParser, foundOptionsFiles);

            // All options applied
            if (appConfigForFile.clearGenerationCaches)
            {
                appConfigForFile.doClearGenerationCaches();
                continue;
            }

            appConfigForFile.checkAdjustDocNumericLevels();
            appConfigForFile.checkTargetFormat();
            appConfigForFile.checkUpdateEmptyGeneratedOutputRootByFilename(curFile);

            Document doc;
            std::string resText     = processMdFile(appConfigForFile, inputFileText, curFile, doc);
            std::string docTitle    = doc.getDocumentTitleAny();
            std::string docLanguage = doc.getDocumentLanguage(appConfigForFile);


            if (!appConfigForFile.batchOutputRoot.empty() && appConfigForFile.copyImageFiles)
            {
                addImageFilesForCopying( imagesToCopy, *fileIt, outputFilename, doc.imageFiles);
            }


            if (!commonLangChanged) // Язык меняли?
            {
                if (umba::string_plus::tolower_copy(commonLang)!=umba::string_plus::tolower_copy(docLanguage))
                {
                    // найден язык, не являющийся дефолтным
                    commonLang = docLanguage; // Используем первое найденное значение, остальные игнорируем
                    commonLangChanged = true;
                }
            }

            std::wstring pageIndexText = umba::fromUtf8(docTitle);
            pagesIndex.emplace_back(pageIndexText, targetRelName);
            // std::map<std::wstring, std::string> pageIndex;
            // std::vector< std::pair<std::wstring, std::string> > pagesIndex;

            try
            {
                umba::filesys::createDirectoryEx( umba::filename::getPath(outputFilename), true /* forceCreatePath */ );

                umba::cli_tool_helpers::writeOutput( outputFilename, umba::cli_tool_helpers::IoFileType::regularFile // outputFileType
                                                   , encoding::ToUtf8(), encoding::FromUtf8()
                                                   , resText, std::string() // bomData
                                                   , true /* fromFile */, true /* utfSource */ , bOverwrite
                                                   );
            } // try
            catch(const std::runtime_error &e)
            {
                LOG_ERR << e.what() << "\n";
                return 4;
            }

        } // for(; fileIt!=foundFiles.end() && folderIt!=foundFilesRootFolders.end(); ++fileIt, ++folderIt)

        if (appConfig.clearGenerationCaches)
        {
            umbaLogStreamMsg << "Exiting - only clear caches action performed\n";
            return 0;
        }

        if (appConfig.batchOutputRoot.empty())
        {
            umba::filename::stripLastPathSep(calculatedCommonPath);
            for(auto &pi : pagesIndex)
            {
                pi.second.erase(0, calculatedCommonPath.size());
                umba::filename::stripFirstPathSep(pi.second);
            }
        }

        std::string langTag = findLangTagByString(commonLang);
        if (langTag.empty())
            langTag = "en";

        std::string pageIndexTitle = marty_tr::tr("title"   , "pages-index", langTag);
        std::string noTitle        = marty_tr::tr("no-title", "pages-index", langTag);

        if (!appConfig.batchPageIndexFileName.empty() && !pagesIndex.empty())
        {
            for(auto it=pagesIndex.begin(); it!=pagesIndex.end(); ++it)
            {
                if (it->first.empty())
                {
                    it->first = umba::fromUtf8(noTitle);
                }

                if (!appConfig.gitAddBatchFileName.empty())
                    gitAddFiles.emplace_back(it->second);
            }

            std::stable_sort( pagesIndex.begin(), pagesIndex.end()
                            , [](const std::pair<std::wstring, std::string> &p1, const std::pair<std::wstring, std::string> &p2)
                              {
                                  return p1.first<p2.first;
                              }
                            );

            std::string pagesIndexText;

            pagesIndexText.append("# ");
            pagesIndexText.append(pageIndexTitle);
            //pagesIndexText.append(1, '\n');
            pagesIndexText.append(1, '\n');
            //marty_tr::tr(const std::string &msgId, std::string catId, std::string langId)

            std::string firstSectionLetter;
            //         marty_utf
            // auto uch = (utf8_char_t)str[curPos];
            //
            // auto symbolNumBytes = getNumberOfBytesUtf8(uch);


            for(std::vector< std::pair<std::wstring, std::string> >::iterator it=pagesIndex.begin(); it!=pagesIndex.end(); ++it)
            {
                std::string thisPageTitleUtf = umba::toUtf8(it->first);
                if (!thisPageTitleUtf.empty())
                {
                    std::size_t firstCharLen = marty_utf::getNumberOfBytesUtf8(thisPageTitleUtf[0]);
                    if (firstCharLen<=thisPageTitleUtf.size())
                    {
                        std::string titleFirstLetter = std::string(thisPageTitleUtf, 0, firstCharLen);
                        if (firstSectionLetter!=titleFirstLetter && !titleFirstLetter.empty())
                        {
                            firstSectionLetter = titleFirstLetter;
                            if (appConfig.batchSplitPageIndex)
                            {
                                pagesIndexText.append("\n\n");
                                pagesIndexText.append(" - **");
                                pagesIndexText.append(firstSectionLetter);
                                pagesIndexText.append("**\n");
                            }
                        }
                    }
                }

                pagesIndexText.append("   - [");
                pagesIndexText.append(thisPageTitleUtf);
                pagesIndexText.append("](");
                pagesIndexText.append(umba::filename::makeCanonical(it->second, '/'));
                pagesIndexText.append(")\n");
            }

            auto targetPagesIndexText = marty_cpp::converLfToOutputFormat(pagesIndexText, appConfig.outputLinefeed);

            std::string pageIndexFileName = appConfig.batchPageIndexFileName;
            if (!umba::filename::isAbsPath(pageIndexFileName))
            {
                if (!appConfig.batchOutputRoot.empty())
                {
                    pageIndexFileName = umba::filename::appendPath(appConfig.batchOutputRoot, pageIndexFileName);
                }
                else
                {
                    pageIndexFileName = umba::filename::appendPath(calculatedCommonPath, pageIndexFileName);
                }
            }

            pageIndexFileName = umba::filename::makeCanonical(pageIndexFileName);

            infoLog << "Writting page index file: " << pageIndexFileName << "\n";

            umba::filesys::createDirectoryEx( umba::filename::getPath(pageIndexFileName), true /* forceCreatePath */ );

            umba::cli_tool_helpers::writeOutput( pageIndexFileName, umba::cli_tool_helpers::IoFileType::regularFile // outputFileType
                                               , encoding::ToUtf8(), encoding::FromUtf8()
                                               , targetPagesIndexText, std::string() // bomData
                                               , true /* fromFile */, true /* utfSource */ , bOverwrite
                                               );

            if (!appConfig.gitAddBatchFileName.empty())
                gitAddFiles.emplace_back(pageIndexFileName);

        } // if (!appConfig.batchPageIndexFileName.empty() && !pagesIndex.empty())

        if (!appConfig.copyImageFiles)
        {
           infoLog << "Copying image files not allowed. Use --copy-image-files/--copy-images option\n";
        }
        else if (appConfig.batchOutputRoot.empty())
        {
           infoLog << "Copying image files not allowed, target path is the same as source path. Use --batch-output-root\n";
        }
        else // if (!appConfig.batchOutputRoot.empty() && appConfig.copyImageFiles)
        {
            copyDocumentImageFiles(infoLog, imagesToCopy, bOverwrite, &gitAddFiles);
        }


        std::string gitAddText;

        if (useBatSyntax)
        {
            gitAddText.append("@echo off\n"
                              "setlocal enableextensions enabledelayedexpansion\n"
                              "for /f \"usebackq tokens=2 delims=:\" %%i in (`chcp`) do set SAVED_CODE_PAGE=%%~i\n"
                              "echo \"%SAVED_CODE_PAGE%\"\n"
                              "chcp 65001\n"
                             );
        }

        for(const auto addFile : gitAddFiles)
        {
            gitAddText.append("git add \"");
            gitAddText.append(umba::filename::makeCanonical(addFile, useBatSyntax?'\\':'/'));
            gitAddText.append("\"\n");
        }

        if (useBatSyntax)
        {
            gitAddText.append("chcp %SAVED_CODE_PAGE%\n");
        }



        if (!appConfig.gitAddBatchFileName.empty())
        {
            std::string gitAddBatchFileName = appConfig.gitAddBatchFileName;
            if (!umba::filename::isAbsPath(gitAddBatchFileName))
            {
                if (!appConfig.batchOutputRoot.empty())
                {
                    gitAddBatchFileName = umba::filename::appendPath(appConfig.batchOutputRoot, gitAddBatchFileName);
                }
                else
                {
                    gitAddBatchFileName = umba::filename::appendPath(calculatedCommonPath, gitAddBatchFileName);
                }
            }
            gitAddBatchFileName = umba::filename::makeCanonical(gitAddBatchFileName);


            gitAddText = marty_cpp::converLfToOutputFormat(gitAddText, appConfig.outputLinefeed);

            infoLog << "Writting 'git add' file: " << gitAddBatchFileName << "\n";

            umba::filesys::createDirectoryEx( umba::filename::getPath(gitAddText), true /* forceCreatePath */ );

            umba::cli_tool_helpers::writeOutput( gitAddBatchFileName, umba::cli_tool_helpers::IoFileType::regularFile // outputFileType
                                               , encoding::ToUtf8(), encoding::FromUtf8()
                                               , gitAddText, std::string() // bomData
                                               , true /* fromFile */, true /* utfSource */ , bOverwrite
                                               );
        }
        return 0;

    }
    else // !appConfig.isBatchMode()
    {
        if (!argsParser.quet  /* && !hasHelpOption */ )
        {
            umbaLogStreamMsg << "Single file mode\n";
        }

        if (inputFilename.empty())
        {
            LOG_ERR << "input file name not taken"
                        << "\n";
            return 5;
        }

        umba::cli_tool_helpers::IoFileType outputFileType = umba::cli_tool_helpers::IoFileType::nameEmpty;
        if (outputFilename.empty())
        {
            //outputFilename = "STDOUT";
            outputFilename = tryMakeOutputFilenameFromInput(inputFilename);
            outputFileType = umba::cli_tool_helpers::IoFileType::regularFile;
            //generateOutputFileNameFromInputFileName
        }

        if (outputFilename.empty())
        {
            outputFilename = "STDOUT";
        }

        outputFileType = umba::cli_tool_helpers::detectFilenameType(outputFilename, false /* !bInput */);

        if (outputFileType==umba::cli_tool_helpers::IoFileType::clipboard)
        {
            LOG_ERR << "invalid output file name"
                        << "\n";
            return 6;
        }

        if (!argsParser.quet)
        {
            umbaLogStreamMsg << ": "<<inputFilename<<"\n";
        }

        std::string inputFileText;
        //if (!umba::filesys::readFile(inputFilename, inputFileText))
        if (!AppConfig<std::string>::readInputFile(inputFilename, inputFileText))
        {
            LOG_ERR << umba::formatMessage("failed to read input file: '$(fileName)'")
                                              .arg("fileName",inputFilename)
                                              .toString()
                        << "\n";
            //errCount++;
            return 7;
        }

        UMBA_USED(lineNo);

        appConfig.setStrictPathFromFilename(inputFilename);



        //std::string
        curFile = inputFilename; // = fileName;
        //unsigned lineNo = 0;


        appConfig.checkFixRenderingTargetName(false /* forView */ );
        std::vector<std::string> foundOptionsFiles;
        findProjectOptionsFiles(curFile, appConfig.renderingTargetName, foundOptionsFiles);
        appConfig = applyProjectOptionsFiles(appConfig, argsParser, foundOptionsFiles);

        // All options applied
        if (appConfig.clearGenerationCaches)
        {
            appConfig.doClearGenerationCaches();
            return 0;
        }


        appConfig.checkAdjustDocNumericLevels();
        appConfig.checkTargetFormat();
        appConfig.checkUpdateEmptyGeneratedOutputRootByFilename(inputFilename);

        bool differentOutputPath = umba::filename::makeCanonicalForCompare(umba::filename::getPath(outputFilename)) != umba::filename::makeCanonicalForCompare(umba::filename::getPath(inputFilename));
        appConfig.singleModeInOutPathsDifferent = differentOutputPath;


        Document doc;
        std::string resText  = processMdFile(appConfig, inputFileText, inputFilename, doc);
        //std::string docTitle = doc.getDocumentTitleAny();

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


            if (!appConfig.copyImageFiles)
            {
               infoLog << "Copying image files not allowed. Use --copy-image-files/--copy-images option\n";
            }
            else if (!differentOutputPath)
            {
               infoLog << "Copying image files not allowed, target path is the same as source path.\n";
            }
            else // if (appConfig.copyImageFiles && differentOutputPath)
            {
                addImageFilesForCopying( imagesToCopy, inputFilename, outputFilename, doc.imageFiles);
                copyDocumentImageFiles(infoLog, imagesToCopy, bOverwrite);
            }

        } // try
        catch(const std::runtime_error &e)
        {
            LOG_ERR << e.what() << "\n";
            return 8;
        }


    }


    return 0;
}

