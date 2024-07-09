/*! \file
    \brief Задаем тут конфигурационные макросы, в основном для отладки каки-то частей/нюансов. Задаём тут, чтобы не засорять проектный файл. Для релиза всё отключаем.
 */

#pragma once


#if !defined(NDEBUG)

    #define LOG_DOC_INSERTIONS_AND_REFS

#endif

