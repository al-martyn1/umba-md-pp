#pragma once


#include "umba/html_base.h"
//
#include "umba_md_html_utils.h"
#include "enums.h"

// umba::md::
namespace umba {
namespace md {

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Если строка не начинается с тэга (ведущие пробелы допустимы), и тэг корректно распознан, то по возвращённому итератору будет лежать либо '>' - всё хорошо, это закрывашка тэга, или '<' - всё хорошо, но внезапно начался другой тэг без закрытия предыдущего.
/*!
    Идеальный вариант - когда возвращается не end, и результат указывает на '<'
*/
template<typename IteratorType>
IteratorType tryParseLineToHtmlTag(umba::html::HtmlTag &parseTo, IteratorType b, IteratorType e)
{
    auto isWhiteSpace = [](char ch)
    {
        return ch==' ' || ch=='\r' || ch=='\n' || ch=='\t';
    };

    while(b!=e && isWhiteSpace(*b)) ++b; // пропускаем возможные пробелы перед открывающей '<'

    parseTo.clear();

    if (b==e)
        return b;

    if (*b!='<')
        return e; // Не является открывающим тэгом

    return parseSingleTag<'<', '>'>(parseTo, b, e);

}

//----------------------------------------------------------------------------
template<typename IteratorType>
IteratorType tryParseLineToHtmlTag(umba::html::HtmlTag &parseTo, IteratorType b, IteratorType e, MdPpTag &foundTag)
{
    foundTag = MdPpTag::unknown;

    auto itRes = tryParseLineToHtmlTag(parseTo, b, e);

    if (itRes==e)
        return e;

    if (*itRes!='>')
        return e;

    if (!parseTo.isTag())
        return e;

    if (!parseTo.isTag()) // если инвалид, текст, или пустое имя тэга - выходим
        return e;

    MdPpTag tagId = enum_deserialize(parseTo.name, MdPpTag::invalid);
    if (tagId!=MdPpTag::invalid)
    {
        foundTag = tagId;
        return itRes;
    }

    // Что за лажу я тут придумал?    
    // typedef typename std::underlying_type<MdPpTag>::type  EnumUnderlyingType;
    //  
    // EnumUnderlyingType curTagId = (EnumUnderlyingType)MdPpTag::begin;
    // for(; curTagId!=(EnumUnderlyingType)MdPpTag::end; ++curTagId)
    // {
    //     if (parseTo.isTag(enum_serialize((MdPpTag)curTagId)))
    //     {
    //         foundTag = (MdPpTag)curTagId;
    //         return itRes;
    //     }
    // }

    return e;
}

template<typename IteratorType>
IteratorType parseTagLineExtra(std::string &filename, std::string &text, IteratorType b, IteratorType e, bool alreadyHasFileAttr)
{
    filename.clear();
    text.clear();

    b = umba::html::helpers::skipSpaces(b, e);
    if (b==e)
        return b;

    if (*b=='!')
    {
        // Это текст, до конца строки
        ++b;
        return umba::html::helpers::readUntilEnd(std::back_inserter(text), b, e);
    }

    std::string strTmp;

    if (alreadyHasFileAttr)
        b = umba::html::helpers::readQuotedOrUntilEnd(std::back_inserter(strTmp), b, e);
    else
        b = umba::html::helpers::readQuotedOrUntilSpace(std::back_inserter(strTmp), b, e);

    if (b!=e)
    {
        if (umba::html::helpers::isSimpleQuot(*b))
            ++b;
    }

    if (!strTmp.empty())
    {
        if (strTmp[0]!='!')
        {
            if (alreadyHasFileAttr)
            {
                text = strTmp;
                return b;
            }
            else
            {
                filename = strTmp;
            }
        }
        else
        {
            strTmp.erase(0,1);
            text = strTmp;
        }

        strTmp.clear();
    }


    b = umba::html::helpers::skipSpaces(b, e);
    if (b==e)
        return b;

    if (*b=='!')
    {
        // Это текст, до конца строки
        ++b;
        return umba::html::helpers::readUntilEnd(std::back_inserter(text), b, e);
    }

    b = umba::html::helpers::readQuotedOrUntilEnd(std::back_inserter(strTmp), b, e);

    if (!strTmp.empty())
    {
        if (strTmp[0]!='!')
        {
            if (filename.empty())
                filename = strTmp;
            else
                text = strTmp;
        }
        else
        {
            strTmp.erase(0,1);
            text = strTmp;
        }

        strTmp.clear();
    }

    return b;

}

//----------------------------------------------------------------------------
//! Мы уже распарсили стартовый тэг, за ним может быть имя файла и заголовок
template<typename IteratorType>
IteratorType parseExtraPossibleFilenameAndTextToHtmlTag(umba::html::HtmlTag &parseTo, IteratorType b, IteratorType e)
{
    if (b==e)
        return b;

    if (*b!='>')
        return b;

    ++b;

    b = umba::html::helpers::skipSpaces(b, e);

    if (b==e)
        return b;

#if 0
    std::string fileName;
    for(; b!=e && !isWhiteSpace(*b); ++b)
    {
        fileName.append(1, *b);
    }

    if (!fileName.empty())
    {
        parseTo.addAttr("file", fileName);
    }

    return b;
#endif

    std::string filename;
    std::string text;
    b = parseTagLineExtra(filename, text, b, e, parseTo.hasAttr("file"));

    if (!filename.empty())
        parseTo.addAttr("file", filename);

    if (!text.empty())
        parseTo.addAttr("title", text);

    return b;
}


// umba::html::helpers

// bool isWhiteSpace(char ch)
// char isSimpleQuot(char ch)


// InputIterator skipSpaces(InputIterator b, InputIterator e)
//
// InputIterator readUntilEnd(OutputIterator out, InputIterator b, InputIterator e)

// //! Читаем текст до пробела или конца строки
// InputIterator readUntilSpace(OutputIterator out, InputIterator b, InputIterator e)
//
// InputIterator readUntilQuot(OutputIterator out, InputIterator b, InputIterator e, char quot)

// InputIterator readQuotedOrUntilSpace(OutputIterator out, InputIterator b, InputIterator e, char quot)

// InputIterator readQuotedOrUntilEnd(OutputIterator out, InputIterator b, InputIterator e)




//----------------------------------------------------------------------------

} // namespace md
} // namespace umba


