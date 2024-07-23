#pragma once


#include "umba/filesys_scanners.h"
//
#include "app_config.h"


// appConfig.mdppExtentions
    // std::vector<std::string>                              mdppExtentions;
    //
    // std::string                                           batchOutputRoot;
    // std::vector<std::string>                              batchExcludeDirs;
    // std::vector<std::string>                              batchExcludeFilesMaskList;
    // std::vector<std::string>                              batchScanPaths;

// struct ScanPathsEntry
// {
//     std::string        path;
//     bool               recurse = false;
// };

template<typename LogMsgType> inline
void batchScanForFiles( const std::vector<ScanPathsEntry> &batchScanPaths
                      , const std::vector<std::string> &extList
                      , const std::vector<std::string> &excludeDirs
                      , const std::vector<std::string> &excludeFilesMaskList
                      , LogMsgType                     &logMsg           // logMsg or logNul
                      , std::vector<std::string>       &foundFiles
                      , std::vector<std::string>       *pFoundFilesRootFolders = 0
                      )
{
    std::vector<std::string> includeMasks; includeMasks.reserve(extList.size());

    for(const auto &ext : extList)
    {
        includeMasks.emplace_back("*." + ext + "^");
    }

    std::vector<std::string> excludeMasks = excludeFilesMaskList;
    // std::vector<std::string> excludeMasks; excludeMasks.reserve( /* excludeDirs.size() + */  excludeFilesMaskList.size());
    // // for(const auto &excludeDir : excludeDirs)
    // // {
    // //     excludeMasks.emplace_back("*/" + excludeDir + "/*");
    // // }
    //
    // excludeMasks.insert(excludeMasks.end(), excludeFilesMaskList.begin(), excludeFilesMaskList.end());


    std::vector<std::string> excludedFiles;
    std::set<std::string>    foundExtentions;

    std::vector<std::string> scanPaths;
    bool bCurRecurse = false;
    bool bHeaderPrinted = false;

    std::vector<ScanPathsEntry>::const_iterator bit = batchScanPaths.begin();
    for(; bit!=batchScanPaths.end(); ++bit)
    {
        if (bit->recurse!=bCurRecurse)
        {
            if (!scanPaths.empty())
            {
                umba::filesys::scanners::scanFolders( scanPaths
                                                    , includeMasks
                                                    , excludeMasks
                                                    , logMsg
                                                    , foundFiles
                                                    , excludedFiles
                                                    , foundExtentions
                                                    , pFoundFilesRootFolders
                                                    , excludeDirs
                                                    , bCurRecurse
                                                    , !bHeaderPrinted
                                                    );
            }

            bHeaderPrinted = true;

            scanPaths      .clear();
            excludedFiles  .clear();
            foundExtentions.clear();

            bCurRecurse = bit->recurse;
            scanPaths.emplace_back(umba::filename::makeCanonical(bit->path));
        }
        else
        {
            scanPaths.emplace_back(umba::filename::makeCanonical(bit->path));
        }
    }

    if (!scanPaths.empty())
    {
        umba::filesys::scanners::scanFolders( scanPaths
                                            , includeMasks
                                            , excludeMasks
                                            , logMsg
                                            , foundFiles
                                            , excludedFiles
                                            , foundExtentions
                                            , pFoundFilesRootFolders
                                            , excludeDirs
                                            , bCurRecurse
                                            , !bHeaderPrinted
                                            );
    }

}


// umba::filesys::scanners::
// template<typename StringType, typename LogMsgType> inline
// void scanFolders( const std::vector<StringType> &scanPaths
//                 , const std::vector<StringType> &includeFilesMaskList
//                 , const std::vector<StringType> &excludeFilesMaskList
//                 , LogMsgType               &logMsg           // logMsg or logNul
//                 , std::vector<std::string> &foundFiles
//                 , std::vector<std::string> &excludedFiles
//                 , std::set<std::string>    &foundExtentions
//                 , std::vector<std::string> *pFoundFilesRootFolders = 0
//                 )
