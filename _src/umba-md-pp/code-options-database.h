#pragma once


#include <string>
#include <unordered_map>
#include <unordered_set>

//
#include "umba/filename.h"

#include "code-processing/types.h"

// #include "code-processing/simple-processing.h"


// umba::md::
namespace umba {
namespace md {


struct ListingBackendGeneratorOptions
{
    std::string backendName;
    std::string listingTag ; // used for this backend

}; // struct ListingBackendGeneratorOptions




class CodeOptions
{

protected:

    std::unordered_map<std::string, ListingBackendGeneratorOptions>    m_backendGeneratorOptions;

    // Depends only on language and user settings for the language, no backend related options here

    bool                                                               m_caseSens = true;

    std::set<std::string>                                              m_languageFilenameExtentions;

    std::unordered_set<std::string>                                    m_cutPrefixes           ; //
    std::unordered_set<std::string>                                    m_genericCutStopPrefixes; // For C++ it can be "//---", for plain C: /*** - separator line, or both of them
    std::unordered_map<std::string,std::string>                        m_commentMarkers        ;

    std::unordered_set<std::string>                                    m_prototypeSkip;

    std::string                                                        m_blockPair;
    std::string                                                        m_statementSeparator; // expression/statement terminator/separator
    std::string                                                        m_assignOperator;


public: // members

    mdpp::code::simpleCodeLinesProcessingFnPtr                         m_funcPrototypeExtractor  = 0;
    mdpp::code::simpleCodeLinesProcessingFnPtr                         m_classPrototypeExtractor = 0;
    mdpp::code::simpleCodeLinesProcessingFnPtr                         m_funcPrototypeFormatter  = 0;
    mdpp::code::simpleCodeLinesProcessingFnPtr                         m_classPrototypeFormatter = 0;


public:

    CodeOptions() = default;
    CodeOptions(const CodeOptions &) = default;
    CodeOptions& operator=(const CodeOptions &) = default;
    CodeOptions(CodeOptions &&) = default;
    CodeOptions& operator=(CodeOptions &&) = default;


    std::unordered_set<std::string> getAllStopPrefixes() const
    {
        auto res = m_cutPrefixes;
        for(const auto &cmntMarkerPair : m_commentMarkers)
            res.insert(cmntMarkerPair.first);
        res.insert(m_genericCutStopPrefixes.begin(), m_genericCutStopPrefixes.end());
        return res;
    }

    std::string getSingleLineCommentMarker() const
    {
        for(auto it=m_commentMarkers.begin(); it!=m_commentMarkers.end(); ++it)
        {
            if (it->second.empty())
                return it->first;
        }

        return std::string();
    }

    bool getMultiLineCommentMarker(std::string &start, std::string &end) const
    {
        for(auto it=m_commentMarkers.begin(); it!=m_commentMarkers.end(); ++it)
        {
            if (!it->second.empty())
            {
                start = it->first ;
                end   = it->second;
                return true;
            }
        }

        return false;
    }

    bool getCommentMarkers(std::string &start, std::string &end) const
    {
        end.clear();
        start = getSingleLineCommentMarker();
        if (!start.empty())
             return true;
        return getMultiLineCommentMarker(start, end);
    }


    //TODO: !!!  Пока у нас строки начала/окончания блока односимвольные, но надо переделать, желательно везде

    std::string getBlockStartString() const
    {
        if (m_blockPair.size()<2)
            return std::string();

        if (m_blockPair.size()==2)
            return std::string(1, m_blockPair[0]);

        auto pos = m_blockPair.find('|');
        if (pos==m_blockPair.npos)
            return std::string();

        return std::string(m_blockPair, 0, pos);
    }

    std::string getBlockEndString() const
    {
        if (m_blockPair.size()<2)
            return std::string();

        if (m_blockPair.size()==2)
            return std::string(1, m_blockPair[1]);

        auto pos = m_blockPair.find('|');
        if (pos==m_blockPair.npos)
            return std::string();

        return std::string(m_blockPair, pos+1, m_blockPair.npos);
    }

    char getBlockStartChar() const
    {
        std::string str = getBlockStartString();
        if (str.empty())
            return 0;
        return str[0];
    }

    char getBlockEndChar() const
    {
        std::string str = getBlockEndString();
        if (str.empty())
            return 0;
        return str[0];
    }

    // --set-code-case-sens
    bool setCaseSens(bool b) { m_caseSens = b; return true; }
    bool getCaseSens() const { return m_caseSens; }

    const std::unordered_set<std::string>& getPrototypeSkip() const { return m_prototypeSkip; }
    // void addPrototypeSkip(const std::string &skip)                  { m_prototypeSkip.insert(skip); }
    // void clrPrototypeSkip(const std::string &skip)                  { m_prototypeSkip.clear(); }
    bool testPrototypeSkip(std::string testFor, const std::unordered_set<std::string> &s) const
    {
        if (!m_caseSens)
            umba::string_plus::tolower(testFor);
        return s.find(testFor)!=s.end();
    }
    bool testPrototypeSkip(const std::string &testFor) const        { return testPrototypeSkip(testFor, m_prototypeSkip); }

    // --set-code-prototype-remove
    bool setPrototypeSkip(std::string strSkipList)
    {
        if (strSkipList.empty())
            return false;

        int op = 0;

        if (strSkipList[0]=='+')
        {
            op = 1;
            strSkipList.erase(0, 1);
        }
        else if (strSkipList[0]=='-')
        {
            op = -1;
            strSkipList.erase(0, 1);
        }

        if (strSkipList.empty())
            return false;

        auto optVec = splitAndTrimAndSkipEmpty(strSkipList, ',');
        if (optVec.empty())
            return false;

        if (op==0)
        {
            m_prototypeSkip.clear();
            op = 1;
        }

        if (!m_caseSens)
        {
            for(auto &s: optVec)
                umba::string_plus::tolower(s);
        }

        for(const auto &s: optVec)
        {
            if (op>0)
               m_prototypeSkip.insert(s);
            else
               m_prototypeSkip.erase(s);
        }

        return true;
    }


    const std::string& getAssignOperator() const
    {
        return m_assignOperator;
    }

    bool setAssignOperator(const std::string& s)
    {
        m_assignOperator = s;
        return true;
    }

    const std::string& getSatementSeparator() const
    {
        return m_statementSeparator;
    }

    bool setCodeProcessingHandler(CodeProcessingHandlerType handlerType, const std::string &handlerName)
    {
        auto handler = umba::mdpp::code::getSimpleProcessorFuction(handlerName);
        if (!handler)
            return false;

        switch(handlerType)
        {
            case CodeProcessingHandlerType::unknown            : return false;
            case CodeProcessingHandlerType::none               : return false;
            case CodeProcessingHandlerType::fnPrototypeExtract : m_funcPrototypeExtractor  = handler; break;
            case CodeProcessingHandlerType::clsPrototypeExtract: m_classPrototypeExtractor = handler; break;
            case CodeProcessingHandlerType::fnPrototypeFormat  : m_funcPrototypeFormatter  = handler; break;
            case CodeProcessingHandlerType::clsPrototypeFormat : m_classPrototypeFormatter = handler; break;
        }

        return true;
    }

    bool setCodeProcessingHandler(const std::string &handlerTypeStr, const std::string &handlerName)
    {
        return setCodeProcessingHandler(enum_deserialize(handlerTypeStr, CodeProcessingHandlerType::unknown), handlerName);
    }

    bool setCodeProcessingHandler(const std::string &handlerTypeNamePair)
    {
        std::string handlerTypeStr, handlerName;
        if (!umba::string_plus::split_to_pair(handlerTypeNamePair, handlerTypeStr, handlerName, ':'))
            return false;
        return setCodeProcessingHandler(handlerTypeStr, handlerName);
    }    

    const std::unordered_set<std::string>& getGenericCutStopPrefixes() const
    {
        return m_genericCutStopPrefixes;
    }

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

    bool setStatementSeparator(const std::string &statementSeparator)
    {
        auto ssUpper = umba::string_plus::toupper_copy(statementSeparator);
        if (ssUpper=="<LF>" || ssUpper=="<NL>")
            m_statementSeparator = "\n";
        else
            m_statementSeparator = statementSeparator;

        return true;
    }

    std::string getStatementSeparator() const
    {
        return m_statementSeparator;
    }

    bool addCutPrefix(const std::string &p)
    {
        m_cutPrefixes.insert(p);
        return true;
    }

    bool addCommentMarker(const std::string &p)
    {
        auto pos = p.find('|');
        if (pos!=p.npos)
            m_commentMarkers[std::string(p, 0, pos)] = std::string(p, pos+1, p.npos);
        else
            m_commentMarkers[p] = std::string();
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

        for(const auto &p : m_cutPrefixes)
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

        for(const auto &p : m_genericCutStopPrefixes)
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



}; // class CodeOptions




class CodeOptionsDatabase
{

protected:

    mutable std::unordered_map<std::string, CodeOptions>        m_codeOptions;
    std::unordered_map<std::string, std::string>                m_languageByExtention;

public:

    CodeOptionsDatabase() = default;
    CodeOptionsDatabase(const CodeOptionsDatabase &) = default;
    CodeOptionsDatabase& operator=(const CodeOptionsDatabase &) = default;
    CodeOptionsDatabase( CodeOptionsDatabase &&) = default;
    CodeOptionsDatabase& operator=(CodeOptionsDatabase &&) = default;


    bool setCodeProcessingHandler(const std::string &lang, const std::string &handlerTypeNamePair)
    {
        return m_codeOptions[lang].setCodeProcessingHandler(handlerTypeNamePair);
    }

    bool setCodeProcessingHandler(const std::string &handlerLangTypeNameTriplet)
    {
        std::string lang, handlerTypeNamePair;
        if (!umba::string_plus::split_to_pair(handlerLangTypeNameTriplet, lang, handlerTypeNamePair, ':'))
            return false;
        return setCodeProcessingHandler(lang, handlerTypeNamePair);
    }

    //! Добавляет отношение Ext->Lang, при этом Ext может, на самом деле, быть двойным расширением, или даже полным именем файла - CMakeLists.txt
    bool addLanguageExtention(const std::string &lang, std::string ext)
    {
        m_codeOptions[lang].addLanguageExtention(ext);
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

    CodeOptions& getCodeOptions(const std::string &lang) const
    {
        return m_codeOptions[lang];
    }

    std::set<std::string> getLanguages() const
    {
        std::set<std::string> res;
        for(const auto &kv : m_codeOptions)
        {
            res.insert(kv.first);
        }

        return res;
    }

    // bool setCaseSens(bool b) { m_caseSens = b; return true; }
    // bool getCaseSens() const { return m_caseSens; }
    // bool setPrototypeSkip(std::string strSkipList)

    bool setCaseSens(const std::string &lang, bool b)
    {
        m_codeOptions[lang].setCaseSens(b);
        return true;
    }

    bool setCaseSens(const std::string &lang, std::string bStr)
    {
        umba::string_plus::tolower(bStr);
        if (bStr=="t" || bStr=="true" || bStr=="y" || bStr=="yes" || bStr=="1")
            m_codeOptions[lang].setCaseSens(true);
        else if (bStr=="f" || bStr=="false" || bStr=="n" || bStr=="no" || bStr=="0")
            m_codeOptions[lang].setCaseSens(false);
        else
            return false;

        return true;
    }

    bool setPrototypeSkip(const std::string &lang, const std::string &s)
    {
        return m_codeOptions[lang].setPrototypeSkip(s);
    }

    bool setAssignOperator(const std::string &lang, const std::string& s)
    {
        return m_codeOptions[lang].setAssignOperator(s);
    }

    bool addCutPrefix(const std::string &lang, const std::string &p)
    {
        m_codeOptions[lang].addCutPrefix(p);
        return true;
    }

    bool addCommentMarker(const std::string &lang, const std::string &p)
    {
        m_codeOptions[lang].addCommentMarker(p);
        return true;
    }

    bool addGenericCutStopPrefix(const std::string &lang, const std::string &p)
    {
        m_codeOptions[lang].addGenericCutStopPrefix(p);
        return true;
    }

    bool setBlockCharacters(const std::string &lang, const std::string &blockPair)
    {
        return m_codeOptions[lang].setBlockCharacters(blockPair);
    }

    std::string getBlockCharacters(const std::string &lang)
    {
        return m_codeOptions[lang].getBlockCharacters();
    }

    bool setStatementSeparator(const std::string &lang, const std::string &statementSeparator)
    {
        return m_codeOptions[lang].setStatementSeparator(statementSeparator);
    }

    std::string getStatementSeparator(const std::string &lang)
    {
        return m_codeOptions[lang].getStatementSeparator();
    }

    // bool setStatementSeparator(const std::string &statementSeparator)
    // {
    //     m_statementSeparator = statementSeparator;
    //     return true;
    // }
    //  
    // std::string getStatementSeparator() const
    // {
    //     return m_statementSeparator;
    // }



    bool setListingTagForBackendGenerator(const std::string &lang, const std::string &tag, const std::string &backend=std::string())
    {
        m_codeOptions[lang].setListingTagForBackendGenerator(tag, backend);
        return true;
    }

    std::string getListingTagForBackendGenerator(const std::string &lang, const std::string &backend=std::string()) const
    {
        //std::unordered_map<std::string, CodeOptions>    m_codeOptions;
        const auto &langOpts = getCodeOptions(lang);

        ListingBackendGeneratorOptions bgo;
        if (!langOpts.findBackendGeneratorOptions(backend, bgo))
            return std::string();

        if (bgo.listingTag.empty())
            langOpts.findBackendGeneratorOptions(backend, bgo);

        return bgo.listingTag;
    }

    bool isCodeTagLine(const std::string &lang, std::string line, std::string *pFoundTagPrefix=0)
    {
        const auto &langOpts = getCodeOptions(lang);
        return langOpts.isCodeTagLine(line, pFoundTagPrefix);
    }

    bool isGenericCutStopLine(const std::string &lang, std::string line) const
    {
        const auto &langOpts = getCodeOptions(lang);
        return langOpts.isGenericCutStopLine(line);
    }



}; // class CodeOptionsDatabase





} // namespace md
} // namespace umba


#include "code-processing/simple-processing.h"



