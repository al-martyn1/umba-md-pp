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


// GRAPHVIZ_DOT - переменная окружения, которую надо проверить до всего остального
// аналогичные переменные надо проверять для остальных тулзей

struct ArgListOptions
{
    ArgListType                 argListType = ArgListType::unknown;
    ArgListValueStyle           argListValueStyle = ArgListValueStyle::unknown;
    //std::string                 fileName;
    std::vector<std::string>    title;


    bool setListType(const std::string &typeStr)
    {
        auto et = enum_deserialize(typeStr, ArgListType::invalid);
        if (et==ArgListType::invalid)
            return false;

        argListType = et;
        return true;
    }

    bool setListValueStyle(const std::string &typeStr)
    {
        auto et = enum_deserialize(typeStr, ArgListValueStyle::invalid);
        if (et==ArgListValueStyle::invalid)
            return false;

        argListValueStyle = et;
        return true;
    }

    void setListTitle( std::string titleStr
                     , bool bReplaceCommas=false // запятые заменяем на символ пайпа только при обработке арзумента командной строки - там с символом пайпа могут быть проблемы
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
    }


}; // struct ArgListOptions



