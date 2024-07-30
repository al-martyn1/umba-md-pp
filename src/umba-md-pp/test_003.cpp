/*! \file
    \brief Тесты токенизера
 */

#define UMBA_TOKENISER_DISABLE_UMBA_TOKENISER_GET_CHAR_CLASS_FUNCTION
// #define UMBA_TOKENISER_TYPES_COMPACT
// #define UMBA_TOKENISER_NO_PAYLOAD_FLAGS

#include "umba/umba.h"
#include "umba/tokeniser.h"
#include "umba/assert.h"

#include <iostream>
#include <map>
#include <deque>

using std::cout;
using std::cerr;


// Inline variables - https://habr.com/ru/companies/otus/articles/561772/
// Про линковку inline - https://en.cppreference.com/w/cpp/language/inline#Explanation

// inline constexpr


template<typename ContainerType>
umba::tokeniser::trie_index_type tokenTrieGoNext(const ContainerType &tokenTrie, umba::tokeniser::trie_index_type curIndex, char ch)
{
    return umba::tokeniser::tokenTrieFindNext(tokenTrie, curIndex, ch);
}


inline
void printTokenTrieNode(const umba::tokeniser::TrieNode &tn)
{
    using namespace std;

    cout << "token   : ";
    if (tn.token>=' ')
       cout << "'" << (char)tn.token << "'";
    else
       cout << " " << (unsigned)(unsigned char)tn.token;
    cout << "\n";

    cout << "payload  : " << tn.payload << "\n";

    cout << "parentIdx: " << tn.parentNodeIndex << "\n";
    cout << "levelIdx : " << tn.lookupChunkStartIndex << "\n";
    cout << "levelSize: " << tn.lookupChunkSize       << "\n";
    cout << "childsIdx: " << tn.childsIndex     << "\n";
    //cout << "tokenId  : " << tn.tokenId         << "\n";

}

#if 1
template<typename ContainerType>
void testTraverseToken(const ContainerType &tokenTrie, const std::string &str)
{
    using namespace std;

    using namespace umba::tokeniser;

    trie_index_type idx = umba::tokeniser::trie_index_invalid; // trie_index_initial;
    //token_type   foundToken = token_id_invalid;
    payload_type   foundPayload = payload_invalid;

    for(auto ch : str)
    {
        trie_index_type nextIdx = tokenTrieGoNext(tokenTrie, idx, (token_type)ch);

        cout << "found index(idx): " << nextIdx << "\n";
        if (nextIdx==umba::tokeniser::trie_index_invalid)
        {
            cout << "symbol ";
            if (ch>=' ')
               cout << "'" << (char)ch << "'";
            else
               cout << " " << (unsigned)(unsigned char)ch;
            cout << " not found, stop\n";
            return;
        }
        else
        {
            printTokenTrieNode(tokenTrie[nextIdx]);
        }

        cout << "\n";

        idx = nextIdx;
    }

    if (idx>=tokenTrie.size())
    {
         cout << "Final index is invalid\n";
    }
    else
    {
        //TrieNode.tokenId
        foundPayload = tokenTrie[idx].payload;
        cout << "Backtrace: ";
        tokenTrieBackTrace(tokenTrie, idx, [](token_type ch) { cout << std::string(1,(char)ch); });
        cout << "\n";
    }

    if (foundPayload==payload_invalid)
    {
        cout << "Tokens sequence is bad, payload not found\n";
    }
    else
    {
        cout << "Found payload (" << str << "): " << (char)foundPayload << "\n";
    }

}


#endif


struct OperatorInfo
{
    umba::tokeniser::payload_type      operatorId;
    std::string                        operatorStr;
};


std::vector<OperatorInfo> operatorInfos = { { UMBA_TOKENISER_TOKEN_OPERATOR_LOGICAL_NOT             , "!"    }
                                          , { 555                                                   , "!+"   }
                                          , { 556                                                   , "!-"   }
                                          , { UMBA_TOKENISER_TOKEN_OPERATOR_NOT_EQUAL               , "!="   }
                                          , { 557                                                   , "!>"   }

                                          , { 558                                                   , "!=+"  }
                                          , { 559                                                   , "!=-"  }
                                          , { UMBA_TOKENISER_TOKEN_OPERATOR_STRICT_NOT_EQUAL        , "!=="  }
                                          , { 560                                                   , "!=>"  }

                                          , { 561                                                   , "!==!" }
                                          , { 562                                                   , "!==+" }
                                          , { 563                                                   , "!==-" }
                                          , { 564                                                   , "!===" }
                                          , { 565                                                   , "!==>" }

                                          , { UMBA_TOKENISER_TOKEN_OPERATOR_ADDITION                , "+"    }
                                          , { UMBA_TOKENISER_TOKEN_OPERATOR_INCREMENT               , "++"   }
                                          , { 566                                                   , "+-"   }
                                          , { UMBA_TOKENISER_TOKEN_OPERATOR_ADDITION_ASSIGNMENT     , "+="   }
                                          , { 567                                                   , "+++"  }
                                          , { 568                                                   , "++-"  }

                                          , { UMBA_TOKENISER_TOKEN_OPERATOR_SUBTRACTION             , "-"    }
                                          , { UMBA_TOKENISER_TOKEN_OPERATOR_DECREMENT               , "--"   }
                                          , { UMBA_TOKENISER_TOKEN_OPERATOR_SUBTRACTION_ASSIGNMENT  , "-="   }
                                          , { UMBA_TOKENISER_TOKEN_OPERATOR_STRUCTURE_DEREFERENCE   , "->"   }
                                          , { 668                                                   , "--!"  }
                                          , { 669                                                   , "--="  }
                                          , { 667                                                   , "-->"  }
                                          , { 666                                                   , "-=>"  }

                                          , { UMBA_TOKENISER_TOKEN_OPERATOR_ASSIGNMENT              , "="    }
                                          , { UMBA_TOKENISER_TOKEN_OPERATOR_EQUAL                   , "=="   }
                                          , { UMBA_TOKENISER_TOKEN_OPERATOR_LAMBDA                  , "=>"   }
                                          , { UMBA_TOKENISER_TOKEN_OPERATOR_STRICT_EQUAL            , "==="  }
                                          };




int main(int argc, char* argv[])
{

    umba::tokeniser::TrieBuilder trieBuilder;

    for(auto opInfo: operatorInfos)
    {
        trieBuilder.addTokenSequence(opInfo.operatorStr, opInfo.operatorId);
    }

    std::vector<umba::tokeniser::TrieNode> trie;
    //trie.reserve(finalTableNumEntries);

    trieBuilder.buildTokenTrie(trie);


    cout << "---------------------\n";

    // Trie size : 32 items, 384 bytes, item size: 12
    // Trie size : 32 items, 1024 bytes, item size: 32
    auto itemSize = sizeof(umba::tokeniser::TrieNode);
    std::cout << "Trie size : " << trie.size() << " items, " << trie.size()*itemSize << " bytes, item size: " << itemSize << "\n";

#if !defined(UMBA_TOKENISER_TRIE_NODE_LEVEL_FIELD_DISABLE)
    umba::tokeniser::tokenTriePrintGraph(trie, std::cout, [](umba::tokeniser::payload_type p) { return std::string(1, (char)p); } );
#endif

    std::cout << "-------\n";
    testTraverseToken(trie, "-->");
    std::cout << "-------\n";
    testTraverseToken(trie, "-=>");

    return 0;
}








