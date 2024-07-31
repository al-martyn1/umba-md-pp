/*! \file
    \brief
 */


#include "umba/umba.h"
#include "umba/tokenizer.h"
#include "umba/assert.h"

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

// https://en.cppreference.com/w/cpp/language/string_literal
std::string textRawDigraph = R"raw(
  digraph {

    r_1 [label="!"]

    r_1->n_2_1;  r_1->n_2_2;  r_1->n_2_3;  r_1->n_2_4
    n_2_3->n_3_1;  n_2_3->n_3_2;  n_2_3->n_3_3;  n_2_3->n_3_4
    n_3_3->n_4_0;  n_3_3->n_4_1;  n_3_3->n_4_2;  n_3_3->n_4_3;  n_3_3->n_4_4

    n_2_1 [label="+"];  n_2_2 [label="-"];  n_2_3 [label="="];  n_2_4 [label=">"]
    n_3_1 [label="+"];  n_3_2 [label="-"];  n_3_3 [label="="];  n_3_4 [label=">"]
    n_4_0 [label="!"];  n_4_1 [label="+"];  n_4_2 [label="-"];  n_4_3 [label="="];  n_4_4 [label=">"]

}
)raw";

std::string textTabsAndSpaces = "  \r\n\n  \t\t  \t\n";

std::string text = textTabsAndSpaces + textRawDigraph;


inline
void printPos(const umba::TextPositionInfo &pos)
{
    cout << pos.lineNumber+1 << ":" << pos.symbolOffset+1; // печатаем человеческие номера
}


int main(int argc, char* argv[])
{
    using namespace umba::tokenizer;

    std::array<CharClass, 128> charClassTable;

    TrieBuilder  operatorsTrieBuilder;
    TrieBuilder  literalsTrieBuilder ;

    std::vector<TrieNode> operatorsTrie;
    std::vector<TrieNode> literalsTrie ;

    payload_type tokenId = 1;


    generation::generateCharClassTable(charClassTable, false /* !addOperatorChars */ );
    std::vector<std::string> operators{"+","-","*","/","%","^","&","|","~","!","=","<",">","+=","-=","*=","/=","%=","^=","&=","|=","<<",">>",">>=","<<=","==","!=","<=",">=","<=>","&&","||","++","--",",","->*",".*","->",":","::",";","?","..."};
    for(const auto &opStr : operators)
    {
        // Устанавливаем класс opchar только тем символам, которые входят в операторы
        generation::setCharClassFlags(charClassTable, opStr, umba::tokenizer::CharClass::opchar);
        operatorsTrieBuilder.addTokenSequence(opStr.begin(), opStr.end(), tokenId);
        ++tokenId;
    }


    operatorsTrieBuilder.buildTokenTrie(operatorsTrie);
    literalsTrieBuilder.buildTokenTrie(literalsTrie);
    // std::vector<TrieNode> operatorsTrie;
    // std::vector<TrieNode> literalsTrie ;

    if (!operatorsTrie.empty())
    {
        std::cout << "--- Operators trie\n";
        umba::tokenizer::tokenTriePrintGraph( operatorsTrie, cout
                                            , [](payload_type p) { return std::string(1, (char)p); }
                                            );
        std::cout << "---\n";
    }

    if (!literalsTrie.empty())
    {
        std::cout << "--- Literals trie\n";
        umba::tokenizer::tokenTriePrintGraph( literalsTrie, cout
                                            , [](payload_type p) { return std::string(1, (char)p); }
                                            );
        std::cout << "---\n";
    }

    using PosCountingIterator = umba::iterator::TextPositionCountingIterator<char>;

// #define UMBA_TOKENIZER_TOKEN_LINEFEED                                          1u
// #define UMBA_TOKENIZER_TOKEN_SPACE                                             2u

    auto parsingHandler = [&](token_type tokenType, const PosCountingIterator b, const PosCountingIterator &e)
    {
        using namespace umba::iterator;

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
            cout << umba::iterator::makeString(b, e);
        }

        cout << "\n";
    };

    auto unexpectedHandler = [&](const PosCountingIterator it, auto srcFile, auto srcLine)
    {
        cout << "Unexpected at " << umba::makeSimpleTextPositionInfoString<std::string>(it.getPosition()) << "\n";
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
        return 1;
    };

    enum State
    {
        stInitial         = 0,
        stReadSpace          ,
        stReadIdentifier     ,
        stReadNumber         ,
        stReadOperator       ,
        stReadUserLiteral

    };

    State st = stInitial;

    PosCountingIterator tokenStartIt;

    for( PosCountingIterator it=PosCountingIterator(text.data(), text.size()); it!=PosCountingIterator(); ++it)
    {
        const auto ch = *it;
        CharClass charClass = charClassTable[charToCharClassTableIndex(ch)];

        switch(st)
        {
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
                        tokenStartIt = it;
                    }
                    else
                    {
                        break; // коллекционируем
                    }
                }
                return unexpectedHandler(it, __FILE__, __LINE__);

            } break;

            case stReadIdentifier:
            {
                return unexpectedHandler(it, __FILE__, __LINE__);

            } break;

            case stReadNumber:
            {
                return unexpectedHandler(it, __FILE__, __LINE__);

            } break;

            case stReadOperator:
            {
                return unexpectedHandler(it, __FILE__, __LINE__);

            } break;

            case stReadUserLiteral:
            {
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

