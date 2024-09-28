#pragma once

#include "umba/umba.h"
//
#include "umba/filename.h"
#include "umba/filesys.h"
#include "umba/shellapi.h"
#include "umba/utility.h"
//
#include "marty_cpp/src_normalization.h"

//
#include <string>
#include <vector>

/*



 */


struct GenerationCacheItem
{
    std::string                  hash      ; //!< Hash
    std::string                  masterFile; //!< Reference file. Can be virtual, if tool generates set of files for single input
    std::vector<std::string>     files     ; //!< If non-empty, than masterFile is virtual, else masterFile is real file

    std::vector<std::string> toStrings() const
    {
        std::vector<std::string> res;
        res.emplace_back(hash + " " + masterFile);

        for(const auto &f : files)
        {
            res.emplace_back(" " + f);
        }
    }

    /*
    
    file format:

    hash    master_file
            file1
            file2
            ...
     */

}; // struct GenerationCacheItem


struct GenerationCache
{
    std::vector<GenerationCacheItem>     items;

    bool findCacheItem(const std::string &hash, const GenerationCacheItem **pFoundCacheItem=0) const
    {
        // Тупой эребор
        for(const auto &item : items)
        {
            if (item.hash==hash)
            {
                if (pFoundCacheItem)
                    *pFoundCacheItem = &item;
                return true;
            }
        }

        return false;
    }

    bool findCacheItem(const std::string &hash, GenerationCacheItem **pFoundCacheItem=0)
    {
        // Тупой эребор
        for(const auto &item : items)
        {
            if (item.hash==hash)
            {
                if (pFoundCacheItem)
                    *pFoundCacheItem = &item;
                return true;
            }
        }

        return false;
    }

    bool replaceCacheItem(const std::string &hash, const std::string &fileName)
    {
        GenerationCacheItem *pFoundCacheItem = 0;
        if (!findCacheItem(hash, &pFoundCacheItem))
            return false;

        if (!pFoundCacheItem)
            return false;

        *pFoundCacheItem = GenerationCacheItem{hash, fileName};

        return true;
    }

    bool replaceCacheItem(const std::string &hash, const std::string &fileName, const std::vector<std::string> &files)
    {
        GenerationCacheItem *pFoundCacheItem = 0;
        if (!findCacheItem(hash, &pFoundCacheItem))
            return false;

        if (!pFoundCacheItem)
            return false;

        *pFoundCacheItem = GenerationCacheItem{hash, fileName, files};

        return true;
    }

    bool addCacheItem(const std::string &hash, const std::string &fileName)
    {
        if (findCacheItem(hash))
            return false;

        items.emplace_back(GenerationCacheItem{hash, fileName});

        return true;
    }

    bool addCacheItem(const std::string &hash, const std::string &fileName, const std::vector<std::string> &files)
    {
        if (findCacheItem(hash))
            return false;

        items.emplace_back(GenerationCacheItem{hash, fileName, files});

        return true;
    }

    bool setCacheItem(const std::string &hash, const std::string &fileName)
    {
        if (replaceCacheItem(hash, fileName))
            return true;

        items.emplace_back(GenerationCacheItem{hash, fileName});

        return true;
    }

    bool setCacheItem(const std::string &hash, const std::string &fileName, const std::vector<std::string> &files)
    {
        if (replaceCacheItem(hash, fileName, files))
            return true;

        items.emplace_back(GenerationCacheItem{hash, fileName, files});

        return true;
    }

    bool parseCacheFileData(const std::string &data)
    {
        items.clear();

        std::vector<std::string> hashFileLines = marty_cpp::splitToLinesSimple(hashFileText);
        for(auto l : hashFileLines)
        {
            umba::string_plus::rtrim(l);
            if (l.empty())
                continue;

            std::string hashStr;
            std::string filenameStr;

            if (l[0]!=' ')
            {
                if (!umba::string_plus::split_to_pair(l, hashStr, filenameStr, ' '))
                {
                    continue;
                }
            }
            else
            {
                umba::string_plus::ltrim(l);
                filenameStr = l;
            }

            if (filenameStr.empty())
                continue;

            if (hashStr.empty())
            {
                if (!items.empty())
                {
                    items.back().files.emplace_back(filenameStr);
                }
            }
            else
            {
                items.emplace_back(GenerationCacheItem{hash, fileName});
            }

        } // for(auto l : hashFileLines)

        return true;

    }

    static

    std::vector<std::string> toStrings() const
    {
        std::vector<std::string> res; res.reserve(items.size());
        for(const auto &i : items)
        {
            umba::utility::vector_emplace_back(res, i.toStrings());
        }

        return res;
    }

    std::string toString( marty_cpp::ELinefeedType lineSep ) const
    {
        return marty_cpp::mergeLines(toStrings(), lineSep, true  /* addTrailingNewLine */ );
    }


}; // struct GenerationCache



// std::vector<std::string> hashFileLines = marty_cpp::splitToLinesSimple(hashFileText);




