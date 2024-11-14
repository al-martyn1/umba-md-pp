/*! \file
    \brief Определение токенов, специфичных для  protogen'а
 */

#pragma once


#define PROTOGEN_TOKEN_OPERATOR_DESCRIPTION                     UMBA_TOKENIZER_TOKEN_OPERATOR_DECREMENT

// Основные ключевые слова - в первом наборе
#define PROTOGEN_TOKEN_NAMESPACE                                (UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST+0x000u)
#define PROTOGEN_TOKEN_STRUCT                                   (UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST+0x001u)
#define PROTOGEN_TOKEN_DEVICE                                   (UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST+0x002u)
#define PROTOGEN_TOKEN_AREA                                     (UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST+0x003u)
#define PROTOGEN_TOKEN_AREA_STATUS                              (UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST+0x004u)
#define PROTOGEN_TOKEN_AREA_CONTROL                             (UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST+0x004u)
//#define PROTOGEN_TOKEN_                                         (UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST+0x003u)


// Всякие импорты типа инклюдов - во втором наборе
#define PROTOGEN_TOKEN_IMPORT                                   (UMBA_TOKENIZER_TOKEN_KEYWORD_SET2_FIRST+0x000u)


// Флаги примитивных типов
#define PROTOGEN_PRIM_TYPE_FLAG_UNSIGNED                        0x400u
#define PROTOGEN_PRIM_TYPE_FLAG_INTEGRAL                        0x200u
#define PROTOGEN_PRIM_TYPE_FLAG_CHAR                            0x100u

// Примитивные типы - в третьем наборе
// Флаги и размер типа кодируем прямо в идентификаторе токена

// char у нас знаковый
#define PROTOGEN_TOKEN_TYPE_CHAR                                (UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_FIRST|PROTOGEN_PRIM_TYPE_FLAG_INTEGRAL|0x001u|PROTOGEN_PRIM_TYPE_FLAG_CHAR)

#define PROTOGEN_TOKEN_TYPE_INT8                                (UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_FIRST|PROTOGEN_PRIM_TYPE_FLAG_INTEGRAL|0x001u)
#define PROTOGEN_TOKEN_TYPE_INT16                               (UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_FIRST|PROTOGEN_PRIM_TYPE_FLAG_INTEGRAL|0x002u)
#define PROTOGEN_TOKEN_TYPE_INT32                               (UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_FIRST|PROTOGEN_PRIM_TYPE_FLAG_INTEGRAL|0x004u)

#define PROTOGEN_TOKEN_TYPE_UINT8                               (UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_FIRST|PROTOGEN_PRIM_TYPE_FLAG_INTEGRAL|0x001u|PROTOGEN_PRIM_TYPE_FLAG_UNSIGNED)
#define PROTOGEN_TOKEN_TYPE_UINT16                              (UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_FIRST|PROTOGEN_PRIM_TYPE_FLAG_INTEGRAL|0x002u|PROTOGEN_PRIM_TYPE_FLAG_UNSIGNED)
#define PROTOGEN_TOKEN_TYPE_UINT32                              (UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_FIRST|PROTOGEN_PRIM_TYPE_FLAG_INTEGRAL|0x004u|PROTOGEN_PRIM_TYPE_FLAG_UNSIGNED)

