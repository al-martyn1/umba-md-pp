#pragma once

#include "app_config.h"
//
#include "marty_cpp/src_normalization.h"

inline
std::string processMdFile(const AppConfig &cfg, std::string fileText, const std::string &fileName)
{
    fileText = marty_cpp::normalizeCrLfToLf(fileText);

    std::vector<std::string> lines = marty_cpp::splitToLinesSimple(fileText);

//std::vector<StringType> splitToLinesSimple(const StringType &str, bool addEmptyLineAfterLastLf = true, typename StringType::value_type lfChar=(typename StringType::value_type)'\n')
//StringType stripLineTrailingSpaces(StringType str)
}

