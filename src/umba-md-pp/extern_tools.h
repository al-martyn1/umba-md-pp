#pragma once


#include "umba/string_plus.h"
#include "umba/utf8.h"
#include "umba/shellapi.h"

// For 'system' function
#include <process.h>
#include <cerrno>


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
//! Создаём консоль для гуишного окна. Если окно не гуишное, то консоль у него уже есть
/*! Это нужно для того, чтобы при вызове внешних программ каждый раз не создавалась консоль - на это
    это может аффектится антивирус.
*/
#if 0
inline
void allocConsole()
{
#if defined(WIN32) && defined(_WIN32)

    AllocConsole();

#else

#endif
}
#endif

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
#if 0
template<typename StringType>
StringType escapeCommandLineArgument(const StringType &str)
{
    // http://learn.microsoft.com/en-us/cpp/c-language/parsing-c-command-line-arguments?view=msvc-170

    bool needEscape = false;
    if (str.find_first_of(umba::string_plus::make_string<StringType>(" \""))!=str.npos)
       needEscape = true;

    if (!needEscape)
        return str;

    using CharType = typename StringType::value_type;

    StringType res; res.reserve(str.size());
    res.append(1, (CharType)'\"');
    for(auto ch : str)
    {
        if (ch==(CharType)'\"')
            res.append(2, (CharType)'\"');
        else
            res.append(1, ch);
    }
    res.append(1, (CharType)'\"');

    return res;
}
#endif
//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
#if 0
template<typename StringType>
StringType makeSystemFunctionCommandString(const StringType &cmd, std::vector<StringType> cmdArgs)
{
    cmdArgs.insert(cmdArgs.begin(), cmd);
    for(auto &cmdArg : cmdArgs)
    {
        cmdArg = escapeCommandLineArgument(cmdArg);
    }

    using CharType = typename StringType::value_type;

    return umba::string_plus::merge<std::string, typename std::vector<StringType>::const_iterator>( cmdArgs.begin(), cmdArgs.end(), (CharType)' '/*, [](auto s) { return s; }*/ );
}
#endif
//----------------------------------------------------------------------------
//! return -1 if error launching extern programm, 0 or >0 - result code of executed comman
#if 0
inline
int safeSystemFunction(std::string *pErrMsg, const std::string &cmd)
{
    // system returns the value that is returned by the command interpreter.
    // It returns the value 0 only if the command interpreter returns the value 0.
    // A return value of -1 indicates an error, and errno is set

    allocConsole();

    int resVal = system(cmd.c_str());
    if (resVal!=0)
    {
        if (pErrMsg)
        {
            if (resVal==-1)
               *pErrMsg = std::string("Launch command failed: ") + std::strerror(errno);
            else
               *pErrMsg = "Command result code: " + std::to_string(resVal);
        }
    }

    return resVal;
}
#endif
//----------------------------------------------------------------------------
#if 0
inline
int safeSystemFunction(std::wstring *pErrMsg, const std::wstring &cmd)
{
    allocConsole();

#if defined(WIN32) && defined(_WIN32)

    int resVal = _wsystem(cmd.c_str());
    if (resVal!=0)
    {
        if (pErrMsg)
        {
            if (resVal==1)
               *pErrMsg = umba::fromUtf8(std::string("Launch command failed: ") + std::strerror(errno));
            else
               *pErrMsg = umba::fromUtf8("Command result code: " + std::to_string(resVal));
        }
    }

    return resVal;

#else

    std::string errMsg;
    int res = safeSystemFunction(&errMsg, umba::toUtf8(cmd))
    if (!errMsg.empty() && pErrMsg)
    {
        *pErrMsg = umba::fromUtf8(errMsg);
    }

    return res;

#endif

}
#endif

//----------------------------------------------------------------------------
#if 0
template<typename StringType>
int safeSystemFunction(StringType *pErrMsg, const StringType &cmd, const std::vector<StringType> &cmdArgs)
{
    return safeSystemFunction(pErrMsg, makeSystemFunctionCommandString(cmd, cmdArgs));
}
#endif

// std::strerror(errno)

//umba::toUtf8(resFilename)


//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename StringType>
StringType findDoxygenBinPathHelper(bool dontTouchSystem)
{
    if (dontTouchSystem)
        return StringType();

#if defined(WIN32) && defined(_WIN32)

    StringType p;
    if (!umba::win32_utils::regQueryAppInstallLocationBin(umba::string_plus::make_string<StringType>("doxygen_is1"), p))
        return StringType();

    return p;

#else

    return StringType();

#endif
}

//----------------------------------------------------------------------------
template<typename StringType>
const StringType& findDoxygenBinPath(bool dontTouchSystem)
{
    static StringType p = findDoxygenBinPathHelper<StringType>(dontTouchSystem);
    return p;
}

//----------------------------------------------------------------------------
template<typename StringType>
StringType findDoxygenExecutableName(bool dontTouchSystem)
{
#if defined(WIN32) && defined(_WIN32)
    return umba::filename::appendPath(findDoxygenBinPath<StringType>(dontTouchSystem), umba::string_plus::make_string<StringType>("doxygen.exe"));
#else
    return umba::string_plus::make_string<StringType>("doxygen");
#endif
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename StringType>
StringType findGraphvizBinPathHelper(bool dontTouchSystem)
{
    if (dontTouchSystem)
        return StringType();

#if defined(WIN32) && defined(_WIN32)

    StringType p;
    if (!umba::win32_utils::regQueryAppInstallLocationBin(umba::string_plus::make_string<StringType>("Graphviz"), p))
        return StringType();

    return p;

#else

    return StringType();

#endif
}

//----------------------------------------------------------------------------
template<typename StringType>
const StringType& findGraphvizBinPath(bool dontTouchSystem)
{
    static StringType p = findGraphvizBinPathHelper<StringType>(dontTouchSystem);
    return p;
}

//----------------------------------------------------------------------------
template<typename StringType>
StringType findGraphvizToolExecutableName(bool dontTouchSystem, const StringType &tool)
{
#if defined(WIN32) && defined(_WIN32)
    return umba::filename::appendPath(findGraphvizBinPath<StringType>(dontTouchSystem), tool+umba::string_plus::make_string<StringType>(".exe"));
#else
    return tool;
#endif
}

//----------------------------------------------------------------------------




