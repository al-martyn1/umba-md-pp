#pragma once

inline
const char* cppHelpersGetNumbersAlphabet( bool upper )
{
    static const char lowers[] = "0123456789abcdef";
    static const char uppers[] = "0123456789ABCDEF";

    return upper ? uppers : lowers;
}

inline
std::string cppHelpersFormatUnsigned( unsigned u, unsigned ss, bool upper, size_t width, char fillChar = 0 )
{
    if (fillChar==0)
    {
        if (ss==10)
           fillChar = ' ';
        else
           fillChar = '0';
    }

    std::string res;
    while( u )
    {
        unsigned d = u % ss;
        u /= ss;
        if (d>=16)
        {
            res.append(1, '#');
        }
        else
        {
            res.append(1, cppHelpersGetNumbersAlphabet(upper)[d] );
        }
    }

    if (res.empty())
        res = "0";

    if (res.size()<width)
    {
        res.append( width-res.size(), fillChar );
    }

    std::reverse( res.begin(), res.end() );

    return res;
}

inline
std::string cppHelpersFormatInt( int i )
{
    return i < 0
         ? std::string("-") + cppHelpersFormatUnsigned( (unsigned)-i, 10, false, 1, ' ' )
         :                    cppHelpersFormatUnsigned( (unsigned)i , 10, false, 1, ' ' )
         ;
}

inline
std::string makeCppName( const std::string &str )
{
    std::string res; res.reserve(str.size());

    std::string::size_type i = 0, size = str.size();
    for(; i!=size; ++i)
       {
        char ch = str[i];

        if ( (ch>='A' && ch<='Z') || (ch>='a' && ch<='z') )
           {
            res.append(1, ch );
           }
        else if ( ch>='0' && ch<='9' )
           {
            if (res.empty())
               res.append(1, '_');
            res.append(1, ch);
            continue;
           }
        else if ( ch=='+' )
           {
            res.append("_PLS_");
            continue;
           }
        else if ( ch=='~' )
           {
            res.append("_NOT_");
            continue;
           }
        else if ( ch=='-' )
           {
            res.append("_MNS_");
            continue;
           }
        else if ( ch=='*' )
           {
            res.append("_MUL_");
            continue;
           }
        else if ( ch=='\\' )
           {
            res.append("_BSL_");
            continue;
           }
        else if ( ch=='/' )
           {
            res.append("_SLH_");
            continue;
           }
        else
           {
            res.append(1, '_');
           }
        
       }

    return res;
}

inline
std::string cppConcat( const std::string &s1, const std::string &s2 )
{
    return s1 + std::string("_") + s2;
}

inline
std::string cppConcat( const std::string &s1, const std::string &s2, const std::string &s3 )
{
    return s1 + std::string("_") + s2 + std::string("_") + s3;
}

inline
std::string cppConcat( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4 )
{
    return s1 + std::string("_") + s2 + std::string("_") + s3 + std::string("_") + s4;
}

inline
std::string cppConcat( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4, const std::string &s5 )
{
    return s1 + std::string("_") + s2 + std::string("_") + s3 + std::string("_") + s4 + std::string("_") + s5;
}

