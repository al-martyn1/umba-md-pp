#pragma once

#include <string>
#include <vector>
#include <map>

//
#include "encoding/encoding.h"
//
#include "umba/program_location.h"
#include "umba/enum_helpers.h"
#include "umba/flag_helpers.h"
//
//#include "umba/regex_helpers.h"
//
#include "umba/filename.h"
#include "umba/filesys.h"
//
#include "marty_cpp/src_normalization.h"

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct AppConfig
{

    #if defined(WIN32) || defined(_WIN32)
        marty_cpp::ELinefeedType outputLinefeed = marty_cpp::ELinefeedType::crlf;
    #else
        marty_cpp::ELinefeedType outputLinefeed = marty_cpp::ELinefeedType::lf;
    #endif

    // bool                                     allFiles = false;
    // bool                                     scanMode = false;
    std::vector<std::string>                 samplesPaths;
    // std::vector<std::string>                 includeFilesMaskList;
    // std::vector<std::string>                 excludeFilesMaskList;

    void addSamplesPaths( const std::vector<std::string> &pl )
    {
        //std::vector<FilenameStringType> &dirs = m_lookupMap[lvl].lookupDirs;
        samplesPaths.insert( samplesPaths.end(), pl.begin(), pl.end() );
    }

    //! Добавляет пути поиска примеров
    /*! Разделителем является спец символ:
        - Win32 - ';' (точка с запятой, semicolon)
        - Linux - ':' (двоеточие, colon)
     */
    void addSamplesPaths( const std::string &pl )
    {
        addSamplesPaths( umba::filename::splitPathList( pl ) );
    }

    static
    std::string autoEncodeToUtf(const std::string &text)
    {
        size_t bomSize = 0;
        //const charDataPtr = 
        encoding::EncodingsApi* pEncodingsApi = encoding::getEncodingsApi();
        std::string detectRes = pEncodingsApi->detect( text.data(), text.size(), bomSize );
        auto cpId = pEncodingsApi->getCodePageByName(detectRes);
        std::string utfText = pEncodingsApi->convert( text.data()+bomSize, text.size()-bomSize, cpId, encoding::EncodingsApi::cpid_UTF8 );
        return utfText;
    }


    bool findSamplesFile(const std::string &lookFor, std::string &foundFullFilename, std::string &foundFileText) const
    {
         for(auto path: samplesPaths)
         {
             auto fullName = umba::filename::appendPath(path, lookFor);
             std::string orgText;
             if (umba::filesys::readFile(fullName, orgText))
             {
                 orgText           = autoEncodeToUtf(orgText);
                 foundFileText     = marty_cpp::normalizeCrLfToLf(orgText);
                 foundFullFilename = fullName;
                 return true;
             }
         }

         return false;
    }



}; // struct AppConfig




