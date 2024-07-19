#pragma once


#include <iterator>
#include <string>
#include <iostream>
#include <type_traits>
#include <vector>
#include <stack>
#include <exception>
#include <stdexcept>

//
// #include <boost/container/static_vector.hpp>

#include "umba/container.h"
#include "umba/filename.h"
#include "umba/string_plus.h"

// 
#include "enums.h"

// 

//----------------------------------------------------------------------------

// umba::md::
namespace umba {
namespace md {

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
struct HtmlTag
{

    enum class TagType
    {
        invalid = -1,
        text    =  0,
        tag         ,  // <tag> open tag
        close       ,  // </tag> close tag
        empty          // <empty-tag/> - не содержит в себе вложенные тэги
    
    };


    TagType                                       tagType   ;
    std::string                                   name      ;
    std::string                                   text      ;
    std::unordered_map<std::string, std::string>  attributes;
    std::vector<HtmlTag>                          childs    ;  // Content 

    void clear()
    {
        tagType = TagType::invalid;
        name      .clear();
        text      .clear();
        attributes.clear();
        childs    .clear();
    }

    bool isTag() const
    {
        return tagType!=TagType::invalid && tagType!=TagType::text && !name.empty();
    }

    bool isTag(const std::string &nameToCompare, bool caseIndependent = true) const
    {
        if (!isTag())
            return false;

        return name == (caseIndependent ? umba::string_plus::tolower_copy(nameToCompare) : nameToCompare);
    }

    bool isCloseTag() const
    {
        if (!isTag())
            return false;

        return tagType==TagType::close;
    }

    bool needCloseTag() const
    {
        if (!isTag())
            return false;

        return tagType==TagType::empty;
    }

    bool hasAttr(const std::string &attrName, bool caseIndependent = true) const
    {
        return attributes.find(caseIndependent ? umba::string_plus::tolower_copy(attrName) : attrName)!=attributes.end();
    }

    std::string getAttrValue(const std::string &attrName, const std::string &defVal, bool caseIndependent = true) const
    {
        auto it = attributes.find(caseIndependent ? umba::string_plus::tolower_copy(attrName) : attrName);
        return it==attributes.end() ? defVal : it->second;
    }

    std::string getAttrValue(const std::string &attrName, bool caseIndependent = true) const
    {
        return getAttrValue(attrName, std::string(), caseIndependent);
    }
    

}; // struct HtmlTag

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Возвращает итератор, указывающий на нераспознанный символ (ошибка), e (end, ошибка, но может не всегда - неожиданное окончание тэга, иногда допустимо), или указывающий на закрывающий '>' (Ок).
template<typename IteratorType>
IteratorType parseSingleTag(HtmlTag &parseTo, IteratorType b, IteratorType e, bool caseIndependent = true)
{
    parseTo.clear();

    auto isWhiteSpace = [](char ch)
    {
        return ch==' ' || ch=='\r' || ch=='\n' || ch=='\t';
    };

    // Точка вроде как допустима в имени, а ':' - namespace разделитель
    auto isNameChar = [](char ch)
    {
        return (ch>='a' && ch<='z')
            || (ch>='A' && ch<='Z')
            || (ch>='0' && ch<='9')
            || (ch>='_' || ch=='-' || ch=='.' || ch==':')
               ;
    };

    auto isNameFirstChar = [](char ch)
    {
        return (ch>='a' && ch<='z')
            || (ch>='A' && ch<='Z')
            || (ch>='_' || ch=='-' || ch=='.' || ch==':')
               ;
    };

    std::string attrName;
    std::string attrVal ;


    auto addAttribute = [&]()
    {
        if (!attrName.empty())
        parseTo.attributes[caseIndependent ? umba::string_plus::tolower_copy(attrName) : attrName] = attrVal;
        attrName.clear();
        attrVal .clear();
    };

    auto finalizeTag = [&]()
    {
        if (parseTo.name.empty())
        {
            parseTo.clear();
            return b;
        }

        addAttribute();

        if (caseIndependent)
            parseTo.name = umba::string_plus::tolower_copy(parseTo.name);

        return b;
    };

    auto finalizeEmptyTag = [&]()
    {
        if (parseTo.name.empty())
        {
            parseTo.clear();
            return b;
        }

        addAttribute();

        if (caseIndependent)
            parseTo.name = umba::string_plus::tolower_copy(parseTo.name);

        if (parseTo.tagType==HtmlTag::TagType::close)
            return b; // Обнаружен символ '/', как в empty тэге - но у нас закрывающий тэг, такой символ был перед именем тэга (</tag/>) - это ошибка

        ++b;
        if (b==e)
            return b;

        if (*b=='>') // Последовательность "/>" должна идти без пробела - надоело пробелы пропускать
            parseTo.tagType = HtmlTag::TagType::empty; // нормальное закрытие empty тэга

        return b; // в любом случае - завершаемся, ошибка или нет, если норм, то тип тэга установлен на empty
    };



    if (b==e)
        return b;

    if (*b!='<')
        return b;

    ++b;

    while(b!=e && isWhiteSpace(*b)) ++b; // пропускаем возможные пробелы перед именем тэга

    if (b==e)
        return b;

    if (*b=='<') // Неожиданое открытие другого тэга
        return b;

    if (*b=='/') // Закрывающий тэг
    {
        ++b;
        while(b!=e && isWhiteSpace(*b)) ++b; // пропускаем возможные пробелы перед именем тэга
        if (b==e)
            return b;

        parseTo.tagType = HtmlTag::TagType::close;
    }
    else
    {
        parseTo.tagType = HtmlTag::TagType::tag;
    }

    // Читаем имя тэга

    if (!isNameFirstChar(*b))
        return finalizeTag();

    parseTo.name.append(1, *b);
    ++b;
    if (b==e)
        return finalizeTag();


    for(; b!=e && isNameChar(*b); ++b)
        parseTo.name.append(1, *b);

    if (b==e)
        return finalizeTag();

    // считали имя тэга
    // тут мы ждём закрытия тэга, закрытия empty тэга, или начало имени атрибута

    while(b!=e)
    {
        while(b!=e && isWhiteSpace(*b)) ++b; // пропускаем возможные пробелы

        if (b==e)
            return finalizeTag();

        if (*b=='>')
            return finalizeTag(); // нормальное закрытие - либо открывающего, либо закрывающего тэга. На вызывающей стороне проверят *b=='>' и убедятся, что всё хорошо

        if (*b=='<')
            return finalizeTag(); // Неожиданое открытие другого тэга

        if (*b=='/') // возможно, это empty тэг - <tag/>
            return finalizeEmptyTag();

        if (!isNameFirstChar(*b))
            return finalizeTag(); // Ожидаем первый символ имени атрибута, но получили какую-то фигню

        if (parseTo.tagType==HtmlTag::TagType::close) // закрывающий тэг
            return finalizeTag(); //  В закрывающем тэге не должно быть никаких атрибутов

        // Таки атрибут
        attrName.clear();
        attrVal .clear();
        attrName.append(1, *b);
        ++b;

        // Первый символ атрибута уже добавлен
        if (b==e)
            return finalizeTag();

        for(; b!=e && isNameChar(*b); ++b) // собираем имя атрибута
            attrName.append(1, *b);

        if (b==e)
            return finalizeTag();

        // ждём символ равно '=' перед значением атрибута
        while(b!=e && isWhiteSpace(*b)) ++b; // пропускаем возможные пробелы
        if (b==e)
            return finalizeTag(); // символ равно '=' не дождались

        if (*b=='/')
            return finalizeEmptyTag();

        if (*b!='=')
            return finalizeTag();

        ++b;
        while(b!=e && isWhiteSpace(*b)) ++b; // пропускаем возможные пробелы

        // Значения у нас пока без кавычек, ибо лень. Надо будет - прикрутим
        for(; b!=e && !isWhiteSpace(*b) && *b!='>' && *b!='<'; ++b)
        {
            attrVal.append(1, *b);
        }

        addAttribute();
    }

    return b;

}

//----------------------------------------------------------------------------
template <typename StreamType>
bool testParseSingleTag(StreamType &s, const std::string &str, char expectedEndChar) // 'E' means end, not symbol 'E' itself
{
    HtmlTag parsedTag;

    auto resIt = parseSingleTag(parsedTag, str.begin(), str.end());

    char resChar = 'E';
    if (resIt!=str.end())
        resChar = *resIt;

    bool bPassed = resChar==expectedEndChar;

    s << "\n";
    s << (bPassed?"+":"-") << " " << "{" << str << "}, ends with: '" << std::string(1, resChar) << "'";
    if (!bPassed)
    {
        s << ", expected: '" << std::string(1, expectedEndChar);
    }
    s << "\n";

    s << "Tag type: ";
    switch(parsedTag.tagType)
    {
        case HtmlTag::TagType::invalid:
            s << "<INVALID>";
            break;

        case HtmlTag::TagType::text:
            s << "Text";
            break;

        case HtmlTag::TagType::tag:
            s << "Tag (open tag)";
            break;

        case HtmlTag::TagType::close:
            s << "End tag (close tag)";
            break;

        case HtmlTag::TagType::empty:
            s << "Empty tag (<tag/>)";
            break;
    }
    s << "\n";

    s << "Tag name: " << (parsedTag.name.empty() ? std::string("<EMPTY>") : parsedTag.name) << "\n";
    if (!parsedTag.attributes.empty())
    {
        s << "Attrs:\n";
        for(const auto &akv: parsedTag.attributes)
        {
            s << "    " << akv.first << "=" << (akv.second.empty() ? std::string("<EMPTY>") : akv.second) << "\n";
        }
    }

    return bPassed;
}

// struct HtmlTag
// {
//     TagType                                       tagType   ;
//     std::string                                   name      ;
//     std::string                                   text      ;
//     std::unordered_map<std::string, std::string>  attributes;
//     std::vector<HtmlTag>                          childs    ;  // Content 


// template<typename IteratorType>
// IteratorType parseSingleTag(HtmlTag &parseTo, IteratorType b, IteratorType e)
// {


//----------------------------------------------------------------------------

} // namespace md
} // namespace umba



