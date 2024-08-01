/*! \file
    \brief
 */

#define UMBA_TOKENIZER_DISABLE_UMBA_TOKENIZER_GET_CHAR_CLASS_FUNCTION


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


//umba::TextPositionInfo

#if 0
enum class CharClass : char_class_underlying_uint_t
{
    // Это не флаг, не обрабатываем
    none             = UMBA_TOKENIZER_CHARCLASS_NONE            ,

    // таким флагом обозначаем пользовательские префиксы для спец обработчиков
    user_flag        = UMBA_TOKENIZER_CHARCLASS_USER_FLAG       ,

    // Не проверяем
    nonprintable     = UMBA_TOKENIZER_CHARCLASS_NONPRINTABLE    ,


    linefeed         = UMBA_TOKENIZER_CHARCLASS_LINEFEED        ,
    space            = UMBA_TOKENIZER_CHARCLASS_SPACE           ,

    opchar           = UMBA_TOKENIZER_CHARCLASS_OPCHAR          ,

    open             = UMBA_TOKENIZER_CHARCLASS_OPEN            , // Флаг для парных символов
    close            = UMBA_TOKENIZER_CHARCLASS_CLOSE           , // Флаг для парных символов

    // не проверяем
    quot             = UMBA_TOKENIZER_CHARCLASS_QUOT            ,

    escape           = UMBA_TOKENIZER_CHARCLASS_ESCAPE            // Для символа '\', который везде используется как escape-символ

    punctuation      = UMBA_TOKENIZER_CHARCLASS_PUNCTUATION     ,

    digit            = UMBA_TOKENIZER_CHARCLASS_DIGIT           ,
    alpha            = UMBA_TOKENIZER_CHARCLASS_ALPHA           ,
    upper            = UMBA_TOKENIZER_CHARCLASS_UPPER           , // Флаг для символов верхнего регистра
    identifier       = UMBA_TOKENIZER_CHARCLASS_IDENTIFIER      ,
    identifier_first = UMBA_TOKENIZER_CHARCLASS_IDENTIFIER_FIRST,
    semialpha        = UMBA_TOKENIZER_CHARCLASS_SEMIALPHA       , // Для символов, которые никуда не вошли, такие как @ # $
};

#endif



// https://en.cppreference.com/w/cpp/language/string_literal
// std::string textRawDigraph = R"raw(
//  0X123 0b10021
//   digraph {
//     a+++b
//     r_1 [label=aaa]
//     b=a+22-b3();
//     pData->second[12].field = -123'456.00;
//     if (c==3 || c>=e)
//         std::cout << eee;
//     r_1->n_2_1;  r_1->n_2_2;  r_1->n_2_3;  r_1->n_2_4
//     n_2_3->n_3_1;  n_2_3->n_3_2;  n_2_3->n_3_3;  n_2_3->n_3_4
//     n_3_3->n_4_0;  n_3_3->n_4_1;  n_3_3->n_4_2;  n_3_3->n_4_3;  n_3_3->n_4_4
// }
// )raw";
//
//     // n_2_1 [label="+"];  n_2_2 [label="-"];  n_2_3 [label="="];  n_2_4 [label=">"]
//     // n_3_1 [label="+"];  n_3_2 [label="-"];  n_3_3 [label="="];  n_3_4 [label=">"]
//     // n_4_0 [label="!"];  n_4_1 [label="+"];  n_4_2 [label="-"];  n_4_3 [label="="];  n_4_4 [label=">"]
//
//
// std::string textTabsAndSpaces = "  \r\n\n  \t\t  \t\n";
//
// std::string text = textTabsAndSpaces + textRawDigraph;


inline
void printPos(const umba::TextPositionInfo &pos)
{
    cout << pos.lineNumber+1 << ":" << pos.symbolOffset+1; // печатаем человеческие номера
}


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


        inputFilename = umba::filename::appendPath(rootPath, std::string("_libs/umba/the.h"));
        // inputFilename = umba::filename::appendPath(rootPath, std::string("_libs/umba/stl_keil_initializer_list.h"));
        // inputFilename = umba::filename::appendPath(rootPath, std::string("_libs/umba/stl_keil_type_traits.h"));
        // inputFilename = umba::filename::appendPath(rootPath, std::string("_libs/umba/string_plus.h"));
        // inputFilename = umba::filename::appendPath(rootPath, std::string("_libs/umba/rgbquad.h"));

        // inputFilename = umba::filename::appendPath(rootPath, "_libs/umba/");
    }




    std::array<CharClass, 128> charClassTable;

    TrieBuilder  numbersTrieBuilder;
    TrieBuilder  bracketsTrieBuilder;
    TrieBuilder  operatorsTrieBuilder;
    TrieBuilder  literalsTrieBuilder ;

    std::vector<TrieNode> numbersTrie ;
    std::vector<TrieNode> bracketsTrie ;
    std::vector<TrieNode> operatorsTrie;
    std::vector<TrieNode> literalsTrie ;


    {
        payload_type numberTokenId = UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FIRST;

        numbersTrieBuilder.addTokenSequence("0b", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_BIN);
        numbersTrieBuilder.addTokenSequence("0B", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_BIN);

        numbersTrieBuilder.addTokenSequence("0d", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DEC);
        numbersTrieBuilder.addTokenSequence("0D", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DEC);

        numbersTrieBuilder.addTokenSequence("4x", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_QUAT);
        numbersTrieBuilder.addTokenSequence("4X", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_QUAT);

        numbersTrieBuilder.addTokenSequence("0" , numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_OCT | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FLAG_MISS_DIGIT);

        numbersTrieBuilder.addTokenSequence("12x", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DUOD);
        numbersTrieBuilder.addTokenSequence("12X", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DUOD);

        numbersTrieBuilder.addTokenSequence("0x", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_HEX);
        numbersTrieBuilder.addTokenSequence("0X", numberTokenId++ | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_HEX);

        // numbersTrieBuilder.addTokenSequence("", numberTokenId++ | );
    }


    bracketsTrieBuilder.addTokenSequence('{', UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_OPEN  );
    bracketsTrieBuilder.addTokenSequence('}', UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_CLOSE );
    bracketsTrieBuilder.addTokenSequence('(', UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN  );
    bracketsTrieBuilder.addTokenSequence(')', UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE );
    bracketsTrieBuilder.addTokenSequence('<', UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN  );
    bracketsTrieBuilder.addTokenSequence('>', UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE );
    bracketsTrieBuilder.addTokenSequence('[', UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN );
    bracketsTrieBuilder.addTokenSequence(']', UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE);


//#define UMBA_TOKENIZER_TOKEN_NUMBER_LITERL_FIRST                               0x0800u
    {

        generation::generateCharClassTable(charClassTable, false /* !addOperatorChars */ );

        operatorsTrieBuilder.addTokenSequence("//", UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST);
        operatorsTrieBuilder.addTokenSequence("/*", UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_START);
        operatorsTrieBuilder.addTokenSequence("*/", UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_END  );

        payload_type operatorTokenId = UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST_GENERIC;
        std::vector<std::string> operators{"+","-","*","/","%","^","&","|","~","!","=","<",">","+=","-=","*=","/=","%=","^=","&=","|=","<<",">>",">>=","<<=","==","!=","<=",">=","<=>","&&","||","++","--",",","->*",".*","->",":","::",";","?","..."};
        for(const auto &opStr : operators)
        {
            // Устанавливаем класс opchar только тем символам, которые входят в операторы
            generation::setCharClassFlags(charClassTable, opStr, umba::tokenizer::CharClass::opchar);
            operatorsTrieBuilder.addTokenSequence(opStr.begin(), opStr.end(), operatorTokenId);
            ++operatorTokenId;
        }
    }


    numbersTrieBuilder.buildTokenTrie(numbersTrie);
    bracketsTrieBuilder.buildTokenTrie(bracketsTrie);
    operatorsTrieBuilder.buildTokenTrie(operatorsTrie);
    literalsTrieBuilder.buildTokenTrie(literalsTrie);
    // std::vector<TrieNode> operatorsTrie;
    // std::vector<TrieNode> literalsTrie ;

    if (!numbersTrie.empty())
    {
        std::cout << "--- Numbers trie\n";
        umba::tokenizer::tokenTriePrintTableGraph( "numbers"
                                                 , numbersTrie, cout
                                                 , [](token_type t) { return std::string(1, (char)t); }
                                                 );
        std::cout << "---\n";
    }

    if (!bracketsTrie.empty())
    {
        std::cout << "--- Brackets trie\n";
        umba::tokenizer::tokenTriePrintTableGraph( "brackets"
                                                 , bracketsTrie, cout
                                                 , [](token_type t) { return std::string(1, (char)t); }
                                                 );
        std::cout << "---\n";
    }

    if (!operatorsTrie.empty())
    {
        std::cout << "--- Operators trie\n";
        umba::tokenizer::tokenTriePrintTableGraph( "operators"
                                                 , operatorsTrie, cout
                                                 , [](token_type t) { return std::string(1, (char)t); }
                                                 );
        std::cout << "---\n";
    }

    if (!literalsTrie.empty())
    {
        std::cout << "--- Literals trie\n";
        umba::tokenizer::tokenTriePrintTableGraph( "literals"
                                                 , literalsTrie, cout
                                                 , [](token_type t) { return std::string(1, (char)t); }
                                                 );
        std::cout << "---\n";
    }

    if (!umba::filesys::readFile(inputFilename, text))
    {
        std::cout << "Failed to read input file: " << inputFilename << "\n";
        return 1;
    }

    cout << "Input file: " << inputFilename << "\n";
    std::cout << "---\n";

    using PosCountingIterator = umba::iterator::TextPositionCountingIterator<char>;

// #define UMBA_TOKENIZER_TOKEN_LINEFEED                                          1u
// #define UMBA_TOKENIZER_TOKEN_SPACE                                             2u

    enum State
    {
        stInitial              = 0,
        stReadSpace               ,
        stReadIdentifier          ,
        stReadNumberPrefix        ,
        stReadNumber              ,
        stReadNumberFloat         ,
        stReadOperator            ,
        stReadUserLiteral         ,
        stReadSingleLineComment   ,
        stReadMultilineLineComment

    };

    State st = stInitial;

    auto getStateStr = [](State s)
    {
        switch(s)
        {
            case stInitial                 : return std::string("Initial");
            case stReadSpace               : return std::string("ReadSpace");
            case stReadIdentifier          : return std::string("ReadIdentifier");
            case stReadNumberPrefix        : return std::string("ReadNumberPrefix");
            case stReadNumber              : return std::string("ReadNumber");
            case stReadNumberFloat         : return std::string("ReadNumberFloat");
            case stReadOperator            : return std::string("ReadOperator");
            case stReadUserLiteral         : return std::string("ReadUserLiteral");
            case stReadSingleLineComment   : return std::string("ReadSingleLineComment");
            case stReadMultilineLineComment: return std::string("ReadMultilineLineComment");
            default:                         return std::string("<UNKNOWN>");
        }
    };

    auto getTokenStr = [](payload_type p)
    {
        switch(p)
        {
            case UMBA_TOKENIZER_TOKEN_UNEXPECTED          : return std::string("<UNEXPECTED>");
            case UMBA_TOKENIZER_TOKEN_LINEFEED            : return std::string("LINEFEED");
            case UMBA_TOKENIZER_TOKEN_SPACE               : return std::string("SPACE");
            case UMBA_TOKENIZER_TOKEN_IDENTIFIER          : return std::string("IDENTIFIER");
            case UMBA_TOKENIZER_TOKEN_NUMBER              : return std::string("NUMBER");
            case UMBA_TOKENIZER_TOKEN_SEMIALPHA           : return std::string("SEMIALPHA");
            case UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER        : return std::string("FLOAT_NUMBER");
            case UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_OPEN  : return std::string("KIND_OF_BRACKET");
            case UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_CLOSE : return std::string("KIND_OF_BRACKET");
            case UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN  : return std::string("KIND_OF_BRACKET");
            case UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE : return std::string("KIND_OF_BRACKET");
            case UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN  : return std::string("KIND_OF_BRACKET");
            case UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE : return std::string("KIND_OF_BRACKET");
            case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN : return std::string("KIND_OF_BRACKET");
            case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE: return std::string("KIND_OF_BRACKET");
            case UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_START: return std::string("COMMENT_START");
            case UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_END  : return std::string("COMMENT_END");
            //case : return std::string("");
            default:

                if (p>=UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST && p<=UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_LAST)
                    return std::string("KIND_OF_SINGLE_LINE_COMMENT");
                if (p>=UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FIRST && p<=UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_TOTAL_LAST)
                    return std::string("KIND_OF_NUMBER");
                if (p>=UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST && p<=UMBA_TOKENIZER_TOKEN_OPERATOR_LAST)
                    return std::string("KIND_OF_OPERATOR");
                if (p>=UMBA_TOKENIZER_TOKEN_USER_LITERAL_FIRST && p<=UMBA_TOKENIZER_TOKEN_USER_LITERAL_LAST)
                    return std::string("KIND_OF_USER_LITERAL");
                return std::string("");
        }
    };


    auto parsingHandler = [&](payload_type tokenType, const PosCountingIterator b, const PosCountingIterator &e)
    {
        using namespace umba::iterator;

        auto curPos = b.getPosition(); // Выводим позицию начала токена

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
            cout << ", " << getTokenStr(tokenType) << ", '" << makeString(b, e) << "'";
        }

        cout << ", state: " << getStateStr(st) << ", in data location " << umba::makeSimpleTextPositionInfoString<std::string>(curPos) << "\n";
    };

    auto unexpectedHandler = [&](const PosCountingIterator it, auto srcFile, auto srcLine)
    {
        auto errPos = it.getPosition(true); // с поиском конца строки (а вообще не надо пока, но пусть)
        std::string erroneousLineText = umba::iterator::makeString(it.getLineStartIterator(), it.getLineEndIterator());
        cout << "Unexpected at " << inputFilename << ":" << umba::makeSimpleTextPositionInfoString<std::string>(errPos) << "\n";
        cout << "Line:" << erroneousLineText << "\n";
        //auto errMarkerO
        cout << "    |" << std::string(errPos.symbolOffset, ' ') << "^";
        if (erroneousLineText.size()>(errPos.symbolOffset+1))
            cout << std::string(erroneousLineText.size()-(errPos.symbolOffset+1), ' ');
        cout << "|\n";

        if (it!=PosCountingIterator())
        {
            char ch = *it;
            cout << "ch: ";
            if (ch>=' ')
               cout << "'" << (char)ch << "'";
            else
               cout << " " << (unsigned)(unsigned char)ch;
            cout << "\n";
        }
        cout << "At " << srcFile << ":" << srcLine << "\n";
        cout << "State: " << getStateStr(st) << "\n";
        return 1;
    };

    // cout << "--- Text:\n";
    // cout << text << "\n";
    // cout << "---\n";


    PosCountingIterator tokenStartIt;
    trie_index_type operatorIdx = trie_index_invalid;

    const bool numbersAllowDigitsSeparator = true; // apos ' (39/0x27) only can be used
    const int numberDefaultBase = 10;
    int numberExplicitBase      =  0;
    trie_index_type numberPrefixIdx = trie_index_invalid;
    payload_type numberTokenId = 0;
    payload_type numberReadedDigits = 0;
    CharClass allowedDigitCharClass = CharClass::none;
    int numbersBase = 0;

    PosCountingIterator commentStartIt;
    payload_type commentTokenId = 0;
    bool allowNestedComments = true;
    unsigned commentNestingLevel = 0;

    //NOTE: !!! Надо ли semialpha проверять, не является ли она началом числового префикса? Наверное, не помешает

    auto performStartReadingNumber = [&](auto ch, auto it)
    {
        tokenStartIt = it;
        numberPrefixIdx = tokenTrieFindNext(numbersTrie, trie_index_invalid, (token_type)ch);
        numberTokenId = 0;
        numberReadedDigits = 0;
        numberExplicitBase = 0;
        if (numberPrefixIdx!=trie_index_invalid) // Найдено начало префикса числа
        {
            st = stReadNumberPrefix;
            allowedDigitCharClass = CharClass::digit; // Потом всё равно зададим, после определения префикса
            numbersBase = 0;
        }
        else
        {
            st = stReadNumber;
            numbersBase = numberDefaultBase;
            allowedDigitCharClass = CharClass::digit;
            if (utils::isNumberHexDigitsAllowed(numbersBase))
                allowedDigitCharClass |= CharClass::xdigit;
        }

    };

    auto performStartReadingOperator = [&](auto ch, auto it)
    {
        tokenStartIt = it;
        operatorIdx = tokenTrieFindNext(operatorsTrie, trie_index_invalid, (token_type)ch);
        if (operatorIdx==trie_index_invalid)
            return false;
        st = stReadOperator;
        return true;
    };

    auto performProcessBracket = [&](auto ch, auto it)
    {
        auto idx = tokenTrieFindNext(bracketsTrie, trie_index_invalid, (token_type)ch);
        if (idx==trie_index_invalid)
            return false;
        parsingHandler(bracketsTrie[idx].payload, it, it+1); // выплюнули
        st = stInitial;
        return true;
    };

    auto processCommentStartFromNonCommented = [&](auto curPayload, auto it)
    {
        commentTokenId = curPayload;
    
        if (utils::isSingleLineCommentToken(curPayload))
        {
            tokenStartIt   = it;
            commentStartIt = it; // Сохранили начало коментария без самого оператора начала коментария - текст коментария потом выдадим по окончании
            st = stReadSingleLineComment;
            operatorIdx = trie_index_invalid;
            return true;
        }
        else if (utils::isMultiLineCommentStartToken(curPayload))
        {
            tokenStartIt   = it;
            commentStartIt = it; // Сохранили начало коментария без самого оператора начала коментария - текст коментария потом выдадим по окончании
            st = stReadMultilineLineComment;
            operatorIdx = trie_index_invalid;
            commentNestingLevel = 1; // Один вход сделали тут
            return true;
        }
        else // коментарий, но не однострочный, и не начало многострочного - ошибка
        {
            return false;
        }
    };



    for( PosCountingIterator it=PosCountingIterator(text.data(), text.size()); it!=PosCountingIterator(); ++it)
    {
        const auto ch = *it;
        CharClass charClass = charClassTable[charToCharClassTableIndex(ch)];

        switch(st)
        {
            explicit_initial:
            case stInitial:
            {
                if (umba::TheFlags(charClass).oneOf(CharClass::linefeed))
                {
                    parsingHandler(UMBA_TOKENIZER_TOKEN_LINEFEED, it, it+1); // Перевод строки мы всегда отдельно выплёвываем
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::space))
                {
                    tokenStartIt = it;
                    st = stReadSpace;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::opchar))
                {
                    if (!performStartReadingOperator(ch, it))
                        return unexpectedHandler(it, __FILE__, __LINE__);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::identifier_first))
                {
                    //parsingHandler(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it); // выплюнули
                    tokenStartIt = it;
                    st = stReadIdentifier;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::digit))
                {
                    performStartReadingNumber(ch, it);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::open, CharClass::close)) // Открывающая или закрывающая скобка
                {
                    if (!performProcessBracket(ch, it))
                        return unexpectedHandler(it, __FILE__, __LINE__);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::semialpha))
                {
                    parsingHandler(UMBA_TOKENIZER_TOKEN_SEMIALPHA, it, it+1); // выплюнули
                }
                else
                {
                    return unexpectedHandler(it, __FILE__, __LINE__);
                }

                //if ((charClass::linefeed))
            } break;

            case stReadSpace:
            {
                if (umba::TheFlags(charClass).oneOf(CharClass::linefeed))
                {
                    parsingHandler(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it);
                    parsingHandler(UMBA_TOKENIZER_TOKEN_LINEFEED, it, it+1); // Перевод строки мы всегда отдельно выплёвываем
                    st = stInitial;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::space))
                {
                    if (*tokenStartIt!=*it) // пробелы бывают разные, одинаковые мы коллекционируем, разные - выплевываем разными пачками
                    {
                        parsingHandler(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it); // выплюнули
                        tokenStartIt = it; // Сохранили начало нового токена
                    }
                    else
                    {
                        break; // коллекционируем
                    }
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::opchar))
                {
                    parsingHandler(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it);
                    if (!performStartReadingOperator(ch, it))
                        return unexpectedHandler(it, __FILE__, __LINE__);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::identifier_first))
                {
                    parsingHandler(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it); // выплюнули
                    tokenStartIt = it;
                    st = stReadIdentifier;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::digit))
                {
                    parsingHandler(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it); // выплюнули
                    performStartReadingNumber(ch, it);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::open, CharClass::close)) // Открывающая или закрывающая скобка
                {
                    parsingHandler(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it); // выплюнули
                    if (!performProcessBracket(ch, it))
                        return unexpectedHandler(it, __FILE__, __LINE__);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::semialpha))
                {
                    parsingHandler(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it); // выплюнули
                    parsingHandler(UMBA_TOKENIZER_TOKEN_SEMIALPHA, it, it+1); // выплюнули
                    st = stInitial;
                }
                else
                {
                    return unexpectedHandler(it, __FILE__, __LINE__);
                }
            } break;

            case stReadIdentifier:
            {
                if (umba::TheFlags(charClass).oneOf(CharClass::identifier, CharClass::identifier_first))
                {
                    break; // коллекционируем
                }

                parsingHandler(UMBA_TOKENIZER_TOKEN_IDENTIFIER, tokenStartIt, it); // выплюнули
                tokenStartIt = it; // Сохранили начало нового токена

                if (umba::TheFlags(charClass).oneOf(CharClass::linefeed))
                {
                    parsingHandler(UMBA_TOKENIZER_TOKEN_LINEFEED, it, it+1); // Перевод строки мы всегда отдельно выплёвываем
                    st = stInitial;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::space))
                {
                    st = stReadSpace;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::opchar))
                {
                    if (!performStartReadingOperator(ch, it))
                        return unexpectedHandler(it, __FILE__, __LINE__);
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::open, CharClass::close)) // Открывающая или закрывающая скобка
                {
                    if (!performProcessBracket(ch, it))
                        return unexpectedHandler(it, __FILE__, __LINE__);
                }
                else
                {
                    return unexpectedHandler(it, __FILE__, __LINE__);
                }
            } break;

            case stReadNumberPrefix:
            {
                //NOTE: !!! У нас пока так: префикс числа начинается с любой цифры, потом могут следовать любые символы, после префикса - те символы, которые разрешены префиксом
                auto nextNumberPrefixIdx = tokenTrieFindNext(numbersTrie, numberPrefixIdx, (token_type)ch);
                if (nextNumberPrefixIdx!=trie_index_invalid)
                {
                    numberPrefixIdx = nextNumberPrefixIdx; // Всё в порядке, префикс числа продолжается
                    break;
                }
                else // Нет продолжения префикса, вероятно, он у нас окончился
                {
                    auto curPayload = numbersTrie[numberPrefixIdx].payload;
                    if (curPayload==payload_invalid) // текущий префикс нифига не префикс
                        return unexpectedHandler(it, __FILE__, __LINE__);

                    bool requiresDigits   = utils::isNumberPrefixRequiresDigits(curPayload);
                    numberPrefixIdx       = trie_index_invalid; // сбрасываем индекс префикса, чтобы потом не париться

                    numberTokenId         = curPayload;
                    numberExplicitBase    = utils::numberPrefixGetBase(numberTokenId);
                    numberReadedDigits    = 0;
                    numbersBase           = numberExplicitBase;
                    allowedDigitCharClass = CharClass::digit;
                    if (utils::isNumberHexDigitsAllowed(numbersBase))
                        allowedDigitCharClass |= CharClass::xdigit;

                    // Теперь тут у нас либо цифра, либо что-то другое
                    if (umba::TheFlags(charClass).oneOf(allowedDigitCharClass) && utils::isDigitAllowed(ch, numbersBase))
                    {
                        //NOTE: !!! Да, сразу после префикса у нас не может быть разделителя разрядов 
                        st = stReadNumber; // Тут у нас годная цифра, продолжаем
                        numberReadedDigits = 1;
                        break;
                    }

                    if (requiresDigits)
                        return unexpectedHandler(it, __FILE__, __LINE__); // нужна хоть одна цифра, а её нет

                    parsingHandler(numberTokenId, tokenStartIt, it); // выплёвываем префикс (он является годным числом и без доп цифр)

                    // у тут понеслась вся та же тема, как и в состоянии stInitial, только без обработки цифр

                    st = stInitial; // на всякий случай, если в stInitial обрабтчике состояние не переустанавливается, а подразумевается, что уже такое и есть
                    goto explicit_initial;

                    #if 0
                    if (umba::TheFlags(charClass).oneOf(CharClass::linefeed))
                    {
                        parsingHandler(UMBA_TOKENIZER_TOKEN_LINEFEED, it, it+1); // Перевод строки мы всегда отдельно выплёвываем
                    }
                    else if (umba::TheFlags(charClass).oneOf(CharClass::space))
                    {
                        tokenStartIt = it;
                        st = stReadSpace;
                    }
                    else if (umba::TheFlags(charClass).oneOf(CharClass::opchar))
                    {
                        if (!performStartReadingOperator(ch, it))
                            return unexpectedHandler(it, __FILE__, __LINE__);
                    }
                    else if (umba::TheFlags(charClass).oneOf(CharClass::identifier_first))
                    {
                        //parsingHandler(UMBA_TOKENIZER_TOKEN_SPACE, tokenStartIt, it); // выплюнули
                        tokenStartIt = it;
                        st = stReadIdentifier;
                    }
                    else if (umba::TheFlags(charClass).oneOf(CharClass::digit))
                    {
                        performStartReadingNumber(ch, it);
                    }
                    else if (umba::TheFlags(charClass).oneOf(CharClass::open, CharClass::close)) // Открывающая или закрывающая скобка
                    {
                        if (!performProcessBracket(ch, it))
                            return unexpectedHandler(it, __FILE__, __LINE__);
                    }
                    else if (umba::TheFlags(charClass).oneOf(CharClass::semialpha))
                    {
                        parsingHandler(UMBA_TOKENIZER_TOKEN_SEMIALPHA, it, it+1); // выплюнули
                    }
                    else
                    {
                        return unexpectedHandler(it, __FILE__, __LINE__);
                    }
                    #endif
                }

            } break;

            case stReadNumber:
            {
                if (umba::TheFlags(charClass).oneOf(allowedDigitCharClass) && utils::isDigitAllowed(ch, numbersBase))
                {
                    break; // Тут у нас годная цифра
                }

                if (numbersAllowDigitsSeparator && ch=='\'')
                {
                    break; // Тут у нас разделитель разрядов
                }

                if (numberTokenId==0 || numberTokenId==payload_invalid)
                    parsingHandler(UMBA_TOKENIZER_TOKEN_NUMBER, tokenStartIt, it); // выплёвываем накопленное число как число без префикса, с системой счисления по умолчанию
                else
                    parsingHandler(numberTokenId, tokenStartIt, it); // выплёвываем накопленное число с явно указанной системой счисления

                //return unexpectedHandler(it, __FILE__, __LINE__);
                // Далее у нас всё как начальном состоянии
                st = stInitial; // на всякий случай, если в stInitial обрабтчике состояние не переустанавливается, а подразумевается, что уже такое и есть
                goto explicit_initial;

            } break;

            case stReadNumberFloat:
            {
                return unexpectedHandler(it, __FILE__, __LINE__);

            } break;


            case stReadOperator:
            {
                if (umba::TheFlags(charClass).oneOf(CharClass::opchar))
                {
                    auto nextOperatorIdx = tokenTrieFindNext(operatorsTrie, operatorIdx, (token_type)ch);

                    if (nextOperatorIdx!=trie_index_invalid)
                    {
                        operatorIdx = nextOperatorIdx; // Всё в порядке, оператор продолжается
                    }
                    else
                    {
                        // Нет продолжения, у нас был, вероятно, полностью заданный оператор
                        auto curPayload = operatorsTrie[operatorIdx].payload;
                        if (curPayload==payload_invalid) // текущий оператор нифига не оператор
                            return unexpectedHandler(it, __FILE__, __LINE__);

                        if (utils::isCommentToken(curPayload)) // на каждом операторе в обрабатываемом тексте у нас это срабатывает. Жирно или нет?
                        {
                            if (!processCommentStartFromNonCommented(curPayload, it))
                                return unexpectedHandler(it, __FILE__, __LINE__);
                            break;
                        }

                        parsingHandler(curPayload, tokenStartIt, it); // выплюнули текущий оператор
                        tokenStartIt = it; // Сохранили начало нового токена

                        operatorIdx = tokenTrieFindNext(operatorsTrie, trie_index_invalid, (token_type)ch); // Начали поиск нового оператора с начала
                        if (operatorIdx==trie_index_invalid)
                            return unexpectedHandler(it, __FILE__, __LINE__);
                    }
                    break; // коллекционируем символы оператора
                }

                // Заканчиваем обработку оператора на неоператорном символе

                if (operatorIdx==trie_index_invalid)
                    return unexpectedHandler(it, __FILE__, __LINE__);
                if (operatorsTrie[operatorIdx].payload==payload_invalid)
                    return unexpectedHandler(it, __FILE__, __LINE__);

                auto curPayload = operatorsTrie[operatorIdx].payload;

                // Всё тоже самое, что и выше, наверное, надо как-то это потом вынести
                if (utils::isCommentToken(curPayload)) // на каждом операторе в обрабатываемом тексте у нас это срабатывает. Жирно или нет?
                {
                    if (!processCommentStartFromNonCommented(curPayload, it))
                        return unexpectedHandler(it, __FILE__, __LINE__);
                    break;
                }

                parsingHandler(curPayload, tokenStartIt, it); // выплюнули
                tokenStartIt = it; // Сохранили начало нового токена

                if (umba::TheFlags(charClass).oneOf(CharClass::linefeed))
                {
                    parsingHandler(UMBA_TOKENIZER_TOKEN_LINEFEED, it, it+1); // Перевод строки мы всегда отдельно выплёвываем
                    st = stInitial;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::space))
                {
                    st = stReadSpace;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::identifier_first))
                {
                    st = stReadIdentifier;
                }
                else if (umba::TheFlags(charClass).oneOf(CharClass::open, CharClass::close)) // Открывающая или закрывающая скобка
                {
                    auto idx = tokenTrieFindNext(bracketsTrie, trie_index_invalid, (token_type)ch);
                    if (idx==trie_index_invalid)
                        return unexpectedHandler(it, __FILE__, __LINE__);
                    parsingHandler(bracketsTrie[idx].payload, it, it+1); // выплюнули
                    st = stInitial;
                }
                else
                {
                    return unexpectedHandler(it, __FILE__, __LINE__);
                }
            } break;

            case stReadUserLiteral:
            {
                return unexpectedHandler(it, __FILE__, __LINE__);

            } break;

            case stReadSingleLineComment:
            {
                if (umba::TheFlags(charClass).oneOf(CharClass::linefeed))
                {
                    //TODO: !!! Разобраться с continuation
                    parsingHandler(commentTokenId, commentStartIt, it);
                    parsingHandler(UMBA_TOKENIZER_TOKEN_LINEFEED, it, it+1); // Перевод строки мы всегда отдельно выплёвываем
                    st = stInitial;
                }

                // Иначе - ничего не делаем

            } break;

            case stReadMultilineLineComment:
            {
                // auto nextOperatorIdx = tokenTrieFindNext(operatorsTrie, operatorIdx, (token_type)ch);
                return unexpectedHandler(it, __FILE__, __LINE__);

            } break;

            default:
            {
                return unexpectedHandler(it, __FILE__, __LINE__);
            }

        }

        // char ch = *it;
        // if (ch>=' ')
        //    cout << "'" << (char)ch << "'";
        // else
        //    cout << " " << (unsigned)(unsigned char)ch;
        //
        // cout << "  ";
        // printPos(it.getPosition());
        // cout << "\n";
    }

    return 0;
}

