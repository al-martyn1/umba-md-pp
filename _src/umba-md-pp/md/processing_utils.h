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




} // namespace md
} // namespace umba