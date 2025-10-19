#pragma once

#include "umba/umba.h"
#include "umba/filename.h"
#include "umba/filesys.h"
#include "umba/shellapi.h"
//
#include "md_pp_html.h"
#include "arg_list_options.h"
#include "extern_tools.h"
#include "generation_cache_info.h"
#include "document.h"
#include "utils.h"
// 
#include "log.h"



// umba::md::
namespace umba {
namespace md {

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename FilenameStringType>
void updateArgListOptions(const AppConfig<FilenameStringType> &appCfg, const umba::html::HtmlTag &mdHtmlTag, ArgListOptions &argListOptions)
{
    // Атрибуты:
    //   file      - имя входного файла
    //   type      - тип списка аргументов
    //   title     - заголовок, может не быть, тогда два пустых значения

    using namespace umba::filename;

    // if (mdHtmlTag.hasAttr("file"))
    // {
    //     auto attrVal = mdHtmlTag.getAttrValue("file", std::string());
    //     argListOptions.file = appendPath(getPath(attrVal), getName(attrVal));
    // }

    if (mdHtmlTag.hasAttr("type"))
    {
        argListOptions.setListType(mdHtmlTag.getAttrValue("type", std::string()));
    }

    if (mdHtmlTag.hasAttr("value-style"))
    {
        argListOptions.setListValueStyle(mdHtmlTag.getAttrValue("value-style", std::string()));
    }

    if (mdHtmlTag.hasAttr("title"))
    {
        argListOptions.setListTitle(mdHtmlTag.getAttrValue("title", std::string()));
    }

    if (mdHtmlTag.hasAttr("section-title"))
    {
        argListOptions.setListSectionTitle(mdHtmlTag.getAttrValue("section-title", std::string()));
    }
    else if (mdHtmlTag.hasAttr("sec-title"))
    {
        argListOptions.setListSectionTitle(mdHtmlTag.getAttrValue("sec-title", std::string()));
    }

}

//----------------------------------------------------------------------------
inline
std::string argListEscape(const std::string &str, char chEscape)
{
    std::string res; res.reserve(str.size());

    // https://www.markdownguide.org/extended-syntax/#tables
    // https://www.markdownguide.org/extended-syntax/#escaping-pipe-characters-in-tables
    // You can display a pipe ( | ) character in a table by using its HTML character code ( &#124; ).

    for(auto ch: str)
    {
        if (ch==chEscape)
        {
            if (ch=='*')
            {
                // res.append("&#42;");
                res.append(1, '\\');
                res.append(1, ch);
            }
            else if (ch=='|')
            {
                // res.append("&#124;");
                res.append(1, '\\');
                res.append(1, ch);
            }
            else
            {
                res.append(1, ch);
            }
        }
        else
        {
            res.append(1, ch);
        }
    }

    return res;
}

//----------------------------------------------------------------------------
inline
std::string argListDecorateValueSingleItem(std::string str, ArgListValueStyle valueStyle, ArgListType argListType)
{
    umba::string_plus::trim(str);
    if (str.empty())
        return str;

    // Если не декорировано
    if (!isMdSimpleDecorated(str))
    {
        if (argListType==ArgListType::table)
        {
            if (valueStyle==ArgListValueStyle::normal) // Декорации не будет - надо защитить табличный разделитель
                str = argListEscape(str, '|');
        }
        else // if (argListType==ArgListType::text)
        {
            str = argListEscape(str, '*');
        }
    }

    return mdSimpleDecorate(str, valueStyle);

}
//----------------------------------------------------------------------------


inline
std::string argListDecorateValue(std::string str, ArgListValueStyle valueStyle, ArgListType argListType)
{
    umba::string_plus::trim(str);
    if (str==",")
        return argListDecorateValueSingleItem(str, valueStyle, argListType);

    auto strVec = marty_cpp::splitToLinesSimple(str, false, ',');

    std::vector<std::string> resVec;

    for(auto v: strVec)
    {
        auto decorated = argListDecorateValueSingleItem(v, valueStyle, argListType);
        if (decorated.empty())
            continue;

        resVec.emplace_back(decorated);

        // umba::string_plus::trim(v);
        // if (v.empty())
        //     continue;
        //  
        // if (!isMdSimpleDecorated(v))
        // {
        //     if (argListType==ArgListType::table)
        //         v = argListEscape(v, '|');
        //     else // if (argListType==ArgListType::text)
        //         v = argListEscape(v, '*');
        // }
        //  
        // resVec.emplace_back(mdSimpleDecorate(v, valueStyle));
    }
    
    std::string resStr; resStr.reserve(str.size());

    for(auto &&d: resVec)
    {
        if (!resStr.empty())
            resStr.append(", ");
        resStr.append(d);
    }

    return resStr;
}

//----------------------------------------------------------------------------
template<typename FilenameStringType>
void processArgListLinesImpl( ArgListOptions argListOptions
                      , Document &doc // Сюда вставляем метаданные, если читаем из внешнего файла, и они там есть
                      , umba::html::HtmlTag &mdHtmlTag
                      , MdPpTag tagType
                      , const FilenameStringType &docFilename
                      , const std::vector<std::string> &tagLines
                      , std::vector<std::string> &resLines
                      )
{

    if (!argListOptions.sectionTitle.empty())
    {
        auto strSecTitle = std::string(6, '#');
        strSecTitle.append(1, ' ');
        strSecTitle.append(mdSimpleDecorate(argListOptions.sectionTitle, ArgListValueStyle::bold));
        resLines.emplace_back(strSecTitle);
        resLines.emplace_back(std::string());
    }

    if (argListOptions.argListType==ArgListType::unknown)
        argListOptions.argListType = ArgListType::table;

    if (argListOptions.argListValueStyle==ArgListValueStyle::unknown)
        argListOptions.argListValueStyle = ArgListValueStyle::bold;

    auto title = argListOptions.title;
    while(title.size()<2)
        title.emplace_back(std::string());

    //std::vector<std::string> argListLines;

    std::vector< std::vector<std::string> > tableLines;
    std::vector<std::string> curTableLine;

    auto expandCurTableLine = [&]()
    {
        if (!curTableLine.empty())
        {
            while(curTableLine.size()<title.size())
                curTableLine.emplace_back(std::string());
        }
    };

    auto appendCurTableLine = [&]()
    {
        expandCurTableLine();

        while(curTableLine.size()>title.size() && curTableLine.back().empty())
            curTableLine.erase(curTableLine.begin()+std::ptrdiff_t(curTableLine.size()-1));

        if (!curTableLine.empty())
            tableLines.emplace_back(curTableLine);

        curTableLine.clear();
    };


    for(auto tagLine : tagLines)
    {
        umba::string_plus::trim(tagLine); // Обрезаем незначащие пробелы с обеих сторон

        //auto tmp = umba::string_plus::ltrim_copy(tagLine);

        if (!tagLine.empty() && tagLine[0]=='-')
        {
            // нашли новый элемент
            appendCurTableLine();
            tagLine.erase(0,1);
            umba::string_plus::trim(tagLine);
            curTableLine.emplace_back(tagLine);
        }
        
        else if (!tagLine.empty() || !curTableLine.empty())
        {
            curTableLine.emplace_back(tagLine);
        }
    }

    appendCurTableLine();

    // argListEscape(const std::string &str, char chEscape)

    if (argListOptions.argListType==ArgListType::table)
    {
        resLines.emplace_back(std::string());

        std::vector<std::string> titleSep;

        for(auto &t : title)
        {
            TableCellAlignment tableCellAlignment = extractTableCellAlignment(t);

            #if 0
            bool bStart = false;
            bool bEnd   = false;

            umba::string_plus::trim(t);

            if (!t.empty() && t.back()==':')
            {
                bEnd = true;
                t.erase(t.size()-1);
            }

            if (!t.empty() && t.front()==':')
            {
                bStart = true;
                t.erase(0, 1);
            }

            umba::string_plus::trim(t);

            std::string strSep;
            if (bStart)
                strSep.append(1, ':');
            strSep.append("-------");
            if (bEnd)
                strSep.append(1, ':');
            #endif

            titleSep.emplace_back(makeMdTableSeparatorCell(tableCellAlignment));
            // if (t.empty())
            //     t = "-";
        }

        {
            auto strTitle = std::string(1, '|');
            for(auto &&t : title)
            {
                strTitle.append(argListEscape(t, '|'));
                strTitle.append(1, '|');
            }
            resLines.emplace_back(strTitle);

            strTitle.assign(1,'|');
            for(auto &&t : titleSep)
            {
                //strTitle.append(argListEscape(l, '|'));
                strTitle.append(t);
                strTitle.append(1, '|');
            }
            resLines.emplace_back(strTitle);
        }

        // Заголовок сделали, делаем таблицу

        std::string composedTableLine;

        for(auto && tableLine : tableLines)
        {
            std::vector<std::string> preparedLine;
            for(auto && l : tableLine)
            {
                if (preparedLine.size()<title.size())
                {
                    preparedLine.emplace_back(l);
                }
                else
                {
                    if (l.empty())
                    {
                        preparedLine.back().append("<br/><br/>");
                    }
                    else
                    {
                        preparedLine.back().append(1, ' ');
                        preparedLine.back().append(l); // First time escape
                    }
                }
            }

            composedTableLine.clear(); // assign(1, '|');

            std::size_t cnt = std::size_t(-1);
            for(auto && l : preparedLine)
            {
                ++cnt;

                if (cnt==0)
                {
                    // mdGetSimpleDecorationChar
                    composedTableLine.append(1, '|');
                    if (!isMdSimpleDecorated(l))
                        l = argListEscape(l, mdGetSimpleDecorationChar(argListOptions.argListValueStyle));
                    l = argListEscape(l, '|');
                    composedTableLine.append(argListDecorateValue(l, argListOptions.argListValueStyle, argListOptions.argListType));
                }
                else
                {
                    composedTableLine.append(1, '|');
                    composedTableLine.append(argListEscape(l, '|'));
                }
            }

            // if (!composedTableLine.empty() && composedTableLine.back()!='|')
            //     composedTableLine.append(1, '|');

            //while(cnt<title.size()-1)
            // while(colCount<title.size())
            // {
            //     ++colCount;
            //     composedTableLine.append(1, '|');
            // }

            composedTableLine.append(1, '|');

            resLines.emplace_back(composedTableLine);
        }
    
        resLines.emplace_back(std::string());

    }

    else // if (argListOptions.argListType==ArgListType::text)
    {
        resLines.emplace_back(std::string());

        for(auto && tableLine : tableLines)
        {
            std::size_t cnt = std::size_t(-1);
            for(auto && l : tableLine)
            {
                ++cnt;

                if (cnt==0)
                {
                    // argListDecorateValue(l, argListOptions.argListValueStyle)
                    // resLines.emplace_back( argListDecorateValue(l, argListOptions.argListValueStyle) std::string("**") + argListEscape(argListEscape(l, '*'), '|') + std::string("** - "));

                    // argListDecorateValue(l, argListOptions.argListValueStyle)
                    // argListEscape(l, '*')
                    resLines.emplace_back( argListDecorateValue(l, argListOptions.argListValueStyle, argListOptions.argListType) + std::string(" - "));

                }
                else if (cnt<(title.size()-1)) // (cnt==1)
                {
                    resLines.back().append(l);
                    if (!l.empty())
                        resLines.back().append(" - ");
                }
                else
                {
                    resLines.emplace_back(l);
                }
            }

            resLines.emplace_back(std::string());
        }

        resLines.emplace_back(std::string());

    }



    // resLines.emplace_back("![" + mdHtmlTag.getAttrValue("title", "Graph") + "](" + umba::filename::makeCanonical(imgLink, '/') + ")");

}

//----------------------------------------------------------------------------
template<typename FilenameStringType>
void processArgListLines( const AppConfig<FilenameStringType> &appCfg
                      , Document &doc // Сюда вставляем метаданные, если читаем из внешнего файла, и они там есть
                      , umba::html::HtmlTag &mdHtmlTag
                      , MdPpTag tagType
                      , const FilenameStringType &docFilename
                      , const std::vector<std::string> &tagLines
                      , std::vector<std::string> &resLines
                      )
{
    ArgListOptions argListOptions;
    auto optIt = appCfg.valListTagOptions.find(tagType);
    if (optIt!=appCfg.valListTagOptions.end())
        argListOptions = optIt->second;
    updateArgListOptions(appCfg, mdHtmlTag, argListOptions);
    processArgListLinesImpl(argListOptions, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
}

//----------------------------------------------------------------------------
// template<typename FilenameStringType>
// void processValListLines( const AppConfig<FilenameStringType> &appCfg
//                       , Document &doc // Сюда вставляем метаданные, если читаем из внешнего файла, и они там есть
//                       , umba::html::HtmlTag &mdHtmlTag
//                       , MdPpTag tagType
//                       , const FilenameStringType &docFilename
//                       , const std::vector<std::string> &tagLines
//                       , std::vector<std::string> &resLines
//                       )
// {
//     auto argListOptions = appCfg.valListTagOptions[tagType]; // valListOptions;
//     updateArgListOptions(appCfg, mdHtmlTag, argListOptions);
//     processArgListLinesImpl(argListOptions, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
// }

//  
//  
// retList,returnList
// optList,optionList
// defList,definitionList
// fieldList                     // aka memberList
//  
// # С тремя позициями вместо двух
// argList3,argumentList3
// valList3,valueList3
// retList3,returnList3
// optList3,optionList3
// defList3,definitionList3
// fieldList3





} // namespace md
} // namespace umba


