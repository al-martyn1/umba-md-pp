#pragma once

//----------------------------------------------------------------------------
// Для соответствующего тэга возвращает строчку с single-line comment префиксом
// Если формат файла не поддерживает коментарии, то увы, метаданных там не может быть
using TagLinesCommentStartMap = std::unordered_map<MdPpTag, std::string, EnumClassHash >;

//------------------------------
inline
TagLinesCommentStartMap makeTagLinesCommentStartMap()
{
    TagLinesCommentStartMap m;
    m[MdPpTag::graph] = "//";
    m[MdPpTag::puml ] = "'";
    m[MdPpTag::argList ] = "'";
    m[MdPpTag::valList ] = "'";
    return m;
}

//------------------------------
inline
const TagLinesCommentStartMap& getTagLinesCommentStartMap()
{
    static auto m = makeTagLinesCommentStartMap();
    return m;
}

//------------------------------
inline
std::string getTagLinesCommentStart(MdPpTag tagType)
{
    const auto &m = getTagLinesCommentStartMap();
    auto it = m.find(tagType);
    if (it==m.end())
        return std::string();

    return it->second;
}

//----------------------------------------------------------------------------
