//----------------------------------------------------------------------------
inline
void simpleDoNothing( int v1)
{
    // Первая версия, до isUrlAbsolute
}

//----------------------------------------------------------------------------
template<typename StringType>
bool isUrlAbsolute(const StringType &strUrl)
{
    using CharType = typename StringType::value_type;

    // looking for scheme: "protocol://..."
    // "://..." - is invalid, missing protocol


    auto colonPos = strUrl.find((CharType)':');

    if (colonPos==strUrl.npos)
        return false;

    if (colonPos==0)
        return false;

    if ((colonPos+1u)>=strUrl.size() || (colonPos+2u)>=strUrl.size())
        return false;

    if (strUrl[colonPos+1u]==(CharType)'/' && strUrl[colonPos+2u]==(CharType)'/')
        return true;

    return false;
}

//----------------------------------------------------------------------------
inline
void simpleDoNothing( char v2)
{
    // Вторая версия, после isUrlAbsolute
}

//----------------------------------------------------------------------------
inline void simpleNothing2( char v3)
{
    // Третья версия, сигнатура в одну строку
}

