#pragma once


#include <string>
#include <unordered_map>
#include <unordered_set>

//
#include "umba/filename.h"


// umba::md::
namespace umba {
namespace md {


struct ListingBackendGeneratorOptions
{
    std::string backendName;
    std::string listingTag ; // used for this backend

}; // struct ListingBackendGeneratorOptions




class LanguageOptions
{

protected:

    std::unordered_map<std::string, ListingBackendGeneratorOptions>    m_backendGeneratorOptions;

    // Depends only on language and user settings for the language, no backend related options here

    std::set<std::string>                                              m_languageFilenameExtentions;

    std::unordered_set<std::string>                                    m_cutPrefixes           ; // 
    std::unordered_set<std::string>                                    m_genericCutStopPrefixes; // For C++ it can be "//---", for plain C: /*** - separator line, or both of them

    std::string                                                        m_blockPair;


public:

    LanguageOptions() = default;
    LanguageOptions(const LanguageOptions &) = default;
    LanguageOptions& operator=(const LanguageOptions &) = default;
    LanguageOptions(LanguageOptions &&) = default;
    LanguageOptions& operator=(LanguageOptions &&) = default;


    bool setListingTagForBackendGenerator(const std::string &tag, const std::string &backend)
    {
        std::unordered_map<std::string, ListingBackendGeneratorOptions>::iterator it = m_backendGeneratorOptions.find(backend);
        if (it!=m_backendGeneratorOptions.end())
        {
            it->second.listingTag = tag;
            return true;
        }

        m_backendGeneratorOptions[backend] = ListingBackendGeneratorOptions{backend, tag};

        return true;
    }

    bool findBackendGeneratorOptions(const std::string &backend, ListingBackendGeneratorOptions &options) const
    {
        std::unordered_map<std::string, ListingBackendGeneratorOptions>::const_iterator it = m_backendGeneratorOptions.find(backend);
        if (it==m_backendGeneratorOptions.end())
            it = m_backendGeneratorOptions.find(std::string());
        if (it==m_backendGeneratorOptions.end())
            return false;
        options = it->second;
        return true;
    }
    
    bool setBlockCharacters(const std::string &blockPair)
    {
        if (blockPair.size()!=2)
            return false;
        m_blockPair = blockPair;
        
        return true;
    }

    std::string getBlockCharacters() const
    {
        return m_blockPair;
    }

    bool addCutPrefix(const std::string &p)
    {
        m_cutPrefixes.insert(p);
        return true;
    }

    bool addGenericCutStopPrefix(const std::string &p)
    {
        m_genericCutStopPrefixes.insert(p);
        return true;
    }

    bool isCodeTagLine(std::string line, std::string *pFoundTagPrefix=0) const
    {
        umba::string_plus::trim(line);

        for(const auto p : m_cutPrefixes)
        {
            if (umba::string_plus::starts_with(line, p))
            {
                if (pFoundTagPrefix)
                   *pFoundTagPrefix = p;
                return true;
            }
        }

        return false;
    }

    bool isGenericCutStopLine(std::string line) const
    {
        umba::string_plus::trim(line);

        for(const auto p : m_genericCutStopPrefixes)
        {
            if (umba::string_plus::starts_with(line, p))
                return true;
        }

        return false;
    }

    bool addLanguageExtention(const std::string &ext)
    {
        m_languageFilenameExtentions.insert(ext);
        return true;
    }

    std::set<std::string> getLanguageExtentions() const
    {
        return m_languageFilenameExtentions;
    }
    


}; // class LanguageOptions




class LanguageOptionsDatabase
{

protected:

    mutable std::unordered_map<std::string, LanguageOptions>    m_languageOptions;
    std::unordered_map<std::string, std::string>                m_languageByExtention;

public:

    LanguageOptionsDatabase() = default;
    LanguageOptionsDatabase(const LanguageOptionsDatabase &) = default;
    LanguageOptionsDatabase& operator=(const LanguageOptionsDatabase &) = default;
    LanguageOptionsDatabase( LanguageOptionsDatabase &&) = default;
    LanguageOptionsDatabase& operator=(LanguageOptionsDatabase &&) = default;

    //! Добавляет отношение Ext->Lang, при этом Ext может, на самом деле, быть двойным расширением, или даже полным именем файла - CMakeLists.txt
    bool addLanguageExtention(const std::string &lang, std::string ext)
    {
        m_languageOptions[lang].addLanguageExtention(ext);
        umba::string_plus::tolower(ext);
        m_languageByExtention[ext] = lang;
        return true;
    }

    std::string findLanguageByFilename(std::string fileName) const
    {
        fileName = umba::filename::getFileName(fileName);
        umba::string_plus::tolower(fileName);

        for(const auto &kv : m_languageByExtention)
        {
            if (umba::string_plus::ends_with(fileName, kv.first))
                return kv.second;
        }

        return std::string();
    }

    LanguageOptions& getLanguageOptions(const std::string &lang) const
    {
        return m_languageOptions[lang];
    }

    std::set<std::string> getLanguages() const
    {
        std::set<std::string> res;
        for(const auto &kv : m_languageOptions)
        {
            res.insert(kv.first);
        }

        return res;
    }

    bool addCutPrefix(const std::string &lang, const std::string &p)
    {
        m_languageOptions[lang].addCutPrefix(p);
        return true;
    }

    bool addGenericCutStopPrefix(const std::string &lang, const std::string &p)
    {
        m_languageOptions[lang].addGenericCutStopPrefix(p);
        return true;
    }

    bool setBlockCharacters(const std::string &lang, const std::string &blockPair)
    {
        return m_languageOptions[lang].setBlockCharacters(blockPair);
    }

    std::string getBlockCharacters(const std::string &lang)
    {
        return m_languageOptions[lang].getBlockCharacters();
    }

    bool setListingTagForBackendGenerator(const std::string &lang, const std::string &tag, const std::string &backend=std::string())
    {
        m_languageOptions[lang].setListingTagForBackendGenerator(tag, backend);
        return true;
    }

    std::string getListingTagForBackendGenerator(const std::string &lang, const std::string &backend=std::string()) const
    {
        //std::unordered_map<std::string, LanguageOptions>    m_languageOptions;
        const auto &langOpts = getLanguageOptions(lang);

        ListingBackendGeneratorOptions bgo;
        if (!langOpts.findBackendGeneratorOptions(backend, bgo))
            return std::string();

        if (bgo.listingTag.empty())
            langOpts.findBackendGeneratorOptions(backend, bgo);

        return bgo.listingTag;
    }

    bool isCodeTagLine(const std::string &lang, std::string line, std::string *pFoundTagPrefix=0)
    {
        const auto &langOpts = getLanguageOptions(lang);
        return langOpts.isCodeTagLine(line, pFoundTagPrefix);
    }

    bool isGenericCutStopLine(const std::string &lang, std::string line) const
    {
        const auto &langOpts = getLanguageOptions(lang);
        return langOpts.isGenericCutStopLine(line);
    }



}; // class LanguageOptionsDatabase





} // namespace md
} // namespace umba
