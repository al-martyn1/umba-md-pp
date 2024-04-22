
#include "enums.h"
#include "marty_simplesquirrel/enums.h"
//
#include <algorithm>
#include <string>
#include <utility>

//----------------------------------------------------------------------------
inline
std::string autoConvertToUtf8(std::string str)
{
    encoding::EncodingsApi* pEncApi = encoding::getEncodingsApi();
    
    std::size_t bomSize = 0;
    std::string detectRes = pEncApi->detect( str, bomSize );
    
    if (bomSize)
    {
       str.erase(0,bomSize);
    }
    
    auto cpId = pEncApi->getCodePageByName(detectRes);
    
    return pEncApi->convert( str, cpId, encoding::EncodingsApi::cpid_UTF8 );
}

//----------------------------------------------------------------------------
inline
std::string replaceFileExt(std::string file, std::string ext)
{
    std::string pathFile = umba::filename::getPathFile(file);
    return umba::filename::appendExt(pathFile, ext);
}

//----------------------------------------------------------------------------
inline
bool readUpdateTranslations(std::string fname, bool allowNoTranslations = true)
{
    if (fname.empty())
    {
        if (allowNoTranslations)
        {
            return true;
        }

        LOG_ERR_OPT<<"Translations file name not taken (--translations)"<< "\n";
        return false;
    }

    std::string jsonTrData;
    if (umba::filesys::readFile(fname, jsonTrData))
    {
        try
        {
            std::string jsonTrDataUtf8 = autoConvertToUtf8(jsonTrData);
            auto m = marty_tr::tr_parse_translations_data(jsonTrDataUtf8);
            marty_tr::tr_add_custom_translations(m);
        }
        catch(const std::exception &e)
        {
            LOG_ERR_OPT<<fname<<": "<<e.what()<< "\n";
            return false;
        }
        catch(...)
        {
            LOG_ERR_OPT<<fname<<": "<<"Unknown error"<< "\n";
            return false;
        }
    }
    else
    {
        if (!allowNoTranslations)
        {
            LOG_ERR_OPT<<appConfig.translationsFile<<": "<<"Failed to read file"<< "\n";
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
inline
bool extractCodeTag(std::string &line, const std::string &tagPrefix)
{
    auto lineCopy = line;
    umba::string_plus::trim(lineCopy);

    if (!umba::string_plus::starts_with_and_strip(lineCopy, tagPrefix))
        return false;

    umba::string_plus::trim(lineCopy);

    line = lineCopy;

    return true;
}

//----------------------------------------------------------------------------
inline
bool isCodeTagLine(std::string line, const std::string &tagPrefix)
{
    umba::string_plus::trim(line);

    return umba::string_plus::starts_with(line, tagPrefix);
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> processListingNestedTagsLines( const std::vector<std::string>               &lines
                                                      , const std::string                            &tagPrefix
                                                      , marty_simplesquirrel::ListingNestedTagsMode  listingNestedTagsMode
                                                      )
{
    std::vector<std::string> resLines; resLines.reserve(lines.size());

    for(auto l: lines)
    {
        if (isCodeTagLine(l, tagPrefix))
        {
            switch(listingNestedTagsMode)
            {
                case marty_simplesquirrel::ListingNestedTagsMode::keep  :
                     resLines.emplace_back(l);
                     break;

                case marty_simplesquirrel::ListingNestedTagsMode::remove:
                     break;

                case marty_simplesquirrel::ListingNestedTagsMode::empty :
                     resLines.emplace_back(std::string());
                     break;

                case marty_simplesquirrel::ListingNestedTagsMode::invalid :
                     break;
            }

            continue;
        }

        resLines.emplace_back(l);
    }

    return resLines;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> trimLeadingSpaces(std::vector<std::string> lines, bool bTrim)
{
    std::size_t minLeadingSpaces = 4096u;
    for(auto &l : lines)
    {
        umba::string_plus::rtrim(l); // убираем конечные пробелы
        if (l.empty())
        {
            continue; // на пустых строках нет смысла париться
        }

        std::size_t i = 0u;
        for(; i!=l.size(); ++i)
        {
            if (l[i]!=' ')
                break;
        }

        // либо брякнулись, либо дошли до конца

        minLeadingSpaces = std::min(minLeadingSpaces, i);
    }

    if (!bTrim)
    {
        return lines;
    }

    if (!minLeadingSpaces)
    {
        return lines;
    }

    for(auto &l : lines)
    {
        if (l.empty())
        {
            continue; // на пустых строках нет смысла париться
        }

        if (l.size()<minLeadingSpaces)
        {
            continue;
        }

        l.erase(0, minLeadingSpaces);
    }

    return lines;
}

//----------------------------------------------------------------------------
inline
bool isCodeTagLinesRange(const std::string &fragmentTag, std::size_t &startLineNo, std::size_t &endLineNo /* last line number +1 */ )
{
    std::size_t dCnt = 0; // считаем дефисы

    for(auto ch: fragmentTag)
    {
        if (ch=='-')
        {
            ++dCnt;
            continue;
        }

        if (ch<'0' || ch>'9')
            return false; // какой-то левый символ
    }

    if (dCnt!=1)
    {
        return false; // неверное количество дефисов
    }

    auto numbersVec = marty_cpp::simple_string_split(fragmentTag, "-"); // делим по дефису

    if (numbersVec.size()!=2)
    {
        return false; // что-то пошло не так
    }

    std::size_t lFirst = 0;
    std::size_t lLast  = 0;

    try
    {
        lFirst = std::stoul(numbersVec[0]);
        lLast  = std::stoul(numbersVec[1]);
    }
    catch(...)
    {
        return false; // что-то пошло не так
    }

    if (lFirst>lLast)
    {
        std::swap(lFirst,lLast);
    }

    if (lFirst==0 || lLast==0)
    {
        return false; // нумерация строк человеческая - с единицы, если нет, то ошибка
    }

    // Уменьшаем до машинной нумерации
    --lFirst;
    --lLast;

    startLineNo = lFirst;
    endLineNo   = lLast+1;

    return true;
}
//----------------------------------------------------------------------------
inline
std::string extractCodeFragment( const std::string                            &lfNormalizedText
                               , const std::string                            &fragmentTag
                               , const std::string                            &tagPrefix
                               , marty_simplesquirrel::ListingNestedTagsMode  listingNestedTagsMode
                               , std::size_t                                  tabSize=4u
                               , bool                                         trimLeadingSpaces_a = true
                               )
{
    std::vector<std::string> lines = marty_cpp::splitToLinesSimple(lfNormalizedText, false /* addEmptyLineAfterLastLf */  /* , lfChar='\n' */ );
    //lines = 
    marty_cpp::expandTabsToSpaces(lines, tabSize);

    if (fragmentTag.empty())
    {
        lines = processListingNestedTagsLines(lines, tagPrefix, listingNestedTagsMode);
        return marty_cpp::mergeLines(trimLeadingSpaces(lines, trimLeadingSpaces_a), marty_cpp::ELinefeedType::lf);
    }

    std::vector<std::string> fragmentLines; fragmentLines.reserve(lines.size());
    if (lines.empty())
    {
        return marty_cpp::mergeLines(fragmentLines, marty_cpp::ELinefeedType::lf);
    }


    std::size_t startLineNo = 0;
    std::size_t endLineNo   = 0;
    if (isCodeTagLinesRange(fragmentTag, startLineNo, endLineNo))
    {
        // стартовая строка должна быть в пределах размера вектора строк
        // пустой вектор строк отброшен ранее, тут уже не паримся, за ноль не перескочим
        startLineNo = std::min(startLineNo, lines.size()-1);
        // Конечный номер строки (+1 за последней нужной) может выходить за пределы размеров вектора исходных сток
        endLineNo   = std::min(endLineNo  , lines.size());

        if (startLineNo>endLineNo)
        {
            std::swap(startLineNo,endLineNo);
        }

        for(auto lNo=startLineNo; lNo!=endLineNo; ++lNo)
        {
            fragmentLines.emplace_back(lines[lNo]);
        }

        return marty_cpp::mergeLines(trimLeadingSpaces(fragmentLines, trimLeadingSpaces_a), marty_cpp::ELinefeedType::lf);
    }

    //bool startFound = false;
    int tagLevel    = 0;

    auto addTagLine = [&](const std::string &line)
    {
        switch(listingNestedTagsMode)
        {
            case marty_simplesquirrel::ListingNestedTagsMode::keep  :
                 fragmentLines.emplace_back(line);
                 break;
         
            case marty_simplesquirrel::ListingNestedTagsMode::remove:
                 break;
         
            case marty_simplesquirrel::ListingNestedTagsMode::empty :
                 fragmentLines.emplace_back(std::string());
                 break;

            case marty_simplesquirrel::ListingNestedTagsMode::invalid :
                 break;
        }
    };


    for(auto l: lines)
    {
        if (!isCodeTagLine(l, tagPrefix))
        {
            //if (startFound)
            if (tagLevel!=0)
            {
                fragmentLines.emplace_back(l);
            }

            continue;
        }

        
        auto tag = l;
        extractCodeTag(tag, tagPrefix);

        bool isTargetTag = (tag==fragmentTag);

        //if (!startFound)
        if (tagLevel==0) // Начало фрагмента ещё не найдено
        {
            if (isTargetTag)
            {
                //startFound = true;
                tagLevel = 1;
                continue; // Сам тэг мы никогда не добавляем, обработка тэгов производится только для вложенных тэгов
            }

            // Фрагмент не найден, просто пропускаем всё
            continue;
        }

        // Тут мы обрабатываем найденный фрагмент
        // Текущая строка - теговая, безтеговые были обработаны в самом начале

        if (!tag.empty())
        {
            // открывающий тэг
            ++tagLevel;
            addTagLine(l);
        }
        else
        {
            // закрывающий тэг
            --tagLevel;
            if (tagLevel!=0) 
            {
                addTagLine(l);
            }
            else
            {
                // Тут, опа, закончился наш блок
                break;
            }
            
        }

    }

    return marty_cpp::mergeLines(trimLeadingSpaces(fragmentLines, trimLeadingSpaces_a), marty_cpp::ELinefeedType::lf);

    // MARTY_ARG_USED(fragmentTag);
    // MARTY_ARG_USED(tagPrefix);
    // MARTY_ARG_USED(listingNestedTagsMode);
    //  
    // return lfNormalizedText; // пока просто всё целиком возвращаем

    //StringType marty_cpp::stripTextTrailingSpaces(const StringType &str)
    //StringType stripLineTrailingSpaces(StringType str)
    //void stripLineTrailingSpaces(std::vector<StringType> &v)
}
        // dotNutDocGen::ListingNestedTagsMode                   listingNestedTagsMode;

// StringType converLfToOutputFormat(const StringType &str, ELinefeedType lfType)

//----------------------------------------------------------------------------
inline
dotNutDocGen::MdExtraProcessingTags charToMdExtraProcessingTag(char ch)
{
    switch(ch)
    {
        case 'c': [[fallthrough]];
        case 'C': return dotNutDocGen::MdExtraProcessingTags::code   ;

        case 't': [[fallthrough]];
        case 'T': return dotNutDocGen::MdExtraProcessingTags::typeref;

        default : return dotNutDocGen::MdExtraProcessingTags::invalid;
    }
}

//----------------------------------------------------------------------------
inline
char mdExtraProcessingTagToChar(dotNutDocGen::MdExtraProcessingTags t, bool upperCase)
{
    switch(t)
    {
        case dotNutDocGen::MdExtraProcessingTags::code   : return upperCase ? 'C' : 'c';
        case dotNutDocGen::MdExtraProcessingTags::typeref: return upperCase ? 'T' : 't';
        case dotNutDocGen::MdExtraProcessingTags::none   : [[fallthrough]];
        case dotNutDocGen::MdExtraProcessingTags::invalid: [[fallthrough]];
        default                            : return (char)0;
    }
}

//----------------------------------------------------------------------------
inline
std::string processDocTemplate( const marty_simplesquirrel::EnumScriptGenerationOptions genOptions
                              , std::string        tpl
                              , const std::string &tplFilename    // оно нужно, чтобы относительно пути к нему вычислять пути к сорцам для выкусывания фрагментов
                              , const std::string &outputFilename
                              )
{
    tpl = marty_cpp::simple_string_replace(tpl, std::string("```codestart\n"), std::string("```") + genOptions.mdCodeTag + "\n");

    /* 
       !#C[coderef] - с добавлением (обрамлением) листингом, в тч задаётся язык листинга, заданный через командную строку
       !#c[coderef] - включение в уже оформленный листинг
       !#T[typeref] - разбор со всем фаршем
       !#t[typeref] - только то, что задано

     */

    using dotNutDocGen::MdExtraProcessingTags;

    enum State
    {
        stNormal    = 0,
        stWaitHash  = 1,
        stWaitType  = 2,
        stWaitOpen  = 3,
        stReadTag   = 4,
    };

    // none      = 0x00,
    // code      = 0x01,
    // c         = 0x01,
    // typeref   = 0x02,
    // r         = 0x02

    const char tagStartChar = '?';

    MdExtraProcessingTags tagType      = MdExtraProcessingTags::none;
    bool                  tagUpperCase = false;
    State                 state        = stNormal;

    std::string refBuf;
    std::string res; res.reserve(tpl.size());
    

    std::string outputPathName = umba::filename::getPathFile(umba::filename::makeCanonical(outputFilename, '/'));

    std::string tplPath = umba::filename::getPath(tplFilename);

    for(std::size_t i=0u; i!=tpl.size(); ++i)
    {
        char ch = tpl[i];

        switch(state)
        {
            case stNormal:
            {
                if (ch==tagStartChar) // возможно, начало тэга
                    state = stWaitHash;
                else
                    res.append(1, ch);
            }
            break;

            case stWaitHash:
            {
                if (ch=='#')
                    state = stWaitType;
                else
                {
                    res.append(1,tagStartChar);
                    res.append(1, ch);
                    state = stNormal;
                    tagType = MdExtraProcessingTags::none;
                }
            }
            break;

            case stWaitType:
            {
                MdExtraProcessingTags tt = charToMdExtraProcessingTag(ch);
                if (tt==MdExtraProcessingTags::invalid)
                {
                    res.append(1,tagStartChar);
                    res.append(1,'#');
                    res.append(1, ch);
                    state = stNormal;
                    tagType = MdExtraProcessingTags::none;
                }
                else
                {
                    state = stWaitOpen;
                    tagUpperCase = (ch>='A' && ch<='Z');
                    tagType = tt;
                }
            }
            break;

            case stWaitOpen:
            {
                if (ch=='[')
                {
                    state = stReadTag;
                }
                else
                {
                    res.append(1,tagStartChar);
                    res.append(1,'#');
                    res.append(1,mdExtraProcessingTagToChar(tagType, tagUpperCase));
                    res.append(1, ch);
                    state = stNormal;
                    tagType = MdExtraProcessingTags::none;
                }
            }
            break;

            case stReadTag:
            {
                if (ch!=']')
                {
                    refBuf.append(1,ch);
                }
                else
                {
                    // Закрываем тэг

                    umba::string_plus::trim(refBuf);

                    // Обработать refBuf
                    if (tagType==MdExtraProcessingTags::code)
                    {
                        std::string codeTag;
                        std::string filename = refBuf;

                        std::string::size_type hashPos = filename.find('#');
                        if (hashPos!=filename.npos)
                        {
                            codeTag .assign(filename, hashPos+1); // Если метка задана с именем файла, то перетираем то, что задано опцией
                            filename.assign(filename, 0, hashPos);
                        }

                        std::string fullFilename = umba::filename::appendPath(tplPath,filename);

                        std::string allCodeText;
                
                        auto listingCodeFile = fullFilename; // umba::filename::makeCanonical(umba::filename::makeAbsPath(fullFilename));
                        if (!umba::filesys::readFile(listingCodeFile, allCodeText))
                        {
                            LOG_ERR_OPT<< fullFilename <<": "<<"Failed to read source code file for listing"<< "\n";
                            throw std::runtime_error("Failed to read file");
                        }
                
                        std::string allCodeTextNormalized = marty_cpp::normalizeCrLfToLf(allCodeText);
                
                        auto listingCode = extractCodeFragment( allCodeTextNormalized, codeTag, genOptions.codeTagPrefix, genOptions.listingNestedTagsMode );

                        if (tagUpperCase)
                        {
                            res.append(genOptions.getCodeStartMdString());
                            //res.append(1u,'\n');
                        }

                        res.append(listingCode);

                        if (tagUpperCase)
                        {
                            if (!res.empty() && res.back()!='\n')
                            {
                                res.append(1u,'\n');
                            }
                            res.append(genOptions.getCodeEndMdString());
                            //res.append(1u,'\n');
                        }


                    }
                    else if (tagType==MdExtraProcessingTags::typeref)
                    {
                        std::string refText;
                        
                        if (tagUpperCase) // форматируем со всем фаршем
                        {
                            refText = marty_simplesquirrel::formatExternalTypeRef( genOptions, refBuf, outputPathName ); 
                        }
                        else
                        {
                            refText = marty_simplesquirrel::formatExternalMemberRef( genOptions, refBuf, refBuf, outputPathName ); 
                        }
                         
                        if (!refText.empty())
                        {
                            res.append(refText);
                        }
                    
                    }

                    refBuf.clear();

                    state = stNormal;
                    tagType = MdExtraProcessingTags::none;

                }
            }
            break;

        }
    
    }

    return res;
}

//----------------------------------------------------------------------------
inline
marty_simplesquirrel::ClassInfo parseApiDefData(std::string apiDefText, const std::string &curFile)
{
    apiDefText = marty_cpp::normalizeCrLfToLf(apiDefText);
    std::vector<std::string> lines = marty_cpp::splitToLinesSimple(apiDefText, false /* addEmptyLineAfterLastLf */  /* , lfChar='\n' */ );

    using marty_simplesquirrel::ApiDefDirective;
    using marty_simplesquirrel::ClassInfo;

    ClassInfo nsInfo;
    ClassInfo clsInfo;

    enum State
    {
        stWaitNamespace,
        stNamespace,
        stNamespaceMembers,
        stNamespaceFunctions,
        stClasses,
        stClass,
        stClassMembers,
        stClassFunctions
    };

    State state = stWaitNamespace;

    for(unsigned /* std::size_t */  lineNo=0u; lineNo!=(unsigned)lines.size(); ++lineNo)
    {
        std::string line = lines[lineNo];
        umba::string_plus::trim(line);

        if (line.empty())
        {
            continue;
        }

        if (line.front()=='#' || line.front()==';') // shell or ini style comment
        {
            continue;
        }

        if (umba::string_plus::starts_with(line, "//")) // C++ style comment
        {
            continue;
        }

        if (line.front()=='$')
        {
            line.erase(0u, 1u);
            umba::string_plus::trim(line);

            std::string directiveStr;

            std::string::size_type spacePos = line.find(' ');
            if (spacePos==line.npos)
            {
                directiveStr = line;
            }
            else
            {
                directiveStr.assign(line, 0, spacePos);                
                line.erase(0, spacePos+1);
                umba::string_plus::trim(directiveStr);
                umba::string_plus::trim(line);
            }

            ApiDefDirective directive = enum_deserialize(directiveStr, ApiDefDirective::invalid);
            if (directive==ApiDefDirective::invalid)
            {
                //LOG_ERR_OPT<< fullFilename <<": "<<""<< "\n";
            }

            switch(directive)
            {
                case ApiDefDirective::_namespace:
                     if (state==stWaitNamespace)
                     {
                         if (line.empty())
                         {
                             LOG_ERR << "Empty namespace name\n";
                             throw std::runtime_error("Failed to parse file");
                         }

                         nsInfo = ClassInfo(line, true /* isNs */ );

                         state  = stNamespace;
                     }
                     else
                     {
                         LOG_ERR << "Unexpected 'namespace' directive\n";
                         throw std::runtime_error("Failed to parse file");
                     }
                     break;

                case ApiDefDirective::members:
                     if (state==stNamespace || state==stNamespaceMembers || state==stNamespaceFunctions)
                     {
                         state = stNamespaceMembers;
                     }
                     else if (state==stClass || state==stClassMembers || state==stClassFunctions)
                     {
                         state = stClassMembers;
                     }
                     else
                     {
                         LOG_ERR << "Unexpected 'members' directive\n";
                         throw std::runtime_error("Failed to parse file");
                     }
                     break;

                case ApiDefDirective::functions:
                     if (state==stNamespace || state==stNamespaceMembers || state==stNamespaceFunctions)
                     {
                         state = stNamespaceFunctions;
                     }
                     else if (state==stClass || state==stClassMembers || state==stClassFunctions)
                     {
                         state = stClassFunctions;
                     }
                     else
                     {
                         LOG_ERR << "Unexpected 'functions' directive\n";
                         throw std::runtime_error("Failed to parse file");
                     }
                     break;

                case ApiDefDirective::classes:
                     if (state==stNamespace || state==stNamespaceMembers || state==stNamespaceFunctions)
                     {
                         state = stClasses;
                     }
                     else
                     {
                         LOG_ERR << "Unexpected 'classes' directive\n";
                         throw std::runtime_error("Failed to parse file");
                     }
                     break;

                case ApiDefDirective::_class:
                     if (state==stClasses)
                     {
                         // ждем класс
                         // нужно имя класса
                         if (line.empty())
                         {
                             LOG_ERR << "Empty class name\n";
                             throw std::runtime_error("Failed to parse file");
                         }

                         state = stClass;
                         clsInfo = ClassInfo(line, false /* not ns */ );
                         // ничего делать больше не надо
                     }
                     else if (state==stClass || state==stClassMembers || state==stClassFunctions)
                     {
                         // Новый класс
                         if (!clsInfo.empty()) // добавляем текущий класс
                         {
                             nsInfo.addClass(clsInfo);
                             clsInfo = ClassInfo(line, false /* not ns */ );
                         }

                         if (line.empty())
                         {
                             LOG_ERR << "Empty class name\n";
                             throw std::runtime_error("Failed to parse file");
                         }
                     }
                     break;

                case ApiDefDirective::invalid: [[fallthrough]];

                default:
                    LOG_ERR << "Unknown directive '" << directiveStr << "'\n";
                    throw std::runtime_error("Failed to parse file");

            }

        }
        else // generic line
        {
            switch(state)
            {
                case stWaitNamespace     :
                     LOG_ERR << "Gotten line while waiting for namespace definition\n";
                     throw std::runtime_error("Failed to parse file");

                case stNamespace         :
                     LOG_ERR << "Gotten line while waiting for members/functions/classes directive\n";
                     throw std::runtime_error("Failed to parse file");

                case stNamespaceMembers  :
                     nsInfo.addMember(line);
                     break;

                case stNamespaceFunctions:
                     nsInfo.addFunc(line);
                     break;

                case stClasses           :
                     LOG_ERR << "Gotten line while waiting for class directive\n";
                     throw std::runtime_error("Failed to parse file");

                case stClass             :
                     LOG_ERR << "Gotten line while waiting for class members/functions directive\n";
                     throw std::runtime_error("Failed to parse file");

                case stClassMembers      :
                     clsInfo.addMember(line);
                     break;

                case stClassFunctions    :
                     clsInfo.addFunc(line);
                     break;

                default :
                    LOG_ERR << "Invalid state\n";
                    throw std::runtime_error("Failed to parse file");

            }
        }

        // if (!umba::string_plus::starts_with_and_strip(lineCopy, tagPrefix))
        //     return false;
    
    }

    if (!clsInfo.empty()) // добавляем текущий класс
    {
        nsInfo.addClass(clsInfo);
    }

    auto rootNs = marty_simplesquirrel::ClassInfo(true);
    rootNs.addClass(nsInfo);

    return rootNs;
}

