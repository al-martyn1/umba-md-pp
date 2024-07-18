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

// 
#include "enums.h"

// 
#include "umba/filename.h"

//----------------------------------------------------------------------------

// umba::md::
namespace umba {
namespace md {

//----------------------------------------------------------------------------
enum class TagType
{
    invalid = -1,
    text    =  0,
    tag           // <tag> open tag
    close         // </tag> close tag
    empty         // <empty-tag/> - не содержит в себе вложенные тэги

};

//----------------------------------------------------------------------------
struct HtmlTag
{
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

    // bool 


}; // struct HtmlTag


//! Возвращает итератор, указывающий на нераспознанный символ (ошибка), e (end, ошибка, но может не всегда - неожиданное окончание тэга, иногда допустимо), или указывающий на закрывающий '>' (Ок).
template<typename IteratorType>
IteratorType parseSingleTag(HtmlTag &parseTo, IteratorType b, IteratorType e)
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

    if (b==e)
        return b;

    if (*b!='<')
        return b;

    ++b;

    while(b!=e && isWhiteSpace(*b)) ++b; // пропускаем возможные пробелы перед именем тэга

    if (b==e)
        return b;

    if (*b=='/')
    {
        ++b;
        while(b!=e && isWhiteSpace(*b)) ++b; // пропускаем возможные пробелы перед именем тэга
        if (b==e)
            return b;

        parseTo.tagType = TagType::close;
    }
    else
    {
        parseTo.tagType = TagType::tag;
    }

    // Читаем имя тэга

    if (!isNameFirstChar(*b))
        return b; 

    parseTo.name.append(1, *b);
    ++b;
    if (b==e)
        return b;


    while(b!=e && isNameChar(*b))
    {
        parseTo.name.append(1, *b);
        ++b;
    }

    if (b==e)
        return b;

    // считали имя тэга

    // тут мы ждём закрытия тэга, закрытия empty тэга, или начало имени атрибута

    while(b!=e)
    {
        while(b!=e && isWhiteSpace(*b)) ++b; // пропускаем возможные пробелы

        if (b==e)
            return b;

        if (*b=='>')
            return b; // нормальное закрытие - либо открывающего, либо закрывающего тэга. На вызывающей стороне проверят *b=='>' и убедятся, что всё хорошо

        if (*b=='/') // возможно, это empty тэг - <tag/>
        {
            if (parseTo.tagType==TagType::close)
            {
                // После имени тэга обнаружен символ '/', как в empty тэге - но у нас закрывающий тэг, 
                // такой символ был перед именем тэга (</tag/>) - это ошибка
                return b;
            }

            // сейчас у нас тут может быть только parseTo.tagType==TagType::tag
            ++b;
            if (b==e)
                return b;

            if (*b=='>')
            {
                parseTo.tagType = TagType::empty; // нормальное закрытие empty тэга
            }

            return b; // в любом случае - завершаемся, ошибка или нет, если норм, то тип тэга установлен на empty
        }


        if (!isNameFirstChar(*b))
        {
            // Ожидаем первый символ имени атрибута, но получили какую-то фигню
            return b;
        }

        // Таки атрибут
        attrName.clear();
        attrVal .clear();
        attrName.append(1, *b);
        ++b;

        // Первый символ атрибута уже добавлен
        if (b==e)
        {
            parseTo.attributes[attrName] = attrVal; // добавляем атрибут без значения
            return b;
        }

        while(b!=e && isNameChar(*b)) // собираем имя атрибута
        {
            attrName.append(1, *b);
            ++b;
        }

        if (b==e)
        {
            parseTo.attributes[attrName] = attrVal; // добавляем атрибут без значения
            return b;
        }

        // ОК

        // ждём символ равно '=' перед значением атрибута
        while(b!=e && isWhiteSpace(*b)) ++b; // пропускаем возможные пробелы
        if (b==e)
        {
            // символ равно '=' не дождались
            parseTo.attributes[attrName] = attrVal; // добавляем атрибут без значения
            return b;
        }

        if (*b=='/')
        {
            parseTo.attributes[attrName] = attrVal; // добавляем атрибут без значения
            return b;

            if (parseTo.tagType==TagType::close)
            {
                // После имени атрибута обнаружен символ '/', как в empty тэге - но у нас закрывающий тэг, 
                // такой символ был перед именем тэга (</tag/>) - это ошибка
                return b;
            }

            // сейчас у нас тут может быть только parseTo.tagType==TagType::tag
            ++b;
            if (b==e)
                return b;

            if (*b=='>')
            {
                parseTo.tagType = TagType::empty; // нормальное закрытие empty тэга
            }

            return b; // в любом случае - завершаемся, ошибка или нет, если норм, то тип тэга установлен на empty

        }

        if (b==e || *b!='=')
        {
            parseTo.attributes[attrName] = attrVal; // добавляем атрибут без значения
            return b;
        }

        if (b==e || *b!='=')
        {
            parseTo.attributes[attrName] = attrVal; // добавляем атрибут без значения
            return b;
        }

    }

    // std::string attrName;
    // std::string attrVal ;
    


    if (*b=='/' && parseTo.tagType==TagType::close)
    {
        // После имени тэга обнаружен символ '/', как в empty тэге - но у нас закрывающий тэг, такой символ был перед именем тэга - это ошибка
        return b;
    }




}


//----------------------------------------------------------------------------

} // namespace md
} // namespace umba



