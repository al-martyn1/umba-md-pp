/*! \file
    \brief
 */

#include "umba/umba.h"
#include "umba/tokeniser.h"


#include <iostream>
#include <map>

using std::cout;
using std::cerr;


// Inline variables - https://habr.com/ru/companies/otus/articles/561772/
// ��� �������� inline - https://en.cppreference.com/w/cpp/language/inline#Explanation

// inline constexpr

/* ��� �� ���������� trie

   � ��� ���� ������ ���������, � ���� ������-�������� - ��������� �� ������� �������.

   ��� ��������� ������ ������� �� ������� � �������� �������� �������� �� ������� ���������������, ���������
   �����-������� � ����� �������, ���� � ������� �� ����� ������� ����� 0 - ��� ����� �������� ������,
   ���� ����� �� ���� - ��� ������, ������ �� �����.

   � ������ �������� �������� ������ ���� ������ ���������� �������� �������� ������, � ������ �������� � ������ ����, ������ �� ������ ����.

   � ������ �������� ����� ������ �������� �������� (OperatorId, ��������), ��� ��������, ������� �������������, ��� �������� �������� ���.

   � ������ �������� �� ������ ������ ������� �������� ������� ���������� ������, ���

 */

// UMBA_TOKENISER_TOKEN_ID_INVALID

// using trie_index_type               = UMBA_TOKENISER_TRIE_INDEX_TYPE;
// inline constexpr trie_index_invalid = UMBA_TOKENISER_TRIE_INDEX_INVALID;
//
// using token_id_type                 = UMBA_TOKENISER_TOKEN_ID_TYPE;
// inline constexpr token_id_invalid   = UMBA_TOKENISER_TOKEN_ID_INVALID;


struct TrieNode
{
    umba::tokeniser::trie_index_type       parent_node_index;
    umba::tokeniser::trie_index_type       level_start_index;
    umba::tokeniser::token_id_type         token_id;
    char                                   symbol;

};

//typedef std::size_t OperatorId;

// umba::tokeniser::
// umba::tokeniser::trie_index_type


struct MapTrieNode
{
    TrieNode                       trieNode;
    std::map<char, MapTrieNode>    childs  ;
};


struct OperatorInfo
{
    umba::tokeniser::token_id_type     operatorId;
    std::string                        operatorStr;
};


std::vector<OperatorInfo>  operators = { { UMBA_TOKENISER_TOKEN_OPERATOR_LOGICAL_NOT             , "!"   }
                                       , { UMBA_TOKENISER_TOKEN_OPERATOR_ASSIGNMENT              , "="   }
                                       , { UMBA_TOKENISER_TOKEN_OPERATOR_ADDITION                , "+"   }
                                       , { UMBA_TOKENISER_TOKEN_OPERATOR_SUBTRACTION             , "-"   }
                                       , { UMBA_TOKENISER_TOKEN_OPERATOR_INCREMENT               , "++"  }
                                       , { UMBA_TOKENISER_TOKEN_OPERATOR_ADDITION_ASSIGNMENT     , "+="  }
                                       , { UMBA_TOKENISER_TOKEN_OPERATOR_DECREMENT               , "--"  }
                                       , { UMBA_TOKENISER_TOKEN_OPERATOR_SUBTRACTION_ASSIGNMENT  , "-="  }
                                       , { UMBA_TOKENISER_TOKEN_OPERATOR_STRUCTURE_DEREFERENCE   , "->"  }
                                       , { UMBA_TOKENISER_TOKEN_OPERATOR_EQUAL                   , "=="  }
                                       , { UMBA_TOKENISER_TOKEN_OPERATOR_NOT_EQUAL               , "!="  }
                                       , { UMBA_TOKENISER_TOKEN_OPERATOR_LAMBDA                  , "=>"  }
                                       , { UMBA_TOKENISER_TOKEN_OPERATOR_STRICT_EQUAL            , "===" }
                                       , { UMBA_TOKENISER_TOKEN_OPERATOR_STRICT_NOT_EQUAL        , "!==" }
                                       };


int main(int argc, char* argv[])
{

    std::map<char, MapTrieNode>   trieNodeMap;



    return 0;
}



