#pragma once

#include <string>
#include <vector>



struct Document
{
    std::vector<std::string>    collectedMetadataTexts; // from all processed documents, each string is an \n multiline text

    std::vector<std::string>    lines;

};

