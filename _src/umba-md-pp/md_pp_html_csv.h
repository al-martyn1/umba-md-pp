#pragma once

#include "umba/umba.h"
#include "umba/filename.h"
#include "umba/filesys.h"
#include "umba/shellapi.h"
//
#include "md_pp_html.h"
#include "csv_table_options.h"
#include "extern_tools.h"
#include "generation_cache_info.h"
#include "document.h"
#include "utils.h"
// 
#include "log.h"


#include "marty_csv/marty_csv.h"


// umba::md::
namespace umba {
namespace md {

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template<typename FilenameStringType>
void updateCsvTableOptions(const AppConfig<FilenameStringType> &appCfg, const umba::html::HtmlTag &mdHtmlTag, CsvTableOptions &csvTableOptions)
{
    // Атрибуты:
    //   file      - имя входного файла
    //   title     - заголовок, может не быть, тогда два пустых значения

    using namespace umba::filename;

    // if (mdHtmlTag.hasAttr("file"))
    // {
    //     auto attrVal = mdHtmlTag.getAttrValue("file", std::string());
    //     argListOptions.file = appendPath(getPath(attrVal), getName(attrVal));
    // }

    // if (mdHtmlTag.hasAttr("type"))
    // {
    //     csvTableOptions.setListType(mdHtmlTag.getAttrValue("type", std::string()));
    // }
    //  
    // if (mdHtmlTag.hasAttr("value-style"))
    // {
    //     csvTableOptions.setListValueStyle(mdHtmlTag.getAttrValue("value-style", std::string()));
    // }

    if (mdHtmlTag.hasAttr("title"))
    {
        csvTableOptions.setListTitle(mdHtmlTag.getAttrValue("title", std::string()));
    }

    if (mdHtmlTag.hasAttr("sep") || mdHtmlTag.hasAttr("separator"))
    {
        auto sepStr = mdHtmlTag.getAttrValue(mdHtmlTag.hasAttr("sep") ? "sep" : "separator", std::string());

        if (sepStr.size()==1)
        {
            csvTableOptions.charSep = sepStr[0];
        }
        else if (sepStr.empty())
        {
            csvTableOptions.charSep = 0; // разделитель не задан, детектим
        }
        else
        {
            auto eSep = enum_deserialize(sepStr, CsvSeparator::invalid);
            switch(eSep)
            {
                case CsvSeparator::invalid     : csvTableOptions.charSep = 0; // detect, но наверное надо доложить об ошибке
                // case CsvSeparator::unknown     : csvTableOptions.charSep = 0; // аналогично invalid
                
                case CsvSeparator::none        : csvTableOptions.charSep = 0; // разделитель не задан, детектим
                case CsvSeparator::auto_       : csvTableOptions.charSep = 0; // разделитель не задан, детектим
                // case CsvSeparator::detect  : // аналогично строке выше

                case CsvSeparator::comma       : csvTableOptions.charSep = ',';
                case CsvSeparator::semicolon   : csvTableOptions.charSep = ';';
                case CsvSeparator::colon       : csvTableOptions.charSep = ':';
                case CsvSeparator::hash        : csvTableOptions.charSep = '#';
                //case CsvSeparator::number      : // аналогично строке выше
                case CsvSeparator::verticalBar : csvTableOptions.charSep = '|';
                // case CsvSeparator::bar         : // аналогично строке выше
                case CsvSeparator::tab         : csvTableOptions.charSep = '\t';

                default                        : csvTableOptions.charSep = 0; // detect
            }
        }
    }

    if (mdHtmlTag.hasAttr("quot") || mdHtmlTag.hasAttr("quotation"))
    {
        auto quotStr = mdHtmlTag.getAttrValue(mdHtmlTag.hasAttr("quot") ? "quot" : "quotation", std::string());

        if (quotStr.size()==1)
        {
            csvTableOptions.charQuot = sepStr[0];
        }
        else if (sepStr.empty())
        {
            csvTableOptions.charQuot = '\"'; // разделитель не задан, используем quot
        }
        else
        {
            auto eSep = enum_deserialize(sepStr, CsvQuot::invalid);
            switch(eSep)
            {
                case CsvQuot::invalid : csvTableOptions.charQuot = 0;
                // case CsvQuot::unknown : // аналогично invalid
                case CsvQuot::none    : csvTableOptions.charQuot = '\"';
                case CsvQuot::auto_   : csvTableOptions.charQuot = 0;
                // case CsvQuot::detect : // аналогично строке выше
                case CsvQuot::quot    : csvTableOptions.charQuot = '\"';
                case CsvQuot::apos    : csvTableOptions.charQuot = '\'';
                case CsvQuot::backtick: csvTableOptions.charQuot = '`';
                // case CsvQuot::tick    : // аналогично строке выше

                default               : csvTableOptions.charQuot = '\"'; // default char
            }

    }

// enum class CsvQuot : std::uint32_t
// {
//     quot       = 0x0002 /*!<  */,
//     apos       = 0x0003 /*!<  */,
//     backtick   = 0x0004 /*!<  */,
//     tick       = 0x0004 /*!<  */
//  
// }; // enum 


    // if (mdHtmlTag.hasAttr("sep") || mdHtmlTag.hasAttr("separator"))
    // {
    //     auto sepStr = mdHtmlTag.getAttrValue(mdHtmlTag.hasAttr("sep") ? "sep" : "separator", std::string());
    //     if (sepStr.size()==1)
    //     {
    //         csvTableOptions.charSep = sepStr[0];
    //     }
    //     else if (sepStr.empty())
    //     {
    //         csvTableOptions.charSep = 0; // разделитель не задан, детектим
    //     }
    //     else
    //     {
    //         auto eSep = enum_deserialize(sepStr, CsvSeparator::invalid);
    //         switch(eSep)
    //         {
    //             case CsvSeparator::invalid  : csvTableOptions.charSep = 0; // detect, но наверное надо доложить об ошибке
    //             case CsvSeparator::none     : csvTableOptions.charSep = 0; // разделитель не задан, детектим
    //             case CsvSeparator::auto_    : csvTableOptions.charSep = 0; // разделитель не задан, детектим
    //             // case CsvSeparator::detect  : // аналогично строке выше
    //             case CsvSeparator::quot     : csvTableOptions.charSep = '\"';
    //             case CsvSeparator::apos     : csvTableOptions.charSep = '\'';
    //             case CsvSeparator::backtick : csvTableOptions.charSep = '`';
    //             // case CsvSeparator::tick     : // аналогично строке выше
    //             default: csvTableOptions.charSep = '\"';
    //         }
    //     }
    // }


//     std::vector<std::string>         title;
//  
//     std::vector<TableCellAlignment>  colAlignments;
//  
//     CsvTitle      = CsvTitle::none; //! Способ формирования заголовка таблицы
//     char charSep  = 0;
//     char charQuot = 0;
//  
//     // csv-title
//     // sep  - none/auto, comma, semicolon, colon, hash, bar/vertical-bar, tab
//     // quot - none/auto, quot, apos, tick/backtick
//  
//  
//  
// // enum class CsvTitle : std::uint32_t
// // {
// //     invalid   = (std::uint32_t)(-1) /*!< ! */,
// //     unknown   = (std::uint32_t)(-1) /*!< ! */,
// //     none      = 0x0000 /*!< ! */,
// //     use       = 0x0001 /*!< Use title from CSV file (firts line threated as title). Missing value (none) threated as `use` */,
// //     no        = 0x0002 /*!< CSV file has no title, use taken title or empty title */,
// //     ignore    = 0x0003 /*!< Ignore title from CSV file and use taken title or empty title */,
// //     merge     = 0x0004 /*!< Merge align and title from `title` attr and CSV file (alignment also used from `title` attribute) */,
// //     align     = 0x0005 /*!< Merge align only from `title` attr, but use title from CSV file */
// //  
// // }; // enum 



}

//----------------------------------------------------------------------------
inline
std::string csvTableEscape(const std::string &str, char chEscape)
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
std::string csvTableDecorateValueSingleItem(std::string str, ArgListValueStyle valueStyle)
{
    umba::string_plus::trim(str);
    if (str.empty())
        return str;

    if (!isMdSimpleDecorated(str))
    {
        // if (argListType==ArgListType::table)
        //     str = argListEscape(str, '|');
        // else // if (argListType==ArgListType::text)
        //     str = argListEscape(str, '*');
    }

    return mdSimpleDecorate(str, valueStyle);

}
//----------------------------------------------------------------------------


inline
std::string csvTableDecorateValue(std::string str, ArgListValueStyle valueStyle)
{
    // umba::string_plus::trim(str);
    // if (str==",")
        return csvTableDecorateValueSingleItem(str, valueStyle);

    // auto strVec = marty_cpp::splitToLinesSimple(str, false, ',');
    //  
    // std::vector<std::string> resVec;
    //  
    // for(auto v: strVec)
    // {
    //     auto decorated = csvTableDecorateValueSingleItem(v, valueStyle);
    //     if (decorated.empty())
    //         continue;
    //  
    //     resVec.emplace_back(decorated);
    //  
    // }
    //  
    // std::string resStr; resStr.reserve(str.size());
    //  
    // for(auto &&d: resVec)
    // {
    //     if (!resStr.empty())
    //         resStr.append(", ");
    //     resStr.append(d);
    // }
    //  
    // return resStr;
}

//----------------------------------------------------------------------------
template<typename FilenameStringType>
void processCsvTableLinesImpl( CsvTableOptions csvTableOptions
                      , Document &doc // Сюда вставляем метаданные, если читаем из внешнего файла, и они там есть
                      , umba::html::HtmlTag &mdHtmlTag
                      , MdPpTag tagType
                      , const FilenameStringType &docFilename
                      , const std::vector<std::string> &tagLines
                      , std::vector<std::string> &resLines
                      )
{

    // auto argListOptions = appCfg.argListOptions;

    // if (argListOptions.argListType==ArgListType::unknown)
    //     argListOptions.argListType = ArgListType::table;

    // if (argListOptions.argListValueStyle==ArgListValueStyle::unknown)
    //     argListOptions.argListValueStyle = ArgListValueStyle::bold;

    auto title = csvTableOptions.title;
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

    //if (csvTableOptions.argListType==ArgListType::table)
    {
        resLines.emplace_back(std::string());

        std::vector<std::string> titleSep;

        for(auto &t : title)
        {
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
            
            titleSep.emplace_back(strSep);
        }

        {
            auto strTitle = std::string(1, '|');
            for(auto &&t : title)
            {
                strTitle.append(csvTableEscape(t, '|'));
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
                    //if (!isMdSimpleDecorated(l))
                    //    l = argListEscape(l, mdGetSimpleDecorationChar(csvTableOptions.argListValueStyle));
                    //l = argListEscape(l, '|');
                    //composedTableLine.append(argListDecorateValue(l, csvTableOptions.argListValueStyle, argListOptions.argListType));
                    composedTableLine.append(csvTableEscape(l, '|'));
                }
                else
                {
                    composedTableLine.append(1, '|');
                    composedTableLine.append(csvTableEscape(l, '|'));
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
    /*
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
    */



    // resLines.emplace_back("![" + mdHtmlTag.getAttrValue("title", "Graph") + "](" + umba::filename::makeCanonical(imgLink, '/') + ")");

}

//----------------------------------------------------------------------------
template<typename FilenameStringType>
void processCsvTableLines( const AppConfig<FilenameStringType> &appCfg
                      , Document &doc // Сюда вставляем метаданные, если читаем из внешнего файла, и они там есть
                      , umba::html::HtmlTag &mdHtmlTag
                      , MdPpTag tagType
                      , const FilenameStringType &docFilename
                      , const std::vector<std::string> &tagLines
                      , std::vector<std::string> &resLines
                      )
{
    // auto argListOptions = appCfg.argListOptions;
    auto csvTableOptions = CsvTableOptions{};
    updateCsvTableOptions(appCfg, mdHtmlTag, csvTableOptions);
    processCsvTableLinesImpl(csvTableOptions, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------

} // namespace md
} // namespace umba


