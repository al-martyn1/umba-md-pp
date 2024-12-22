/*! \file
    \brief Делаем для теста раскраску Protogen кода в HTML и вывод потокенно в текст
 */

#include "umba/umba.h"
//
#include "umba/tokenizer.h"
#include "umba/assert.h"
#include "umba/filename.h"
#include "umba/filesys.h"
#include "umba/tokenizer/token_filters.h"
#include "umba/app_main.h"
//
//#include "umba/tokenizer/lang/plantuml.h"
//
#include "umba/debug_helpers.h"

//
#include "umba/text_position_info.h"
#include "umba/iterator.h"
#include "umba/the.h"
//
#include "utils.h"
//
#include "marty_cpp/src_normalization.h"
//

#include <iostream>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <sstream>
#include <iomanip>

//
#include "smpd-tokenizer.h"


#include "css_style.h"



using std::cout;
using std::cerr;


bool highLightMode = false;
std::string inputFilename;




UMBA_MAIN()
{
    // auto t1 = getCharClassTable();
    // auto t2 = getTrieVector();
    // auto t3 = getString();
    // auto t4 = getIterator();

    using namespace umba::tokenizer;

    std::vector<std::string>  inputFiles;

    #if 0
    std::cout << "Callout ranges:";
    std::cout << std::hex << "\n";

    #define PRINT_SMPD_TOKEN_OPERATOR_CALLOUT_PAIR(a,b) std::cout << a << " - " << b << "\n"

    PRINT_SMPD_TOKEN_OPERATOR_CALLOUT_PAIR(SMPD_TOKEN_OPERATOR_CALLOUT_TO_LEFT_TOP         , SMPD_TOKEN_OPERATOR_CALLOUT_TO_LEFT_TOP_LAST    );
    PRINT_SMPD_TOKEN_OPERATOR_CALLOUT_PAIR(SMPD_TOKEN_OPERATOR_CALLOUT_TO_LEFT_BOTTOM      , SMPD_TOKEN_OPERATOR_CALLOUT_TO_LEFT_BOTTOM_LAST );
    PRINT_SMPD_TOKEN_OPERATOR_CALLOUT_PAIR(SMPD_TOKEN_OPERATOR_CALLOUT_TO_RIGHT_TOP        , SMPD_TOKEN_OPERATOR_CALLOUT_TO_RIGHT_TOP_LAST   );
    PRINT_SMPD_TOKEN_OPERATOR_CALLOUT_PAIR(SMPD_TOKEN_OPERATOR_CALLOUT_TO_RIGHT_BOTTOM     , SMPD_TOKEN_OPERATOR_CALLOUT_TO_RIGHT_BOTTOM_LAST);
    PRINT_SMPD_TOKEN_OPERATOR_CALLOUT_PAIR(SMPD_TOKEN_OPERATOR_CALLOUT_TO_LEFT             , SMPD_TOKEN_OPERATOR_CALLOUT_TO_LEFT_LAST        );
    PRINT_SMPD_TOKEN_OPERATOR_CALLOUT_PAIR(SMPD_TOKEN_OPERATOR_CALLOUT_TO_RIGHT            , SMPD_TOKEN_OPERATOR_CALLOUT_TO_RIGHT_LAST       );
    PRINT_SMPD_TOKEN_OPERATOR_CALLOUT_PAIR(SMPD_TOKEN_OPERATOR_CALLOUT_TO_TOP              , SMPD_TOKEN_OPERATOR_CALLOUT_TO_TOP_LAST         );
    PRINT_SMPD_TOKEN_OPERATOR_CALLOUT_PAIR(SMPD_TOKEN_OPERATOR_CALLOUT_TO_BOTTOM           , SMPD_TOKEN_OPERATOR_CALLOUT_TO_BOTTOM_LAST      );
    PRINT_SMPD_TOKEN_OPERATOR_CALLOUT_PAIR(SMPD_TOKEN_OPERATOR_CALLOUT_SHELF_TO_RIGHT      , SMPD_TOKEN_OPERATOR_CALLOUT_SHELF_TO_RIGHT_LAST );
    PRINT_SMPD_TOKEN_OPERATOR_CALLOUT_PAIR(SMPD_TOKEN_OPERATOR_CALLOUT_SHELF_TO_LEFT       , SMPD_TOKEN_OPERATOR_CALLOUT_SHELF_TO_LEFT_LAST  );
    PRINT_SMPD_TOKEN_OPERATOR_CALLOUT_PAIR(SMPD_TOKEN_OPERATOR_AFTER_CALLOUTS_FIRST        , SMPD_TOKEN_OPERATOR_AFTER_CALLOUTS_FIRST        );
    std::cout << "\n\n";
    #endif


    for(int argIdx=1; argIdx<argc; ++argIdx)
    {
        if (false)
        {
        }
        else
        {
            inputFiles.emplace_back(argv[argIdx]);
        }
    }

    inputFiles.clear();
    inputFiles.emplace_back("F:\\_github\\umba-tools\\umba-md-pp\\tests\\simple-drawing\\test01.smpd");

    if (umba::isDebuggerPresent() || inputFiles.empty())
    {
        std::string cwd = umba::filesys::getCurrentDirectory();
        std::cout << "Working Dir: " << cwd << "\n";
        std::string rootPath;

        #if (defined(WIN32) || defined(_WIN32))


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

            if (!rootPath.empty())
                rootPath = umba::filename::appendPathSepCopy(rootPath);

        #endif

        inputFiles.clear();

        //inputFiles.emplace_back(umba::filename::appendPath(rootPath, std::string("tests/suffix_gluing_sample.h")));
        inputFiles.emplace_back("C:\\work\\projects\\auxware\\protogen\\tests\\test01.ptg");
        //inputFiles.emplace_back("F:\\_github\\umba-tools\\protogen\\tests\\test01.ptg");

        
        
    }


    auto tokenizerBuilder = protogen::makeTokenizerBuilderProtogen<char>();
    using TokenizerBuilderType = decltype(tokenizerBuilder);

    //using tokenizer_type         = std::decay<decltype(tokenizer)>;
    using tokenizer_type         = typename TokenizerBuilderType::tokenizer_type;
    using InputIteratorType      = typename tokenizer_type::iterator_type;
    using tokenizer_char_type    = typename tokenizer_type::value_type;
    using messages_string_type   = typename tokenizer_type::messages_string_type;
    using token_parsed_data_type = typename tokenizer_type::token_parsed_data_type;



    std::ostringstream oss;
    //auto &oss = std::cout;
    bool bOk = true;
    // bool inPreprocessor = false;

    auto tokenHandler =     [&]( auto &tokenizer
                               , bool bLineStart, payload_type tokenType
                               , InputIteratorType b, InputIteratorType e
                               , token_parsed_data_type parsedData // std::basic_string_view<tokenizer_char_type> parsedData
                               , messages_string_type &errMsg
                               ) -> bool
                            {
                                UMBA_USED(parsedData);
                                UMBA_USED(errMsg);
                                UMBA_USED(tokenizer);

                                if (tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN || tokenType==UMBA_TOKENIZER_TOKEN_CTRL_RST)
                                {
                                    return true;
                                }

                                if (highLightMode)
                                {
                                    printTokenHtml(oss, tokenType, b, e);
                                }
                                else
                                {
                                    printToken(oss, tokenType, b, e, bLineStart);
                                }
                                return true;
                            };


    auto tokenizer = protogen::makeTokenizerProtogen(tokenizerBuilder, tokenHandler);
    //tokenizer.setRawMode()


    // it может быть равен end, поэтому тут дополнительно передаётся itEnd
    // Но надо брать не [it,itEnd), а только it
    tokenizer.unexpectedHandler = [&](auto &tokenizer, InputIteratorType it, InputIteratorType itEnd, const char* srcFile, int srcLine) -> bool
                             {
                                 UMBA_USED(tokenizer);
                                 printError(std::cout, inputFilename, UMBA_TOKENIZER_TOKEN_UNEXPECTED, it, itEnd, srcFile, srcLine);
                                 return false;
                             };

    tokenizer.reportUnknownOperatorHandler = [&](auto &tokenizer, InputIteratorType b, InputIteratorType e)
                             {
                                 //std::cout << "Possible unknown operator: '" << umba::iterator::makeString(b, e) << "'\n";
                                 UMBA_USED(tokenizer); UMBA_USED(b); UMBA_USED(e);
                             };

    tokenizer.reportStringLiteralMessageHandler = [&](auto &tokenizer, bool bErr, InputIteratorType it, const messages_string_type &msg)
                             {
                                 //std::cout << "String: '" << umba::iterator::makeString(it) << "': " << msg << "\n";
                                 UMBA_USED(tokenizer); UMBA_USED(bErr); UMBA_USED(it); UMBA_USED(msg);
                             };



    if (inputFiles.empty())
    {
        std::cout << "No input files taken\n";
        return 1;
    }

    #if defined(WIN32) || defined(_WIN32)
        marty_cpp::ELinefeedType outputLinefeed = marty_cpp::ELinefeedType::crlf;
    #else
        marty_cpp::ELinefeedType outputLinefeed = marty_cpp::ELinefeedType::lf;
    #endif


    for(const auto &fn: inputFiles)
    {
        inputFilename = fn;

        if (inputFilename=="--")
        {
            highLightMode = !highLightMode;
            continue;
        }

        std::string text;

        std::cout << "\nProcessing: '" << inputFilename << "'\n";

        if (!umba::filesys::readFile(inputFilename, text))
        {
            std::cout << "Failed to read input file\n";
            continue;
        }

        if (text.empty())
        {
            std::cout << "Input file is empty\n";
            continue;
        }

        if (text.back()!='\n' && text.back()!='\r')
        {
            std::cout << "Warning: no linefeed at end of file\n";
        }

        oss = std::ostringstream();
        bOk = true;

        if (highLightMode)
            oss<<"<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"utf-8\"/>\n<style>\n" << cssStyle << "\n</style>\n</head>\n<body>\n<pre>\n";

#if defined(USE_TRY_CATCH)
        try
        {
#endif
            auto itBegin = InputIteratorType(text.data(), text.size());
            auto itEnd   = InputIteratorType();
            tokenizer.tokenizeInit();
            InputIteratorType it = itBegin;
            for(; it!=itEnd && bOk; ++it)
            {
                if (!tokenizer.tokenize(it, itEnd))
                {
                    bOk = false;
                }
            }

            if (bOk)
            {
                bOk = tokenizer.tokenizeFinalize(itEnd);
            }

            //if (bOk)
            {
                if (highLightMode)
                    oss<<"</pre>\n</body>\n</html>\n";

                if (inputFilename.empty())
                {
                    std::cout << oss.str() << "\n";
                }
                else
                {
#if defined(DUPLICATE_TO_STD_OUT)
                    std::cout << oss.str() << "\n";
#endif
                    auto resultText = marty_cpp::converLfToOutputFormat(oss.str(), outputLinefeed);
                    auto outputFilename = umba::filename::replaceExtention(inputFilename, std::string((highLightMode?"html":"txt")) );
                    std::cout << "Writting output to '" << outputFilename << "' - ";
                    if (!umba::filesys::writeFile(outputFilename, resultText, true /* overwrite */ ))
                        std::cout << "Failed";
                    else
                        std::cout << "Success";
                    std::cout << "\n";
                }

            }

#if defined(USE_TRY_CATCH)
        }
        catch(const std::exception &e)
        {
#if defined(DUPLICATE_TO_STD_OUT)
                    std::cout << oss.str() << "\n";
#endif

            // !!! Inform about exception
            auto resultText = marty_cpp::converLfToOutputFormat("!!!"  + oss.str(), outputLinefeed);
            auto outputFilename = umba::filename::replaceExtention(inputFilename, std::string("html"));
            umba::filesys::writeFile(outputFilename, resultText, true /* overwrite */ );

            std::cout << "!!!\n";
        }
#endif

        #if 0
        std::string text;

        std::cout << "\nProcessing: '" << inputFilename << "'\n";

        if (!umba::filesys::readFile(inputFilename, text))
        {
            std::cout << "Failed to read input file\n";
            continue;
        }

        if (text.empty())
        {
            std::cout << "Input file is empty\n";
            continue;
        }

        if (text.back()!='\n' && text.back()!='\r')
        {
            std::cout << "Warning: no linefeed at end of file\n";
        }

        //oss = std::ostringstream();
        bOk = true;
        oss << "---------" << "\n";

        //oss<<"<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"utf-8\"/>\n<style>\n" << cssStyle << "\n</style>\n</head>\n<body>\n<pre>\n";

        try
        {
            auto itBegin = InputIteratorType(text.data(), text.size());
            auto itEnd   = InputIteratorType();
            tokenizer.tokenizeInit();
            InputIteratorType it = itBegin;
            for(; it!=itEnd && bOk; ++it)
            {
                if (!tokenizer.tokenize(it, itEnd))
                {
                    bOk = false;
                }
            }

            if (bOk)
            {
                bOk = tokenizer.tokenizeFinalize(itEnd);
            }

            //if (bOk)
            {
                //oss<<"</pre>\n</body>\n</html>\n";

                if (inputFilename.empty())
                {
                    //std::cout << oss.str() << "\n";
                }
                else
                {
// #if defined(DUPLICATE_TO_STD_OUT)
//                     std::cout << oss.str() << "\n";
// #endif
//                     auto resultText = marty_cpp::converLfToOutputFormat(oss.str(), outputLinefeed);
//                     auto outputFilename = umba::filename::replaceExtention(inputFilename, std::string("html"));
//                     std::cout << "Writting output to '" << outputFilename << "' - ";
//                     if (!umba::filesys::writeFile(outputFilename, resultText, true /* overwrite */ ))
//                         std::cout << "Failed";
//                     else
//                         std::cout << "Success";
//                     std::cout << "\n";
                }

            }

        }
        catch(const std::exception &e)
        {
            // // !!! Inform about exception
            // auto resultText = marty_cpp::converLfToOutputFormat("!!!"  + oss.str(), outputLinefeed);
            // auto outputFilename = umba::filename::replaceExtention(inputFilename, std::string("html"));
            // umba::filesys::writeFile(outputFilename, resultText, true /* overwrite */ );

            std::cout << "!!!\n";
        }

        oss << "---------" << "\n";
        #endif

    }

    return bOk ? 0 : 1;

}


