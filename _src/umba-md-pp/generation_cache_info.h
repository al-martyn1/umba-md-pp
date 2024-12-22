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


struct GenerationCacheInfoItem
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

        return res;
    }

    /*
    
    file format:

    hash    master_file
            file1
            file2
            ...
     */

}; // struct GenerationCacheInfoItem


struct GenerationCacheInfo
{
    std::vector<GenerationCacheInfoItem>     items;

    bool findCacheItem(const std::string &hash, const GenerationCacheInfoItem **pFoundCacheItem=0) const
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

    bool findCacheItem(const std::string &hash, GenerationCacheInfoItem **pFoundCacheItem=0)
    {
        // Тупой эребор
        for(auto &item : items)
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

    bool clearSlaveFiles(const std::string &hash)
    {
        GenerationCacheInfoItem *pFoundCacheItem = 0;
        if (!findCacheItem(hash, &pFoundCacheItem))
            return false;
        pFoundCacheItem->files.clear();
        return true;
    }

    bool addSlaveFile(const std::string &hash, const std::string &slaveFile)
    {
        GenerationCacheInfoItem *pFoundCacheItem = 0;
        if (!findCacheItem(hash, &pFoundCacheItem))
            return false;
        pFoundCacheItem->files.emplace_back(slaveFile);
        return true;
    }

    bool replaceCacheItem(const std::string &hash, const std::string &fileName)
    {
        GenerationCacheInfoItem *pFoundCacheItem = 0;
        if (!findCacheItem(hash, &pFoundCacheItem))
            return false;

        if (!pFoundCacheItem)
            return false;

        *pFoundCacheItem = GenerationCacheInfoItem{ hash, fileName, {} };

        return true;
    }

    bool replaceCacheItem(const std::string &hash, const std::string &fileName, const std::vector<std::string> &files)
    {
        GenerationCacheInfoItem *pFoundCacheItem = 0;
        if (!findCacheItem(hash, &pFoundCacheItem))
            return false;

        if (!pFoundCacheItem)
            return false;

        *pFoundCacheItem = GenerationCacheInfoItem{hash, fileName, files};

        return true;
    }

    bool addCacheItem(const std::string &hash, const std::string &fileName)
    {
        if (findCacheItem(hash))
            return false;

        items.emplace_back(GenerationCacheInfoItem{ hash, fileName, {} });

        return true;
    }

    bool addCacheItem(const std::string &hash, const std::string &fileName, const std::vector<std::string> &files)
    {
        if (findCacheItem(hash))
            return false;

        items.emplace_back(GenerationCacheInfoItem{hash, fileName, files});

        return true;
    }

    bool setCacheItem(const std::string &hash, const std::string &fileName)
    {
        if (replaceCacheItem(hash, fileName))
            return true;

        items.emplace_back(GenerationCacheInfoItem{ hash, fileName, {} });

        return true;
    }

    bool setCacheItem(const std::string &hash, const std::string &fileName, const std::vector<std::string> &files)
    {
        if (replaceCacheItem(hash, fileName, files))
            return true;

        items.emplace_back(GenerationCacheInfoItem{hash, fileName, files});

        return true;
    }

    bool parseCacheFileData(const std::string &data)
    {
        items.clear();

        std::vector<std::string> hashFileLines = marty_cpp::splitToLinesSimple(data);
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
                items.emplace_back(GenerationCacheInfoItem{ hashStr, filenameStr, {} });
            }

        } // for(auto l : hashFileLines)

        return true;

    }

    // Проверяет кеш на валидность, если нет, то создаёт новую запись или чистит старую, и возвращает false
    bool checkCacheValidOrUpdate(const std::string &hashStr, const std::string &masterFileForCmp)
    {
        GenerationCacheInfoItem *pGenerationCacheInfoItem = 0;

        LOG_INFO("cache") << "Check for hash '" << hashStr << "'\n";
        LOG_INFO("cache") << "File: '" << masterFileForCmp << "'\n";

        if (!findCacheItem(hashStr, &pGenerationCacheInfoItem))
        {
            LOG_INFO("cache") << "Hash not found, add new entry\n";
            addCacheItem(hashStr, masterFileForCmp);
            return false;
        }

        UMBA_ASSERT(pGenerationCacheInfoItem!=0); // Такого не должно происходить никогда

        // Элемент кеша найден
        if (pGenerationCacheInfoItem->masterFile!= masterFileForCmp)
        {
            LOG_INFO("cache") << "Hash found, but file name mismatch, resetting cache entry to new file name\n";
            LOG_INFO("cache") << "File: '" << pGenerationCacheInfoItem->masterFile << "'\n";

            // Но имя файла не соответствует
            pGenerationCacheInfoItem->masterFile = masterFileForCmp;
            pGenerationCacheInfoItem->files.clear(); 
            return false;
        }

        // Надо проверить существование всех файлов
        if (pGenerationCacheInfoItem->files.empty())
        {
            // Нет списка реальных файлов, виртуальный файл является реальным, проверяем его
            if (!umba::filesys::isFileExist(pGenerationCacheInfoItem->masterFile))
            {
                LOG_INFO("cache") << "Hash found, but file '" << pGenerationCacheInfoItem->masterFile << "' is missing\n";
                return false;
            }
        }

        // Проверяем файлы по списку
        for(const auto &f : pGenerationCacheInfoItem->files)
        {
            if (!umba::filesys::isFileExist(f))
            {
                // При новом запуске имена и количество файлов может изменится, но мастер файл остаётся прежним
                LOG_INFO("cache") << "Hash found, but file '" << f << "' is missing\n";
                pGenerationCacheInfoItem->files.clear(); 
                return false;
            }
        }

        LOG_INFO("cache") << "File is up to date: '" << masterFileForCmp << "'\n";
        return true; // Всё в порядке, кеш валиден, ничего делать не надо
    }

    std::vector<std::string> toStrings() const
    {
        std::vector<std::string> res; res.reserve(items.size());
        for(const auto &i : items)
        {
            umba::utility::vector_push_back(res, i.toStrings());
        }

        return res;
    }

    std::string toString( marty_cpp::ELinefeedType lineSep ) const
    {
        return marty_cpp::mergeLines(toStrings(), lineSep, true  /* addTrailingNewLine */ );
    }

    std::vector<std::string> getFileNamesByHash(const std::string &hashStr) const
    {
        const GenerationCacheInfoItem *pFoundCacheItem = 0;

        if (!findCacheItem(hashStr, &pFoundCacheItem))
            return std::vector<std::string>();

        if (!pFoundCacheItem)
            return std::vector<std::string>();

        if (pFoundCacheItem->files.empty())
            return std::vector<std::string>(1, pFoundCacheItem->masterFile);

        return pFoundCacheItem->files;
    }


}; // struct GenerationCacheInfo







