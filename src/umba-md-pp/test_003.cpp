/*! \file
    \brief
 */

#define UMBA_TOKENISER_DISABLE_UMBA_TOKENISER_GET_CHAR_CLASS_FUNCTION

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

/* Как мы организуем trie

   У нас есть массив элементов, и есть индекс-итератор - указатель на текущий элемент.

   При получении нового символа мы начиная с текущего элемента проходим по массиву последовательно, сравнивая
   ключи-символы с нашим текущим, если в массиве на месте символа лежит 0 - это конец текущего уровня,
   если дошли до него - это значит, ничего не нашли.

   В каждом элементе текущего уровня есть индекс стартового элемента текущего уровня (levelStartIndex),
   и индекс элемента с уровня выше, откуда мы пришли сюда (parentNodeIndex).

   В каждом элементе также храним полезную нагрузку (tokenId, например), или значение, которое сигнализирует, что полезной нагрузки нет.

   В каждом элементе мы храним индекс первого элемента таблицы следующего уровня, или trie_index_invalid, если детей нет

 */

// UMBA_TOKENISER_TOKEN_ID_INVALID

// using trie_index_type               = UMBA_TOKENISER_TRIE_INDEX_TYPE;
// inline constexpr trie_index_invalid = UMBA_TOKENISER_TRIE_INDEX_INVALID;
//
// using token_id_type                 = UMBA_TOKENISER_TOKEN_ID_TYPE;
// inline constexpr token_id_invalid   = UMBA_TOKENISER_TOKEN_ID_INVALID;

// // plain структура, никаких конструкторов и инициализаций - чисто сишечная структура
// typedef struct tag_umba_tokeniser_trie_node
// {
//     UMBA_TOKENISER_TRIE_INDEX_TYPE       parentNodeIndex;
//     UMBA_TOKENISER_TRIE_INDEX_TYPE       levelStartIndex;
//     UMBA_TOKENISER_TRIE_INDEX_TYPE       levelSize      ;
//     UMBA_TOKENISER_TRIE_INDEX_TYPE       childsIndex    ;
//  
//     UMBA_TOKENISER_TOKEN_ID_TYPE         tokenId        ;
//     char                                 symbol         ;
//  
// } umba_tokeniser_trie_node;


template<typename ContainerType>
umba::tokeniser::trie_index_type tokenTrieGoNext(const ContainerType &tokenTrie, umba::tokeniser::trie_index_type curIndex, char ch)
{
    return umba::tokeniser::tokenTrieFindNext(tokenTrie, curIndex, ch);
}


inline
void printTokenTrieNode(const umba::tokeniser::TrieNode &tn)
{
    using namespace std;

    cout << "symbol   : "; 
    if (tn.symbol>=' ')
       cout << "'" << (char)tn.symbol << "'";
    else
       cout << " " << (unsigned)(unsigned char)tn.symbol;
    cout << "\n";

    cout << "tokenId   : " << tn.tokenId << "\n";

    cout << "parentIdx: " << tn.parentNodeIndex << "\n";
    cout << "levelIdx : " << tn.lookupChunkStartIndex << "\n";
    cout << "levelSize: " << tn.lookupChunkSize       << "\n";
    cout << "childsIdx: " << tn.childsIndex     << "\n";
    cout << "tokenId  : " << tn.tokenId         << "\n";

}

#if 1
template<typename ContainerType>
void testTraverseToken(const ContainerType &tokenTrie, const std::string &str)
{
    using namespace std;

    using namespace umba::tokeniser;

    trie_index_type idx = umba::tokeniser::trie_index_invalid; // trie_index_initial;
    token_id_type   foundToken = token_id_invalid;

    for(auto ch : str)
    {
        //printTokenTrieNode
        trie_index_type nextIdx = tokenTrieGoNext(tokenTrie, idx, ch);

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
        foundToken = tokenTrie[idx].tokenId;
        cout << "Backtrace: ";
        tokenTrieBackTrace(tokenTrie, idx, [](char ch) { cout << ch; });
        cout << "\n";
    }
    
    if (foundToken==token_id_invalid)
    {
        cout << "Token not found\n";
    }
    else
    {
        cout << "Found token (" << str << "): " << foundToken << "\n";
    }

}

    // void printTokenTrieNode(const umba::tokeniser::TrieNode &tn)
    // umba::tokeniser::trie_index_type tokenTrieGoNext(const ContainerType &trie, umba::tokeniser::trie_index_type curIndex, char ch)

#endif

//typedef std::size_t OperatorId;

// umba::tokeniser::
// umba::tokeniser::trie_index_type

struct OperatorInfo
{
    umba::tokeniser::token_id_type     operatorId;
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


// struct MapTrieNode
// {
//     umba::tokeniser::TrieNode      trieNode;
//     std::map<char, MapTrieNode>    childs  ;
//  
//     MapTrieNode() : trieNode(), childs()
//     {
//         trieNodeInitMakeUninitialized(trieNode);
//     }
//  
//     MapTrieNode(const MapTrieNode &) = default;
//     MapTrieNode& operator=(const MapTrieNode &) = default;
//     MapTrieNode(MapTrieNode &&) = default;
//     MapTrieNode& operator=(MapTrieNode &&) = default;
//  
// };
//  
// typedef std::map<char, MapTrieNode> TrieNodesMap;




int main(int argc, char* argv[])
{

    // TrieNodesMap  trieNodesMap;
    // std::size_t finalTableNumEntries = 0;
    //  
    // for(auto opInfo: operatorInfos)
    // {
    //     auto pCurMap = &trieNodesMap;
    //     MapTrieNode *pCurNode = 0;
    //  
    //     for(auto ch: opInfo.operatorStr)
    //     {
    //         auto &curMap = *pCurMap;
    //         pCurNode = &curMap[ch]; // .second;
    //         pCurMap  = &pCurNode->childs;
    //  
    //         pCurNode->trieNode.symbol = ch;
    //  
    //         ++finalTableNumEntries;
    //     }
    //  
    //     // Прошагали всё символы обрабатываемой строки
    //     // Финальный узел у нас есть
    //     // Записываем туда идентификатор оператора
    //     pCurNode->trieNode.tokenId = opInfo.operatorId;
    // }

    umba::tokeniser::TrieBuilder trieBuilder;

    for(auto opInfo: operatorInfos)
    {
        trieBuilder.addTokenSequence(opInfo.operatorStr, opInfo.operatorId);
    }

    std::vector<umba::tokeniser::TrieNode> trie; 
    //trie.reserve(finalTableNumEntries);

    trieBuilder.buildTokenTrie(trie);

#if 0
    struct QueItem
    {
        umba::tokeniser::TrieBuilder::TrieNodesMap   *pMap;
        umba::tokeniser::trie_index_type             level;
        umba::tokeniser::trie_index_type             parentNodeIndex;
        //umba::tokeniser::trie_index_type    lookupChunkStartIndex;
        //umba::tokeniser::trie_index_type    levelChildsSize;
    };


    std::deque<QueItem>  que;
    //QueItem firstItem = {&trieNodesMap, umba::tokeniser::trie_index_invalid, umba::tokeniser::trie_index_initial};
    //que.emplace_back(firstItem);
    que.emplace_back( QueItem{ &trieBuilder.m_trieNodesMap
                             , 0                                    // level
                             , umba::tokeniser::trie_index_invalid  // parentNodeIndex
                             // , 0                                      // levelStartIndex
                             // , trieNodesMap.size()                    // levelSize
                             // , 0                                      // lookupChunkStartIndex
                             }
                    );

    std::size_t nodeIdx = 0;
    while(!que.empty())
    {
        QueItem qi = que.front();  que.pop_front();
        auto &m = *qi.pMap;
        //std::size_t childsLevelSize = 0;
        for(auto &kv : m)
        {
            kv.second.nodeIndex = nodeIdx++;
            kv.second.level     = qi.level;
            if (!kv.second.childs.empty())
            {
                que.emplace_back(QueItem{&kv.second.childs, kv.second.level+1 /* kv.second.childs.nodeIndex */ });
            }
        }
    }

    que.emplace_back( QueItem{ &trieBuilder.m_trieNodesMap
                             , 0                                    // level
                             , umba::tokeniser::trie_index_invalid  // parentNodeIndex
                             }
                    );

    while(!que.empty())
    {
        QueItem qi = que.front();  que.pop_front();
        auto &m = *qi.pMap;
        std::size_t curParentNodeIndex = qi.parentNodeIndex;

        if (m.empty())
        {
            continue;
        }

        umba::tokeniser::trie_index_type lookupChunkStartIndex = m.begin()->second.nodeIndex;
        for(auto &kv : m)
        {
            trie.emplace_back(kv.second.trieNode); // tokenId тут уже настроен, а childsIndex - инвалид
            trie.back().parentNodeIndex       = curParentNodeIndex;
            trie.back().lookupChunkStartIndex = lookupChunkStartIndex;
            trie.back().lookupChunkSize       = m.size();
            trie.back().symbol                = kv.first;

            if (!kv.second.childs.empty())
            {
                trie.back().childsIndex = kv.second.childs.begin()->second.nodeIndex;
                que.emplace_back(QueItem{&kv.second.childs, kv.second.level+1, kv.second.nodeIndex});
            }
        }
    }
#endif

// typedef struct tag_umba_tokeniser_trie_node
// {
//     UMBA_TOKENISER_TRIE_INDEX_TYPE       parentNodeIndex      ;
//     UMBA_TOKENISER_TRIE_INDEX_TYPE       lookupChunkStartIndex; /* Одно и то же значение для всех элементов lookupChunk'а */
//     UMBA_TOKENISER_TRIE_INDEX_TYPE       lookupChunkSize      ; /* Одно и то же значение для всех элементов lookupChunk'а */
//     UMBA_TOKENISER_TRIE_INDEX_TYPE       childsIndex          ;
// #if !defined(UMBA_TOKENISER_TRIE_NODE_LEVEL_FIELD_DISABLE)
//     UMBA_TOKENISER_TRIE_INDEX_TYPE       level                ; // Нужно, чтобы делать красивый граф таблицы trie
// #endif
//     UMBA_TOKENISER_TOKEN_ID_TYPE         tokenId              ;
//     char                                 symbol               ;
//  
// } umba_tokeniser_trie_node;




#if 0
    while(!que.empty())
    {
        QueItem qi = que.front();
        que.pop_front();

        // cout << "---------------------\n";

        std::size_t curParentNodeIndex = qi.parentNodeIndex;
        std::size_t levelStartIndex    = qi.levelStartIndex;
        std::size_t levelSize          = qi.levelSize;
        std::size_t curIndex           = qi.lookupChunkStartIndex;

        auto &m = *qi.pMap;
        //std::size_t curLevelSize    = m.size();
        std::size_t childsLevelSize = 0;
        for(auto &kv : m)
        {
            childsLevelSize += kv.second.childs.size();
        }


        //std::size_t nextLevelStartIndex = levelStartIndex+childsLevelSize; // m.size();
        //std::size_t nextLevelStartIndex = levelStartIndex+m.size();
        std::size_t nextLevelStartIndex  = levelStartIndex+levelChildsSize;
        std::size_t buddyLevelStartIndex = levelStartIndex+m.size();
        
        // cout << "levelStartIndex      : " << levelStartIndex    << "\n";
        // cout << "levelChildsSize      : " << levelChildsSize    << "\n";
        // cout << "curParentNodeIndex   : " << curParentNodeIndex << "\n";
        // cout << "curIndex             : " << curIndex           << "\n";
        // cout << "curLevelSize         : " << curLevelSize       << "\n";
        // cout << "childsLevelSize      : " << childsLevelSize    << "\n";
        // cout << "nextLevelStartIndex  : " << nextLevelStartIndex  << "\n";
        // cout << "buddyLevelStartIndex : " << buddyLevelStartIndex << "\n";

        for(auto &kv : m)
        {
            // cout << "---\n";

            trie.emplace_back(kv.second.trieNode); // tokenId тут уже настроен, а childsIndex - инвалид

            // cout << "    Trie size: " << trie.size() << "\n";
            // cout << "    curIndex : " << curIndex    << "\n";

            trie[curIndex].parentNodeIndex = curParentNodeIndex;
            trie[curIndex].levelStartIndex = levelStartIndex;
            trie[curIndex].levelSize       = m.size();
            trie[curIndex].symbol          = kv.first;

            if (!kv.second.childs.empty())
            {
                trie[curIndex].childsIndex = nextLevelStartIndex;
                //que.emplace_back(QueItem{&kv.second.childs, curIndex, buddyLevelStartIndex, childsLevelSize});
                nextLevelStartIndex += kv.second.childs.size();
            }
            
            ++curIndex;
        }
    
    }
#endif

    cout << "---------------------\n";

    std::cout << "Trie size : " << trie.size() << " items, " << trie.size()*sizeof(umba::tokeniser::TrieNode) << " bytes\n";
    umba::tokeniser::tokenTriePrintGraph(trie, std::cout);

    std::cout << "-------\n";
    testTraverseToken(trie, "-->");
    std::cout << "-------\n";
    testTraverseToken(trie, "-=>");

    return 0;
}


// Надо пройти по текущей мапе, положить все TrieNode в массив
// [level 0][level 1 char 1][level 1 char 2][level 1 char 3][level 2 char 1][level 2 char 2]...








