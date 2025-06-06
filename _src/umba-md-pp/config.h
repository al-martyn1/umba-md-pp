/*! \file
    \brief Задаем тут конфигурационные макросы, в основном для отладки каки-то частей/нюансов. Задаём тут, чтобы не засорять проектный файл. Для релиза всё отключаем.
 */

#pragma once

#define UMBA_MD_PP_DEBUG

// Если релиз (NDEBUG), то сбрасываем UMBA_MD_PP_DEBUG, но можно закоментить строчку, и тогда будет и релизе наш отладочный режим
#if defined(NDEBUG)
    // #undef UMBA_MD_PP_DEBUG
#endif


#if defined(UMBA_MD_PP_DEBUG)

    // Тут включаем/выключаем опции отладки

    // Вроде отладили, выключаем
    //#define LOG_DOC_INSERTIONS_AND_REFS
    //#define LOG_PROCESSING_UPDINDOCREFS_BEFORE_AFTER
    //#define LOG_PROCESSING_TRANSFORM_BEFORE_AFTER

    //#define LOG_PROCESSING_PROCESS_TEXT_LINES_SIMPLE_PRINT_PNG_LINES
    //#define LOG_PROCESSING_EXTRACT_IMAGE_LINKS

#endif



