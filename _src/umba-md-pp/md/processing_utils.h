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
inline
std::string getAlertHtmlColor(AlertType alertType)
{
    // https://colorscheme.ru/html-colors.html
    switch(alertType)
    {
        case AlertType::note      : return "#4169E1"; // RoyalBlue #4169E1
        case AlertType::tip       : return "#2E8B57"; // SeaGreen #2E8B57
        case AlertType::important : return "#8A2BE2"; // BlueViolet #8A2BE2
        case AlertType::warning   : return "#800000"; // Maroon #800000
        case AlertType::caution   : return "#8B4513"; // SaddleBrown #8B4513
        case AlertType::todo      : return "#FF8C00"; // DarkOrange #FF8C00
        case AlertType::invalid   : return "#FF0000"; // Red #FF0000
        default                   : return "#4169E1"; // RoyalBlue #4169E1 as note
    }
}

//----------------------------------------------------------------------------
inline
std::string getDefaultAlertPunctuation(AlertType alertType)
{
    switch(alertType)
    {
        case AlertType::note      : return ".";
        case AlertType::tip       : return ".";
        case AlertType::important : return "!";
        case AlertType::warning   : return "!";
        case AlertType::caution   : return "!";
        case AlertType::todo      : return ".";
        case AlertType::invalid   : return "!";
        default                   : return "!";
    }
}

//----------------------------------------------------------------------------
inline
void parseAlertStartLine(std::string line, AlertType &alertType, std::string &punctStr, std::string &alertTitle)
{
    umba::string_plus::trim(line);
    if (!umba::string_plus::starts_with_and_strip(line, ("!!!")))
        throw std::runtime_error("processAlertLine: something goes wrong");

    // Вычитываем строки из букв, если есть - это типа алерта
    std::size_t i = 0u;
    for(; i!=line.size(); ++i)
    {
        auto ch = line[i];
        if ( (ch>='A' && ch<='Z')
          || (ch>='a' && ch<='z')
           )
        {
            continue;
        }

        break;
    }

    auto alertStr  = std::string(line, 0, i);
    alertType = enum_deserialize(alertStr, AlertType::invalid);
    if (alertType==AlertType::invalid)
        alertType = AlertType::warning;

    line.erase(0, i);

    for(i = 0u; i!=line.size(); ++i)
    {
        auto ch = line[i];
        if (ch==' ')
            break;
    }

    punctStr  = std::string(line, 0, i);
    line.erase(0, i);
    umba::string_plus::trim(line);

    if (punctStr!=":" && punctStr!="." && punctStr!="!" && punctStr!="-"  /* && punctStr!="" */ )
       punctStr.clear();

    if (punctStr.empty())
        punctStr = getDefaultAlertPunctuation(alertType);

    if (punctStr=="-")
        punctStr = " - ";
    else
        punctStr += " ";

    alertTitle = line;

}

//----------------------------------------------------------------------------
// alertStart
// alertLine
// alertEnd

template<typename FilenameStringType> inline
bool processAlertLine(LineHandlerEvent event, const AppConfig<FilenameStringType> &appCfg, std::string str, std::vector<std::string> &generatedLines)
{
    AlertStyle alertStyle = appCfg.getAlertStyle();

    if (event==LineHandlerEvent::alertStart)
    {
        AlertType alertType;
        std::string alertTitle;
        std::string punctStr;
        parseAlertStartLine(str, alertType, punctStr, alertTitle);
    
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
    /*
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
    */
        auto makeGitHubTitleStr = [&](bool addPunct)
        {
            std::string alertHeaderStr;
    
            // Если TODO, то заголовок вида "TODO: Заголовок!"
            // Иначе "Заголовок!"
            // На месте '!' - знак препинания, который задан
            if (alertType==AlertType::todo)
            {
                alertHeaderStr = alertTitle.empty() ? std::string("TODO: ") : std::string("TODO: ") + alertTitle;
            }
            else // not todo
            {
                alertHeaderStr = alertTitle;
            }

            if (!alertHeaderStr.empty() && addPunct)
                alertHeaderStr += punctStr;

            return alertHeaderStr;
        };

        if (alertStyle==AlertStyle::github)
        {
            // GitHub не поддерживает кастомные заголовки у алертсов
            // Если задан заголовок, его делаем болдом в начале текста второй строкой
    
            generatedLines.emplace_back("> " + getAlertTypeGfmStr(alertType));
    
            std::string alertHeaderStr = makeGitHubTitleStr(true); // add punctuation
    
            if (!alertHeaderStr.empty())
            {
                // !!! Тут нужно добавить экранирование символов умножения, но они маловероятны, поэтому пока не делаем
                generatedLines.emplace_back("> **" + alertHeaderStr + "**" + " ");
            }

        }
        else if (alertStyle==AlertStyle::gitlab)
        {
            std::string alertHeaderStr = makeGitHubTitleStr(false); // don't add punctuation
            generatedLines.emplace_back("> " + getAlertTypeGfmStr(alertType) + " " + alertHeaderStr);
        }
        else if (alertStyle==AlertStyle::text)
        {
            if (alertTitle.empty())
            {
                auto it = appCfg.alertTitles.find(alertType);
                if (it!=appCfg.alertTitles.end())
                    alertTitle = it->second;
            }

            std::string alertHeaderStr = makeGitHubTitleStr(true); // add punctuation
            if (!alertHeaderStr.empty())
                generatedLines.emplace_back("**" + alertHeaderStr + "**" + " ");
        }
        else if (alertStyle==AlertStyle::blockquote)
        {
            if (alertTitle.empty())
            {
                auto it = appCfg.alertTitles.find(alertType);
                if (it!=appCfg.alertTitles.end())
                    alertTitle = it->second;
            }

            std::string alertHeaderStr = makeGitHubTitleStr(true); // add punctuation
            if (!alertHeaderStr.empty())
                generatedLines.emplace_back("> **" + alertHeaderStr + "**" + " ");
        }
        else if (alertStyle==AlertStyle::backtickNote)
        {
            if (alertTitle.empty())
            {
                auto it = appCfg.alertTitles.find(alertType);
                if (it!=appCfg.alertTitles.end())
                    alertTitle = it->second;
            }

            std::string alertHeaderStr = makeGitHubTitleStr(true); // add punctuation
            if (!alertHeaderStr.empty())
                generatedLines.emplace_back("**" + alertHeaderStr + "**" + " ");
            generatedLines.emplace_back("```notice");
        }
        else // if (alertStyle==AlertStyle::div)
        {
            std::string color = getAlertHtmlColor(alertType);
            std::string alertHeaderStr = makeGitHubTitleStr(false); // don't add punctuation
            generatedLines.emplace_back(std::string("<div style=\"border-color: ") + color + "; border-left: 4px solid; margin: 5px; padding: 8px;\"><div><strong><span style=\"color: " + color + ";\">" + alertHeaderStr + "</span></strong></div><div>");
        }
    
    }
    else if (event==LineHandlerEvent::alertLine)
    {
        if (alertStyle==AlertStyle::github)
        {
            generatedLines.emplace_back("> " + str);
        }
        else if (alertStyle==AlertStyle::gitlab)
        {
            generatedLines.emplace_back("> " + str);
        }
        else if (alertStyle==AlertStyle::text)
        {
            generatedLines.emplace_back(str);
        }
        else if (alertStyle==AlertStyle::blockquote)
        {
            generatedLines.emplace_back("> > " + str);
        }
        else if (alertStyle==AlertStyle::backtickNote)
        {
            generatedLines.emplace_back(str);
        }
        else // if (alertStyle==AlertStyle::div)
        {
            generatedLines.emplace_back(str);
        }
    }
    else if (event==LineHandlerEvent::alertEnd)
    {
        if (alertStyle==AlertStyle::github)
        {
            generatedLines.emplace_back(std::string());
        }
        else if (alertStyle==AlertStyle::gitlab)
        {
            generatedLines.emplace_back(std::string());
        }
        else if (alertStyle==AlertStyle::text)
        {
            generatedLines.emplace_back(std::string());
        }
        else if (alertStyle==AlertStyle::blockquote)
        {
            generatedLines.emplace_back(std::string());
        }
        else if (alertStyle==AlertStyle::backtickNote)
        {
            generatedLines.emplace_back(std::string("```"));
            generatedLines.emplace_back(std::string());
        }
        else // if (alertStyle==AlertStyle::div)
        {
            generatedLines.emplace_back("</div></div>");
        }
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

