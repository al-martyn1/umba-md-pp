#pragma once

#include "app_config.h"
#include "document.h"
#include "umba/filename.h"
#include "umba/filesys.h"
//
#include <string>


//std::size_t h1 = std::hash<std::string>{}(s.first_name);

inline
std::string generateTempSubfolderNameByInputFileName(const std::string &name)
{
    std::size_t h = std::hash<std::string>{}(name);
    return std::to_string(h);
}

inline
std::wstring generateTempSubfolderNameByInputFileName(const std::wstring &name)
{
    std::size_t h = std::hash<std::wstring>{}(name);
    return std::to_wstring(h);
}

template<typename StringType> inline
bool createTempFolder(StringType &finalPath, const StringType &inputFileName, const StringType &appName=umba::string_plus::make_string<StringType>("umba-md-pp-view"))
{
    StringType tempRoot = umba::filesys::getTempFolderPath<StringType>();
    umba::filesys::createDirectory(tempRoot);

    StringType umbaMdPpViewerTempRoot = umba::filename::appendPath(tempRoot, umba::string_plus::make_string<StringType>(".") + appName);
    umba::filesys::createDirectory(umbaMdPpViewerTempRoot);

    StringType curFileTempRoot = umba::filename::appendPath(umbaMdPpViewerTempRoot, generateTempSubfolderNameByInputFileName(inputFileName));
    umba::filesys::createDirectory(curFileTempRoot);

    if (umba::filesys::isPathDirectory(curFileTempRoot))
    {
        finalPath = curFileTempRoot;
        return true;
    }

    return false;
}

inline
marty_cpp::ELinefeedType getConfigsLinefeed()
{
    #if defined(WIN32) || defined(_WIN32)
        return marty_cpp::ELinefeedType::crlf;
    #else
        return marty_cpp::ELinefeedType::lf;
    #endif
}

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
std::string generateDoxyfile(const AppConfig<FilenameStringType> &appCfg, const Document &doc)
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
    lines.emplace_back("FILE_PATTERNS          = .md");
    lines.emplace_back("RTF_OUTPUT             = rtf");
    lines.emplace_back("OUTPUT_DIRECTORY       = doxy");
    lines.emplace_back("RTF_EXTENSIONS_FILE    = doxygen_rtf.cfg");
    lines.emplace_back("INPUT                  = document.md");

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
    str = doc.getMetaTagValueAsSingleLineText(appCfg, "language", ",");
    if (!str.empty())
    {
        lines.emplace_back("OUTPUT_LANGUAGE        = " + str);
    }

    return marty_cpp::mergeLines(lines, getConfigsLinefeed(), true  /* addTrailingNewLine */ );
}

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

    return marty_cpp::mergeLines(lines, getConfigsLinefeed(), true  /* addTrailingNewLine */ );
}

inline
std::string generateFinalFilenameFromTitle(const std::string &titleStr)
{
    // https://stackoverflow.com/questions/1976007/what-characters-are-forbidden-in-windows-and-linux-directory-names

    static const std::string restrictedChars = "^*\"/\\<>:|?.";
    // ^ / * " / \ < > : | ?

    std::string resFilename; resFilename.reserve(titleStr.size());
    for(auto ch : titleStr)
    {
        if ((unsigned)(int)ch<(unsigned)' ' || restrictedChars.find(ch)!=restrictedChars.npos)
        {
            ch = '_';
        }

        resFilename.append(1,ch);
    }

    return resFilename;
}


/*
RTF Ext

Title           = 
Author
Version

Manager
Subject
Comments
Keywords
*/

