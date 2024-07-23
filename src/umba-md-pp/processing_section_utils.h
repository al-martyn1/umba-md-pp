/*! \file
    \brief Помогалки для processing.h
 */
#pragma once


//----------------------------------------------------------------------------
inline
bool splitHeaderLine(const std::string &line, std::string &levelStr, std::string &headerText)
{
    std::size_t hashPos = 0;
    while(hashPos!=line.size() && line[hashPos]!='#' )
    {
        ++hashPos;
    }

    if (hashPos==line.size())
        return false;

    std::size_t nextPos = hashPos;
    while(nextPos!=line.size() && line[nextPos]=='#' )
    {
        ++nextPos;
    }

    levelStr.assign(line, hashPos, nextPos-hashPos);

    while(nextPos!=line.size() && line[nextPos]==' ' )
    {
        ++nextPos;
    }

    headerText.assign(line, nextPos, line.npos);

    umba::string_plus::rtrim(headerText, [](char ch) { return ch=='#'; } );
    umba::string_plus::rtrim(headerText);

    umba::string_plus::rtrim(levelStr);

    return true;
}

//----------------------------------------------------------------------------
inline
bool isSectionNumberChar(char ch)
{
    if (ch>='0' && ch<='9')
        return true;

    if (ch=='.')
        return true;

    return false;
}

//----------------------------------------------------------------------------
inline
bool isSectionNumberStringHelper(const std::string &str)
{
    for(auto ch: str)
    {
        if (isSectionNumberChar(ch))
           continue;

        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
inline
bool isSectionNumber(const std::string &str, bool allowAppendixStyleNumbers=false)
{
    if (str.empty())
       return false;

    // Первый символ может быть латинской буквой
    if (allowAppendixStyleNumbers)
    {
        if (str[0]>='A' && str[0]<='Z' && isSectionNumberStringHelper(std::string(str, 1, std::string::npos)))
            return true;
    }

    if (isSectionNumberStringHelper(str))
        return true;

    return false;
}

//----------------------------------------------------------------------------
inline
bool isAppendixSectionNumber(const std::string &str)
{
    if (str.empty())
       return false;

    if (str[0]>='A' && str[0]<='Z')
        return true;

    return false;
}

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::string generateSectionIdImpl(const AppConfig<FilenameStringType> &appCfg, std::string secText)
{
    if (appCfg.targetRenderer==TargetRenderer::github)
    {
    }
    else if (appCfg.targetRenderer==TargetRenderer::doxygen)
    {
        // Для доксигена зачем-то делаю исключение и чекаю-удаляю номер раздела
        // auto spacePos = secText.find(' ');
        // if (spacePos!=secText.npos)
        // {
        //     if (isSectionNumber(std::string(secText, 0, spacePos)))
        //     {
        //         secText.erase(0, spacePos+1);
        //     }
        // }
    }
    else
    {
    }

    umba::string_plus::rtrim(secText);

    if (appCfg.targetRenderer==TargetRenderer::github)
    {
        return umba::generateIdFromText_forGitHub(secText, "user-content-");
    }
    else if (appCfg.targetRenderer==TargetRenderer::gitlab)
    {
        return umba::generateIdFromText_forGitHub(secText);
    }
    else if (appCfg.targetRenderer==TargetRenderer::doxygen)
    {
        auto res = umba::generateIdFromText_generic(secText, '-');
        umba::string_plus::trim(res, [](char ch) { return ch=='-'; } );
        return res;
    }
    else
    {
        return std::string();
    }
}

//----------------------------------------------------------------------------
template<typename FilenameStringType> inline
std::string generateSectionId(const AppConfig<FilenameStringType> &appCfg, const std::string secLine, std::size_t *pLevel=0, std::string *pHeaderText=0)
{
    if (pLevel)
    {
        *pLevel = 0;
    }

    std::string levelStr;
    std::string headerText;

    if (!splitHeaderLine(secLine, levelStr, headerText))
        return std::string();

    if (pHeaderText)
    {
        *pHeaderText = headerText;
    }

    if (headerText.empty())
        return std::string();

    // Доксигеновская тема, не завязываемся на неё
    // if (headerText.back()=='}') // already has id?
    //     return std::string();

    // if (appCfg.targetRenderer!=TargetRenderer::doxygen)
    //     return std::string(); // Нужно только для доксигена

    if (pLevel)
    {
        *pLevel = levelStr.size();
    }

    // Не делаем никаких исключений для

    // if (headerText.back()==']' && appCfg.targetRenderer==TargetRenderer::doxygen)
    // {
    //     // У нас есть идентификаторы в квадратных скобках, по ним мы генерим якоря (только doxygen)
    //     std::size_t idx = headerText.size();
    //     for(; idx!=0 && headerText[idx-1]!='['; --idx) {}
    //
    //     if (idx==0)
    //     {
    //         return generateSectionIdImpl(appCfg, headerText);
    //     }
    //
    //     std::string takenId = std::string(headerText, idx, headerText.size()-idx-1);
    //     return generateSectionIdImpl(appCfg, takenId);
    // }
    // else
    {
        return generateSectionIdImpl(appCfg, headerText);
    }

}

//----------------------------------------------------------------------------





