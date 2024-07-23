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
#include "umba/filename.h"
#include "umba/macros.h"
#include "umba/macro_helpers.h"
//
#include "language-options-database.h"
//
//#include "umba/regex_helpers.h"
//
#include "umba/filename.h"
#include "umba/filesys.h"
//
#include "marty_cpp/src_normalization.h"
//
#include "graph_viz_options.h"

//
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>


//
#include "log.h"



//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
class ImageFileForCopyInfo
{
protected:

    std::string    sourceFilename;
    std::string    sourceFilenameForCompare;
    std::string    targetFilename;
    std::string    targetFilenameForCompare;

    std::string    imageLink        ; // Оригинальный линк
    std::string    imageLinkTarget  ; // Модифицированный (возможно) линк
    std::string    documentFilename ; // Файл документа, в котором  используется линк


public:

    ImageFileForCopyInfo() = default;
    ImageFileForCopyInfo(const ImageFileForCopyInfo&) = default;
    ImageFileForCopyInfo& operator=(const ImageFileForCopyInfo&) = default;
    ImageFileForCopyInfo(ImageFileForCopyInfo&&) = default;
    ImageFileForCopyInfo& operator=(ImageFileForCopyInfo&&) = default;

    void setSourceFilename(const std::string &f)
    {
        sourceFilename           = umba::filename::makeCanonical(f);
        sourceFilenameForCompare = umba::filename::makeCanonicalForCompare(f);
    }

    std::string getSourceFilename() const
    {
        return sourceFilename;
    }

    std::string getSourceFilenameForCompare() const
    {
        return sourceFilenameForCompare;
    }

    void setTargetFilename(const std::string &f)
    {
        targetFilename           = umba::filename::makeCanonical(f);
        targetFilenameForCompare = umba::filename::makeCanonicalForCompare(f);
    }

    std::string getTargetFilename() const
    {
        return targetFilename;
    }

    std::string getTargetFilenameForCompare() const
    {
        return targetFilenameForCompare;
    }

    void setImageLink(const std::string &f)
    {
        imageLink = f;
    }

    std::string getImageLink() const
    {
        return imageLink;
    }

    void setImageLinkTarget(const std::string &f)
    {
        imageLinkTarget = f;
    }

    std::string getImageLinkTarget() const
    {
        return imageLinkTarget;
    }

    void setDocumentFilename(const std::string &f)
    {
        documentFilename = f;
    }

    std::string getDocumentFilename() const
    {
        return documentFilename;
    }

    bool isSourceFilenameTheSame(const std::string &f) const
    {
        return sourceFilenameForCompare==umba::filename::makeCanonicalForCompare(f);
    }

    bool isTargetFilenameTheSame(const std::string &f) const
    {
        return targetFilenameForCompare==umba::filename::makeCanonicalForCompare(f);
    }


}; // class ImageFileForCopyInfo

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
bool addImageFilesForCopying( std::map<std::string, ImageFileForCopyInfo>       &imagesToCopy
                           , const std::string                                  &sourceDocumentFilename
                           , const std::string                                  &targetDocumentFilename
                           , const std::unordered_map<std::string, std::string> &imagLinks // src -> dst
                           )
{
    std::string inputPath  = umba::filename::getPath(sourceDocumentFilename);
    std::string outputPath = umba::filename::getPath(targetDocumentFilename);

    bool res = true;

    for(const auto &imgFilePair : imagLinks)
    {
        const auto &imgSrcLink = imgFilePair.first ;
        const auto &imgTgtLink = imgFilePair.second;

        ImageFileForCopyInfo imgInfo;
        imgInfo.setSourceFilename(umba::filename::makeCanonical(umba::filename::appendPath(inputPath , imgSrcLink)));
        imgInfo.setTargetFilename(umba::filename::makeCanonical(umba::filename::appendPath(outputPath, imgTgtLink)));
        imgInfo.setImageLink(imgSrcLink);
        imgInfo.setImageLinkTarget(imgTgtLink);
        imgInfo.setDocumentFilename(sourceDocumentFilename);


        std::map<std::string, ImageFileForCopyInfo>::const_iterator imgIt = imagesToCopy.find(imgInfo.getTargetFilenameForCompare());
        if (imgIt==imagesToCopy.end())
        {
            imagesToCopy[imgInfo.getTargetFilenameForCompare()] = imgInfo;
        }
        else
        {
            if (!imgIt->second.isSourceFilenameTheSame(imgInfo.getSourceFilenameForCompare()))
            {
                // Вообще, такое не должно происходить
                LOG_WARN_OPT("img-copy-tgt-exist") << "target file already added for copying, but source files are different (target file: '" << imgIt->second.getTargetFilename() << "')\n";
                LOG_WARN_OPT("img-copy-tgt-exist") << "previosly added source file is: '" << imgIt->second.getSourceFilename() << "'\n";
                LOG_WARN_OPT("img-copy-tgt-exist") << "added from '" << imgIt->second.getDocumentFilename() << "' ('" << imgIt->second.getImageLink() << "')\n";
                LOG_WARN_OPT("img-copy-tgt-exist") << "current source file is: '" << imgInfo.getSourceFilename() << "'\n";
                LOG_WARN_OPT("img-copy-tgt-exist") << "added from '" << imgInfo.getDocumentFilename() << "' ('" << imgInfo.getImageLink() << "')\n";
                res = false;
            }
        }
    }

    return res;
}

//----------------------------------------------------------------------------
template<typename LogStreamType>
inline
bool copyDocumentImageFiles(LogStreamType & logStream, const std::map<std::string, ImageFileForCopyInfo> &imagesToCopy, bool bOverwrite, std::vector<std::string> *pGitAddFiles=0)
{
    if (!imagesToCopy.empty())
    {
        logStream << "Copying image files\n";
    }
    else
    {
        logStream << "Copying image files: nothing to copy\n";
    }

    bool res = true;
    std::map<std::string, ImageFileForCopyInfo>::const_iterator it = imagesToCopy.begin();
    for(; it!=imagesToCopy.end(); ++it)
    {
        const ImageFileForCopyInfo &imgInfo = it->second;
        const auto &srcFile = imgInfo.getSourceFilename();
        const auto &tgtFile = imgInfo.getTargetFilename();

        umba::filesys::createDirectoryEx<std::string>( umba::filename::getPath(tgtFile), true /* forceCreatePath */ );

        logStream << "    Copying image file '" << srcFile << "' to '" << tgtFile << "'\n";

        if (!CopyFileA(srcFile.c_str(), tgtFile.c_str(), bOverwrite ? FALSE : TRUE)) // ошибка, если существует и не режим overwrite
        {
            DWORD err = GetLastError();
            if (err==ERROR_FILE_NOT_FOUND)
            {
                LOG_WARN_OPT("img-copy") << "failed to copy image file: source file '" << srcFile << "' not exist\n";
            }
            else if (err==ERROR_FILE_EXISTS)
            {
                LOG_WARN_OPT("img-copy") << "failed to copy image file: target file '" << tgtFile << "' already exist\n";
            }
            else
            {
                LOG_WARN_OPT("img-copy") << "failed to copy image file: error " << err << "\n";
            }

            res = false;
        }
        else
        {
            if (pGitAddFiles)
                pGitAddFiles->emplace_back(tgtFile);
        }

    }

    return res;
}





//----------------------------------------------------------------------------
// struct LangOptions
// {
//     std::string listingTag;
//     std::string cutPrefix ;
//
// };


struct ScanPathsEntry
{
    std::string        path;
    bool               recurse = false;
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

    umba::md::LanguageOptionsDatabase                     languageOptionsDatabase;

    GraphVizOptions                                       graphVizOptions;

    //std::unordered_map<FilenameStringType, std::string>   extToLang  ;
    //std::unordered_map<std::string, LangOptions>          langOptions;
    std::unordered_set<SnippetOptions>                    snippetOptions;
    umba::macros::StringStringMap<std::string>            conditionVars;     // Изначально предназначалось для проверки условий, но теперь и для макроподстановок

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

    std::vector<std::string>                              mdppExtentions;

    std::string                                           batchOutputRoot;
    std::vector<std::string>                              batchExcludeDirs;
    std::vector<std::string>                              batchExcludeFilesMaskList;
    //std::vector<std::string>                              batchScanPaths;
    std::vector<ScanPathsEntry>                           batchScanPaths;

    //bool                                                  batchGeneratePagesIndex = false;
    std::string                                           batchPageIndexFileName;
    bool                                                  batchSplitPageIndex           = false;
    bool                                                  copyImageFiles                = false;
    bool                                                  stripExtentions               = false;
    bool                                                  flattenImageLinks             = false;
    bool                                                  singleModeInOutPathsDifferent = false;

    bool                                                  viewerCopyToSourceLocation = false;
    bool                                                  viewerCopyFilenameDocTitle = false;

    std::string                                           gitAddBatchFileName;

    bool isGitAddBatchFileNameIsBatFile() const
    {
        if (gitAddBatchFileName.empty())
            return false;
        auto ext = umba::string_plus::tolower_copy(umba::filename::getExt(gitAddBatchFileName));
        if (ext=="bat" || ext=="cmd")
            return true;
        return false;
    }

    bool getEffectiveFlattenImageLinksOption() const
    {
        if (!isBatchMode())
        {
            if (!singleModeInOutPathsDifferent) // выходной путь - тот же, нет копирования, нет коррекции
                return false;

        }
        else // batch mode
        {
            if (batchOutputRoot.empty()) // выходной путь - тот же, нет копирования, нет коррекции
                return false;
        }

        if (!copyImageFiles)
            return false;

        return flattenImageLinks;
    }

    // bool isBatchMode() const


    bool addBatchScanPath(const std::string &path, bool bRecurse)
    {
        std::vector<ScanPathsEntry>::iterator it = std::find_if( batchScanPaths.begin(), batchScanPaths.end()
                                                               , [=](const ScanPathsEntry &spe)
                                                                 {
                                                                     return umba::filename::makeCanonicalForCompare(spe.path)==umba::filename::makeCanonicalForCompare(path);
                                                                 }
                                                               );
        if (it==batchScanPaths.end())
        {
            batchScanPaths.emplace_back(ScanPathsEntry{path, bRecurse});
            return true;
        }

        // Тут у нас найден путь, но опция рекурсии может быть отличной от новой

        if (it->recurse==bRecurse) // Опция рекурсии та же
            return true; // всё хорошо, ничего не добавляем

        // Опции рекурсии отличаются, одна из них рекурсивная
        it->recurse = true; // Рекурсивная бьёт нерекурсивную всегда

        return true;
    }

    bool addBatchScanPaths(const std::vector<std::string> &paths, bool bRecurse)
    {
        for(const auto &p : paths)
        {
            if (!addBatchScanPath(p, bRecurse))
                return false;
        }

        return true;
    }

    bool addBatchScanPaths(const std::string &paths, bool bRecurse)
    {
        return addBatchScanPaths(marty_cpp::splitToLinesSimple(paths, false, ','), bRecurse);
    }


    bool isBatchMode() const
    {
        return !batchScanPaths.empty();
    }

    std::unordered_set<std::string> getSupportedExtentionsSet() const
    {
        std::unordered_set<std::string> extSet;

        for(auto mdppExt : mdppExtentions)
        {
            extSet.insert(mdppExt);
            extSet.insert(getTargetFileExtention(mdppExt));
        }

        return extSet;
    }

    std::vector<std::string> getSupportedExtentionsVector() const
    {
        auto s = getSupportedExtentionsSet();
        return std::vector<std::string>(s.begin(), s.end());
    }

    std::string getSupportedExtentionsString() const
    {
        auto s = getSupportedExtentionsSet();
        return umba::string_plus::merge<std::string, std::unordered_set<std::string>::const_iterator>( s.begin(), s.end(), ','/*, [](auto s) { return s; }*/ );
    }

    bool isSupportedExtention(std::string e) const
    {
        e = umba::string_plus::tolower_copy(e);
        auto supportedSet = getSupportedExtentionsSet();
        if (supportedSet.find(e)==supportedSet.end())
            return false;
        return true;
    }

    bool isSupportedSourceExtention(std::string e) const
    {
        e = umba::string_plus::tolower_copy(e);
        auto itFound = std::find(mdppExtentions.begin(), mdppExtentions.end(), e);
        return itFound!=mdppExtentions.end();
    }

    static
    std::string getTargetFileExtention(std::string srcFileExtention)
    {
        // source file extentions (default config): _md, .md_, .markdown, ._markdown, .markdown_
        umba::string_plus::trim(srcFileExtention, umba::string_plus::is_one_of<char>("._"));
        return srcFileExtention;
    }

    bool addMdppExtention(std::string ext)
    {
        // inplace trim
        umba::string_plus::trim(ext, umba::string_plus::is_one_of<char>("."));

        if (ext.empty())
            return false;

        ext = umba::string_plus::tolower_copy(ext);

        std::vector<std::string>::const_iterator it = std::find(mdppExtentions.begin(), mdppExtentions.end(), ext);
        if (it==mdppExtentions.end())
        {
            mdppExtentions.emplace_back(ext);
        }

        return true;
    }

    bool addMdppExtentions(const std::vector<std::string> &extList)
    {
        for(const auto &ext : extList)
        {
            if (!addMdppExtention(ext))
                return false;
        }

        return true;
    }

    bool addMdppExtentions(const std::string &extListStr)
    {
        return addMdppExtentions(marty_cpp::splitToLinesSimple(extListStr, false, ','));
    }

    FilenameStringType getSamplesPathsAsMergedString(const FilenameStringType &delim) const
    {
        return umba::string_plus::merge< FilenameStringType, typename std::vector<FilenameStringType>::const_iterator >( samplesPaths.begin(), samplesPaths.end(), delim );
    }

    FilenameStringType getAllLangFileExtentions( const FilenameStringType &extListDelim = umba::string_plus::make_string<FilenameStringType>(", ")
                                               , const FilenameStringType &langDelim    = umba::string_plus::make_string<FilenameStringType>(": ")
                                               , const FilenameStringType &topListDelim = umba::string_plus::make_string<FilenameStringType>("; ")
                                               ) const
    {
        std::vector<FilenameStringType> topList;

        std::set<std::string> langs = languageOptionsDatabase.getLanguages();

        for(const auto l: langs)
        {
            std::vector<std::string> langExtsPair;
            langExtsPair.emplace_back(l);

            umba::md::LanguageOptions langOpt = languageOptionsDatabase.getLanguageOptions(l);
            const auto &extsSet = langOpt.getLanguageExtentions();
            langExtsPair.emplace_back(umba::string_plus::merge< std::string, typename std::set<std::string>::const_iterator >( extsSet.begin(), extsSet.end(), umba::string_plus::make_string<std::string>(extListDelim) ));
            topList.emplace_back(umba::string_plus::make_string<FilenameStringType>(umba::string_plus::merge< std::string, typename std::vector<std::string>::const_iterator >( langExtsPair.begin(), langExtsPair.end(), umba::string_plus::make_string<std::string>(langDelim) )));
        }

        return umba::string_plus::merge< FilenameStringType, typename std::vector<FilenameStringType>::const_iterator >( topList.begin(), topList.end(), topListDelim);


        /*
        typedef std::map<std::string, std::vector<FilenameStringType> > LangToExtListMapType;
        LangToExtListMapType langToExtList;

        typedef decltype(extToLang) map_type;
        typename map_type::const_iterator it = extToLang.begin();
        for(; it!=extToLang.end(); ++it)
        {
            langToExtList[it->second].emplace_back(it->first);
        }

        std::vector<FilenameStringType> topList;

        typename LangToExtListMapType::const_iterator lit = langToExtList.begin();
        for(; lit!=langToExtList.end(); ++lit)
        {
            FilenameStringType strExtList = umba::string_plus::merge< FilenameStringType, typename std::vector<FilenameStringType>::const_iterator >( lit->second.begin(), lit->second.end(), extListDelim );
            FilenameStringType lang;
            umba::utfFromTo(lit->first, lang);

            std::vector<FilenameStringType> langWithExtList;
            langWithExtList.emplace_back(lang);
            langWithExtList.emplace_back(strExtList);

            topList.emplace_back(umba::string_plus::merge< FilenameStringType, typename std::vector<FilenameStringType>::const_iterator >( langWithExtList.begin(), langWithExtList.end(), langDelim ));
        }

        return umba::string_plus::merge< FilenameStringType, typename std::vector<FilenameStringType>::const_iterator >( topList.begin(), topList.end(), topListDelim );
        */

    }

    // std::unordered_map<FilenameStringType, std::string>   extToLang  ;
    // std::unordered_map<std::string, LangOptions>          langOptions;

// template<typename StringTypeFrom, typename StringTypeTo>
// StringTypeTo utfFromToReturn(const StringTypeFrom &from)



    void checkTargetFormat()
    {
        if (targetRenderer!=TargetRenderer::doxygen)
            return;

        if (((std::uint32_t)targetFormat & (std::uint32_t)TargetFormat::printable)==0)
            return;

        updateProcessingOptions("parse-github-alerts");
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

        if (umba::string_plus::starts_with_and_strip(name, "^"))
        {
            conditionVars[name]                                 = value;
            conditionVars["^"+umba::string_plus::toupper_copy(name)] = value;
        }
        else
        {
            conditionVars[name] = value;
        }

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
        if (umba::md::deserializeSnippetOptions(opts, &snippetOptions)!=SnippetOptionsParsingResult::ok)
        {
            return false;
        }

        snippetOptions.erase(SnippetOptions::doc); // document insert mode can't be configured by default, only explicit mode allowed for doc mode

        return true;
    }

    bool updateProcessingOptions(const std::string &opts)
    {
        if (umba::md::deserializeProcessingOptions(opts, processingOptions)!=SnippetOptionsParsingResult::ok)
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
         lookFor = umba::filename::makeCanonical( lookFor, (typename FilenameStringType::value_type)'/'
                                                , umba::string_plus::make_string<FilenameStringType>(".")
                                                , umba::string_plus::make_string<FilenameStringType>("..")
                                                , true // keepLeadingParents
                                                );

         for(auto path: samplesPathsVec)
         {
             auto fullName = umba::filename::makeCanonical(umba::filename::appendPath(path, lookFor));
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

    /*
    static
    FilenameStringType normalizeExt(FilenameStringType ext)
    {
        while(!ext.empty() && ext.front()==(typename FilenameStringType::value_type)'.')
        {
            ext.erase(0,1);
        }

        return marty_cpp::toLower(ext);
    }
    */

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
            //ext = normalizeExt(ext);
            if (ext.empty())
                continue;

            //extToLang[ext] = lang;

            languageOptionsDatabase.addLanguageExtention(lang, ext);

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

    std::string getLangByFilename(const FilenameStringType &fileName) const
    {
        /*
        ext = normalizeExt(ext);

        // if (ext.empty())
        //     continue;

        //typename std::unordered_map<typename FilenameStringType, std::string>::const_iterator it = extToLang.find(ext);
        typedef decltype(extToLang) map_type;
        typename map_type::const_iterator it = extToLang.find(ext);
        if (it==extToLang.end())
        {
            return std::string();
        }

        return it->second;
        */
        return languageOptionsDatabase.findLanguageByFilename(umba::string_plus::make_string<std::string>(fileName));
    }

    // std::string getLangByFilename(const FilenameStringType &name) const
    // {
    //     return getLangByExt(umba::filename::getFileExtention(name));
    // }

    //----------------------------------------------------------------------------


    //----------------------------------------------------------------------------
    // --set-lang-cut-prefix=nut,//!#
    bool addCutPrefix(const std::string &lang, const std::string &cutPrefix)
    {
        //langOptions[lang].cutPrefix = cutPrefix;
        languageOptionsDatabase.addCutPrefix(lang, cutPrefix);
        return true;
    }

    bool addCutPrefix(const std::string &langPrefixPair)
    {
        std::string lang, cutPrefix;
        if (!umba::string_plus::split_to_pair(langPrefixPair, lang, cutPrefix, ':'))
        {
            return false;
        }

        return addCutPrefix(lang, cutPrefix);
    }

    bool addSeparatorLinePrefix(const std::string &lang, const std::string &sepPrefix)
    {
        //langOptions[lang].cutPrefix = cutPrefix;
        languageOptionsDatabase.addGenericCutStopPrefix(lang, sepPrefix);
        return true;
    }

    bool addSeparatorLinePrefix(const std::string &langPrefixPair)
    {
        std::string lang, sepPrefix;
        if (!umba::string_plus::split_to_pair(langPrefixPair, lang, sepPrefix, ':'))
        {
            return false;
        }

        return addSeparatorLinePrefix(lang, sepPrefix);
    }

    bool setBlockCharacters(const std::string &lang, const std::string &blockPair)
    {
        //langOptions[lang].cutPrefix = cutPrefix;
        return languageOptionsDatabase.setBlockCharacters(lang, blockPair);
    }

    bool setBlockCharacters(const std::string &langBlockPair)
    {
        std::string lang, blockPair;
        if (!umba::string_plus::split_to_pair(langBlockPair, lang, blockPair, ':'))
        {
            return false;
        }

        return setBlockCharacters(lang, blockPair);
    }


    // std::string getLangCutPrefix(const std::string &lang) const
    // {
    //     std::unordered_map<std::string, LangOptions>::const_iterator it = langOptions.find(lang);
    //     if (it==langOptions.end())
    //     {
    //         return std::string();
    //     }
    //     return it->second.cutPrefix;
    // }


    //----------------------------------------------------------------------------
    // --set-lang-listing-tag=nut,sq
    bool setLangListingTag(const std::string &lang, const std::string &listingTag, const std::string &backend=std::string())
    {
        languageOptionsDatabase.setListingTagForBackendGenerator(lang, listingTag, backend);
        //langOptions[lang].listingTag = listingTag;
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

    std::string getLangListingTag(const std::string &lang, const std::string &backend=std::string()) const
    {
        // std::unordered_map<std::string, LangOptions>::const_iterator it = langOptions.find(lang);
        // if (it==langOptions.end())
        // {
        //     return std::string();
        // }
        // return it->second.listingTag;

        return languageOptionsDatabase.getListingTagForBackendGenerator(lang, backend);
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




