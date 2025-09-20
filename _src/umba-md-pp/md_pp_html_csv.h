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

//
#include <sstream>


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
        csvTableOptions.setTitle(mdHtmlTag.getAttrValue("title", std::string()));
    }

    if (mdHtmlTag.hasAttr("strict"))
    {
        auto strictStr = umba::string_plus::tolower_copy(mdHtmlTag.getAttrValue("strict", std::string()));
        if (strictStr.empty())
        {
            csvTableOptions.strict = true;
        }
        else if (strictStr=="t" || strictStr=="true" || strictStr=="1" || strictStr=="y")
        {
            csvTableOptions.strict = true;
        }
        else
        {
            csvTableOptions.strict = false;
        }
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
                                                 break;
                
                case CsvSeparator::none        : csvTableOptions.charSep = 0; // разделитель не задан, детектим
                                                 break;
                
                case CsvSeparator::auto_       : csvTableOptions.charSep = 0; // разделитель не задан, детектим
                // case CsvSeparator::detect  : // аналогично строке выше
                                                 break;
                

                case CsvSeparator::comma       : csvTableOptions.charSep = ',';
                                                 break;
                
                case CsvSeparator::semicolon   : csvTableOptions.charSep = ';';
                                                 break;
                
                case CsvSeparator::colon       : csvTableOptions.charSep = ':';
                                                 break;
                
                case CsvSeparator::hash        : csvTableOptions.charSep = '#';
                //case CsvSeparator::number      : // аналогично строке выше
                                                 break;
                
                case CsvSeparator::verticalBar : csvTableOptions.charSep = '|';
                // case CsvSeparator::bar         : // аналогично строке выше
                                                 break;
                
                case CsvSeparator::tab         : csvTableOptions.charSep = '\t';
                                                 break;

                default                        : csvTableOptions.charSep = 0; // detect
            }
        }
    }

    if (mdHtmlTag.hasAttr("quot") || mdHtmlTag.hasAttr("quotation"))
    {
        auto quotStr = mdHtmlTag.getAttrValue(mdHtmlTag.hasAttr("quot") ? "quot" : "quotation", std::string());

        if (quotStr.size()==1)
        {
            csvTableOptions.charQuot = quotStr[0];
        }
        else if (quotStr.empty())
        {
            csvTableOptions.charQuot = '\"'; // разделитель не задан, используем quot
        }
        else
        {
            auto eSep = enum_deserialize(quotStr, CsvQuot::invalid);
            switch(eSep)
            {
                case CsvQuot::invalid : csvTableOptions.charQuot = 0;
                // case CsvQuot::unknown : // аналогично invalid
                                        break;
                
                case CsvQuot::none    : csvTableOptions.charQuot = 0;
                                        break;
                
                case CsvQuot::auto_   : csvTableOptions.charQuot = 0;
                // case CsvQuot::detect : // аналогично строке выше
                                        break;
                
                case CsvQuot::quot    : csvTableOptions.charQuot = '\"';
                                        break;
                
                case CsvQuot::apos    : csvTableOptions.charQuot = '\'';
                                        break;
                
                case CsvQuot::backtick: csvTableOptions.charQuot = '`';
                // case CsvQuot::tick    : // аналогично строке выше
                                        break;
                

                default               : csvTableOptions.charQuot = '\"'; // default char
            }
        }
    }

    if (!mdHtmlTag.hasAttr("csv-title"))
    {
        if (csvTableOptions.title.empty())
            csvTableOptions.csvTitle = CsvTitle::use;
        else
            csvTableOptions.csvTitle = CsvTitle::merge;
    }
    else
    {
        auto csvTitleStr = mdHtmlTag.getAttrValue("csv-title", std::string());

        if (csvTitleStr.empty())
        {
            // Если заголовок не пустой, мержим его с заголовком из CSV с приоритетом из атрибута title
            if (csvTableOptions.title.empty())
                csvTableOptions.csvTitle = CsvTitle::use;
            else
                csvTableOptions.csvTitle = CsvTitle::merge;
        }
        else
        {
            auto eTitle = enum_deserialize(csvTitleStr, CsvTitle::invalid);

            if (eTitle==CsvTitle::invalid)
            {
                if (csvTableOptions.title.empty())
                    csvTableOptions.csvTitle = CsvTitle::use;
                else
                    csvTableOptions.csvTitle = CsvTitle::merge;
            }
            else if (eTitle==CsvTitle::none)
            {
                csvTableOptions.csvTitle = CsvTitle::no;
            }
            else
            {
                csvTableOptions.csvTitle = eTitle;
            }
        }
    }


    if (mdHtmlTag.hasAttr("multi-csv"))
    {
        auto sepStr = mdHtmlTag.getAttrValue("multi-csv", std::string());

        if (sepStr.empty())
            sepStr = "<<<--->>>";
            //sepStr = "-------";

        csvTableOptions.multiTableSeparator = sepStr;
    }

    if (mdHtmlTag.hasAttr("multi-title"))
    {
        csvTableOptions.setMultiTitle(mdHtmlTag.getAttrValue("multi-title", std::string()));
        // Если задан `multi-title`, но не задан `multi-csv`, то задаём для `multi-csv` дефолтный разделитель
        if (csvTableOptions.multiTableSeparator.empty())
            csvTableOptions.multiTableSeparator = "<<<--->>>";
    }

}

//----------------------------------------------------------------------------
inline
std::string csvTableEscape(const std::string &str)
{
    std::string res; res.reserve(str.size());

    // https://www.markdownguide.org/extended-syntax/#tables
    // https://www.markdownguide.org/extended-syntax/#escaping-pipe-characters-in-tables
    // You can display a pipe ( | ) character in a table by using its HTML character code ( &#124; ).

    for(auto ch: str)
    {
        if (ch=='|' || ch=='*' || ch=='(' || ch==')')
        {
            res.append(1, '\\');
            res.append(1, ch);
        }
        else if (ch=='\n')
        {
            res.append("<br/><br/>");
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

    // auto title = csvTableOptions.title;
    // while(title.size()<2)
    //     title.emplace_back(std::string());

    //std::vector<std::string> argListLines;

    bool bMulti = false;

    std::vector<std::string> tablesSingleLine;
    if (!csvTableOptions.multiTableSeparator.empty())
    {
        bMulti = true;
    }

    {
        std::string curTable; curTable.reserve(tagLines.size()/2);

        for(std::size_t lineIdx=0u; lineIdx!=tagLines.size(); ++lineIdx)
        {
            const auto &l = tagLines[lineIdx];

            if (lineIdx==(tagLines.size()-1) && l.empty())
                 break;

            if (!bMulti)
            {
                curTable.append(1, '\n');
                curTable.append(l);
                continue;
            }

            // Сначала проверяем, не начинается ли строка с разделителя таблиц, и только потом удаляем пробелы и проверяем точно
            // Это для того, чтобы не делать trim для каждой строки
            // И да, разделитель должен быть в начале строки, а вот пробелы в конце - опциональны
            if (umba::string_plus::starts_with(l, csvTableOptions.multiTableSeparator) && umba::string_plus::ltrim_copy(l)==csvTableOptions.multiTableSeparator)
            //if (l==csvTableOptions.multiTableSeparator)
            {
                tablesSingleLine.push_back(curTable);
                curTable.clear();
                continue;
            }

            curTable.append(1, '\n');
            curTable.append(l);
        }

        if (!curTable.empty())
            tablesSingleLine.push_back(curTable);
    }

    char charSepCfg  = csvTableOptions.charSep ;
    char charQuotCfg = csvTableOptions.charQuot;

    // if (charSep==0 || charQuot==0)
    {
        // Детектим кавычки и/или разделители для всех таблиц сразу. Или нет? Могут быть разными?
        // Но количество полей может быть разным, а значит - разное матожидание и дисперсия символов,
        // и разделитель будет продетекчен неверно
    }

    static const std::string seps  = "\t;,:|#";
    static const std::string quots = "\"\'`";

    for(std::size_t csvIdx=0u; csvIdx!=tablesSingleLine.size(); ++csvIdx )
    {
        const auto &csv = tablesSingleLine[csvIdx];

        char charSep  = charSepCfg ;
        char charQuot = charQuotCfg;

        if (charQuot==0)
        {
            charQuot = marty::csv::detectQuotes(csv, seps, quots);
            if (charQuot==0)
                charQuot = '\"';
        }

        if (charSep==0)
        {
            charSep = marty::csv::detectSeparators(csv.begin(), csv.end(), seps, charQuot);
            if (charSep==0)
                charSep = ';';
        }

        resLines.push_back(std::string());

        if (bMulti)
        {
            auto multiTitle = umba::string_plus::ltrim_copy(csvTableOptions.getMultiTitle(csvIdx));
            if (!multiTitle.empty())
            {
                if (multiTitle.back()!='.' && multiTitle.back()!='!' && multiTitle.back()!='?' && multiTitle.back()!=':' && multiTitle.back()!=';')
                    multiTitle.push_back(':');
                resLines.push_back(multiTitle);
                resLines.push_back(std::string());
            }
        }

        // marty::csv::ParseResult 
        auto parseResult = marty::csv::parse(csv, charSep, charQuot, csvTableOptions.strict);

        bool bGood = true;
        if (!parseResult.errors.empty())
        {
            if (csvTableOptions.strict)
                bGood = false;
        }
        
        if (!bGood)
        {
            resLines.push_back(std::string("```"));
            for(const auto &err : parseResult.errors)
            {
                std::ostringstream oss;
                marty::csv::printError(oss, err);
                resLines.push_back(oss.str());
            }
            resLines.push_back(std::string("```"));
        }
        else if (parseResult.data.empty())
        {
            resLines.push_back(std::string("```"));
            resLines.push_back(std::string("No data"));
            resLines.push_back(std::string("```"));
        }
        else // Тут выводим таблицу
        {
            // Для начала надо найти макс количество колонок
            // std::vector<std::vector<std::string>> data;
            std::size_t maxSize = 0;
            for(const auto &l : parseResult.data)
            {
                if (maxSize<l.size())
                    maxSize = l.size();
            }

            std::vector<std::string>        title;
            std::vector<TableCellAlignment> colAligns;

            auto csvIt = parseResult.data.begin();

            const auto csvTitle = csvTableOptions.csvTitle;

            if (csvTitle==CsvTitle::none || csvTitle==CsvTitle::no)
            {
                // В CVS-файле нет заголовка
                // Используем title из атрибутов
                for(std::size_t colIdx=0u; colIdx!=maxSize; ++colIdx)
                {
                    title.push_back(csvTableOptions.getTitle(colIdx));
                    colAligns.push_back(csvTableOptions.getColAlignment(colIdx));
                }
            }
            else if (csvTitle==CsvTitle::ignore)
            {
                // Игнорируем заголовок из CVS-файла
                // Пропускаем строку CVS-файла
                // Используем title из атрибутов
                for(std::size_t colIdx=0u; colIdx!=maxSize; ++colIdx)
                {
                    title.push_back(csvTableOptions.getTitle(colIdx));
                    colAligns.push_back(csvTableOptions.getColAlignment(colIdx));
                }

                ++csvIt; // Пропускаем строку CVS-файла с заголовком
            }
            else if (csvTitle==CsvTitle::use) // Использовать только CSV заголовок
            {
                for(std::size_t colIdx=0u; colIdx!=maxSize; ++colIdx)
                {
                    title.push_back(csvTableOptions.getVectorValueAt(*csvIt, colIdx, std::string()));
                    colAligns.push_back(TableCellAlignment::left);
                }

                ++csvIt; // Пропускаем строку CVS-файла с заголовком
            }
            else if (csvTitle==CsvTitle::merge)
            {
                for(std::size_t colIdx=0u; colIdx!=maxSize; ++colIdx)
                {
                    auto t = csvTableOptions.getTitle(colIdx);
                    // Если в заданном title пропуск - используем заголовок из CVS
                    if (t.empty())
                        t = csvTableOptions.getVectorValueAt(*csvIt, colIdx, std::string());
                    title.push_back(t);
                    colAligns.push_back(csvTableOptions.getColAlignment(colIdx)); // Выравнивание берём из заголовка в атрибуте title
                }

                ++csvIt; // Пропускаем строку CVS-файла с заголовком
            }
            else if (csvTitle==CsvTitle::align)
            {
                for(std::size_t colIdx=0u; colIdx!=maxSize; ++colIdx)
                {
                    title.push_back(csvTableOptions.getVectorValueAt(*csvIt, colIdx, std::string())); // Заголовок из CVS
                    colAligns.push_back(csvTableOptions.getColAlignment(colIdx)); // Выравнивание берём из заголовка в атрибуте title
                }

                ++csvIt; // Пропускаем строку CVS-файла с заголовком
            }

            // Если почему-то заголовки не получились нормально сформировать
            while(title.size()<maxSize)
                title.push_back(std::string());

            while(colAligns.size()<maxSize)
                colAligns.push_back(TableCellAlignment::left);

            std::string tmpLine = std::string(1, '|');
            //auto initTmpLine = [&]() { tmpLine.assign(1, '|'); };
            //auto closeCell   = [&]() { tmpLine.append(1, '|'); };
            auto addCell     = [&](const std::string &c) { tmpLine.append(csvTableEscape(c)); tmpLine.append(1, '|'); };
            auto addLine     = [&]() { resLines.push_back(tmpLine); tmpLine.assign(1, '|'); };

            for(const auto &t : title)
                addCell(t);
            addLine();
            
            for(const auto &ca : colAligns)
                addCell(makeMdTableSeparatorCell(ca));
            addLine();

            for(; csvIt!=parseResult.data.end(); ++csvIt)
            {
                const auto & rowCells = *csvIt;
                for(std::size_t colIdx=0u; colIdx!=maxSize; ++colIdx)
                {
                    addCell(csvTableOptions.getVectorValueAt(rowCells, colIdx, std::string()));
                }
                addLine();
            }

        } // else // Тут выводим таблицу

        resLines.push_back(std::string());

    } // for(std::size_t csvIdx=0u; csvIdx!=tablesSingleLine.size(); ++csvIdx )


// csvTableEscape(const std::string &str)

    #if 0
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
    #endif

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


