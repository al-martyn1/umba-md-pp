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

#include <iostream>
#include <iomanip>
#include <string>
#include <filesystem>

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
#include "umba/scanners.h"
#include "umba/filesys.h"
#include "umba/filename.h"
#include "umba/format_message.h"

//
#include "encoding/encoding.h"


#if defined(WIN32) || defined(_WIN32)

    #define HAS_CLIPBOARD_SUPPORT 1
    #include "umba/clipboard_win32.h"

#endif


#include "app_config.h"




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

AppConfig appConfig;

// Конфиг версии
#include "app_ver_config.h"
// Принтуем версию
#include "print_ver.h"
// Парсер параметров командной строки
#include "arg_parser.h"



int main(int argc, char* argv[])
{

    using namespace umba::omanip;


    auto argsParser = umba::command_line::makeArgsParser( ArgParser()
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
        argsParser.args.push_back("@../umba-tr.rsp");
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

    std::string inputFileText;
    if (!umba::filesys::readFile(inputFilename, inputFileText))
    {
        LOG_ERR_OPT << umba::formatMessage("failed to read input file: '$(fileName)'")
                                          .arg("fileName",inputFilename)
                                          .toString()
                    << "\n";
        //errCount++;
        return 1;
    }


    std::string curFile = inputFilename; // = fileName;
    unsigned lineNo = 0;

    inputFileText = AppConfig::autoEncodeToUtf(inputFileText);



    //TODO: !!! если файл существует, его надо обнулить

    std::string resultText = marty_cpp::converLfToOutputFormat(inputFileText, appConfig.outputLinefeed);



    try
    {
        if (!argsParser.quet)
        {
            umbaLogStreamMsg << "Writting output to: "<<outputFilename<<"\n";
        }

        umba::cli_tool_helpers::writeOutput( outputFilename, outputFileType
                                           , encoding::ToUtf8(), encoding::FromUtf8()
                                           , resultText, std::string() // bomData
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

