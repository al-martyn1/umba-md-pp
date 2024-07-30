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
std::string text = R"raw(digraph {

    r_1 [label="!"]

    r_1->n_2_1;  r_1->n_2_2;  r_1->n_2_3;  r_1->n_2_4
    n_2_3->n_3_1;  n_2_3->n_3_2;  n_2_3->n_3_3;  n_2_3->n_3_4
    n_3_3->n_4_0;  n_3_3->n_4_1;  n_3_3->n_4_2;  n_3_3->n_4_3;  n_3_3->n_4_4

    n_2_1 [label="+"];  n_2_2 [label="-"];  n_2_3 [label="="];  n_2_4 [label=">"]
    n_3_1 [label="+"];  n_3_2 [label="-"];  n_3_3 [label="="];  n_3_4 [label=">"]
    n_4_0 [label="!"];  n_4_1 [label="+"];  n_4_2 [label="-"];  n_4_3 [label="="];  n_4_4 [label=">"]

}
)raw";



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

    std::cout << "--- Operators trie\n";
    umba::tokenizer::tokenTriePrintGraph( operatorsTrie, cout
                                        , [](payload_type p) { return std::string(1, (char)p); }
                                        );
    std::cout << "---\n";

    std::cout << "--- Literals trie\n";
    umba::tokenizer::tokenTriePrintGraph( literalsTrie, cout
                                        , [](payload_type p) { return std::string(1, (char)p); }
                                        );
    std::cout << "---\n";

    {
        auto charClass = charClassTable[charToCharClassTableIndex('(')];
        bool bRes = umba::TheFlags(charClass).oneOf(CharClass::open, CharClass::close);
        cout << "Is brace '(': " << (bRes?"true":"false") << "\n";
    }
    {
        auto charClass = charClassTable[charToCharClassTableIndex('!')];
        bool bRes = umba::TheFlags(charClass).oneOf(CharClass::open, CharClass::close);
        cout << "Is brace '!': " << (bRes?"true":"false") << "\n";
    }

    {
        auto charClass = charClassTable[charToCharClassTableIndex('8')];
        bool bRes = umba::TheFlags(charClass).allOf(CharClass::identifier, CharClass::alpha);
        cout << "Is identifier && alpha '8': " << (bRes?"true":"false") << "\n";
    }
    {
        auto charClass = charClassTable[charToCharClassTableIndex('a')];
        bool bRes = umba::TheFlags(charClass).allOf(CharClass::identifier, CharClass::alpha);
        cout << "Is identifier && alpha 'a': " << (bRes?"true":"false") << "\n";
    }

    {
        char ch = 'a';
        bool bRes = umba::TheValue(ch).oneOf('a', 'b', 'c');
        cout << "Is 'a' one of 'a', 'b', 'c': " << (bRes?"true":"false") << "\n";
    }

    {
        char ch = 'a';
        bool bRes = umba::TheValue(ch).oneOf('b', 'c', 'd');
        cout << "Is 'a' one of 'b', 'c', 'd': " << (bRes?"true":"false") << "\n";
    }


    // setCharClassFlags(charClasses, braceChars[0], umba::tokenizer::CharClass::open );
    // setCharClassFlags(charClasses, braceChars[1], umba::tokenizer::CharClass::close);


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

    // The - https://rsdn.org/forum/cpp.applied/8555847.1
    using PosCountingIterator = umba::iterator::TextPositionCountingIterator<char>;


    for( PosCountingIterator it=PosCountingIterator(text.data(), text.size()); it!=PosCountingIterator(); ++it)
    {
        const auto ch = *it;
        //CharClass charClass = charToCharClassTableIndex(ch);

        switch(st)
        {
            case stInitial:
            {
                //if ((charClass::linefeed))
            } break;

            case stReadSpace:
            {
            } break;

            case stReadIdentifier:
            {
            } break;

            case stReadNumber:
            {
            } break;

            case stReadOperator:
            {
            } break;

            case stReadUserLiteral:
            {
            } break;



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

