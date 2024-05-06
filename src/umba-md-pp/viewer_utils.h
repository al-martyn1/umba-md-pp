#pragma once

#include "app_config.h"
#include "document.h"

//
#include "umba/umba.h"
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
    lines.emplace_back("FILE_PATTERNS          = *.md");
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
    str = doc.getDocumentLanguage(appCfg);
    if (!str.empty())
        str = findLangTagByString(str);
    if (!str.empty())
        str = findGoxygenLanguageByLangTag(str);
    if (str.empty())
        str = "English";
    lines.emplace_back("OUTPUT_LANGUAGE        = " + str);
    

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
        if ((unsigned)(int)ch<=(unsigned)' ' || restrictedChars.find(ch)!=restrictedChars.npos)
        {
            ch = '_';
        }

        resFilename.append(1,ch);
    }

    return resFilename;
}

inline
std::wstring generateFinalFilenameFromTitle(const std::wstring &titleStr)
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

    return resFilename;
}

inline
bool isWindows32OnWindows64()
{
    #if !defined(WIN32) && !defined(_WIN32)

        return false; // not a windows at all

    #else

        //#if defined(WIN64) || defined(_WIN64)

        //    return false; // 64хбитное приложение, нет проблем

        //#else

            // https://stackoverflow.com/questions/14184137/how-can-i-determine-whether-a-process-is-32-or-64-bit

            typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

            BOOL bIsWow64 = FALSE;

            auto hKernel = GetModuleHandle(TEXT("kernel32"));
            if (hKernel)
            {
	            LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(hKernel,"IsWow64Process");
                if (fnIsWow64Process)
                {
                    if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
                    {
                        //handle error
                    }                
                }
            }

            return bIsWow64 ? true : false;

        //#endif

    #endif
}


typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

inline
std::wstring findDoxygenExecutableName()
{
    #if !defined(WIN32) && !defined(_WIN32)

        return L"doxygen";

    #else

        // https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-functions

        std::wstring doxygen = L"doxygen.exe";


        // Doxygen можно поискать тут:
        // Компьютер\HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\doxygen_is1
        // "Inno Setup: App Path"
        // "InstallLocation"

        // KEY_WOW64_64KEY

        REGSAM samDesired = KEY_READ;
        if (isWindows32OnWindows64()) // 32х-битные системы сейчас конечно уже экзотика, но на всякий случай - я же и на XP могу работать
        {
            samDesired |= KEY_WOW64_64KEY;
        }

        HKEY hk = 0;
        LSTATUS status = RegOpenKeyExW( HKEY_LOCAL_MACHINE
                                      //, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Conan Package Manager_is1"
                                      , L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\doxygen_is1"
                                      //, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
                                      //, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion"
                                      //, L"SOFTWARE\\Microsoft\\Windows"
                                      //, L"SOFTWARE\\Microsoft"
                                      //, L"SOFTWARE"
                                      , 0 // ulOptions
                                      , samDesired
                                      , &hk
                                      );
        if (status==ERROR_SUCCESS)
        {
            wchar_t buf[1024];
            DWORD type;
            DWORD cbData = sizeof(buf); // in bytes
            status = RegQueryValueExW( hk
                                     , L"InstallLocation"
                                     , 0 // reserved
                                     , &type
                                     , (LPBYTE)&buf[0]
                                     , &cbData
                                     );

            if (status==ERROR_SUCCESS && type==REG_SZ)
            {
                if (cbData>(sizeof(buf)-1))
                    cbData = sizeof(buf)-1;

                std::size_t numChars = cbData/sizeof(wchar_t);
                if (numChars>0)
                {
                    if (buf[numChars-1]==0)
                        --numChars;
                }

                if (numChars>0)
                {
	                doxygen = std::wstring(buf, numChars);
	                doxygen = umba::filename::appendPath(doxygen, std::wstring(L"bin"));
                    doxygen = umba::filename::appendPath(doxygen, std::wstring(L"doxygen.exe"));
                }

            }
        
            RegCloseKey(hk);
        }

        return doxygen;

    #endif
}


inline
void showErrorMessageBox(std::string str)
{

	#if defined(WIN32) || defined(_WIN32)
	
	    MessageBoxA( 0 // hwnd
                   , str.c_str()
                   , "Umba Markdown PP Viewer"
                   , MB_OK | MB_ICONERROR
                   );
	
	#endif

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

