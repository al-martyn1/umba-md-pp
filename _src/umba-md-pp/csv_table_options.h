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

    CsvTitle    сsvTitle   = CsvTitle::none; //! Способ формирования заголовка таблицы
    char        charSep    = 0;
    char        charQuot   = '\"';
    bool        strict     = false;

    std::string multiTableSeparator; // "-------" или типа того

    std::vector<std::string>         multiTitles; // "Заголовки" таблиц, а не заголовки таблицы

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
        // while(title.size()<2)
        //     title.emplace_back(std::string());

        for(auto &t: title)
        {
            colAlignments.push_back(extractTableCellAlignment(t));
        }
    }

    void setMultiTitle( std::string titleStr
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

        multiTitles = marty_cpp::splitToLinesSimple(titleStr, false, '|');
    }

    template<typename T>
    static
    T getVectorValueAt(const std::vector<T> &vec, std::size_t idx, T defVal)
    {
        return idx<vec.size() ? vec[idx] : defVal;
    }

    TableCellAlignment getColAlignment(std::size_t idx) const
    {
        return getVectorValueAt(colAlignments, idx, TableCellAlignment::left);
    }

    std::string getTitle(std::size_t idx) const
    {
        return getVectorValueAt(title, idx, std::string());
    }

    std::string getMultiTitle(std::size_t idx) const
    {
        return getVectorValueAt(multiTitles, idx, std::string());
    }


}; // struct CsvTableOptions



