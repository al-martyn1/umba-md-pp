/*! \file
    \brief Помогалки для processing.h
 */
#pragma once

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Misc "is a..." functions

//----------------------------------------------------------------------------
inline
bool isInsertCommand(std::string line)
{
    umba::string_plus::trim(line);
    return (umba::string_plus::starts_with(line, ("#!insert")) || umba::string_plus::starts_with(line, ("#$insert")));
}

//----------------------------------------------------------------------------
inline
bool isTocCommand(std::string line)
{
    umba::string_plus::trim(line);
    return (umba::string_plus::starts_with(line, ("#!toc")) || umba::string_plus::starts_with(line, ("#$toc")));
}

//----------------------------------------------------------------------------
inline
bool isSingleLineComment(std::string line)
{
    umba::string_plus::trim(line);
    return umba::string_plus::starts_with(line, ("#//"));
}

//----------------------------------------------------------------------------
inline
bool isMultiLineCommentStart(std::string line)
{
    umba::string_plus::trim(line);
    return umba::string_plus::starts_with(line, ("#/*"));
}

//----------------------------------------------------------------------------
inline
bool isMultiLineCommentEnd(std::string line)
{
    umba::string_plus::trim(line);
    return umba::string_plus::ends_with(line, ("#*/"));
}

//----------------------------------------------------------------------------
inline
bool isListingCommand(std::string line)
{
    umba::string_plus::trim(line);
    return umba::string_plus::starts_with(line, std::string("```")) || umba::string_plus::starts_with(line, std::string("~~~"));
}

//----------------------------------------------------------------------------
inline
bool isMetaStartCommand(std::string line)
{
    umba::string_plus::trim(line);
    return umba::string_plus::starts_with(line, ("---"));
}

//----------------------------------------------------------------------------
inline
bool isMetaEndCommand(std::string line)
{
    umba::string_plus::trim(line);
    return umba::string_plus::starts_with(line, ("---")) || umba::string_plus::starts_with(line, ("..."));
}

//----------------------------------------------------------------------------
inline
bool isHeaderCommand(std::string line)
{
    umba::string_plus::trim(line);

    std::size_t idx = 0u;

    while(idx!=line.size() && line[idx]=='#') { ++idx; }

    return idx>0;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Blockquotes

//----------------------------------------------------------------------------
inline
std::size_t getBlockQuoteLevel(const std::string &line)
{
    std::size_t bqLevel = 0;
    for(auto ch: line)
    {
        if (ch=='>')
        {
            ++bqLevel;
            continue;
        }
        else if (ch==' ')
        {
            continue;
        }
        else
        {
            return bqLevel;
        }

    }

    return bqLevel;
}

//----------------------------------------------------------------------------
inline
bool isBlockQuoteLine(const std::string &line)
{
    return getBlockQuoteLevel(line) > 0;
}

//----------------------------------------------------------------------------
inline
std::string trimBlockQuote(const std::string &line)
{
    auto pos = line.find_first_not_of("> ");
    if (pos==line.npos)
        return line;
    return std::string(line, pos);
}

//----------------------------------------------------------------------------
inline
std::string makeBlockQuotePrefix(std::size_t lvl)
{
    std::string res; res.reserve(lvl*2+1);
    for(std::size_t i=0; i!=lvl; ++i)
    {
        res.append(" >");
    }

    if (lvl)
        res.append(1u, ' ');

    return res;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Lists

//----------------------------------------------------------------------------
inline
char getListMarker(std::string line)
{
    umba::string_plus::trim(line);

    if (line.empty()) // вторым должен идти пробел
        return (char)0;

    char chMarker = line[0];

    if ((chMarker=='-' || chMarker=='+' || chMarker=='*'))
    {
        if (line.size()<2)
            return chMarker; // если нет пробела, и строка закончилась - это тоже элемент списка, но пустой
        
        if (line[1]==' ') // есть ещё символ, и это - пробел - это элемент списка
            return chMarker;
    }
        

    if (!umba::parse_utils::isDigit(chMarker))
    {
        return (char)0;
    }

    if (line.size()<2) // маркер ненумерованного списка состоит минимум из двух символов - цифра и точка
        return (char)0;
    
    std::size_t pos = 1;
    for(; pos!=line.size(); ++pos)
    {
        if (umba::parse_utils::isDigit(line[pos]))
        {
            continue;
        }

        if (line[pos]!='.') // последовательность цифр в маркере нумерованного списка должна закончится на точку и пробел
        {
            return (char)0;
        }

        ++pos;
        break;
    }

    if (pos==line.size()) // после точки строка закончилась?
        return '1';

    if (line[pos]!='.')
    {
        return (char)0;
    }

    return '1';

}

//----------------------------------------------------------------------------
inline
bool isListLine(const std::string &line)
{
    return getListMarker(line)!=0;
}

//----------------------------------------------------------------------------
inline
std::size_t getListLevel(const std::string &line)
{
    auto pos = line.find_first_not_of(" ");
    return pos;
}

//----------------------------------------------------------------------------
inline
std::string trimListLine(const std::string &line)
{
    //TODO: !!! Доделать
    return std::string();
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Misc utils

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
void makeShureEmptyLine(std::vector<std::string> &lines)
{
    if (lines.empty())
    {
        lines.emplace_back(std::string());
    }
    else
    {
        std::string lastLine = lines.back();
        umba::string_plus::trim(lastLine);
        if (!lastLine.empty())
        {
            lines.emplace_back(std::string());
        }
    }
}

