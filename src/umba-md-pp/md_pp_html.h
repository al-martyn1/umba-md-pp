#pragma once


#include "umba_md_html_utils.h"


// umba::md::
namespace umba {
namespace md {



//! Если строка не начинается с тэга (ведущие пробелы допустимы), и тэг корректно распознан, то по возвращённому итератору будет лежать либо '>' - всё хорошо, это закрывашка тэга, или '<' - всё хорошо, но внезапно начался другой тэг без закрытия предыдущего.
/*!
    Идеальный вариант - когда возвращается не end, и результат указывает на '<'
*/
template<typename IteratorType>
IteratorType tryParseLineToHtmlTag(umba::html::HtmlTag &parseTo, IteratorType b, IteratorType e, bool caseIndependent = true)
{
    auto isWhiteSpace = [](char ch)
    {
        return ch==' ' || ch=='\r' || ch=='\n' || ch=='\t';
    };

    parseTo.clear();

    while(b!=e && isWhiteSpace(*b)) ++b; // пропускаем возможные пробелы перед открывающей '<'

    if (b==e)
        return b;

    if (b!='<')
        return e; // Не является открывающим тэгом

    return parseSingleTag<'<', '>'>(parsedTo, b, e);

}







//----------------------------------------------------------------------------

} // namespace md
} // namespace umba


