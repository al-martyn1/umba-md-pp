#pragma once

#include "umba/umba.h"
#include "umba/filename.h"
#include "umba/filesys.h"
//
#include "md_pp_html.h"
#include "graph_viz_options.h"

// umba::md::
namespace umba {
namespace md {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Мы уже распарсили стартовый тэг <graph>, за ним может быть имя файла
template<typename IteratorType>
IteratorType parsePossibleFilenameToGraphTag(umba::html::HtmlTag &parseTo, IteratorType b, IteratorType e)
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
inline
void updateGraphVizOptions(const umba::html::HtmlTag &mdHtmlTag, GraphVizOptions &graphVizOptions)
{
    // Атрибуты:
    //   file      - имя входного файла
    //   save-as   - имя файла для сохранения (без расширения) 
    //   scale     - масштаб
    //   type      - тип графа

    using namespace umba::filename;

    if (mdHtmlTag.hasAttr("file"))
    {
        auto attrVal = mdHtmlTag.getAttrValue("file", std::string());
        graphVizOptions.setSaveFileName(appendPath(getPath(attrVal), getName(attrVal)));
    }

    if (mdHtmlTag.hasAttr("save-as"))
    {
        auto attrVal = mdHtmlTag.getAttrValue("save-as", std::string());
        graphVizOptions.setSaveFileName(appendPath(getPath(attrVal), getName(attrVal)));
    }

    if (mdHtmlTag.hasAttr("scale"))
    {
        graphVizOptions.setScale(mdHtmlTag.getAttrValue("scale", std::string()));
    }

    if (mdHtmlTag.hasAttr("type"))
    {
        graphVizOptions.setGraphType(mdHtmlTag.getAttrValue("type", std::string()));
    }

}

//----------------------------------------------------------------------------

template<typename FilenameStringType>
void processGraphLines( const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType
                      , const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines
                      )
{
    // dot -Tsvg -s72 -o test001_72.svg test001.dot

    // Копируем опции graphVizOptions из appCfg
    // Обновляем их из тэга
    // Генерируем в savePath временный файл dot, и сохраняем туда наши строки
    // Генерируем в savePath имя временного файла, удаляем его, если существует
    // Вызываем dot или другую тулзу, генерируем картинку
    // Если временный файл с картинкой не существует - ошибка генерации
    // Выводим об этом сообщение
    // Но если файл с картинкой есть (с прошлого раза) - то делаем ссылку
    // Удаляем временные файлы

    // Оптимизация
    // Во временном каталоге заводим файл с хэшами входных данных
    // Если хэш не поменялся, и целевой файл есть, то делать ничего не надо


}
//----------------------------------------------------------------------------

} // namespace md
} // namespace umba


