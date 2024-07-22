#pragma once


#include "umba_md_html_utils.h"
#include "enums.h"

// umba::md::
namespace umba {
namespace md {



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

    typedef typename std::underlying_type<MdPpTag>::type  EnumUnderlyingType;

    EnumUnderlyingType curTagId = (EnumUnderlyingType)MdPpTag::begin;
    for(; curTagId!=(EnumUnderlyingType)MdPpTag::end; ++curTagId)
    {
        if (parseTo.isTag(enum_serialize((MdPpTag)curTagId)))
        {
            foundTag = (MdPpTag)curTagId;
            return itRes;
        }
    }

    return e;
}



//----------------------------------------------------------------------------

} // namespace md
} // namespace umba


