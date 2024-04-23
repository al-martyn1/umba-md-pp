#pragma once

#include "app_config.h"
//
#include "marty_cpp/src_normalization.h"


inline
bool isInsertCommand(std::string line)
{
    umba::string_plus::trim(line);

    return umba::string_plus::starts_with(line, ("#!insert"));
}

inline
bool isListingCommand(std::string line)
{
    umba::string_plus::trim(line);

    return umba::string_plus::starts_with(line, ("```"));
}

inline
bool isHeaderCommand(std::string line)
{
    umba::string_plus::trim(line);

    std::size_t idx = 0u;

    while(idx!=line.size() && line[idx]=='#') { ++idx; }

    return idx>0;
}


std::string processMdFile(const AppConfig &appCfg, std::string fileText, const std::string &curFilename);

inline
std::vector<std::string> processMdFileLines(const AppConfig &appCfg, const std::vector<std::string> &lines, const std::string &curFilename)
{
    bool inListing = false;

    return lines;
}


inline
std::string processMdFile(const AppConfig &appCfg, std::string fileText, const std::string &curFilename)
{
    //fileText = marty_cpp::normalizeCrLfToLf(fileText);
    std::vector<std::string> lines = marty_cpp::splitToLinesSimple(fileText);

    auto resLines = processMdFileLines(appCfg, lines, curFilename);

    return marty_cpp::mergeLines(resLines, appCfg.outputLinefeed, true  /* addTrailingNewLine */ );
}



