#pragma once

#include "processing_includes.h"
//
#include "config.h"



//----------------------------------------------------------------------------
template<typename FilenameStringType>
using TagLineExtraParser = std::function<std::string::const_iterator (const AppConfig<FilenameStringType>&, umba::html::HtmlTag&, MdPpTag, std::string::const_iterator, std::string::const_iterator)>;

//------------------------------
template<typename FilenameStringType>
using TagLineExtraParsersMap = std::unordered_map<MdPpTag, TagLineExtraParser<FilenameStringType>, EnumClassHash >;

//------------------------------
template<typename FilenameStringType>
TagLineExtraParsersMap<FilenameStringType> makeTagLineExtraParsersMap()
{
    TagLineExtraParsersMap<FilenameStringType> m;

    m[MdPpTag::graph    ] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::puml     ] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::csvTable ] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };


    
    m[MdPpTag::argList  ] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::valList  ] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::retList  ] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::optList  ] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::defList  ] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::fieldList] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::termList ] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::argList2 ] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::valList2 ] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::retList2 ] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::optList2 ] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::defList2 ] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::fieldList2] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    m[MdPpTag::termList2] = [](const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
                        {
                            UMBA_USED(appCfg);
                            UMBA_USED(tagType);
                            return umba::md::parseExtraPossibleFilenameAndTextToHtmlTag(mdHtmlTag, b, e);
                        };

    return m;
}

//------------------------------
template<typename FilenameStringType>
const TagLineExtraParsersMap<FilenameStringType>& getTagLineExtraParsersMap()
{
    static auto m = makeTagLineExtraParsersMap<FilenameStringType>();
    return m;
}

//------------------------------
template<typename FilenameStringType>
std::string::const_iterator tagLineExtraParse(const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, std::string::const_iterator b, std::string::const_iterator e)
{
    const auto &m = const_cast< TagLineExtraParsersMap<FilenameStringType>& >(getTagLineExtraParsersMap<FilenameStringType>());
    auto it = m.find(tagType);
    if (it==m.end())
        return e;

    return it->second(appCfg, mdHtmlTag, tagType, b, e);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename FilenameStringType>
using TagLinesProcessor = std::function<void (const AppConfig<FilenameStringType>&, Document&, umba::html::HtmlTag&, MdPpTag, const FilenameStringType&, const std::vector<std::string>&, std::vector<std::string>&)>;

//------------------------------
template<typename FilenameStringType>
using TagLinesProcessorsMap = std::unordered_map<MdPpTag, TagLinesProcessor<FilenameStringType>, EnumClassHash >;

//------------------------------
template<typename FilenameStringType>
TagLinesProcessorsMap<FilenameStringType> makeTagLinesProcessorsMap()
{
    TagLinesProcessorsMap<FilenameStringType> m;

    m[MdPpTag::graph    ] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processGraphLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    m[MdPpTag::puml     ] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processDiagramLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    m[MdPpTag::csvTable ] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processCsvTableLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };


    m[MdPpTag::argList  ] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processArgListLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    m[MdPpTag::valList  ] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processArgListLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    m[MdPpTag::retList  ] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processArgListLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    m[MdPpTag::optList  ] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processArgListLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    m[MdPpTag::defList  ] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processArgListLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    m[MdPpTag::fieldList] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processArgListLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    m[MdPpTag::termList ] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processArgListLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    m[MdPpTag::argList2 ] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processArgListLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    m[MdPpTag::valList2 ] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processArgListLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    m[MdPpTag::retList2 ] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processArgListLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    m[MdPpTag::optList2 ] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processArgListLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    m[MdPpTag::defList2 ] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processArgListLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };
    m[MdPpTag::fieldList2] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processArgListLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    m[MdPpTag::termList2] = [](const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
                        {
                             return umba::md::processArgListLines(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
                        };

    return m;
}

//------------------------------
template<typename FilenameStringType>
const TagLinesProcessorsMap<FilenameStringType>& getTagLinesProcessorsMap()
{
    static auto m = makeTagLinesProcessorsMap<FilenameStringType>();
    return m;
}

//------------------------------
template<typename FilenameStringType>
void tagLinesProcess(const AppConfig<FilenameStringType> &appCfg, Document& doc, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType, const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines)
{
    const auto &m = const_cast< TagLinesProcessorsMap<FilenameStringType>& >(getTagLinesProcessorsMap<FilenameStringType>());
    auto it = m.find(tagType);
    if (it==m.end())
        return;

    it->second(appCfg, doc, mdHtmlTag, tagType, docFilename, tagLines, resLines);
}

//----------------------------------------------------------------------------
