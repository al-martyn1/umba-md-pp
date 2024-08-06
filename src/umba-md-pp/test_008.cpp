/*! \file
    \brief Делаем для теста раскраску плюсового кода в HTML
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

using std::cout;
using std::cerr;



template<typename StringType>
StringType getTokenKindString(umba::tokenizer::payload_type p)
{
    switch(p)
    {
        case UMBA_TOKENIZER_TOKEN_UNEXPECTED                       : return umba::string_plus::make_string<StringType>("unxp");
        case UMBA_TOKENIZER_TOKEN_LINEFEED                         : return umba::string_plus::make_string<StringType>("");
        case UMBA_TOKENIZER_TOKEN_SPACE                            : return umba::string_plus::make_string<StringType>("");
        case UMBA_TOKENIZER_TOKEN_IDENTIFIER                       : return umba::string_plus::make_string<StringType>("ident");
        case UMBA_TOKENIZER_TOKEN_SEMIALPHA                        : return umba::string_plus::make_string<StringType>("");
        // https://en.wikipedia.org/wiki/Bracket
        case UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_OPEN               : return umba::string_plus::make_string<StringType>("curly");
        case UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_CLOSE              : return umba::string_plus::make_string<StringType>("curly");
        case UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN               : return umba::string_plus::make_string<StringType>("round");
        case UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE              : return umba::string_plus::make_string<StringType>("round");
        case UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN               : return umba::string_plus::make_string<StringType>("angle");
        case UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE              : return umba::string_plus::make_string<StringType>("angle");
        case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN              : return umba::string_plus::make_string<StringType>("square");
        case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE             : return umba::string_plus::make_string<StringType>("square");
        case UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_START: return umba::string_plus::make_string<StringType>("cmnt");
        case UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_END  : return umba::string_plus::make_string<StringType>("cmnt");
        //case : return umba::string_plus::make_string<StringType>("");
        default:

            if (p>=UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FIRST && p<=UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_LAST)
            {
                if (p&UMBA_TOKENIZER_TOKEN_FLOAT_FLAG)
                    return umba::string_plus::make_string<StringType>("num");
                else
                    return umba::string_plus::make_string<StringType>("num");
            }

            if (p>=UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST && p<=UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_LAST)
                return umba::string_plus::make_string<StringType>("cmnt");

            if (p>=UMBA_TOKENIZER_TOKEN_NUMBER_USER_LITERAL_FIRST && p<=UMBA_TOKENIZER_TOKEN_NUMBER_USER_LITERAL_LAST)
                return umba::string_plus::make_string<StringType>("num");

            if (p>=UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST && p<=UMBA_TOKENIZER_TOKEN_OPERATOR_LAST)
                return umba::string_plus::make_string<StringType>("op");

            if (p>=UMBA_TOKENIZER_TOKEN_STRING_LITERAL_FIRST && p<=UMBA_TOKENIZER_TOKEN_STRING_LITERAL_LAST)
                return umba::string_plus::make_string<StringType>("str");

            if (p>=UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST && p<=UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_LAST)
                return umba::string_plus::make_string<StringType>("kwd1");

            if (p>=UMBA_TOKENIZER_TOKEN_KEYWORD_SET2_FIRST && p<=UMBA_TOKENIZER_TOKEN_KEYWORD_SET2_LAST)
                return umba::string_plus::make_string<StringType>("kwd2");

            if (p>=UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_FIRST && p<=UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_LAST)
                return umba::string_plus::make_string<StringType>("kwd3");

            if (p>=UMBA_TOKENIZER_TOKEN_KEYWORD_SET4_FIRST && p<=UMBA_TOKENIZER_TOKEN_KEYWORD_SET4_LAST)
                return umba::string_plus::make_string<StringType>("kwd4");

            return umba::string_plus::make_string<StringType>("");
    }
}

/*
  color:blue
}

VSCode statements AF00DB
       keywords   0000FF
       known types 267F99
       brackets    319331
       comment     008000
       highlighted str 008000
       str         A31515

.m .com {
  color:green
}
.m .str {
  color:#8b0000
}
.m .lineQuote {
  color:#c1e5b9
}
.m .lineQuote.level1 {
  color:#137900
}
.m .lineQuote.level2 {
  color:#74b967
}
.m .lineQuote.level3 {
  color:#9fd095
}
.m .tagline {
  color:#d3acac;
  text-indent:5mm;
  padding-top:5mm
}
.m TABLE.formatter {
  margin:5px
}
.m TH.formatter {
  font-family:Verdana,Geneva,sans-serif;
  background-color:#d4d4d4;
  font-size:x-small
}

*/

auto cssStyle = R"--(
.unxp{
}

.ident{
}

.curly{
}

.round{
}

.angle{
}

.square{
}

.cmnt{
  color:olive;
  font-style: italic;
}

.num{
}

.op{
  font-weight: bold;
}

.str{
  color:DarkRed;

}

.kwd1{
  color:blue;
}

.kwd2{
  color:green;
}

)--";




struct TokenInfo
{
    umba::tokenizer::payload_type                        tokenType;
    umba::iterator::TextPositionCountingIterator<char>   b;
    umba::iterator::TextPositionCountingIterator<char>   e;
};


template<typename StreamType, typename InputIteratorType>
StreamType& printToken(StreamType &ss, umba::tokenizer::payload_type tokenType, InputIteratorType b, InputIteratorType e)
{
    auto kindStr = getTokenKindString<std::string>(tokenType);
    if (kindStr.empty())
    {
        ss << umba::escapeStringXmlHtml(umba::iterator::makeString(b, e));
    }
    else
    {
        ss << "<span class=\"" << kindStr << "\">" << umba::escapeStringXmlHtml(umba::iterator::makeString(b, e)) << "</span>";
    }

    return ss;
}





int main(int argc, char* argv[])
{
    // auto t1 = getCharClassTable();
    // auto t2 = getTrieVector();
    // auto t3 = getString();
    // auto t4 = getIterator();

    using namespace umba::tokenizer;

    std::vector<std::string>  inputFiles;

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

    if (umba::isDebuggerPresent() || inputFiles.empty())
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

        inputFiles.clear();
        inputFiles.emplace_back(umba::filename::appendPath(rootPath, std::string("_libs/umba/the.h")));
        // inputFiles.emplace_back(umba::filename::appendPath(rootPath, std::string("_libs/umba/stl_keil_initializer_list.h")));
        // inputFiles.emplace_back(umba::filename::appendPath(rootPath, std::string("_libs/umba/stl_keil_type_traits.h")));
        // inputFiles.emplace_back(umba::filename::appendPath(rootPath, std::string("_libs/umba/string_plus.h")));
        inputFiles.emplace_back(umba::filename::appendPath(rootPath, std::string("_libs/umba/rgbquad.h")));

        // inputFiles.emplace_back(umba::filename::appendPath(rootPath, std::string("_libs/umba/")));

        // inputFiles.emplace_back(umba::filename::appendPath(rootPath, std::string("_libs/marty_decimal/tests/src/regression_tests.cpp")));

    }




    payload_type numberTokenId = UMBA_TOKENIZER_TOKEN_NUMBER_USER_LITERAL_FIRST;

    umba::tokenizer::CppEscapedSimpleQuotedStringLiteralParser<char>  cppEscapedSimpleQuotedStringLiteralParser;

    auto tokenizer = TokenizerBuilder<char>().generateStandardCharClassTable()

                                             .addNumbersPrefix("0b", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_BIN)
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


    // Пока с kind не определился, и пихаем в keywords всё
    std::unordered_map<std::string, int> cppKeywords;
    std::unordered_map<std::string, int> cppPreprocessorKeywords;
    {
        // https://en.cppreference.com/w/cpp/keyword
        std::vector<std::string> allCppKeywords = { "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept", "auto"
                                                  , "bitand", "bitor", "bool", "break"
                                                  , "case", "catch", "char", "char8_t", "char16_t", "char32_t", "class", "compl", "concept", "const", "consteval"
                                                  , "constexpr", "constinit", "const_cast", "continue", "co_await", "co_return", "co_yield"
                                                  , "decltype", "default", "delete", "do", "double", "dynamic_cast"
                                                  , "else", "enum", "explicit", "export", "extern"
                                                  , "false", "float", "for", "friend"
                                                  , "goto"
                                                  , "if", "inline", "int"
                                                  , "long", "mutable"
                                                  , "namespace", "new", "noexcept", "not", "not_eq", "nullptr"
                                                  , "operator", "or", "or_eq", "private", "protected", "public"
                                                  , "reflexpr", "register", "reinterpret_cast", "requires", "return"
                                                  , "short", "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "synchronized"
                                                  , "template", "this", "thread_local", "throw", "true", "try", "typedef", "typeid", "typename"
                                                  , "union", "unsigned", "using", "virtual", "void", "volatile"
                                                  , "wchar_t", "while", "xor", "xor_eq"
                                                  // identifiers with special meaning
                                                  , "final", "override", "transaction_safe", "transaction_safe_dynamic", "import", "module"
                                                  };
        for(const auto &kw : allCppKeywords)
        {
            cppKeywords[kw] = 0;
        }
    }

    {
        std::vector<std::string> keywords = { "if", "elif", "else", "endif", "ifdef", "ifndef", "elifdef", "elifndef", "define", "undef", "include", "line"
                                            , "error", "warning", "pragma", "defined", "__has_include", "__has_cpp_attribute", "export", "import", "module"
                                            };

        for(const auto &kw : keywords)
        {
            cppPreprocessorKeywords[kw] = 0;
        }

        cppPreprocessorKeywords["define"] = 1;

    }




    std::string inputFilename;
    std::ostringstream oss;
    bool bOk = true;

    //using tokenizer_type      = std::decay<decltype(tokenizer)>;
    using tokenizer_type       = decltype(tokenizer);
    using InputIteratorType    = typename tokenizer_type::iterator_type;
    using tokenizer_char_type  = typename tokenizer_type::value_type;
    using messages_string_type = typename tokenizer_type::messages_string_type;


    std::vector<TokenInfo> bufferedTokens;

    tokenizer.tokenHandler = [&](bool bLineStart, payload_type tokenType, InputIteratorType b, InputIteratorType e, std::basic_string_view<tokenizer_char_type> parsedData, messages_string_type &errMsg) -> bool
                             {
                                 if (tokenType==UMBA_TOKENIZER_TOKEN_FIN)
                                     return true;

                                 if (tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
                                 {
                                     oss << "\n";
                                     return true;
                                 }

                                 if (tokenType==UMBA_TOKENIZER_TOKEN_LINE_CONTINUATION)
                                 {
                                     oss << "\\\n";
                                     return true;
                                 }

                                 if (tokenType==UMBA_TOKENIZER_TOKEN_IDENTIFIER)
                                 {
                                     auto identStr = umba::iterator::makeString(b, e);
                                     if (cppKeywords.find(identStr)!=cppKeywords.end())
                                         tokenType = UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST;
                                     else if (cppKeywords.find(identStr)!=cppKeywords.end())
                                         tokenType = UMBA_TOKENIZER_TOKEN_KEYWORD_SET2_FIRST;
                                 }

                                 printToken(oss, tokenType, b, e);

                                 return true;

                                 #if 0
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

                                 cout << /*", state: " << getStateStr(st) <<*/ ", in data location " << curPos.toString<std::string>() ;
                                 cout << (bLineStart?"*** Line start":"") << "\n";
                                 #endif
                             };

    tokenizer.unexpectedHandler = [&](InputIteratorType it, InputIteratorType itEnd, const char* srcFile, int srcLine) -> bool
                             {
                                 #if 0
                                 if (it==itEnd)
                                 {
                                     cout << "Unexpected end of file";
                                     return false;
                                 }

                                 auto errPos = it.getPosition(true); // с поиском конца строки (а вообще не надо пока, но пусть)
                                 std::string erroneousLineText = umba::iterator::makeString(it.getLineStartIterator(), it.getLineEndIterator());
                                 cout << "Unexpected at " << inputFilename << ":" << errPos.toString<std::string>() << "\n";
                                 cout << "Line:" << erroneousLineText << "\n";
                                 auto errMarkerStr = std::string(erroneousLineText.size(), ' ');
                                 if (errPos.symbolOffset>=errMarkerStr.size())
                                     errMarkerStr.append(1,'^');
                                 else
                                     errMarkerStr[errPos.symbolOffset] = '^';
                                 cout << "    |" << errMarkerStr << "|\n";

                                 if (it!=InputIteratorType())
                                 {
                                     char ch = *it;
                                     cout << "ch: " << umba::escapeStringC(std::string(1,ch)) << "\n";
                                 }
                                 cout << "At " << srcFile << ":" << srcLine << "\n";
                                 //cout << "State: " << getStateStr(st) << "\n";
                                 return false;
                                 #endif

                                 return false;
                             };

    tokenizer.reportUnknownOperatorHandler = [&](InputIteratorType b, InputIteratorType e)
                             {
                                 //cout << "Possible unknown operator: '" << umba::iterator::makeString(b, e) << "'\n";
                             };

    tokenizer.reportStringLiteralMessageHandler = [&](bool bErr, InputIteratorType it, const messages_string_type &msg)
                             {
                                 #if 0
                                 auto errPos = it.getPosition(true); // с поиском конца строки (а вообще не надо пока, но пусть)
                                 std::string erroneousLineText = umba::iterator::makeString(it.getLineStartIterator(), it.getLineEndIterator());
                                 cout << (bErr ? "Error: " : "Warning: ") << msg << " at " << inputFilename << ":" << errPos.toString<std::string>() << "\n";
                                 cout << "Line:" << erroneousLineText << "\n";
                                 auto errMarkerStr = std::string(erroneousLineText.size(), ' ');
                                 if (errPos.symbolOffset>=errMarkerStr.size())
                                     errMarkerStr.append(1,'^');
                                 else
                                     errMarkerStr[errPos.symbolOffset] = '^';
                                 cout << "    |" << errMarkerStr << "|\n";
                                 #endif
                             };

    //
    // auto reportPossibleUnknownOperatorLambda = [&](const PosCountingIterator b, const PosCountingIterator &e)
    // {
    //     cout << "Possible unknown operator: '" << makeString(b, e) << "'\n";
    // };
    //
    // auto reportStringLiteralMessageLambda = [&](bool bErr, const PosCountingIterator it, const std::string msg)
    // {
    //     auto errPos = it.getPosition(true); // с поиском конца строки (а вообще не надо пока, но пусть)
    //     std::string erroneousLineText = umba::iterator::makeString(it.getLineStartIterator(), it.getLineEndIterator());
    //     cout << (bErr ? "Error: " : "Warning: ") << msg << " at " << inputFilename << ":" << umba::makeSimpleTextPositionInfoString<std::string>(errPos) << "\n";
    //     cout << "Line:" << erroneousLineText << "\n";
    //     auto errMarkerStr = std::string(erroneousLineText.size(), ' ');
    //     if (errPos.symbolOffset>=errMarkerStr.size())
    //         errMarkerStr.append(1,'^');
    //     else
    //         errMarkerStr[errPos.symbolOffset] = '^';
    //     cout << "    |" << errMarkerStr << "|\n";
    // };

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


    for(const auto fn: inputFiles)
    {
        inputFilename = fn;

        std::string text;

        if (!umba::filesys::readFile(inputFilename, text))
        {
            std::cout << "Failed to read input file '" << inputFilename << "'\n";
            continue;
        }

        if (text.empty())
        {
            std::cout << "Input file '" << inputFilename << "' empty\n";
            continue;
        }

        if (text.back()!='\n' && text.back()!='\r')
        {
            std::cout << "Warning: file '" << inputFilename << "': no linefeed at end of file\n";
        }

        oss = std::ostringstream();
        bOk = true;

        oss<<"<html>\n<head>\n<meta charset=\"utf-8\"/>\n<style>\n" << cssStyle << "\n</style>\n</head>\n<body>\n<pre>\n";

        auto itBegin = InputIteratorType(text.data(), text.size());
        auto itEnd   = InputIteratorType();
        tokenizer.tokenizeInit();
        for( InputIteratorType it=itBegin; it!=itEnd && bOk; ++it)
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

        if (bOk)
        {
            oss<<"</pre>\n</body>\n</html>\n";

            auto resultText = marty_cpp::converLfToOutputFormat(oss.str(), outputLinefeed);
            auto outputFilename = umba::filename::replaceExtention(inputFilename, std::string("html"));
            std::cout << "Writting output to '" << outputFilename << "' - ";
            if (!umba::filesys::writeFile(outputFilename, resultText, true /* overwrite */ ))
                std::cout << "Failed";
            else
                std::cout << "Success";
            std::cout << "\n";
        }

    }


    // cout << "Input file: " << inputFilename << "\n";
    // std::cout << "---\n";



    //tikenizeInit()


    return bOk ? 0 : 1;

}


