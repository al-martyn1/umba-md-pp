/*! \file
    \brief Разные утилитки
 */

#include "filename.h"
#include "filesys.h"

//
#include <string>


inline
bool findProjectOptionsFileImpl(const std::string &mdFile, std::string &foundOptionsFile, const std::vector<std::string> &names)
{
    std::string optPath = umba::filename::getPath(mdFile);

    while(true)
    {
        for(const auto &name : names)
        {
            std::string optFile = umba::filename::appendPath(optPath, name);
            if (umba::filesys::isFileReadable(optFile))
            {
                foundOptionsFile = optFile;
                return true;
            }
        }

        stripLastPathSep(optPath);
        std::string optPathNext = umba::filename::getPath(optPath);
        if (optPathNext.empty() || optPathNext==optPath)
        {
            return false;
        }

        optPath = optPathNext;
    }

}

inline
bool findProjectOptionsFile(const std::string &mdFile, std::string &foundOptionsFile)
{
    return findProjectOptionsFileImpl(mdFile, foundOptionsFile, std::vector<std::string>{".md-pp.options", ".umba-md-pp.options"})
}

