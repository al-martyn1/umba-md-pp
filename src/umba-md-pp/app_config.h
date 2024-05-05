#pragma once


#include "snippet_options.h"
//
#include "encoding/encoding.h"
//
#include "umba/program_location.h"
#include "umba/enum_helpers.h"
#include "umba/flag_helpers.h"
#include "umba/string_plus.h"
#include "umba/id_gen.h"
//
//#include "umba/regex_helpers.h"
//
#include "umba/filename.h"
#include "umba/filesys.h"
//
#include "marty_cpp/src_normalization.h"

//
#include <string>
#include <vector>
#include <map>
#include <unordered_map>



//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
struct LangOptions
{
    std::string listingTag;
    std::string cutPrefix ;

};

//----------------------------------------------------------------------------
template<typename FilenameStringType>
struct AppConfig
{

    #if defined(WIN32) || defined(_WIN32)
        marty_cpp::ELinefeedType outputLinefeed = marty_cpp::ELinefeedType::crlf;
    #else
        marty_cpp::ELinefeedType outputLinefeed = marty_cpp::ELinefeedType::lf;
    #endif

    std::vector<FilenameStringType>                       samplesPaths;
    FilenameStringType                                    strictPath ;
    std::unordered_map<FilenameStringType, std::string>   extToLang  ;
    std::unordered_map<std::string, LangOptions>          langOptions;
    std::unordered_set<SnippetOptions>                    snippetOptions;
    std::unordered_map<std::string, std::string>          conditionVars;

    std::unordered_set<ProcessingOptions>                 processingOptions;

    TargetRenderer                                        targetRenderer = TargetRenderer::github;
    TargetFormat                                          targetFormat   = TargetFormat::md;

    std::unordered_map<std::string, std::string>          metaTagReplaceMap;
    std::unordered_map<std::string, std::string>          metaTagSerializeMap;
    std::unordered_map<std::string, MetaTagType>          metaTagTypeMap;
    std::vector<std::string>                              metaTagsSerializeList;

    unsigned                                              numSecMaxLevel = 0;
    unsigned                                              tocMaxLevel = 0;

    std::size_t                                           restrictPathRaise = 0;

    std::string                                           documentDefaultLanguage;
    std::string                                           documentForceLanguage;



    void checkTargetFormat()
    {
        if (targetRenderer!=TargetRenderer::doxygen)
            return;

        if (((std::uint32_t)targetFormat & (std::uint32_t)TargetFormat::printable)==0)
            return;

        updateProcessingOptions("parse-github-alerts")
    }

    bool setMetaTagSerializeList(std::string str)
    {
        if (str.empty())
            return true;

        if (str.front()=='+')
        {
            str.erase(0,1);
        }
        else
        {
            metaTagsSerializeList.clear();
        }

        auto tags = marty_cpp::splitToLinesSimple(str, false, ',');

        for(auto t: tags)
        {
            metaTagsSerializeList.emplace_back(makeCanonicalMetaTag(t));
        }

        return true;
    }

    bool addMetaTagType(const std::string &typeStr, const std::vector<std::string> &tags)
    {
        auto type = enum_deserialize(typeStr, MetaTagType::invalid);
        if (type==MetaTagType::invalid)
            return false;

        for(auto tag : tags)
        {
            tag = makeCanonicalMetaTag(tag);
            metaTagTypeMap[tag] = type;
        }

        return true;
    }

    bool addMetaTagType(const std::string &typeStr, const std::string &tags)
    {
        return addMetaTagType(typeStr, marty_cpp::splitToLinesSimple(tags, false, ','));
    }

    bool addMetaTagType(const std::string &str)
    {
        std::string f;
        std::string s;
        if (!umba::string_plus::split_to_pair(str, f, s, ':'))
            return false;

        return addMetaTagType(f, s);
    }

    MetaTagType getMetaTagType(const std::string &tag) const
    {
        std::unordered_map<std::string, MetaTagType>::const_iterator  mit = metaTagTypeMap.find(makeCanonicalMetaTag(tag));
        return mit!=metaTagTypeMap.end() ? mit->second : MetaTagType::textFirst;
    }

    // std::string getMetaTagText(const std::string &tag) const
    // {
    //  
    // }
    

    void setStrictPath(const FilenameStringType &p)
    {
        strictPath = umba::filename::stripLastPathSepCopy(umba::filename::makeCanonical(p));
    }

    void setStrictPathFromFilename(const FilenameStringType &fName)
    {
        setStrictPath(umba::filename::getPath(fName));
    }

    //! Возвращает true, если включение запрещено
    bool checkIsInsertRestricted(const FilenameStringType &fName) const
    {
        std::size_t raiseVal = restrictPathRaise;
        if (raiseVal>4)
            raiseVal = 4;

        if (strictPath.empty())
        {
            return false; // не запрещено - путь ограничитель не задан
        }

        if (!testProcessingOption(ProcessingOptions::strictInsert))
        {
            return false; // не запрещено по опциям
        }

        FilenameStringType raisedPath = strictPath;
        for(std::size_t i=0u; i!=raiseVal; ++i)
        {
            raisedPath = umba::filename::getPath(raisedPath);
        }

        if (umba::filename::isSubPathName(raisedPath, fName))
        {
            return false; // не запрещено - файл расположен в пределах разрешенного корня
        }

        return true; // Запрещено
    }


    void checkAdjustDocNumericLevels()
    {
		// Всё, что попадает в содержание, должно иметь номера
		// numSecMaxLevel - то, что нумеруется
		// tocMaxLevel    - то, что попадает в содержание

        if ( numSecMaxLevel // Лимит уровня для нумерации установлен
          && tocMaxLevel    // Лимит того, что попадает в содержание - тоже установлен
           ) 
		{
            if (numSecMaxLevel<tocMaxLevel)
                numSecMaxLevel = tocMaxLevel;
        }
    }

    bool addMetaTagReplace(const std::string &t, const std::string &r)
    {
        metaTagReplaceMap[marty_cpp::toLower(t)] = marty_cpp::toLower(r);
        return true;
    }

    bool addMetaTagReplace(const std::string &trPair)
    {
        std::string t;
        std::string r;
        if (!umba::string_plus::split_to_pair(trPair, t, r, ':'))
        {
            return false;
        }

        return addMetaTagReplace(t, r);
    }

    std::string makeCanonicalMetaTag(std::string t) const
    {
        t = umba::transliterate(t);
        //std::string candis = marty_cpp::toLower(umba::generateIdFromText_generic(t, 0));
        t = marty_cpp::toLower(t);
        auto it = metaTagReplaceMap.find(t);
        return it!=metaTagReplaceMap.end() ? it->second : t;
        //Candace Dutton
        //Candice Patton
        // candidate
    }

    bool addMetaTagSerialize(const std::string &t, const std::string &r)
    {
        metaTagSerializeMap[marty_cpp::toLower(t)] = r;
        return true;
    }

    bool addMetaTagSerialize(const std::string &trPair)
    {
        std::string t;
        std::string r;
        if (!umba::string_plus::split_to_pair(trPair, t, r, ':'))
        {
            return false;
        }

        return addMetaTagSerialize(t, r);
    }

    std::string serializeMetaTag(const std::string &t) const
    {
        auto it = metaTagSerializeMap.find(t);
        return it!=metaTagSerializeMap.end() ? it->second : t;
    }


    bool addConditionVar(std::string name, std::string value)
    {
        umba::string_plus::trim(name);
        umba::string_plus::trim(value);

        if (name.empty())
            return false;

        conditionVars[name] = value;
        return true;
    }

    bool addConditionVar(std::string nameValue)
    {
        umba::string_plus::trim(nameValue);
        
        std::vector<std::string> nameValuePair = marty_cpp::splitToLinesSimple(nameValue, false, ':');
        if (nameValuePair.empty())
            return false;
        if (nameValuePair.size()<2)
            return addConditionVar(nameValue,std::string());
        else
            return addConditionVar(nameValuePair[0],nameValuePair[1]);
    }

    bool updateInsertOptions(const std::string &opts)
    {
        if (deserializeSnippetOptions(opts, &snippetOptions)!=SnippetOptionsParsingResult::ok)
        {
            return false;
        }

        snippetOptions.erase(SnippetOptions::doc); // document insert mode can't be configured by default, only explicit mode allowed for doc mode

        return true;
    }

    bool updateProcessingOptions(const std::string &opts)
    {
        if (deserializeProcessingOptions(opts, processingOptions)!=SnippetOptionsParsingResult::ok)
        {
            return false;
        }

        return true;
    }

    bool testProcessingOption(ProcessingOptions opt) const
    {
        auto baseOpt = (ProcessingOptions)(((std::uint32_t)opt)|0x0001u);
    
        if (processingOptions.find(baseOpt)!=processingOptions.end())
            return true;
    
        return false;
    }



    // umba::string_plus::trim(lineCopy);
    // if (!umba::string_plus::starts_with_and_strip(lineCopy, tagPrefix))
    //     return false;

    // std::unordered_set<SnippetOptions>                    snippetOptions;

    bool addSamplesPaths(const std::vector<FilenameStringType> &pl, const FilenameStringType &basePath)
    {
        //std::vector<FilenameStringType> &dirs = m_lookupMap[lvl].lookupDirs;

        std::vector<FilenameStringType> absPaths; absPaths.reserve(pl.size());
        for(auto p: pl)
        {
            p = umba::filename::makeAbsPath(p, basePath);
            absPaths.emplace_back(umba::filename::makeCanonical(p));
        }
        
        samplesPaths.insert( samplesPaths.end(), absPaths.begin(), absPaths.end() );
        return true;
    }

    //! Добавляет пути поиска примеров
    /*! Разделителем является спец символ:
        - Win32 - ';' (точка с запятой, semicolon)
        - Linux - ':' (двоеточие, colon)
     */
    bool addSamplesPaths(const FilenameStringType &pl, const FilenameStringType &basePath)
    {
        return addSamplesPaths(umba::filename::splitPathList( pl ), basePath);
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

    static
    bool readInputFile(const std::string &inputFilename, std::string &inputFileText)
    {
        std::string inputFileTextOrg;

        if (!umba::filesys::readFile(inputFilename, inputFileTextOrg))
        {
            return false;
        }

        inputFileText = autoEncodeToUtf(inputFileTextOrg);
        inputFileText = marty_cpp::normalizeCrLfToLf(inputFileText);

        return true;
    }

    static
    bool readInputFile(const std::wstring &inputFilename, std::string &inputFileText)
    {
        std::string inputFileTextOrg;

        if (!umba::filesys::readFile(inputFilename, inputFileTextOrg))
        {
            return false;
        }

        inputFileText = autoEncodeToUtf(inputFileTextOrg);
        inputFileText = marty_cpp::normalizeCrLfToLf(inputFileText);

        return true;
    }

    static
    bool findSamplesFileImpl(const std::vector<FilenameStringType> &samplesPathsVec, FilenameStringType lookFor, FilenameStringType &foundFullFilename, std::string &foundFileText)
    {
         lookFor = umba::filename::makeCanonical(lookFor);

         for(auto path: samplesPathsVec)
         {
             auto fullName = umba::filename::appendPath(path, lookFor);
             if (readInputFile(fullName, foundFileText))
             {
                  foundFullFilename = fullName;
                  return true;
             }
             // std::string orgText;
             // if (umba::filesys::readFile(fullName, orgText))
             // {
             //     orgText           = autoEncodeToUtf(orgText);
             //     foundFileText     = marty_cpp::normalizeCrLfToLf(orgText);
             //     foundFullFilename = fullName;
             //     return true;
             // }
         }

         return false;
    }

    bool findSamplesFile(const FilenameStringType &lookFor, FilenameStringType &foundFullFilename, std::string &foundFileText) const
    {
        return findSamplesFileImpl(samplesPaths, lookFor, foundFullFilename, foundFileText);
    }

    bool findSamplesFileUseExtraPath(const FilenameStringType &lookFor, FilenameStringType &foundFullFilename, FilenameStringType &foundFileText, const FilenameStringType &extraPath) const
    {
        auto p = samplesPaths;
        p.emplace_back(extraPath);
        return findSamplesFileImpl(p, lookFor, foundFullFilename, foundFileText);
    }

    bool findSamplesFileUseExtraPathFromFilename(const FilenameStringType &lookFor, FilenameStringType &foundFullFilename, std::string &foundFileText, const FilenameStringType &fileInsertedFrom) const
    {
        return findSamplesFileUseExtraPath(lookFor, foundFullFilename, foundFileText, umba::filename::getPath(fileInsertedFrom));
    }

    static
    bool findDocFileByPath(const FilenameStringType &lookFor, std::string &foundFullFilename, std::string &foundFileText, const std::string &includedFromPath)
    {
        std::vector<FilenameStringType> docPaths;
        docPaths.emplace_back(includedFromPath);
        return findSamplesFileImpl(docPaths, lookFor, foundFullFilename, foundFileText);
    }

    static
    bool findDocFileByIncludedFromFilename(const FilenameStringType &lookFor, FilenameStringType &foundFullFilename, std::string &foundFileText, const FilenameStringType &includedFromFile)
    {
        return findDocFileByPath(lookFor, foundFullFilename, foundFileText, umba::filename::getPath(includedFromFile));
    }

    static
    FilenameStringType normalizeExt(FilenameStringType ext)
    {
        while(!ext.empty() && ext.front()==(typename FilenameStringType::value_type)'.')
        {
            ext.erase(0,1);
        }

        return marty_cpp::toLower(ext);
    }
    

    //----------------------------------------------------------------------------
    // Option helpers for --add-lang-file-extention=cpp:cpp,cxx,c++,cc,h,hpp,h++

    bool addLangExtentions(std::string lang, const std::vector<FilenameStringType> &extList)
    {
        if (lang.empty())
            return false;

        //lang = marty_cpp::toLower(lang);

        std::size_t cnt = 0;
        for(auto ext: extList)
        {
            ext = normalizeExt(ext);
            if (ext.empty())
                continue;

            extToLang[ext] = lang;

            ++cnt;
        }

        return cnt>0;
    }

    bool addLangExtentions(const std::string &lang, const FilenameStringType &extList)
    {
        std::vector<FilenameStringType> extListVec = marty_cpp::splitToLinesSimple(extList, false, (typename FilenameStringType::value_type)',');
        return addLangExtentions(lang, extListVec);
    }

    bool addLangExtentions(const std::string &langAndExts)
    {
        std::vector<FilenameStringType> langExtsPair = marty_cpp::splitToLinesSimple(langAndExts, false, (typename FilenameStringType::value_type)':');
        if (langExtsPair.size()<2)
            return false;
        return addLangExtentions(langExtsPair[0], langExtsPair[1]);
    }

    std::string getLangByExt(FilenameStringType ext) const
    {
        ext = normalizeExt(ext);

        // if (ext.empty())
        //     continue;

        std::unordered_map<FilenameStringType, std::string>::const_iterator it = extToLang.find(ext);
        if (it==extToLang.end())
        {
            return std::string();
        }

        return it->second;
    }

    std::string getLangByFilename(const FilenameStringType &name) const
    {
        return getLangByExt(umba::filename::getFileExtention(name));
    }

    //----------------------------------------------------------------------------


    //----------------------------------------------------------------------------
    // --set-lang-cut-prefix=nut,//!#
    bool setLangCutPrefix(const std::string &lang, const std::string &cutPrefix)
    {
        langOptions[lang].cutPrefix = cutPrefix;
        return true;
    }

    bool setLangCutPrefix(const std::string &langPrefixPair)
    {   
        std::string lang, cutPrefix;
        if (!umba::string_plus::split_to_pair(langPrefixPair, lang, cutPrefix, ':'))
        {
            return false;
        }

        return setLangCutPrefix(lang, cutPrefix);
    }

    std::string getLangCutPrefix(const std::string &lang) const
    {
        std::unordered_map<std::string, LangOptions>::const_iterator it = langOptions.find(lang);
        if (it==langOptions.end())
        {
            return std::string();
        }
        return it->second.cutPrefix;
    }


    //----------------------------------------------------------------------------
    // --set-lang-listing-tag=nut,sq
    bool setLangListingTag(const std::string &lang, const std::string &listingTag)
    {
        langOptions[lang].listingTag = listingTag;
        return true;
    }

    bool setLangListingTag(const std::string &langTagPair)
    {   
        std::string lang, listingTag;
        if (!umba::string_plus::split_to_pair(langTagPair, lang, listingTag, ':'))
        {
            return false;
        }

        return setLangListingTag(lang, listingTag);
    }

    std::string getLangListingTag(const std::string &lang) const
    {
        std::unordered_map<std::string, LangOptions>::const_iterator it = langOptions.find(lang);
        if (it==langOptions.end())
        {
            return std::string();
        }
        return it->second.listingTag;
    }



// bool hasEq = umba::string_plus::split_to_pair(cmdLineArg, opt, optArg, '=');
    //std::unordered_map<std::string, LangOptions>          langOptions;
// struct LangOptions
// {
//     std::string listingTag;
//     std::string cutPrefix ;
//  
// };

//std::vector<StringType> splitToLinesSimple(const StringType &str, bool addEmptyLineAfterLastLf = true, typename StringType::value_type lfChar=(typename StringType::value_type)'\n')


    //std::vector<std::string> lines = marty_utext::splitToLinesSimple(normUtext, true /* addEmptyLineAfterLastLf */ );

// inline char    toLower( char ch )     { return isUpper(ch) ? ch-'A'+'a' : ch; }
// inline char    toUpper( char ch )     { return isLower(ch) ? ch-'a'+'A' : ch; }
//  
// inline wchar_t toLower( wchar_t ch )  { return (wchar_t)(isUpper(ch) ? ch-L'A'+L'a' : ch); }
// inline wchar_t toUpper( wchar_t ch )  { return (wchar_t)(isLower(ch) ? ch-L'a'+L'A' : ch); }

// --cut-options=lineno,notrim,notag
// --set-lang-cut-prefix=nut,//!#
// --set-lang-code-suffix=nut,nut

    // std::unordered_map<std::string, std::string>          extToLang  ;
    // std::unordered_map<std::string, LangOptions>          langOptions;




}; // struct AppConfig




