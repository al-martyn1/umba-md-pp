/*! \file
    \brief Токенизатор/лексер для protogen'а
 */

#pragma once

#include "umba/umba.h"
//
#include "umba/tokenizer.h"
#include "umba/tokenizer/token_filters.h"
#include "umba/tokenizer/string_literal_parsing.h"
#include "umba/string_plus.h"

//
#include "smpd-tokens.h"




// protogen::
namespace protogen {


template< typename CharType                     //!< Input chars type
        , typename UserDataType        = void*  //!< Пользовательские данные
        , typename CharClassTableType  = std::array<umba::tokenizer::CharClass, 128> //!< Таблица классов символов
        , typename TrieVectorType      = std::vector<umba::tokenizer::TrieNode>      //!< Тип Trie
        , typename StringType          = std::basic_string<CharType>  //!< Тип строки, которая хранит входные символы
        , typename MessagesStringType  = std::string  //!< Тип строки, которая используется для сообщений (в том числе и от внешних суб-парсеров)
        , typename InputIteratorType   = umba::iterator::TextPositionCountingIterator<CharType>
        , typename InputIteratorTraits = umba::iterator::TextIteratorTraits<InputIteratorType>
        , typename TokenizerType       = umba::tokenizer::Tokenizer< CharType, UserDataType, CharClassTableType, TrieVectorType, StringType, MessagesStringType, InputIteratorType >
        >
umba::tokenizer::TokenizerBuilder<CharType, UserDataType, CharClassTableType, TrieVectorType, StringType, MessagesStringType, InputIteratorType, InputIteratorTraits, TokenizerType>
makeTokenizerBuilderProtogen()
{
    using CppStringLiteralParser     = umba::tokenizer::CppEscapedSimpleQuotedStringLiteralParser<CharType, MessagesStringType, InputIteratorType, InputIteratorTraits>;
    using AngleBracketsLiteralParser = umba::tokenizer::SimpleQuotedStringLiteralParser<CharType, MessagesStringType, InputIteratorType, InputIteratorTraits>;

    using umba::string_plus::make_string;

    auto tokenizerBuilder = umba::tokenizer::TokenizerBuilder<CharType, UserDataType, CharClassTableType, TrieVectorType, StringType, MessagesStringType, InputIteratorType, InputIteratorTraits, TokenizerType>()

                          .generateStandardCharClassTable()


                          .addNumbersPrefix(make_string<StringType>("0b"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN)
                          .addNumbersPrefix(make_string<StringType>("0B"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN, true) // allowUseExistingToken

                          .addNumbersPrefix(make_string<StringType>("0d"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC)
                          .addNumbersPrefix(make_string<StringType>("0D"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC, true) // allowUseExistingToken

                          .addNumbersPrefix(make_string<StringType>("0") , UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_OCT | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FLAG_MISS_DIGIT)

                          .addNumbersPrefix(make_string<StringType>("0x"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX)
                          .addNumbersPrefix(make_string<StringType>("0X"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX, true) // allowUseExistingToken


                          .addBrackets(make_string<StringType>("{}"), UMBA_TOKENIZER_TOKEN_CURLY_BRACKETS )
                          .addBrackets(make_string<StringType>("()"), UMBA_TOKENIZER_TOKEN_ROUND_BRACKETS )
                          .addBrackets(make_string<StringType>("<>"), UMBA_TOKENIZER_TOKEN_ANGLE_BRACKETS )
                          .addBrackets(make_string<StringType>("[]"), UMBA_TOKENIZER_TOKEN_SQUARE_BRACKETS)


                          .addSingleLineComment(make_string<StringType>("//"), UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST)
                          .setMultiLineComment(make_string<StringType>("/*"), make_string<StringType>("*/"))


                          // Операторы # и ## доступны только внутри директивы define препроцессора.
                          // Для этого вначале работы мы сбрасываем признак umba::tokenizer::CharClass::opchar,
                          // при получении маркера директивы define - устанавливаем его,
                          // и при окончании блока препроцессора опять сбрасываем

                          // .addOperator(make_string<StringType>("#"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_CC_PP_CONCAT                  )
                          // .addOperator(make_string<StringType>("##" ), UMBA_TOKENIZER_TOKEN_OPERATOR_CC_PP_STRINGIFY               )
 
                          //.addOperator(make_string<StringType>("."  ), UMBA_TOKENIZER_TOKEN_OPERATOR_DOT                           )
                          //.addOperator(make_string<StringType>("..."), UMBA_TOKENIZER_TOKEN_OPERATOR_VA_ARGS                       )
                          //.addOperator(make_string<StringType>("+"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_ADDITION                      )
                          //.addOperator(make_string<StringType>("-"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION                   )
                          //.addOperator(make_string<StringType>("*"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION                )
                          //.addOperator(make_string<StringType>("/"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_DIVISION                      )
                          //.addOperator(make_string<StringType>("%"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_MODULO                        )
                          //.addOperator(make_string<StringType>("++" ), UMBA_TOKENIZER_TOKEN_OPERATOR_INCREMENT                     )
                          //.addOperator(make_string<StringType>("--" ), UMBA_TOKENIZER_TOKEN_OPERATOR_DECREMENT                     )
                          //.addOperator(make_string<StringType>("==" ), UMBA_TOKENIZER_TOKEN_OPERATOR_EQUAL                         )
                          //.addOperator(make_string<StringType>("!=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_NOT_EQUAL                     )
                          //.addOperator(make_string<StringType>(">"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER                       )
                          //.addOperator(make_string<StringType>("<"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_LESS                          )
                          //.addOperator(make_string<StringType>(">=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER_EQUAL                 )
                          //.addOperator(make_string<StringType>("<=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_LESS_EQUAL                    )
                          //.addOperator(make_string<StringType>("<=>"), UMBA_TOKENIZER_TOKEN_OPERATOR_THREE_WAY_COMPARISON          )
                          //.addOperator(make_string<StringType>("!"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_NOT                   )
                          //.addOperator(make_string<StringType>("&&" ), UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_AND                   )
                          //.addOperator(make_string<StringType>("||" ), UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_OR                    )
                          //.addOperator(make_string<StringType>("~"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_NOT                   )
                          //.addOperator(make_string<StringType>("&"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND                   )
                          //.addOperator(make_string<StringType>("|"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_OR                    )
                          //.addOperator(make_string<StringType>("^"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_XOR                   )
                          //.addOperator(make_string<StringType>("<<" ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_LEFT            )
                          //.addOperator(make_string<StringType>(">>" ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT           )
                          //.addOperator(make_string<StringType>("="  ), UMBA_TOKENIZER_TOKEN_OPERATOR_ASSIGNMENT                    )
                          //.addOperator(make_string<StringType>("+=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_ADDITION_ASSIGNMENT           )
                          //.addOperator(make_string<StringType>("-=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION_ASSIGNMENT        )
                          //.addOperator(make_string<StringType>("*=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION_ASSIGNMENT     )
                          //.addOperator(make_string<StringType>("/=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_DIVISION_ASSIGNMENT           )
                          //.addOperator(make_string<StringType>("%=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_MODULO_ASSIGNMENT             )
                          //.addOperator(make_string<StringType>("&=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND_ASSIGNMENT        )
                          //.addOperator(make_string<StringType>("|=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_OR_ASSIGNMENT         )
                          //.addOperator(make_string<StringType>("^=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_XOR_ASSIGNMENT        )
                          //.addOperator(make_string<StringType>("<<="), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_LEFT_ASSIGNMENT )
                          //.addOperator(make_string<StringType>(">>="), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT_ASSIGNMENT)
                          //.addOperator(make_string<StringType>(","  ), UMBA_TOKENIZER_TOKEN_OPERATOR_COMMA)
                          //.addOperator(make_string<StringType>("->" ), UMBA_TOKENIZER_TOKEN_OPERATOR_MEMBER_OF_POINTER             )
                          //.addOperator(make_string<StringType>("->*"), UMBA_TOKENIZER_TOKEN_OPERATOR_POINTER_TO_MEMBER_OF_POINTER  )
                          //.addOperator(make_string<StringType>(".*" ), UMBA_TOKENIZER_TOKEN_OPERATOR_POINTER_TO_MEMBER_OF_OBJECT   )
                          //.addOperator(make_string<StringType>(":"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_TERNARY_ALTERNATIVE           )
                          //.addOperator(make_string<StringType>("?"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_TERNARY_CONDITIONAL           )
                          //.addOperator(make_string<StringType>("::" ), UMBA_TOKENIZER_TOKEN_OPERATOR_SCOPE_RESOLUTION              )
                          //.addOperator(make_string<StringType>(";"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_EXPRESSION_END                )
                          //.addOperator( )

                          .template addStringLiteralParser<CppStringLiteralParser>( UMBA_TOKENIZER_TOKEN_STRING_LITERAL
                                                                                  , { make_string<StringType>("\"")   // UMBA_TOKENIZER_TOKEN_STRING_LITERAL itself
                                                                                    , make_string<StringType>("\'")   // UMBA_TOKENIZER_TOKEN_CHAR_LITERAL (UMBA_TOKENIZER_TOKEN_STRING_LITERAL+1)
                                                                                    }
                                                                                  )
                          .template addStringLiteralParser< AngleBracketsLiteralParser >(UMBA_TOKENIZER_TOKEN_ANGLE_BACKETS_STRING_LITERAL, { make_string<StringType>("<") } )

                          ;

    return tokenizerBuilder;

}

template<typename TokenizerBuilder, typename TokenHandler>
//typename TokenizerBuilder::tokenizer_type makeTokenizerCpp(const TokenizerBuilder &builder, TokenHandler tokenHandler, bool suffixGluing=true, bool preprocessorFilter=true)
typename TokenizerBuilder::tokenizer_type makeTokenizerProtogen(TokenizerBuilder builder, TokenHandler tokenHandler)
{

    using tokenizer_type = typename TokenizerBuilder::tokenizer_type;
    using string_type    = typename tokenizer_type::string_type;

    using SimpleSequenceComposingFilter       = umba::tokenizer::filters::SimpleSequenceComposingFilter<tokenizer_type>;
    using IdentifierToKeywordConversionFilter = umba::tokenizer::filters::IdentifierToKeywordConversionFilter<tokenizer_type>;
    using RawCharsCollectingFilter            = umba::tokenizer::filters::RawCharsCollectingFilter<tokenizer_type>;

    auto tokenizer = builder.makeTokenizer();
    tokenizer.tokenHandler = tokenHandler;


    auto options = tokenizer.getOptions();
    options.unclassifiedCharsRaw = false;
    tokenizer.setOptions(options);


    // !!! Фильтры, установленные позже, отрабатывают раньше

    #if 0
    tokenizer.template installTokenFilter<IdentifierToKeywordConversionFilter>( UMBA_TOKENIZER_TOKEN_IDENTIFIER
                                                                              , PROTOGEN_TOKEN_AREA
                                                                              , std::unordered_map<string_type, umba::tokenizer::payload_type>

                                                                                { {"status"        , PROTOGEN_TOKEN_AREA_STATUS   }
                                                                                , {"readonly"      , PROTOGEN_TOKEN_AREA_STATUS   }
                                                                                , {"ro"            , PROTOGEN_TOKEN_AREA_STATUS   }

                                                                                , {"control"       , PROTOGEN_TOKEN_AREA_CONTROL  }
                                                                                , {"writearea"     , PROTOGEN_TOKEN_AREA_CONTROL  }
                                                                                , {"readwrite"     , PROTOGEN_TOKEN_AREA_CONTROL  }
                                                                                , {"rw"            , PROTOGEN_TOKEN_AREA_CONTROL  }

                                                                                //, {""          ,        }
                                                                                }
                                                                              );

    tokenizer.template installTokenFilter<IdentifierToKeywordConversionFilter>( UMBA_TOKENIZER_TOKEN_IDENTIFIER
                                                                              , std::unordered_map<string_type, umba::tokenizer::payload_type>

                                                                                { {"namespace"       , PROTOGEN_TOKEN_NAMESPACE    }
                                                                                , {"struct"          , PROTOGEN_TOKEN_STRUCT       }
                                                                                , {"device"          , PROTOGEN_TOKEN_DEVICE       }
                                                                                , {"area"            , PROTOGEN_TOKEN_AREA         }
                                                                                
                                                                                , {"char"            , PROTOGEN_TOKEN_TYPE_CHAR    }
                                                                                , {"int8"            , PROTOGEN_TOKEN_TYPE_INT8    }
                                                                                , {"int16"           , PROTOGEN_TOKEN_TYPE_INT16   }
                                                                                , {"int32"           , PROTOGEN_TOKEN_TYPE_INT32   }
                                                                                , {"uint8"           , PROTOGEN_TOKEN_TYPE_UINT8   }
                                                                                , {"uint16"          , PROTOGEN_TOKEN_TYPE_UINT16  }
                                                                                , {"uint32"          , PROTOGEN_TOKEN_TYPE_UINT32  }

                                                                                //, {""          ,        }
                                                                                }
                                                                              );
    #endif

    // tokenizer.template installTokenFilter<umba::tokenizer::filters::DblSquareBracketOpenComposingFilter <tokenizer_type> >();
    // tokenizer.template installTokenFilter<umba::tokenizer::filters::DblSquareBracketCloseComposingFilter<tokenizer_type> >();

    // tokenizer.template installTokenFilter<RawCharsCollectingFilter>();

    
    // !!! Фильтры, установленные позже, отрабатывают раньше

    // if (preprocessorFilter)
    //     tokenizer.template installTokenFilter<umba::tokenizer::filters::CcPreprocessorFilter<typename TokenizerBuilder::tokenizer_type> >();
    //  
    // if (suffixGluing)
    //     tokenizer.template installTokenFilter<umba::tokenizer::filters::SimpleSuffixGluingFilter<typename TokenizerBuilder::tokenizer_type> >();

    // Символ хэша приобретает значение операторного только внутри директивы define
    // По умолчанию он не операторный
    // Сбрасываем операторный флаг для символа '#'
    // tokenizer.setResetCharClassFlags('#', umba::tokenizer::CharClass::none, umba::tokenizer::CharClass::opchar); // Ничего не устанавливаем, сбрасываем opchar

    return tokenizer;
}



} // namespace protogen

// protogen::
