/*! \file
    \brief
 */

#include "umba/umba.h"
//
#include "umba/tokenizer.h"
#include "umba/assert.h"
#include "umba/filename.h"
#include "umba/filesys.h"
//
#include "umba/debug_helpers.h"

//
#include "umba/text_position_info.h"
#include "umba/iterator.h"
#include "umba/the.h"

#include <iostream>
#include <map>
#include <deque>

using std::cout;
using std::cerr;


int main(int argc, char* argv[])
{
    using namespace umba::tokenizer;

    std::string text;
    std::string inputFilename;
    if (argc>1)
    {
        inputFilename = argv[1];
    }

    if (umba::isDebuggerPresent() || inputFilename.empty())
    {
        std::string cwd = umba::filesys::getCurrentDirectory<std::string>();
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


        // inputFilename = umba::filename::appendPath(rootPath, std::string("_libs/umba/the.h"));
        // inputFilename = umba::filename::appendPath(rootPath, std::string("_libs/umba/stl_keil_initializer_list.h"));
        // inputFilename = umba::filename::appendPath(rootPath, std::string("_libs/umba/stl_keil_type_traits.h"));
        // inputFilename = umba::filename::appendPath(rootPath, std::string("_libs/umba/string_plus.h"));
        // inputFilename = umba::filename::appendPath(rootPath, std::string("_libs/umba/rgbquad.h"));

        // inputFilename = umba::filename::appendPath(rootPath, std::string("_libs/umba/"));

        inputFilename = umba::filename::appendPath(rootPath, std::string("_libs/marty_decimal/tests/src/regression_tests.cpp"));
        
    }

#if 1

    if (!umba::filesys::readFile(inputFilename, text))
    {
        std::cout << "Failed to read input file: " << inputFilename << "\n";
        return 1;
    }

#else
        // text = "    { return __ils.begin(); }\n";
        // text = "blue  = (argb    ) & 0xFFu;";
        // text = " i--/* comment */; \n";
        // text = " i--// comment\n; \n";
        text = "    DECIMAL_CTOR_TEST( 3.1415  1415 0x1415   ,     \"3.1415\" );";

#endif

    cout << "Input file: " << inputFilename << "\n";
    std::cout << "---\n";



    payload_type numberTokenId = UMBA_TOKENIZER_TOKEN_NUMBER_USER_LITERAL_FIRST;

    umba::tokenizer::CppEscapedSimpleQuotedStringLiteralParser<char>  cppEscapedSimpleQuotedStringLiteralParser;

    auto tokenizer = TokenizerBuilder<char>().addNumbersPrefix("0b", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_BIN)                                                     
                                             .addNumbersPrefix("0B", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_BIN)                                                     

                                             .addNumbersPrefix("0d", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DEC)                                                     
                                             .addNumbersPrefix("0D", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DEC)                                                     

                                             .addNumbersPrefix("4x", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_QUAT)                                                     
                                             .addNumbersPrefix("4X", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_QUAT)                                                     

                                             .addNumbersPrefix("0" , numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_OCT | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FLAG_MISS_DIGIT)

                                             .addNumbersPrefix("12x", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DUOD)                                                    
                                             .addNumbersPrefix("12X", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DUOD)                                                    

                                             .addNumbersPrefix("0x", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_HEX)                                                      
                                             .addNumbersPrefix("0X", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_HEX)                                                      


                                             .addBrackets("{}", UMBA_TOKENIZER_TOKEN_CURLY_BRACKETS )
                                             .addBrackets("()", UMBA_TOKENIZER_TOKEN_ROUND_BRACKETS )
                                             .addBrackets("<>", UMBA_TOKENIZER_TOKEN_ANGLE_BRACKETS )
                                             .addBrackets("[]", UMBA_TOKENIZER_TOKEN_SQUARE_BRACKETS)


                                             .addSingleLineComment("//", UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST)
                                             .setMultiLineComment("/*", "*/")


                                             .addOperators(UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST_GENERIC, std::vector<std::string>{".","...",".*","+","-","*","/","%","^","&","|","~","!","=","<",">","+=","-=","*=","/=","%=","^=","&=","|=","<<",">>",">>=","<<=","==","!=","<=",">=","<=>","&&","||","++","--",",","->*","->",":","::",";","?"})


                                             .addStringLiteralParser("\'", &cppEscapedSimpleQuotedStringLiteralParser)
                                             .addStringLiteralParser("\"", &cppEscapedSimpleQuotedStringLiteralParser)


                                             .makeTokenizer();

// using PosCountingIterator = umba::iterator::TextPositionCountingIterator<char>;

    //using tokenizer_type      = std::decay<decltype(tokenizer)>;
    using tokenizer_type      = decltype(tokenizer);
    using InputIteratorType   = typename tokenizer_type::iterator_type;
    using tokenizer_char_type = typename tokenizer_type::value_type;

    tokenizer.tokenHandler = [&](payload_type tokenType, InputIteratorType b, InputIteratorType e, std::basic_string_view<tokenizer_char_type> parsedData) -> void
                             {
                                 using namespace umba::iterator;
                         
                                 auto curPos = b.getPosition(); // Выводим позицию начала токена
                         
                                 //cout << umba::tokenizer::utils::iterator_getRawValueTypePointer(b) << "\n";
                         
                                 cout << "token_type: " << tokenType;
                                 if (tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
                                 {
                                     cout << ", linefeed, '\\n'"; // "CRLF";
                                 }
                                 else if (tokenType==UMBA_TOKENIZER_TOKEN_SPACE)
                                 {
                                     cout << ", " << (*b==' '?"spaces":"tabs") << ", len=" << makeStringView(b, e).size();
                                 }
                                 else
                                 {
                                     cout << ", ";
                                     if (tokenType==UMBA_TOKENIZER_TOKEN_ESCAPE || tokenType==UMBA_TOKENIZER_TOKEN_LINE_CONTINUATION)
                                         cout << umba::escapeStringC(getTokenizerTokenStr<std::string>(tokenType));
                                     else
                                        cout << getTokenizerTokenStr<std::string>(tokenType);
                                     cout << ", '" << makeString(b, e) << "'";
                                 }
                         
                                 cout << /*", state: " << getStateStr(st) <<*/ ", in data location " << curPos.toString<std::string>() << "\n";
                             };


    auto itBegin = InputIteratorType(text.data(), text.size());
    auto itEnd   = InputIteratorType();
    bool bOk     = true;
    for( InputIteratorType it=itBegin; it!=itEnd; ++it)
    {
        if (!tokenizer.tokenize(it, itEnd))
        {
            bOk = false;
            break;
        }
    }

    if (bOk)
    {
        bOk = tokenizer.tokenizeFinalize(itEnd);
    }



#if 0
public: // handler types

    using token_handler_type                         = std::function<void(payload_type, InputIteratorType, InputIteratorType, std::basic_string_view<value_type>)>;
    using unexpected_handler_type                    = std::function<bool(InputIteratorType, const char*, int)>;
    using report_unknown_operator_handler_type       = std::function<void(InputIteratorType,InputIteratorType)>;
    using report_string_literal_message_handler_type = std::function<void(bool, InputIteratorType, const MessagesStringType &)>;


//------------------------------
public: // handlers

    token_handler_type                          tokenHandler                     ;
    unexpected_handler_type                     unexpectedHandler                ;
    report_unknown_operator_handler_type        reportUnknownOperatorHandler     ;
    report_string_literal_message_handler_type  reportStringLiteralMessageHandler;
#endif



}


