#pragma once


#include "umba/string_plus.h"
#include "umba/utf8.h"
#include "umba/shellapi.h"
#include "umba/env.h"

// For 'system' function
#include <process.h>
#include <cerrno>

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline std::string findDoxygenBinPathImpl(bool dontTouchSystem)
{
    if (dontTouchSystem)
        return std::string();

#if defined(WIN32) && defined(_WIN32)

    std::wstring p;
    if (!umba::win32_utils::regQueryAppInstallLocationBin(std::wstring(L"doxygen_is1"), p))
        return std::string();

    return umba::toUtf8(p);

#else

    return std::string();

#endif
}

//----------------------------------------------------------------------------
inline const std::string& findDoxygenBinPath(bool dontTouchSystem)
{
    static std::string p = findDoxygenBinPathImpl(dontTouchSystem);
    return p;
}

//----------------------------------------------------------------------------
inline std::string findDoxygenExecutableName(bool dontTouchSystem)
{
    std::string toolExePathName;

    if (umba::env::getVar("MDPP_DOXYGEN", toolExePathName))
        return toolExePathName;

    if (umba::env::getVar("DOXYGEN", toolExePathName))
        return toolExePathName;

    return umba::filename::appendPath(findDoxygenBinPath(dontTouchSystem), std::string("doxygen" UMBA_FILESYS_EXE_EXT) );
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline std::string findGraphvizBinPathImpl(bool dontTouchSystem)
{
    if (dontTouchSystem)
        return std::string();

#if defined(WIN32) && defined(_WIN32)

    std::wstring p;
    if (!umba::win32_utils::regQueryAppInstallLocationBin(std::wstring(L"Graphviz"), p))
        return std::string();

    return umba::toUtf8(p);

#else

    return std::string();

#endif
}

//----------------------------------------------------------------------------
inline const std::string& findGraphvizBinPath(bool dontTouchSystem)
{
    static std::string p = findGraphvizBinPathImpl(dontTouchSystem);
    return p;
}

//----------------------------------------------------------------------------
inline std::string findGraphvizToolExecutableName(bool dontTouchSystem, const std::string &graphvizTool)
{
    std::string graphvizToolUpper = umba::string_plus::toupper_copy(graphvizTool);

    std::string toolExePathName; // GRAPHVIZ_DOT...

    if (umba::env::getVar("MDPP_GRAPHVIZ_"+graphvizToolUpper, toolExePathName))
        return toolExePathName;

    if (umba::env::getVar("GRAPHVIZ_"+graphvizToolUpper, toolExePathName))
        return toolExePathName;

    return umba::filename::appendPath(findGraphvizBinPath(dontTouchSystem), graphvizTool+UMBA_FILESYS_EXE_EXT);
}

//----------------------------------------------------------------------------




