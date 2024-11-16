/*! \file
    \brief Определение токенов, специфичных для simple drawing
 */

#pragma once

/*
сноска.              footnote.         
полка.               shelf.            
полка у сноски.      shelf at footnote.
графическая сноска.  graphic footnote. 

выноски callouts

выноска полка - callout shelf

4  ----
8  --------
16 ----------------
32 --------------------------------

Больше 32 не надо

минимальное количество минусов какое?
Может 3, чтобы с декрементом не пересекалось?

---\  - callout влево вверх
---/  - callout влево вниз
/---  - callout вправо вверх
\---  - callout вправо вниз

4

<---  - горизонтальная налево
--->  - горизонтальная направо
|---  - вертикальная вниз
|^--- - вертикальная вверх

8

right shelf (after callout) .---
left shelf (before callout) ---.

---\.---  callout влево вверх with right shelf
---.---\  callout влево вверх with left shelf


горизонтальная налево со сноской налево  : ---.<---
горизонтальная налево со сноской направо : <---.--- (запрещённый вариант, синтаксически канает, надо запретить при разборе)
горизонтальная направо со сноской налево : --->---. (запрещённый вариант, синтаксически канает, надо запретить при разборе)
горизонтальная направо со сноской направо: --->.---

вертикальная вниз со сноской налево      : ---.|---
вертикальная вниз со сноской направо     : |---.---
вертикальная вверх со сноской налево     : ---.|^---
вертикальная вверх со сноской направо    : |^---.---

callout влево вверх со сноской налево    : ---.---\
callout влево вверх со сноской направо   : ---\.---
                               
callout влево вниз со сноской налево     : ---.---/
callout влево вниз со сноской направо    : ---/.---
                               
callout вправо вверх со сноской налево   : ---./---
callout вправо вверх со сноской направо  : /---.---
                               
callout вправо вниз со сноской налево    : ---.\---
callout вправо вниз со сноской направо   : \---.---



10

Нельзя после сноски налево вверх сделать полку направо

Диапазон пользовательских операторов
0x2FFF - 0x2800 = 2047 (0x7FF)

32 - 0x20

*/

#if 0

#define UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST                                           0x2000u
#define UMBA_TOKENIZER_TOKEN_OPERATOR_LAST                                            0x2FFFu

#define UMBA_TOKENIZER_TOKEN_USER_OPERATOR_FIRST                                      0x2800u
#define UMBA_TOKENIZER_TOKEN_USER_OPERATOR_LAST                                       UMBA_TOKENIZER_TOKEN_OPERATOR_LAST

#endif

// ---\  callout влево вверх
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_LEFT_TOP               (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x000)
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_LEFT_TOP_LAST          (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x01F)

// ---/  callout влево вниз
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_LEFT_BOTTOM            (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x020)
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_LEFT_BOTTOM_LAST       (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x03F)

// /---  callout вправо вверх
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_RIGHT_TOP              (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x040)
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_RIGHT_TOP_LAST         (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x05F)

// \---  callout вправо вниз
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_RIGHT_BOTTOM           (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x060)
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_RIGHT_BOTTOM_LAST      (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x07F)

// <--- - горизонтальная налево
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_LEFT                   (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x080)
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_LEFT_LAST              (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x09F)

// ---> - горизонтальная направо
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_RIGHT                  (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x0A0)
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_RIGHT_LAST             (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x0BF)

// ---|^ - вертикальная вверх
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_TOP                    (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x0C0)
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_TOP_LAST               (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x0DF)

// ---| - вертикальная вниз
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_BOTTOM                 (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x0E0)
#define SMPD_TOKEN_OPERATOR_CALLOUT_TO_BOTTOM_LAST            (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x0FF)

// .--- - right callout shelf
#define SMPD_TOKEN_OPERATOR_CALLOUT_SHELF_TO_RIGHT            (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x100)
#define SMPD_TOKEN_OPERATOR_CALLOUT_SHELF_TO_RIGHT_LAST       (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x11F)

// ---. - left callout shelf
#define SMPD_TOKEN_OPERATOR_CALLOUT_SHELF_TO_LEFT             (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x120)
#define SMPD_TOKEN_OPERATOR_CALLOUT_SHELF_TO_LEFT_LAST        (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST + 0x13F)



/*

Как крепятся сноски
По умолчанию выбирается угол или сторона прямоугольника, соответствующий направлению сноски
Для окружности это тоже вычислимо, как и для прямоугольника со скруглёнными углами
Если что-то ещё - будем думать

rect      - прямоугольник с "острыми" углами
roundrect - все углы скруглены

roundtop     - верхние скруглены, нижние обычные
roundbottom  - нижние скруглены

roundleft    - левые скруглены
roundright   - правые скруглены

roundlefttop     - скруглён левый верхний
roundrighttop    - скруглён правый верхний

roundleftbottom  - скруглён левый нижний
roundrightbottom - скруглён правый нижний


Диапазон пользовательских операторов
0x2FFF - 0x2800 = 2047 (0x7FF)

32 - 0x20

*/


// #define SMPD_TOKEN_OPERATOR_
// #define SMPD_TOKEN_OPERATOR_
// #define SMPD_TOKEN_OPERATOR_




#if 0

#define UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST                                           0x2000u
#define UMBA_TOKENIZER_TOKEN_OPERATOR_LAST                                            0x2FFFu

#define UMBA_TOKENIZER_TOKEN_USER_OPERATOR_FIRST                                      0x2800u
#define UMBA_TOKENIZER_TOKEN_USER_OPERATOR_LAST                                       UMBA_TOKENIZER_TOKEN_OPERATOR_LAST




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

#endif