/*! \file
    \brief Разные утилитки
 */

#pragma once


#include "umba/filename.h"
#include "umba/filesys.h"
#include "umba/string_plus.h"
#include "umba/transliteration.h"
//
#include "tr/tr.h"
//
#include "log.h"


//
#include <string>
#include <algorithm>
#include <iterator>


//----------------------------------------------------------------------------
template<typename StringType> inline
StringType simpleHtmlEscape(const StringType &str)
{
    StringType res; res.reserve(str.size()+16);

    for(auto ch: str)
    {
        switch((char)ch)
        {
            case '<' : res.append(umba::string::make_string<StringType>("&lt;"  )); break;
            case '>' : res.append(umba::string::make_string<StringType>("&gt;"  )); break;
            case '&' : res.append(umba::string::make_string<StringType>("&amp;" )); break;
            case '\"': res.append(umba::string::make_string<StringType>("&quot;")); break;
            case '\'': res.append(umba::string::make_string<StringType>("&apos;")); break;
            default  : res.append(1, ch);
        }
    }

    return res;
}

//----------------------------------------------------------------------------
template<typename StringType> inline
std::vector<StringType> extractMetaLinesFromDocument(std::vector<StringType> &docLines, const StringType &linePrefix=StringType())
{
    using CharType = typename StringType::value_type;

    std::vector<StringType> resVec;

    if (docLines.empty())
        return resVec;

    typename std::vector<StringType>::const_iterator it = docLines.begin();

    const StringType metaSep = linePrefix + StringType(3,(CharType)'-');

    if (umba::string_plus::rtrim_copy(*it)!=metaSep)
        return resVec;

    auto rmStartIt = it;
    //auto itPrev = it;
    ++it;

    //StringType lastRtrimmedLine;

    for(; it!=docLines.end(); ++it /* , ++prevIt */ )
    {
        //lastRtrimmedLine = umba::string_plus::rtrim_copy(*it);
        StringType rtrimmedLine = umba::string_plus::rtrim_copy(*it);

        // Перво-наперво проверяем на metaSep
        if (rtrimmedLine==metaSep)
        {
            // нашли разделитель окончания метатегов
            auto copyStartIt = rmStartIt; copyStartIt++; // копируем без стартового metaSep
            std::copy(copyStartIt, it, std::back_inserter(resVec)); // текущую строку не копируем, поэтому используем it

            // При необходимости удаляем префикс
            if (!linePrefix.empty())
            {
                for(auto &s : resVec)
                {
                    umba::string_plus::rtrim(s);
                    s.erase(0,linePrefix.size());
                }
            }

            ++it; // удаляем, включая конечный metaSep.
            docLines.erase(rmStartIt, it);

            // Скопировали в вектор метастрок и удалили префикс, если было надо
            // Удалили в исходном списке строки метаданных, включая стартовый и завершающий metaSep
            // Можно возвращать результат

            return resVec;
        }

        // Тут надо проверить, строка должна начинаться с префикса, если он задан
        // Если префикс задан, но строка не начинается с него - что-то пошло не так
        if (!linePrefix.empty() && !umba::string_plus::starts_with(rtrimmedLine, linePrefix))
        {
            //return resVec; // Возвращаем пустой результат, ничего не удаляя в исходном векторе строк
            break;
        }

        // Тут тупо фигачим дальше в поисках конца метасекции или некондиционной строки, которая этот поиск прерывает
    }

    // Дошли до конца вектора, но окончания метасекции так и не нашли 
    return resVec; // Возвращаем пустой результат, ничего не удаляя в исходном векторе строк
}

//----------------------------------------------------------------------------
template<typename StringType> inline
std::vector<StringType> splitAndTrimAndSkipEmpty(const StringType &str, typename StringType::value_type ch)
{
    // на замену marty_cpp::splitToLinesSimple(paths, false, ',')

    // return marty_cpp::splitToLinesSimple(str, false, ch);

    using CharType = typename StringType::value_type;

    auto vec = marty_cpp::splitToLinesSimple(str, false, ch);

    for(auto &v : vec)
    {
        v = marty_cpp::simple_trim(v, [](CharType ch) { return ch==' '; });
    }

    std::vector<StringType> res; res.reserve(vec.size());
    for(auto && v : vec)
    {
        if (!v.empty())
            res.emplace_back(v);
    }

    return res;
}

//----------------------------------------------------------------------------
//! Простая функция разбора списка опций, parseSnippetTag слишком сложная, и заточена на конкретное применение
inline
std::vector<std::string> simpleParseOptions(std::string str)
{
    using ResType = std::vector<std::string>;

    str = marty_cpp::simple_trim(str, [](char ch) { return ch==' '; });

    if (str.size()<2)
        return ResType(); // нет обрамляющих скобок

    if (str.front()!='{' || str.back()!='}')
        return ResType(); // нет обрамляющих скобок

    str.erase(str.size()-1);
    str.erase(0, 1);

    return splitAndTrimAndSkipEmpty(str, ',');
}

//----------------------------------------------------------------------------
//! Версия simpleParseOptions с исключением повторений
inline
std::vector<std::string> simpleParseOptionsUnique(const std::string &str)
{
    auto valsVec = simpleParseOptions(str);
    return std::vector<std::string>(valsVec.begin(), std::unique(valsVec.begin(), valsVec.end()));
}

//----------------------------------------------------------------------------
template<typename StringType> inline
StringType generateOutputFileNameFromInputFileName(StringType name)
{
    auto pathFile = umba::filename::getPathFile(name);

    auto ext      = umba::filename::getFileExtention(name);

    ext = umba::string_plus::trim( ext
                                 , [](auto ch)
                                   {
                                       return ch=='_';
                                   }
                                 );
    return umba::filename::appendExt(pathFile, ext);
}

//----------------------------------------------------------------------------
template<typename StringType> inline
StringType tryMakeOutputFilenameFromInput(StringType name)
{
    name = umba::filename::makeCanonical(name);

    auto path           = umba::filename::getPath(name);
    auto fileNameNoPath = umba::filename::getFileName(name);

    if (umba::string_plus::starts_with_and_strip(fileNameNoPath, umba::string_plus::make_string<StringType>(".")))
    {
        // Если имя файла начинается с точки - убрали точку и зашибись
        return umba::filename::appendPath(path, fileNameNoPath);
    }

    auto pathFile = umba::filename::getPathFile(name);
    auto ext      = umba::filename::getFileExtention(name);
    if (umba::string_plus::starts_with_and_strip(ext, umba::string_plus::make_string<StringType>("_")))
    {
        // Если расширение файла начинается с подчеркивания - убрали его и зашибись
        return umba::filename::appendExtention(pathFile, ext);
    }

    if (umba::string_plus::ends_with_and_strip(ext, umba::string_plus::make_string<StringType>("_")))
    {
        // Если расширение файла заканчивается с подчеркиванием - убрали его и зашибись
        return umba::filename::appendExtention(pathFile, ext);
    }

    return StringType();

}

//----------------------------------------------------------------------------
/*
    1) Ищем файл
       $(MDFILE).md-pp.options
       $(MDFILE).umba-md-pp.options
       Один из этих, или все?

    2) Для вьювера ищем также
       $(MDFILE).md-pp-view.options
       $(MDFILE).umba-md-pp-view.options
       Причем для вьювера ищем раньше (а обрабатываться будут в обратной последовательности позже)
       Один из этих, или все?

    3) Также ищем файл
       $(MDFILE).md-pp-$(MdPpRenderingTargetName).options
       $(MDFILE).umba-md-pp-$(MdPpRenderingTargetName).options
       Его ищем раньше всех (обрабатывается позже всех)

       Или даже так - для вьювера RenderingTargetName выставляем как view, и тогда пп 2 и 3 прекрасно совмещаются в один

    4) Теперь отрезаем от имени файла путь, и для каждого каталога в иерархии ищем файлы
       .md-pp-$(MdPpRenderingTargetName).options
       .umba-md-pp-$(MdPpRenderingTargetName).options
       .md-pp.options
       .umba-md-pp.options
       Ищем все эти файлы, не останавливаясь, если какой-то найден

       Также ищем файлы
       .md-pp.base.options
       .umba-md-pp.base.options

       При нахождении base-файла поиск файлов опций останавливается

*/

inline
void findProjectOptionsFiles(const std::string &mdFile, std::string renderingTargetName, std::vector<std::string> &foundOptionsFiles)
{
    LOG_INFO("opt-files") << "-----------------------------------------" << "\n";
    LOG_INFO("opt-files") << "Lookup for options files\n";
    LOG_INFO("opt-files") << "Rendering target (--rendering-target-name): '" << renderingTargetName << "'\n";
    LOG_INFO("opt-files") << "-------" << "\n";

    std::string optPath = umba::filename::getPath(mdFile);

    auto getPathLen = [](const std::string &p)
    {
        std::vector<std::string> parts = umba::filename::splitPath(p);
        return parts.size();
    };

    // std::vector<std::string> parts = umba::filename::splitPath(optPath);
    //  
    // std::size_t numCheckLevels = 0;
    // {
    //     if (parts.size()>2)
    //         numCheckLevels = parts.size() - 2;
    //  
    //     if (!numCheckLevels)
    //         numCheckLevels = 1; // Хотя бы текущий уровень надо проверить
    //  
    //     LOG_INFO("opt-files") << "Path parts:" << "\n";
    //     for(auto p: parts)
    //     {
    //         LOG_INFO("opt-files") << "  " << p << "\n";
    //     }
    //  
    //     
    //     LOG_INFO("opt-files") << "numCheckLevelsnumCheckLevels: " << numCheckLevels << "\n";
    //      
    //     LOG_INFO("opt-files") << "-------" << "\n";
    //  
    // }

    if (renderingTargetName.empty())
       renderingTargetName = "default";

    std::vector<std::string> extList;
    extList.emplace_back(".umba-md-pp-" + renderingTargetName + ".options");
    extList.emplace_back(".md-pp-"      + renderingTargetName + ".options");
    extList.emplace_back(".umba-md-pp.options");
    extList.emplace_back(".md-pp.options");

    std::vector<std::string> stopList;
    stopList.emplace_back(".umba-md-pp.base.options");
    stopList.emplace_back(".md-pp.base.options");

    // std::vector<std::string> checkNamesList;
    // std::vector<std::string> stopNamesList;

    std::vector<std::string> inputNames;
    inputNames.emplace_back(mdFile);
    {
        std::string newExt = umba::filename::getExt(mdFile);
        umba::string_plus::trim(newExt, umba::string_plus::is_one_of<char>("_"));
        std::string name2 = umba::filename::appendExt(umba::filename::getPathFile(mdFile), newExt);
        if (name2!=mdFile)
            inputNames.emplace_back(name2);
    }

    foundOptionsFiles.clear();

    // Проверяем опции для конкретного файла
    for(const auto &ext : extList)
    {
        for(const auto &inputName : inputNames)
        {
            std::string testName = umba::filename::appendExt(inputName, ext);
            if (umba::filesys::isFileReadable(testName))
            {
                foundOptionsFiles.emplace_back(testName);
                LOG_INFO("opt-files") << "+ Lookup for: '" << testName << "' - Found\n";
            }
            else
            {
                LOG_INFO("opt-files") << "- Lookup for: '" << testName << "' - Not found\n";
            }
        }
    }

    // https://ru.wikipedia.org/wiki/%D0%94%D0%BE%D0%BC%D0%B0%D1%88%D0%BD%D0%B8%D0%B9_%D0%BA%D0%B0%D1%82%D0%B0%D0%BB%D0%BE%D0%B3
    // https://en.wikipedia.org/wiki/Home_directory
    // /usr/home/<username>
    // /user/home/<username>
    // /home/<username>
    // /var/users/<username>
    // /Users/<username> - Mac

    // По умолчанию - лимит два уровня до корня
    // Обрабатываемый файл лежит в D:\work\project - тут "D:", "work" и "project" - последняя проверенная папка будет D:\work
    // Обрабатываемый файл лежит в D:\project      - тут "D:" и "project" - последняя проверенная папка будет D:\project
    // Лимит - 2

    // !!! надо обдумать для других возможных путей

    // Проект лежит в сети - надо проверить префикс, сетевой ли. В сетевом пути на первом месте - хост - он как диск в винде, так что тоже - лимит - 2

    // Win32
    // Обрабатываемый файл лежит в C:\Users\<user>
    // В реале файл должен лежать в C:\Users\<user>\Local Bla-bla\<Documents>\Something

    // Linux/Unix
    // /usr/home/<username>
    // /user/home/<username>
    // /home/<username>
    // /var/users/<username>

    // Mac
    // /Users/<username> - Mac

    // Проверяем опции по каталогам

    bool bStop = false;
    //for(std::size_t i=0; !bStop && i!=numCheckLevels; ++i)
    while(!bStop)
    {
        if (getPathLen(optPath)<2)
        {
            bStop = true;
            continue;
        }

        //LOG_INFO("opt-files") << "i: " << i << "\n";

        for(const auto &ext : extList)
        {
            std::string testName = umba::filename::appendPath(optPath, ext);
            if (umba::filesys::isFileReadable(testName))
            {
                foundOptionsFiles.emplace_back(testName);
                LOG_INFO("opt-files") << "+ Lookup for: '" << testName << "' - Found\n";
            }
            else
            {
                LOG_INFO("opt-files") << "- Lookup for: '" << testName << "' - Not found\n";
            }
        }

        for(const auto &stopExt : stopList)
        {
            std::string testName = umba::filename::appendPath(optPath, stopExt);
            if (umba::filesys::isFileReadable(testName))
            {
                foundOptionsFiles.emplace_back(testName);
                bStop = true;
                LOG_INFO("opt-files") << "+ Lookup for (stop file): '" << testName << "' - Found\n";
            }
            else
            {
                LOG_INFO("opt-files") << "- Lookup for (stop file): '" << testName << "' - Not found\n";
            }
        }

        umba::filename::stripLastPathSep(optPath);
        std::string optPathNext = umba::filename::getPath(optPath);
        if (optPathNext.empty() || optPathNext==optPath)
        {
            bStop = true;
        }

        optPath = optPathNext;
    }

    std::reverse(foundOptionsFiles.begin(), foundOptionsFiles.end()); // Обратный порядок обработки - самые последние найденные файлы обрабатываются первыми

    LOG_INFO("opt-files") << "-------" << "\n";
    if (foundOptionsFiles.empty())
    {
        LOG_INFO("opt-files") << "No option files found\n";
    }
    else if (foundOptionsFiles.size()==1)
    {
        LOG_INFO("opt-files") << "Found only one option file: '" << foundOptionsFiles[0] << "'\n";
    }
    else
    {
        LOG_INFO("opt-files") << "Option files apply order:" << "\n";
        for(const auto &fname: foundOptionsFiles)
        {
            LOG_INFO("opt-files") << "  " << fname << "\n";
        }
    }

    
    LOG_INFO("opt-files") << "-----------------------------------------" << "\n";

}

template<typename AppConfigType, typename ArgParserType> inline
AppConfigType applyProjectOptionsFiles(AppConfigType &appCfg, ArgParserType &argsParser, const std::vector<std::string> &projectOptionsFiles)
{
    AppConfigType appCfgRes = appCfg;

    if (!projectOptionsFiles.empty())
    {
        appCfg.setStrictPathFromFilename(projectOptionsFiles[0]); // рестрикции задаем по самому первому (верхнему в файловой иерархии) файлу
        for(const auto& projectOptionsFile: projectOptionsFiles)
        {
            appCfg.pushSamplesPaths();
            argsParser.pushOptionsFileName(projectOptionsFile);
            LOG_INFO("config") << "-----------------------------------------" << "\n";
            LOG_INFO("config") << "Processing options file: '" << projectOptionsFile << "'\n";
            argsParser.parseOptionsFile(projectOptionsFile);
            LOG_INFO("config") << "-----------------------------------------" << "\n";
            argsParser.popOptionsFileName();
            appCfg.popSamplesPathsAndInsertNewAtFront();
        }
    }

    // Обновлялся appCfg, appCfgRes - предыдущее состояние до обновления
    std::swap(appCfgRes, appCfg);
    // Обменяли, теперь appCfg - исходный, appCfgRes - новый
    return appCfgRes;

}

#if 0
//----------------------------------------------------------------------------
inline
bool findProjectOptionsFile(const std::string &mdFile, std::string &foundOptionsFile)
{
    return umba::cli_tool_helpers::findProjectOptionsFile(mdFile, foundOptionsFile, std::vector<std::string>{".md-pp.options", ".umba-md-pp.options"});
}

//----------------------------------------------------------------------------
inline
bool findProjectOptionsFile(const std::wstring &mdFile, std::wstring &foundOptionsFile)
{
    return umba::cli_tool_helpers::findProjectOptionsFile(mdFile, foundOptionsFile, std::vector<std::wstring>{L".md-pp.options", L".umba-md-pp.options"});
}

//----------------------------------------------------------------------------
#endif


//----------------------------------------------------------------------------
inline
std::string findGoxygenLanguageByLangTag(std::string langTag)
{
    auto trNullErrHandler = marty_tr::makeErrReportHandler( [](marty_tr::MsgNotFound what, const std::string& msg, const std::string& catId, const std::string& langId)
                                                              {
                                                              }
                                                          );

    auto autoRestoreTrErrHandler = mtr::AutoRestoreErrReportHandler(mtr::tr_set_err_handler(&trNullErrHandler));
    auto autoEmptyMsgNotExist    = mtr::AutoEmptyMsgNotExist(mtr::tr_set_empty_msg_not_exist(true));

    if (mtr::tr_has_msg(langTag, "doxygen-language", "en"))
    {
        return mtr::tr(langTag, "doxygen-language", "en");
    }

    return "English";

}

//----------------------------------------------------------------------------
inline
std::string findLangTagByString(std::string strLang)
{

    auto trNullErrHandler = marty_tr::makeErrReportHandler( [](marty_tr::MsgNotFound what, const std::string& msg, const std::string& catId, const std::string& langId)
                                                              {
                                                              }
                                                          );

    auto autoRestoreTrErrHandler = mtr::AutoRestoreErrReportHandler(mtr::tr_set_err_handler(&trNullErrHandler));
    auto autoEmptyMsgNotExist    = mtr::AutoEmptyMsgNotExist(mtr::tr_set_empty_msg_not_exist(true));

    umba::string_plus::trim(strLang);

    std::string langTag = mtr::getLocaleLanguageTag(strLang);

    if (!langTag.empty())
    {
        return langTag;
    }

    std::string langNaturalName    ;
    std::string locationNaturalName;
    umba::string_plus::split_to_pair(strLang, langNaturalName, locationNaturalName, '/');
    umba::string_plus::trim(langNaturalName    );
    umba::string_plus::trim(locationNaturalName);

    langNaturalName      = umba::transliterate(langNaturalName);
    langNaturalName      = marty_cpp::toLower (langNaturalName);

    locationNaturalName  = umba::transliterate(locationNaturalName);
    locationNaturalName  = marty_cpp::toLower (locationNaturalName);


    std::string foundCanonicalLangName    ;
    std::string foundCanonicalLocationName;


    // natural-language-to-canonical
    // natural-location-to-canonical
    // language-location-to-lang-tag


    if (!mtr::tr_has_msg(langNaturalName, "natural-language-to-canonical", "en"))
    {
        return std::string();
    }

    foundCanonicalLangName = mtr::tr(langNaturalName, "natural-language-to-canonical", "en");


    if (mtr::tr_has_msg(locationNaturalName, "natural-location-to-canonical", "en"))
    {
        foundCanonicalLocationName = mtr::tr(locationNaturalName, "natural-location-to-canonical", "en");
    }

    std::string canonicalLangLocation = foundCanonicalLangName;
    if (!foundCanonicalLocationName.empty())
    {
        canonicalLangLocation.append(1,'/');
        canonicalLangLocation.append(foundCanonicalLocationName);
    }

    if (mtr::tr_has_msg(canonicalLangLocation, "language-location-to-lang-tag", "en"))
    {
        return mtr::tr(canonicalLangLocation, "language-location-to-lang-tag", "en");
    }

    if (mtr::tr_has_msg(foundCanonicalLangName, "language-location-to-lang-tag", "en"))
    {
        return mtr::tr(foundCanonicalLangName, "language-location-to-lang-tag", "en");
    }

    return std::string();

#if 0

    // Как найти каноничное название языка?

    /*
      Для каждого известного языка zz-ZZ:

          1) Получить список переводов названий языка в категории "marty-tr/language-name" (при langTag zz-ZZ)
             пройтись по списку. Если перевод совпадает (а лучше транлитерация и в нижнем регистре), то мы нашли каноническое название языка (на английском)
          2) Получить список переводов названий стран в категории "marty-tr/language-location" (при langTag zz-ZZ)
             пройтись по списку. Если перевод совпадает (а лучше транлитерация и в нижнем регистре), то мы нашли каноническое название страны (на английском)
          3) Пройтись по сету идентификаторов языков, где язык и страна совпадут, то такой и LangTag.
             Если не нашли, то проходимся также, но без сравнения без страны
    */

    static const std::string langNameCatId = "marty-tr/language-name";
    static const std::string langLocCatId  = "marty-tr/language-location";

    std::string langNaturalName    ;
    std::string locationNaturalName;
    umba::string_plus::split_to_pair(strLang, langNaturalName, locationNaturalName, '/');
    umba::string_plus::trim(langNaturalName    );
    umba::string_plus::trim(locationNaturalName);

    std::string langNaturalNameForCmp     = langNaturalName    ;
    std::string locationNaturalNameForCmp = locationNaturalName;

    if (useTransiteration)
    {
        langNaturalNameForCmp      = umba::transliterate(langNaturalNameForCmp);
        langNaturalNameForCmp      = marty_cpp::toLower (langNaturalNameForCmp);

        locationNaturalNameForCmp  = umba::transliterate(locationNaturalNameForCmp);
        locationNaturalNameForCmp  = marty_cpp::toLower (locationNaturalNameForCmp);
    }


    std::string foundCanonicalLangNaturalName    ;
    std::string foundCanonicalLocationNaturalName;


    std::map<std::string, mtr::StringLocaleInfo> locales = mtr::getStringLocaleInfoMap();

    std::map<std::string, mtr::StringLocaleInfo>::const_iterator lcIt = locales.begin();
    for(; lcIt!=locales.end(); ++lcIt)
    {
        // if (!foundCanonicalLangNaturalName.empty() && !foundCanonicalLocationNaturalName.empty())
        //     break;

        const std::string &langTag = lcIt->first;

        if (foundCanonicalLangNaturalName.empty())
        {
            std::vector<std::string> canonicalLangNameMsgids = mtr::tr_get_msgids(langNameCatId, langTag);

            for(const auto &canonicalLangNameMsgid: canonicalLangNameMsgids)
            {
                if (!mtr::tr_has_msg(canonicalLangNameMsgid, langNameCatId, langTag))
                {
                    continue;
                }

                auto valForCompare = mtr::tr(canonicalLangNameMsgid, langNameCatId, langTag);
                if (useTransiteration)
                {
                    valForCompare  = umba::transliterate(valForCompare);
                    valForCompare  = marty_cpp::toLower (valForCompare);
                }

                if (langNaturalNameForCmp==valForCompare)
                {
                    foundCanonicalLangNaturalName = canonicalLangNameMsgid;
                    break;
                }
            }
        } // if (foundCanonicalLangNaturalName.empty())


        if (foundCanonicalLocationNaturalName.empty())
        {
            std::vector<std::string> canonicalLocationNameMsgids = mtr::tr_get_msgids(langLocCatId, langTag);

            for(const auto &canonicalLocationNameMsgid: canonicalLocationNameMsgids)
            {
                if (canonicalLocationNameMsgid=="-" || canonicalLocationNameMsgid=="_")
                {
                    continue;
                }

                if (!mtr::tr_has_msg(canonicalLocationNameMsgid, langLocCatId, langTag))
                {
                    continue;
                }

                auto valForCompare = mtr::tr(canonicalLocationNameMsgid, langLocCatId, langTag);
                if (useTransiteration)
                {
                    valForCompare  = umba::transliterate(valForCompare);
                    valForCompare  = marty_cpp::toLower (valForCompare);
                }

                if (locationNaturalNameForCmp==valForCompare)
                {
                    foundCanonicalLocationNaturalName = canonicalLocationNameMsgid;
                    break;
                }
            }
        } // if (foundCanonicalLocationNaturalName.empty())

        //tr_enumerate_msgids(THandler handler, std::string catId, std::string langId)
    }

    lcIt = locales.begin();
    for(; lcIt!=locales.end(); ++lcIt)
    {
        const std::string &langTag  = lcIt->first;
        const std::string &lang     = lcIt->second.lang    ;
        const std::string &location = lcIt->second.location;

        if (foundCanonicalLangNaturalName==lang && foundCanonicalLocationNaturalName==location && !location.empty())
        {
            return langTag;
        }
    }


    lcIt = locales.begin();
    for(; lcIt!=locales.end(); ++lcIt)
    {
        const std::string &langTag  = lcIt->first;
        const std::string &lang     = lcIt->second.lang    ;
        const std::string &location = lcIt->second.location;

        if (foundCanonicalLangNaturalName==lang)
        {
            return langTag;
        }
    }


    return std::string();
#endif

}




