#pragma once

#include "app_config.h"
#include "document.h"

//
#include "umba/umba.h"
#include "umba/filename.h"
#include "umba/filesys.h"
#include "umba/shellapi.h"

#include <string>




//std::size_t h1 = std::hash<std::string>{}(s.first_name);

template<typename StringType>
StringType makeFileNameWithReducedPath(const StringType &inputFilename, const std::size_t numPathLevels=2u)
{
    StringType tmpInputFileName;
    tmpInputFileName = umba::filename::getFileName(inputFilename);
    StringType tmpPath = umba::filename::getPath(inputFilename);
    for(std::size_t i=0u; i!=numPathLevels; ++i)
    {
        auto tmpPathPart = umba::filename::getFileName(tmpPath);
        if (tmpPathPart.empty() || tmpPathPart==tmpPath)
            break;
        tmpPath = umba::filename::getPath(tmpPath);
        tmpInputFileName = umba::filename::appendPath(tmpPathPart, tmpInputFileName);
    }

    return tmpInputFileName;
}

//----------------------------------------------------------------------------
inline
std::string generateTempSubfolderNameByInputFileName(const std::string &name)
{
    std::size_t h = std::hash<std::string>{}(name);

    std::string nameNoPath = makeFileNameWithReducedPath(name);
    //umba::filename::getFileName(name);
    for(auto &ch: nameNoPath)
    {
        if (ch=='.' || ch=='\\' || ch=='/' || (ch>=0 && ch<=' '))
            ch = '_';
    }

    return std::to_string(h) + "_" + nameNoPath;
}

//----------------------------------------------------------------------------
inline
std::wstring generateTempSubfolderNameByInputFileName(const std::wstring &name)
{
    std::size_t h = std::hash<std::wstring>{}(name);

    std::wstring nameNoPath = makeFileNameWithReducedPath(name);
    //umba::filename::getFileName(name);
    for(auto &ch: nameNoPath)
    {
        if (ch==L'.' || ch==L'\\' || ch==L'/' || (ch>=0 && ch<=L' '))
            ch = L'_';
    }

    return std::to_wstring(h) + L"_" + nameNoPath;
}

//----------------------------------------------------------------------------
bool createTempFolder(std::string &finalPath, const std::string &inputFileName, const std::string &appName="umba-md-pp-view")
{
    std::string tempRoot = umba::filesys::getTempFolderPath();
    umba::filesys::createDirectory(tempRoot);

    std::string umbaMdPpViewerTempRoot = umba::filename::appendPath(tempRoot, "." + appName);
    umba::filesys::createDirectory(umbaMdPpViewerTempRoot);

    std::string generatedSimpleFolderName = generateTempSubfolderNameByInputFileName(inputFileName);
    std::string curFileTempRoot = umba::filename::appendPath(umbaMdPpViewerTempRoot, generatedSimpleFolderName);
    finalPath = curFileTempRoot;

    if (!umba::filesys::createDirectory(curFileTempRoot) && !umba::filesys::isLastErrorAlreadyExists())
    {
        return false;
    }

    if (umba::filesys::isPathDirectory(curFileTempRoot))
    {
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------
bool removeTempFolder(std::string &finalPath, const std::string &appName="umba-md-pp-view")
{
    std::string tempRoot = umba::filesys::getTempFolderPath();
    //umba::filesys::createDirectory(tempRoot);

    std::string umbaMdPpViewerTempRoot = umba::filename::appendPath(tempRoot, "." + appName);
    //umba::filesys::createDirectory(umbaMdPpViewerTempRoot);

    finalPath = umbaMdPpViewerTempRoot;

    return umba::shellapi::deleteDirectory(umbaMdPpViewerTempRoot);

}


//----------------------------------------------------------------------------
/*
PROJECT_BRIEF          = ""
# OUTPUT_LANGUAGE        = English
OUTPUT_LANGUAGE        = Russian
INPUT                  = Имя файла или каталога

EXCLUDE                =
EXCLUDE_PATTERNS       =
OUTPUT_DIRECTORY       = doxy

// Вроде надо бы, но хз, что сюда добавить
IMAGE_PATH             =

COMPACT_RTF            = NO

RTF_STYLESHEET_FILE    =

AUTOLINK_SUPPORT       = YES

BRIEF_MEMBER_DESC      = YES
REPEAT_BRIEF           = YES
ALWAYS_DETAILED_SEC    = YES
FULL_PATH_NAMES        = YES
STRIP_FROM_PATH        =
STRIP_FROM_INC_PATH    =
SHORT_NAMES            = NO
JAVADOC_AUTOBRIEF      = NO
QT_AUTOBRIEF           = NO
INHERIT_DOCS           = YES
SEPARATE_MEMBER_PAGES  = NO
IDL_PROPERTY_SUPPORT   = YES
EXTRACT_ALL            = YES

*/
template<typename FilenameStringType> inline
std::string generateDoxyfile(const AppConfig<FilenameStringType> &appCfg, const Document &doc, const std::string &documentPath)
{
    std::vector<std::string> lines;

    lines.emplace_back("DOXYFILE_ENCODING      = UTF-8");
    lines.emplace_back("INPUT_ENCODING         = UTF-8");
    lines.emplace_back("MARKDOWN_SUPPORT       = YES");
    lines.emplace_back("TAB_SIZE               = 4");
    lines.emplace_back("RECURSIVE              = NO");
    lines.emplace_back("GENERATE_RTF           = YES");
    lines.emplace_back("RTF_HYPERLINKS         = YES");

    lines.emplace_back("EXCLUDE                = doxy");
    lines.emplace_back("FILE_PATTERNS          = *.md");
    lines.emplace_back("RTF_OUTPUT             = rtf");
    lines.emplace_back("OUTPUT_DIRECTORY       = doxy");
    lines.emplace_back("RTF_EXTENSIONS_FILE    = doxygen_rtf.cfg");
    lines.emplace_back("INPUT                  = document.md");
    lines.emplace_back("IMAGE_PATH             = " + umba::filename::getPath(documentPath));


    std::string str = doc.getDocumentTitleAny();
    if (!str.empty())
    {
        lines.emplace_back("PROJECT_NAME           = " + str);
    }

    str = doc.getMetaTagValueAsSingleLineText(appCfg, "version", ",");
    if (!str.empty())
    {
        lines.emplace_back("PROJECT_NUMBER         = " + str);
    }

    // English/Russian
    str = doc.getDocumentLanguage(appCfg);
    if (!str.empty())
        str = findLangTagByString(str);
    if (!str.empty())
        str = findGoxygenLanguageByLangTag(str);
    if (str.empty())
        str = "English";
    lines.emplace_back("OUTPUT_LANGUAGE        = " + str);


    return marty_cpp::mergeLines(lines, marty_cpp::getSystemDefaultLinefeedType(), true  /* addTrailingNewLine */ );
}

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::string generateDoxygenRtfCfg(const AppConfig<FilenameStringType> &appCfg, const Document &doc)
{
    std::vector<std::string> lines;

    std::string str = doc.getDocumentTitleAny();
    if (!str.empty())
    {
        lines.emplace_back("Title = " + str);
    }

    str = doc.getMetaTagValueAsSingleLineText(appCfg, "version", ",");
    if (!str.empty())
    {
        lines.emplace_back("Version = " + str);
    }

    str = doc.getMetaTagValueAsSingleLineText(appCfg, "author", ",");
    if (!str.empty())
    {
        lines.emplace_back("Author = " + str);
    }

    return marty_cpp::mergeLines(lines, marty_cpp::getSystemDefaultLinefeedType(), true  /* addTrailingNewLine */ );
}

//----------------------------------------------------------------------------
inline
std::string generateFinalFilenameFromTitle(const std::string &titleStr, bool bTransliterate)
{
    // https://stackoverflow.com/questions/1976007/what-characters-are-forbidden-in-windows-and-linux-directory-names

    static const std::string restrictedChars = "^*\"/\\<>:|?.";
    // ^ / * " / \ < > : | ?

    std::string resFilename; resFilename.reserve(titleStr.size());
    for(auto ch : titleStr)
    {
        if ((unsigned)(int)ch<=(unsigned)' ' || restrictedChars.find(ch)!=restrictedChars.npos)
        {
            ch = '_';
        }

        resFilename.append(1,ch);
    }

    if (bTransliterate)
    {
        resFilename = umba::transliterate(resFilename);
    }


    return resFilename;
}

//----------------------------------------------------------------------------
inline
std::wstring generateFinalFilenameFromTitle(const std::wstring &titleStr, bool bTransliterate)
{
    // https://stackoverflow.com/questions/1976007/what-characters-are-forbidden-in-windows-and-linux-directory-names

    static const std::wstring restrictedChars = L"^*\"/\\<>:|?.";
    // ^ / * " / \ < > : | ?

    std::wstring resFilename; resFilename.reserve(titleStr.size());
    for(auto ch : titleStr)
    {
        if ((unsigned)(int)ch<=(unsigned)' ' || restrictedChars.find(ch)!=restrictedChars.npos)
        {
            ch = '_';
        }

        resFilename.append(1,ch);
    }

    if (bTransliterate)
    {

        resFilename = umba::fromUtf8(umba::transliterate(umba::toUtf8(resFilename)));
    }

    return resFilename;
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
inline
void showErrorMessageBox(const std::string &str)
{
    #if defined(UMBA_MD_PP_VIEW_CONSOLE)
        LOG_MSG << str << "\n";
    #else
        umba::shellapi::showMessageBox(str, "Umba Markdown PP Viewer", umba::shellapi::MessageBoxKind::iconError);
    #endif
}

//----------------------------------------------------------------------------


