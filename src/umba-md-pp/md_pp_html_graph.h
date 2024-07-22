#pragma once

// umba::md::
namespace umba {
namespace md {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Мы уже распарсили стартовый тэг <graph>, за ним может быть имя файла
IteratorType parseLineToGraphTag(umba::html::HtmlTag &parseTo, IteratorType b, IteratorType e)
{
    if (b==e)
        return b;

    if (*b!='>')
        return b;

    ++b;

    auto isWhiteSpace = [](char ch)
    {
        return ch==' ' || ch=='\r' || ch=='\n' || ch=='\t';
    };

    while(b!=e && isWhiteSpace(*b)) ++b; // пропускаем возможные пробелы перед именем файла

    if (b==e)
        return b;


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
}


//----------------------------------------------------------------------------

} // namespace md
} // namespace umba


