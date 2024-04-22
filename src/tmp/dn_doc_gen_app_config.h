#pragma once

#include <string>
#include <vector>
#include <map>

#include "umba/program_location.h"
#include "umba/filename.h"

#include <time.h>

#include "marty_cpp/marty_cpp.h"
#include "marty_cpp/src_normalization.h"

#include "cppHelpers.h"
// #include "textUtils.h"

#include "enums.h"



//----------------------------------------------------------------------------
#if defined(min)

    #undef min

#endif

#if defined(max)

    #undef max

#endif




//----------------------------------------------------------------------------
struct AppConfig
{
    // umba-2c & umba-2rcfs commons
    //bool                                     quet       = false;
    bool                                                  bOverwrite           = false  ; //!< Overwrite output file, if exist
    std::string                                           targetName; // or path
    std::string                                           translationsFile; // or path
    dotNutDocGen::DocGenCommand                           docGenCommand = dotNutDocGen::DocGenCommand::unknown;
    marty_simplesquirrel::EnumScriptGenerationOptions     generationOptions;

    std::vector<std::string>                              apiDefFiles;

    std::string                                           sourcesRoot; // root for including sources/source listings

    std::size_t                                           tabSize = 4u;

    std::string                                           outputFile; // output file for listing/document modes

    std::string                                           indexFile;

    std::string                                           batFile;
    std::string                                           batFileLineTemplate;

    //dotNutDocGen::ListingNestedTagsMode                   listingNestedTagsMode;
    dotNutDocGen::ListingUpdateTrMode                     listingUpdateTrMode;

    std::string                                           listingCodeFile;
    //std::string                                           codeTagPrefix; // for listings
    std::string                                           codeTag; // for listings
    std::string                                           listingTrBaseId; // for listings

    std::string                                           docTemplateFile;


}; // struct AppConfig




