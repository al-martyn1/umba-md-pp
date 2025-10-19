#pragma once

#include "base_includes.h"

#include "processing_base.h"


namespace umba {
namespace md {


//----------------------------------------------------------------------------
template<typename HeaderLineHandler, typename FilenameStringType> inline
std::vector<std::string> processHeaderLines(const AppConfig<FilenameStringType> &appCfg, Document &doc, const FilenameStringType &curFilename, const std::vector<std::string> &lines, HeaderLineHandler headerHandler)
{
    auto handler = [&](LineHandlerEvent event, std::vector<std::string> &resLines, std::string &line, std::size_t idx, std::size_t lastLineIdx)
    {
        UMBA_USED(lastLineIdx);
        UMBA_USED(idx);

        if (event!=LineHandlerEvent::headerCommand)
        {
            return true;
        }

        if (headerHandler(line))
        {
            //resLines.emplace_back(line);
            return true;
        }

        return false;
    };

    return processLines(appCfg, doc, curFilename, lines, handler);

}

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::vector<std::string> raiseHeaders(const AppConfig<FilenameStringType> &appCfg, Document &doc, const FilenameStringType &curFilename, const std::vector<std::string> &lines, int raiseVal)
{
    // Ограничиваем изменение разумной величиной
    if (raiseVal>3)
        raiseVal = 3;
    if (raiseVal<-3)
        raiseVal = -3;

    if (!raiseVal)
         return lines;

    auto raiseHeader = [&](std::string &line) -> bool
    {
        std::string levelStr;
        std::string headerText;

        if (!splitHeaderLine(line, levelStr, headerText))
            return true;

        std::size_t newHeaderSize = levelStr.size();

        // Заголовки самого минимального уровня не трогаем
        if (newHeaderSize>=6)
           return true;

        int rv = raiseVal;

        if (rv<0)
        {
            rv = -rv;
            newHeaderSize += (std::size_t)rv;
        }
        else
        {
            if (rv<=(int)newHeaderSize)
            {
                newHeaderSize -= (std::size_t)rv;
            }
            else
            {
                newHeaderSize = 0;
            }
        }

        if (newHeaderSize==0)
           newHeaderSize = 1;

        line = std::string(newHeaderSize, '#') + std::string(1u, ' ') + headerText;

        return true;
    };

    return processHeaderLines(appCfg, doc, curFilename, lines, raiseHeader);
}

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
int findHeadersTopLevel(const AppConfig<FilenameStringType> &appCfg, Document &doc, const FilenameStringType &curFilename, const std::vector<std::string> &lines)
{
    int topLevel = 65535;

    auto findHeadersTopLevelLambda = [&](std::string &line) -> bool
    {
        std::string levelStr;
        std::string headerText;

        if (!splitHeaderLine(line, levelStr, headerText))
            return false; // don't add line to result

        int curHeaderLevel = int(levelStr.size());
        topLevel = std::min(topLevel, curHeaderLevel);

        return false;
    };

    processHeaderLines(appCfg, doc, curFilename, lines, findHeadersTopLevelLambda);

    return topLevel==65535 ? -1 : topLevel;
}

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
bool processAlertFirstLine(const AppConfig<FilenameStringType> &appCfg, std::string str, std::vector<std::string> &generatedLines)
{
    umba::string_plus::trim(str);

    if (!umba::string_plus::starts_with_and_strip(str, ("!!!")))
        throw std::runtime_error("processAlertFirstLine: something goes wrong");

    std::size_t i = 0u;
    for(; i!=str.size(); ++i)
    {
        auto ch = str[i];
        if ( (ch>='A' && ch<='Z')
          || (ch>='a' && ch<='z')
           )
        {
            continue;
        }

        break;
    }

    auto alertStr  = std::string(str, 0, i);
    auto alertType = enum_deserialize(alertStr, AlertType::invalid);
    if (alertType==AlertType::invalid)
        alertType = AlertType::warning;

    str.erase(0, i);

    for(i = 0u; i!=str.size(); ++i)
    {
        auto ch = str[i];
        if (ch==' ')
            break;
    }

    auto punctStr  = std::string(str, 0, i);
    str.erase(0, i);
    umba::string_plus::trim(str);

    if (punctStr!=":" && punctStr!="." && punctStr!="!" && punctStr!="-"  /* && punctStr!="" */ )
       punctStr.clear();

    if (punctStr.empty())
        punctStr = ":";

    if (punctStr=="-")
        punctStr = " - ";

    const std::string &strTitle = str;
    // alertType, punctStr, str - title
    // !!! Тут надо быть внимательным

    auto getAlertTypeGfmStr = [](AlertType alertType) -> std::string
    {
        switch(alertType)
        {
            case AlertType::note      : return "[!NOTE]";
            case AlertType::tip       : return "[!TIP]";
            case AlertType::important : return "[!IMPORTANT]";
            case AlertType::warning   : return "[!WARNING]";
            case AlertType::caution   : return "[!CAUTION]";
            case AlertType::todo      : return "[!NOTE]";
            case AlertType::invalid   : return "[!WARNING]";
            default                   : return "[!WARNING]";
        
        }
    };

    auto getAlertTypeDefaultTextStr = [](AlertType alertType) -> std::string
    {
        switch(alertType)
        {
            case AlertType::note      : return "Note";
            case AlertType::tip       : return "Tip";
            case AlertType::important : return "Important";
            case AlertType::warning   : return "Warning";
            case AlertType::caution   : return "Caution";
            case AlertType::todo      : return "TODO";
            case AlertType::invalid   : return "Warning";
            default                   : return "Warning";
        
        }
    };

    AlertStyle alertStyle = appCfg.getAlertStyle();
    if (alertStyle==AlertStyle::github)
    {
        // GitHub не поддерживает кастомные заголовки у алертсов
        // Если задан заголовок, его делаем болдом в начале текста второй строкой

        generatedLines.emplace_back("> " + getAlertTypeGfmStr(alertType));

        // Если TODO, то заголовок вида "TODO: Заголовок!"
        // Иначе "Заголовок!"
        // На месте '!' - знак препинания, который задан

        std::string alertHeaderStr;

        if (alertType==AlertType::todo)
        {
            alertHeaderStr = "TODO";
            if (!strTitle.empty())
            {
                alertHeaderStr += ": ";
                alertHeaderStr += strTitle; 
            }

            alertHeaderStr += punctStr;

            generatedLines.emplace_back("> **" + alertHeaderStr + "**" + " "); // !!! Тут нужно добавить экранирование символов умножения, но они маловероятны, поэтому пока не делаем
        }
        else // not todo
        {
            if (!strTitle.empty())
            {
                alertHeaderStr = strTitle + punctStr;
            }

            if (!alertHeaderStr.empty())
                generatedLines.emplace_back("> **" + alertHeaderStr + "**" + " "); // !!! Тут нужно добавить экранирование символов умножения, но они маловероятны, поэтому пока не делаем
        }
    }
    else if (alertStyle==AlertStyle::gitlab)
    {
        std::string alertHeaderStr = getAlertTypeGfmStr(alertType);

        if (alertType==AlertType::todo)
        {
            if (strTitle.empty())
                alertHeaderStr = "TODO";
            else
                alertHeaderStr = "TODO: " + strTitle;
        }
        else
        {
            alertHeaderStr = strTitle;
        }

        generatedLines.emplace_back("> " + getAlertTypeGfmStr(alertType) + " " + alertHeaderStr);
    }
    else // if (alertStyle==AlertStyle::text)
    {
        // !!! Надо доделать алертс для текстового варианта

        // std::string alertHeaderStr;
        // // if (alertType==AlertType::todo)
        //  
        // appCfg
        // getAlertTypeDefaultTextStr
    }

    return true;
}

// inline
// bool isAlertCommand(std::string line)
// {
//     umba::string_plus::trim(line);
//     return umba::string_plus::starts_with(line, ("!!!"));
// }



} // namespace md
} // namespace umba

