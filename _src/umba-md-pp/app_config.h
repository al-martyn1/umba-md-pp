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
#include "umba/filesys.h"
#include "umba/env.h"
#include "umba/macros.h"
#include "umba/macro_helpers.h"
//
#include "code-options-database.h"
//
//#include "umba/regex_helpers.h"
//
#include "umba/filename.h"
#include "umba/filesys.h"
//
#include "marty_cpp/src_normalization.h"
//
#include "graph_viz_options.h"
#include "plantuml_options.h"
#include "arg_list_options.h"
//
#include "image_helpers.h"
#include "batch_utils.h"
//
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <stack>


//
#include "log.h"



//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename FilenameStringType>
struct AppConfig
{

    marty_cpp::ELinefeedType outputLinefeed = marty_cpp::getSystemDefaultLinefeedType();

    bool                                                  verboseMode = false;

    std::stack< std::vector<FilenameStringType> >         samplesPathsStack;
    std::vector<FilenameStringType>                       samplesPaths;
    FilenameStringType                                    strictPath  ;

    umba::md::CodeOptionsDatabase                         codeOptionsDatabase;

    GraphVizOptions                                       graphVizOptions;
    PlantUmlOptions                                       plantUmlOptions;

    std::unordered_map<MdPpTag, ArgListOptions>           valListTagOptions ; // arg-list, val-list, opt-list etc
    // ArgListOptions                                        argListOptions ;
    // ArgListOptions                                        valListOptions ;

    bool                                                  clearGenerationCaches = false;

    mutable std::string                                   java;
    mutable std::string                                   javaHome;

    mutable std::string                                   plantUml; // jar

    //std::unordered_map<FilenameStringType, std::string>   extToLang  ;
    //std::unordered_map<std::string, LangOptions>          langOptions;
    std::unordered_set<SnippetOptions>                    snippetOptions;
    std::unordered_map<SnippetOptions, int>               intSnippetOptions;
    umba::macros::StringStringMap<std::string>            conditionVars;     // Изначально предназначалось для проверки условий, но теперь и для макроподстановок

    std::unordered_set<ProcessingOptions>                 processingOptions;

    TargetRenderer                                        targetRenderer = TargetRenderer::github;
    TargetFormat                                          targetFormat   = TargetFormat::md;

    std::string                                           renderingTargetName; // github, gitlab, local - какое-то имя, задающее цель.
                                                                               // Так, мы можем генерить документы по месту, а можем в отдельную папочку для github wiki
                                                                               // И в другую папочку, для своего сайта, например.
                                                                               // И везде могут понадобится отдельные опции
                                                                               // Также заведем переменную MdPpRenderingTargetName - её можно использовать в условных включениях, 
                                                                               // например, или для включения различных для каждого выходгого типа файлов.
                                                                               // По умолчанию будет строка default

    std::unordered_map<std::string, std::string>          metaTagReplaceMap;
    std::unordered_map<std::string, std::string>          metaTagSerializeMap;
    std::unordered_map<std::string, MetaTagType>          metaTagTypeMap;
    std::vector<std::string>                              metaTagsSerializeList;
    std::set<std::string>                                 metaTagsSerializeSet;
    std::vector<std::string>                              documentMetaTagsList;
    std::set<std::string>                                 documentMetaTagsSet;
    //bool                                                  addDocumentMetaTags = false; // force add metatags to document body (not to document meta block)

    unsigned                                              numSecMaxLevel = 0;
    unsigned                                              tocMaxLevel = 0;

    std::size_t                                           restrictPathRaise = 0;

    std::string                                           documentDefaultLanguage = "en-US";
    std::string                                           documentForceLanguage;

    std::vector<std::string>                              mdppExtentions;

    std::string                                           batchOutputRoot;
    std::vector<std::string>                              batchExcludeDirs;
    std::vector<std::string>                              batchExcludeFilesMaskList;
    //std::vector<std::string>                              batchScanPaths;
    std::vector<ScanPathsEntry>                           batchScanPaths;


    std::vector<std::string>                              todoScanPaths;
    std::vector<std::string>                              todoFileNames;


    //bool                                                  batchGeneratePagesIndex = false;
    std::string                                           batchPageIndexFileName;
    bool                                                  generatePageIndexFile         = false;
    bool                                                  batchSplitPageIndex           = false;
    bool                                                  copyImageFiles                = false;
    bool                                                  stripExtentions               = false;
    bool                                                  flattenImageLinks             = false;
    bool                                                  singleModeInOutPathsDifferent = false;

    bool                                                  viewerCopyToSourceLocation = false;
    bool                                                  viewerCopyFilenameDocTitle = false;

    std::string                                           gitAddBatchFileName;


    bool                                                  dontLookupForDoxygen  = false;
    bool                                                  dontLookupForGraphviz = false;


    AppConfig()
    {
        addConditionVar("Lang", "en-US");
    }

    void doClearGenerationCaches() const
    {
        graphVizOptions.deleteHashFile();
        plantUmlOptions.deleteHashFile();
    }
    
    void setGeneratedOutputRoot(const std::string &path)
    {
        plantUmlOptions.savePath = path;
        graphVizOptions.savePath = path;
    }

    void checkUpdateEmptyGeneratedOutputRootByFilename(const std::string &filename)
    {
        LOG_INFO("config") << "Check for generated images output path (Graphviz/PlantUML/etc...)\n";

        graphVizOptions.savePath = umba::filename::makeCanonical(graphVizOptions.savePath);
        plantUmlOptions.savePath = umba::filename::makeCanonical(plantUmlOptions.savePath);

        std::string filePath = umba::filename::makeCanonical(umba::filename::getPath(filename));
        std::string generatedOutputRoot = umba::filename::appendPath(filePath, std::string("img.generated"));

        if (plantUmlOptions.savePath.empty())
        {
            LOG_INFO("config") << "PlantUML generated images output path is not set, setting it to: '" << generatedOutputRoot << "'\n";
            plantUmlOptions.savePath = generatedOutputRoot;
        }

        if (graphVizOptions.savePath.empty())
        {
            LOG_INFO("config") << "Graphviz generated images output path is not set, setting it to: '" << generatedOutputRoot << "'\n";
            graphVizOptions.savePath = generatedOutputRoot;
        }

        #if defined(UMBA_MD_PP_VIEW)

        LOG_INFO("config") << "Modify generated images output paths for viewer\n";
        umba::filename::stripLastPathSep(plantUmlOptions.savePath);
        umba::filename::stripLastPathSep(graphVizOptions.savePath);

        // auto replaceDots = [](std::string str)
        // {
        //     for(auto &ch: str)
        //     {
        //         if (ch=='.')
        //             ch = '_';
        //     }
        //  
        //     return str;
        // };

        auto getLastPathPartsFlatten = [&](const std::string &path, std::size_t nParts)
        {
            std::vector<std::string> parts = umba::filename::splitPath(path);
            if (parts.size()<nParts)
                nParts = parts.size();
            std::size_t nPartsDelete = parts.size() - nParts;

            auto partsRes = std::vector<std::string>(parts.begin()+ std::ptrdiff_t(nPartsDelete), parts.end());
            std::string resStr = umba::string_plus::merge<std::string, std::vector<std::string>::const_iterator>( partsRes.begin(), partsRes.end(), '_'/*, [](auto s) { return s; }*/ );
            //return replaceDots(resStr);
            return umba::filename::flattenPath(resStr, false); // no keep ext
        };

        // plantUmlOptions.savePath = "_view_" + getLastPathPartsFlatten(filename, 3);
        // graphVizOptions.savePath = "_view_" + getLastPathPartsFlatten(filename, 3);

        plantUmlOptions.savePath =  /* replaceDots( */ plantUmlOptions.savePath /* ) */  + ".view_" + getLastPathPartsFlatten(filename, 3);
        graphVizOptions.savePath =  /* replaceDots( */ graphVizOptions.savePath /* ) */  + ".view_" + getLastPathPartsFlatten(filename, 3);


        #endif
    }

    std::string getJava() const
    {
        if (!java.empty())
            return java;

        if (javaHome.empty())
        {
            umba::env::getVar /* <std::string> */ ("MDPP_JAVA_HOME", javaHome);
        }

        if (javaHome.empty())
        {
            umba::env::getVar /* <std::string> */ ("JAVA_HOME", javaHome);
        }

        if (javaHome.empty())
            return std::string();

        java = umba::filename::makeCanonical(umba::filename::appendPath(javaHome, std::string("/bin/java" UMBA_FILESYS_EXE_EXT)));

        return java;
    }

    std::string getPlantUml() const
    {
        if (!plantUml.empty())
            return plantUml;

        if (plantUml.empty())
        {
            umba::env::getVar /* <std::string> */ ("MDPP_PLANTUML_JAR", plantUml);
        }

        if (plantUml.empty())
        {
            umba::env::getVar /* <std::string> */ ("MDPP_PLANTUML", plantUml);
        }

        if (plantUml.empty())
        {
            umba::env::getVar /* <std::string> */ ("PLANTUML_JAR", plantUml);
        }

        if (plantUml.empty())
        {
            umba::env::getVar /* <std::string> */ ("PLANTUML", plantUml);
        }

        return plantUml;
    }

    // mutable std::string                                   java;
    // mutable std::string                                   javaHome;
    //  
    // mutable std::string                                   plantUml;

    // umba::macros::StringStringMap<std::string>            conditionVars;     // Изначально предназначалось для проверки условий, но теперь и для макроподстановок
    //
    // std::unordered_set<ProcessingOptions>                 processingOptions;
    //
    // TargetRenderer                                        targetRenderer = TargetRenderer::github;
    // TargetFormat                                          targetFormat   = TargetFormat::md;
    //
    // std::string                                           renderingTargetName; // github, gitlab, local - какое-то имя, задающее цель.

    void checkFixRenderingTargetName(bool forView)
    {
        if (forView)
        {
            renderingTargetName = "view";
        }
        else
        {
            if (renderingTargetName.empty())
                renderingTargetName = "default";
        }

        conditionVars["MdPpRenderingTargetName"] = renderingTargetName;
    }

    void pushSamplesPaths()
    {
       samplesPathsStack.push(samplesPaths);
       samplesPaths.clear();
    }

    void popSamplesPathsAndInsertNewAtFront()
    {
       if (samplesPathsStack.empty())
           return;

       std::vector<FilenameStringType> tmpPaths = samplesPathsStack.top();
       samplesPathsStack.pop();
       samplesPaths.insert(samplesPaths.end(), tmpPaths.begin(), tmpPaths.end());
    }


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
        return addBatchScanPaths(splitAndTrimAndSkipEmpty(paths, ','), bRecurse);
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
        return addMdppExtentions(splitAndTrimAndSkipEmpty(extListStr, ','));
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

        std::set<std::string> langs = codeOptionsDatabase.getLanguages();

        for(const auto &l: langs)
        {
            std::vector<std::string> langExtsPair;
            langExtsPair.emplace_back(l);

            umba::md::CodeOptions langOpt = codeOptionsDatabase.getCodeOptions(l);
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
        umba::string_plus::trim(str, umba::string_plus::is_one_of<char>(" "));

        if (str.empty())
            return true;

        if (str[0]=='-')
        {
            metaTagsSerializeList.clear();
            str.erase(0,1);
        }
        else if (str.front()=='+')
        {
            str.erase(0,1);
        }
        else
        {
            metaTagsSerializeList.clear();
        }

        umba::string_plus::trim(str, umba::string_plus::is_one_of<char>(" "));

        auto tags = splitAndTrimAndSkipEmpty(str, ',');

        for(auto t: tags)
        {
            auto canonicalTag = makeCanonicalMetaTag(t);
            if (metaTagsSerializeSet.find(canonicalTag)!=metaTagsSerializeSet.end())
                continue;
            metaTagsSerializeList.emplace_back(canonicalTag);
        }

        return true;
    }

    bool setDocumentMetaTagList(std::string str)
    {
        umba::string_plus::trim(str, umba::string_plus::is_one_of<char>(" "));

        if (str.empty())
            return true;

        if (str[0]=='-')
        {
            metaTagsSerializeList.clear();
            str.erase(0,1);
        }
        else if (str.front()=='+')
        {
            str.erase(0,1);
        }
        else
        {
            metaTagsSerializeList.clear();
        }

        umba::string_plus::trim(str, umba::string_plus::is_one_of<char>(" "));

        auto tags = splitAndTrimAndSkipEmpty(str, ',');

        for(auto t: tags)
        {
            auto canonicalTag = makeCanonicalMetaTag(t);
            if (documentMetaTagsSet.find(canonicalTag)!=documentMetaTagsSet.end())
                continue;
            documentMetaTagsList.emplace_back(canonicalTag);
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
        return addMetaTagType(typeStr, splitAndTrimAndSkipEmpty(tags, ','));
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
            conditionVars[name]                                      = value;
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

        const static std::string sep = std::string(":");
        std::string first, second;
        umba::string_plus::split_to_pair( nameValue, first, second, sep);
        //std::vector<std::string> nameValuePair = splitAndTrimAndSkipEmpty(nameValue, ':');
        // if (nameValuePair.empty())
        //     return false;
        // if (nameValuePair.size()<2)
        //     return addConditionVar(nameValue,std::string());
        // else
        //     return addConditionVar(nameValuePair[0],nameValuePair[1]);

        return addConditionVar(first,second);
    }

    bool updateInsertOptions(const std::string &opts)
    {
        if (umba::md::deserializeSnippetOptions(opts, &snippetOptions, &intSnippetOptions)!=SnippetOptionsParsingResult::ok)
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

    //! Добавляет пути поиска примеров. О разделителях см. описание umba::filename::splitPathList
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
    bool readInputFile(const std::string &a_inputFilename, std::string &inputFileText)
    {
        std::string inputFileTextOrg;

        if (!umba::filesys::readFile(a_inputFilename, inputFileTextOrg))
        {
            return false;
        }

        inputFileText = autoEncodeToUtf(inputFileTextOrg);
        inputFileText = marty_cpp::normalizeCrLfToLf(inputFileText);

        return true;
    }

    static
    bool readInputFile(const std::wstring &a_inputFilename, std::string &inputFileText)
    {
        std::string inputFileTextOrg;

        if (!umba::filesys::readFile(a_inputFilename, inputFileTextOrg))
        {
            return false;
        }

        inputFileText = autoEncodeToUtf(inputFileTextOrg);
        inputFileText = marty_cpp::normalizeCrLfToLf(inputFileText);

        return true;
    }


    // static
    bool findSamplesFileImpl(const std::vector<FilenameStringType> &samplesPathsVec, FilenameStringType lookFor, FilenameStringType &foundFullFilename, std::string &foundFileText) const
    {
         lookFor = umba::filename::makeCanonical( lookFor, (typename FilenameStringType::value_type)'/'
                                                , umba::string_plus::make_string<FilenameStringType>(".")
                                                , umba::string_plus::make_string<FilenameStringType>("..")
                                                , true // keepLeadingParents
                                                );

         LOG_INFO("snippet-lookup") << "Looking for file: " << lookFor << "\n";
         // if (verboseMode)
         // {
         //     umbaLogStreamMsg << "Looking for file: " << lookFor << "\n";
         // }

         for(auto path: samplesPathsVec)
         {
             auto fullName = umba::filename::makeCanonical(umba::filename::appendPath(path, lookFor));

             if (verboseMode)
             {

             }

             if (readInputFile(fullName, foundFileText))
             {
                 // if (verboseMode)
                 // {
                 //     umbaLogStreamMsg << "  + Looking in: " << path << " (" << fullName << ") - ";
                 //     umbaLogStreamMsg << "Found\n";
                 // }
                 LOG_INFO("snippet-lookup") << "  + Looking in: " << path << " (" << fullName << ") - " << "Found\n";

                 foundFullFilename = fullName;
                 return true;
             }

             // if (verboseMode)
             // {
             //     umbaLogStreamMsg << "  - Looking in: " << path << " (" << fullName << ") - ";
             //     umbaLogStreamMsg << "Not found\n";
             // }

             LOG_INFO("snippet-lookup") << "  - Looking in: " << path << " (" << fullName << ") - " << "Not found\n";

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

    // static
    bool findDocFileByPath(const FilenameStringType &lookFor, std::string &foundFullFilename, std::string &foundFileText, const std::string &includedFromPath) const
    {
        std::vector<FilenameStringType> docPaths;
        docPaths.emplace_back(includedFromPath);
        return findSamplesFileImpl(docPaths, lookFor, foundFullFilename, foundFileText);
    }

    //static
    bool findDocFileByIncludedFromFilename(const FilenameStringType &lookFor, FilenameStringType &foundFullFilename, std::string &foundFileText, const FilenameStringType &includedFromFile) const
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
    // Option helpers for --add-code-file-extention=cpp:cpp,cxx,c++,cc,h,hpp,h++

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

            codeOptionsDatabase.addLanguageExtention(lang, ext);

            ++cnt;
        }

        return cnt>0;
    }

    bool addLangExtentions(const std::string &lang, const FilenameStringType &extList)
    {
        std::vector<FilenameStringType> extListVec = splitAndTrimAndSkipEmpty(extList, (typename FilenameStringType::value_type)',');
        return addLangExtentions(lang, extListVec);
    }

    bool addLangExtentions(const std::string &langAndExts)
    {
        std::vector<FilenameStringType> langExtsPair = splitAndTrimAndSkipEmpty(langAndExts, (typename FilenameStringType::value_type)':');
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
        return codeOptionsDatabase.findLanguageByFilename(umba::string_plus::make_string<std::string>(fileName));
    }

    // std::string getLangByFilename(const FilenameStringType &name) const
    // {
    //     return getLangByExt(umba::filename::getFileExtention(name));
    // }

    //----------------------------------------------------------------------------


    //----------------------------------------------------------------------------



    bool setCodeCaseSens(const std::string &lang, const std::string &bStr)       { return codeOptionsDatabase.setCaseSens(lang, bStr); }
    bool setCodePrototypeSkip(const std::string &lang, const std::string &s)     { return codeOptionsDatabase.setPrototypeSkip(lang, s); }
    bool setCodeAssignOperator(const std::string &lang, const std::string& s)    { return codeOptionsDatabase.setAssignOperator(lang, s); }

    bool setCodeCaseSens(const std::string &langValPair)
    {
        std::string lang, val;
        if (!umba::string_plus::split_to_pair(langValPair, lang, val, ':'))
        {
            return false;
        }

        return setCodeCaseSens(lang, val);
    }

    bool setCodePrototypeSkip(const std::string &langValPair)
    {
        std::string lang, val;
        if (!umba::string_plus::split_to_pair(langValPair, lang, val, ':'))
        {
            return false;
        }

        return setCodePrototypeSkip(lang, val);
    }

    bool setCodeAssignOperator(const std::string &langValPair)
    {
        std::string lang, val;
        if (!umba::string_plus::split_to_pair(langValPair, lang, val, ':'))
        {
            return false;
        }

        return setCodeAssignOperator(lang, val);
    }


    // --set-code-cut-prefix=nut,//!#
    bool addCodeCutPrefix(const std::string &lang, const std::string &cutPrefix) { codeOptionsDatabase.addCutPrefix(lang, cutPrefix);  return true; }

    bool addCodeCutPrefix(const std::string &langPrefixPair)
    {
        std::string lang, cutPrefix;
        if (!umba::string_plus::split_to_pair(langPrefixPair, lang, cutPrefix, ':'))
        {
            return false;
        }

        return addCodeCutPrefix(lang, cutPrefix);
    }

    bool addCodeCommentMarker(const std::string &lang, const std::string &cutPrefix) { codeOptionsDatabase.addCommentMarker(lang, cutPrefix); return true; }

    bool addCodeCommentMarker(const std::string &langPrefixPair)
    {
        std::string lang, cutPrefix;
        if (!umba::string_plus::split_to_pair(langPrefixPair, lang, cutPrefix, ':'))
        {
            return false;
        }

        return addCodeCommentMarker(lang, cutPrefix);
    }

    bool addCodeSeparatorLinePrefix(const std::string &lang, const std::string &sepPrefix)
    {
        //langOptions[lang].cutPrefix = cutPrefix;
        codeOptionsDatabase.addGenericCutStopPrefix(lang, sepPrefix);
        return true;
    }

    bool addCodeSeparatorLinePrefix(const std::string &langPrefixPair)
    {
        std::string lang, sepPrefix;
        if (!umba::string_plus::split_to_pair(langPrefixPair, lang, sepPrefix, ':'))
        {
            return false;
        }

        return addCodeSeparatorLinePrefix(lang, sepPrefix);
    }

    bool setCodeBlockCharacters(const std::string &lang, const std::string &blockPair)
    {
        return codeOptionsDatabase.setBlockCharacters(lang, blockPair);
    }

    bool setCodeBlockCharacters(const std::string &langBlockPair)
    {
        std::string lang, blockPair;
        if (!umba::string_plus::split_to_pair(langBlockPair, lang, blockPair, ':'))
        {
            return false;
        }

        return setCodeBlockCharacters(lang, blockPair);
    }

    bool setCodeStatementSeparator(const std::string &lang, const std::string &statementSeparator)
    {
        return codeOptionsDatabase.setStatementSeparator(lang, statementSeparator);
    }

    bool setCodeStatementSeparator(const std::string &langStatementSeparator)
    {
        std::string lang, statementSeparator;
        if (!umba::string_plus::split_to_pair(langStatementSeparator, lang, statementSeparator, ':'))
        {
            return false;
        }

        return setCodeStatementSeparator(lang, statementSeparator);
    }


    //----------------------------------------------------------------------------
    bool setCodeProcessingHandler(const std::string &handlerLangTypeNameTriplet)
    {
        return codeOptionsDatabase.setCodeProcessingHandler(handlerLangTypeNameTriplet);
    }

    //----------------------------------------------------------------------------
    // --set-code-listing-tag=nut,sq
    bool setLangListingTag(const std::string &lang, const std::string &listingTag, const std::string &backend=std::string())
    {
        codeOptionsDatabase.setListingTagForBackendGenerator(lang, listingTag, backend);
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

        return codeOptionsDatabase.getListingTagForBackendGenerator(lang, backend);
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
// --set-code-cut-prefix=nut,//!#
// --set-code-code-suffix=nut,nut

    // std::unordered_map<std::string, std::string>          extToLang  ;
    // std::unordered_map<std::string, LangOptions>          langOptions;




}; // struct AppConfig




