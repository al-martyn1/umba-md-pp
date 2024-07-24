#pragma once


#if defined(WIN32) || defined(_WIN32)
    #include "umba/win32_utils.h"
    #include <shellapi.h>
#endif

#include "umba/string_plus.h"

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




