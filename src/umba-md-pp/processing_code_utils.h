/*! \file
    \brief Помогалки для processing.h
 */
#pragma once


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
bool isCodeTagLine(std::string line, const std::string &tagPrefix)
{
    umba::string_plus::trim(line);

    return umba::string_plus::starts_with(line, tagPrefix);
}

//----------------------------------------------------------------------------
inline
bool hasIdentifierChars(const std::string str)
{
    for(char ch: str)
    {
        if (ch>='a' && ch<='z')
            return true;
        if (ch>='A' && ch<='Z')
            return true;
        if (ch>='0' && ch<='9')
            return true;
        if (ch=='_')
            return true;
    }

    return false;
}

//----------------------------------------------------------------------------
inline
bool extractCodeTagFromLine(std::string &line, const std::string &tagPrefix)
{
    auto lineCopy = line;
    umba::string_plus::trim(lineCopy);

    if (!umba::string_plus::starts_with_and_strip(lineCopy, tagPrefix))
        return false;

    umba::string_plus::trim(lineCopy);

    auto spacePos = lineCopy.find(' ');
    if (spacePos!=lineCopy.npos)
    {
        lineCopy.erase(spacePos, lineCopy.npos);
    }

    line = lineCopy;

    if (!hasIdentifierChars(line))
        line.clear();

    return true;
}

//----------------------------------------------------------------------------
inline
bool extractCodeTagFromLine(std::string line, const std::string &tagPrefix, std::string &targetTag)
{
    if (!extractCodeTagFromLine(line, tagPrefix))
        return false;

    targetTag = line;

    return true;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> extractCodeFragment( std::vector<std::string>    lines
                                            , std::size_t                 &firstFoundLineIdx
                                            , const std::string           &targetFragmentTag
                                            , const std::string           &tagPrefix
                                            , ListingNestedTagsMode       listingNestedTagsMode
                                            , std::size_t                 tabSize=4u
                                            //, bool                        trimLeadingSpaces_a = true
                                            )
{
    marty_cpp::expandTabsToSpaces(lines, tabSize);

    if (targetFragmentTag.empty() || lines.empty())
    {
        firstFoundLineIdx = 0;
        return lines;
    }

    std::vector<std::string> fragmentLines; fragmentLines.reserve(lines.size());


    auto addTagLine = [&](const std::string &line)
    {
        switch(listingNestedTagsMode)
        {
            case ListingNestedTagsMode::keep  :
                 fragmentLines.emplace_back(line);
                 break;
         
            case ListingNestedTagsMode::remove:
                 break;
         
            case ListingNestedTagsMode::empty :
                 fragmentLines.emplace_back(std::string());
                 break;

            case ListingNestedTagsMode::invalid :
                 break;
        }
    };

    //std::stack<std::string>  openedTags;
    std::vector<std::string> openedTags;

    auto isClosingTag = [&](std::string tagName)
    {
        if (openedTags.empty())
            return false;

        if (tagName.empty())
            return true; // разрешаем закрывающим тэгам не иметь имени

        if (openedTags.back()==tagName)
            return true; // текущий тэг закрывает того, что на стеке - имя одно
    
        return false;
    };

    auto openCodeFragment = [&](std::string tagName)
    {
        openedTags.emplace_back(tagName);
    };

    auto closeCurTag = [&]()
    {
        if (openedTags.empty())
            return;
        openedTags.pop_back();
    };

    auto isTargetFragmentTagOpened = [&]()
    {
        for(const auto &openedTag: openedTags)
        {
            if (openedTag==targetFragmentTag)
                return true;
        }

        return false;
    };

    auto isTargetFragmentTagOnTop = [&]()
    {
        if (openedTags.empty())
            return false;

        if (openedTags.back()==targetFragmentTag)
            return true; // искомый тэг на вершине
    
        return false;
    };


    std::size_t lineIdx = 0;

    for(; lineIdx!=lines.size(); ++lineIdx)
    {
        auto l = lines[lineIdx]; 

        if (!isCodeTagLine(l, tagPrefix))
        {
            // Строка обычная, не тэговая
            if (isTargetFragmentTagOpened())
            {
                fragmentLines.emplace_back(l);
            }

            continue;
        }

        // Строка - тэговая
        std::string curTag;
        extractCodeTagFromLine(l, tagPrefix, curTag);

        if (isClosingTag(curTag))
        {
            if (isTargetFragmentTagOnTop())
            {
                // Закрываем целевой тэг
                closeCurTag(); // на самом деле - насрать, всё равно из цикла выходим
                break;
            }
            else
            {
                // закрываем какой-то левый тэг
                closeCurTag();
                addTagLine(l);
                continue;
            }
        }

        // Если тэг не закрывающий, то он - открывающий
        // Или он может быть пустым при пустом стеке
        if (curTag.empty())
        {
            continue; // Игнорим закрывающие тэги, когда не было открывающих
        }

        if (curTag==targetFragmentTag)
        {
            firstFoundLineIdx = lineIdx+1; // начали со строки, которая следует за открывающим тэгом
        }

        // Если наш целевой тэг открыт, то надо добавить строчку, а тут идёт обработка вложенных тэгов
        if (isTargetFragmentTagOpened())
        {
            //fragmentLines.emplace_back(l);
            addTagLine(l);
        }

        openCodeFragment(curTag);
    }

    return fragmentLines;
}


