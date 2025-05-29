#pragma once

#include "umba/umba.h"
#include "umba/filename.h"
#include "umba/filesys.h"
#include "umba/string_plus.h"
//
#include "utils.h"
//
#include "enums.h"
// 
#include "log.h"
//
#include <string>


struct CsvTableOptions
{
    // ArgListType                 argListType = ArgListType::unknown;
    // ArgListValueStyle           argListValueStyle = ArgListValueStyle::unknown;
    //std::string                 fileName;

    std::vector<std::string>         title;

    std::vector<TableCellAlignment>  colAlignments;

    CsvTitle      = CsvTitle::none; //! Способ формирования заголовка таблицы
    char charSep  = 0;
    char charQuot = 0;

    std::string multiTableSeparator; // "-------" или типа того

    std::vector<std::string>         multiTitles; // "Заголовки" таблиц, а не заголовки таблицы

    // csv-title
    // sep  - none/auto, comma, semicolon, colon, hash, bar/vertical-bar, tab
    // quot - none/auto, quot, apos, tick/backtick


// enum class CsvQuot : std::uint32_t
// {
//     invalid       = (std::uint32_t)(-1) /*!< ! */,
//     unknown       = (std::uint32_t)(-1) /*!< ! */,
//     none          = 0x0000 /*!< ! */,
//     auto_         = 0x0001 /*!< Autodetect CSV quot */,
//     detect        = 0x0001 /*!< Autodetect CSV quot */,
//     comma         = 0x0002 /*!<  */,
//     semicolon     = 0x0003 /*!<  */,
//     colon         = 0x0004 /*!<  */,
//     hash          = 0x0005 /*!<  */,
//     number        = 0x0005 /*!<  */,
//     verticalBar   = 0x0006 /*!<  */,
//     bar           = 0x0006 /*!<  */,
//     tab           = 0x0007 /*!<  */
//  
// }; // enum 

// enum class CsvSeparator : std::uint32_t
// {
//     invalid    = (std::uint32_t)(-1) /*!< ! */,
//     unknown    = (std::uint32_t)(-1) /*!< ! */,
//     none       = 0x0000 /*!< ! */,
//     auto_      = 0x0001 /*!< Autodetect CSV separator */,
//     detect     = 0x0001 /*!< Autodetect CSV separator */,
//     quot       = 0x0002 /*!<  */,
//     apos       = 0x0003 /*!<  */,
//     backtick   = 0x0004 /*!<  */,
//     tick       = 0x0004 /*!<  */
//  
// }; // enum 

// enum class CsvTitle : std::uint32_t
// {
//     invalid   = (std::uint32_t)(-1) /*!< ! */,
//     unknown   = (std::uint32_t)(-1) /*!< ! */,
//     none      = 0x0000 /*!< ! */,
//     use       = 0x0001 /*!< Use title from CSV file (firts line threated as title). Missing value (none) threated as `use` */,
//     no        = 0x0002 /*!< CSV file has no title, use taken title or empty title */,
//     ignore    = 0x0003 /*!< Ignore title from CSV file and use taken title or empty title */,
//     merge     = 0x0004 /*!< Merge align and title from `title` attr and CSV file (alignment also used from `title` attribute) */,
//     align     = 0x0005 /*!< Merge align only from `title` attr, but use title from CSV file */
//  
// }; // enum 


    // bool setListType(const std::string &typeStr)
    // {
    //     auto et = enum_deserialize(typeStr, ArgListType::invalid);
    //     if (et==ArgListType::invalid)
    //         return false;
    //  
    //     argListType = et;
    //     return true;
    // }
    //  
    // bool setListValueStyle(const std::string &typeStr)
    // {
    //     auto et = enum_deserialize(typeStr, ArgListValueStyle::invalid);
    //     if (et==ArgListValueStyle::invalid)
    //         return false;
    //  
    //     argListValueStyle = et;
    //     return true;
    // }

    //TableCellAlignment tableCellAlignment = extractTableCellAlignment(t);

    void setTitle( std::string titleStr
                 , bool bReplaceCommas=false // запятые заменяем на символ пайпа только при обработке аргумента командной строки - там с символом пайпа могут быть проблемы
                 )
    {
        umba::string_plus::trim(titleStr);

        for(auto &ch: titleStr)
        {
            if (ch=='|')
                bReplaceCommas = false; // Но у нас есть символ пайпа, наверное, опция командной строки передана через респонз файл
        }

        if (bReplaceCommas)
        {
            for(auto &ch: titleStr)
            {
                if (ch==',')
                    ch = '|';
            }
        }

        if (!titleStr.empty() && titleStr.front()=='|')
            titleStr.erase(0, 1);

        if (!titleStr.empty() && titleStr.back()!='|')
            titleStr.append(1, '|');

        title = marty_cpp::splitToLinesSimple(titleStr, false, '|');
        while(title.size()<2)
            title.emplace_back(std::string());

        for(auto &t: title)
        {
            colAlignments.push_back(extractTableCellAlignment(t));
        }
    }

    TableCellAlignment getColAlignment(std::size_t idx) const
    {
        return idx<colAlignments.size() ? colAlignments[idx] : TableCellAlignment::left;
    }




}; // struct ArgListOptions



