#pragma once

#include "umba/umba.h"
#include "umba/filename.h"
#include "umba/filesys.h"
#include "umba/shellapi.h"

//
#include "md_pp_html.h"
#include "graph_viz_options.h"

#include "extern_tools.h"

// 
#include "log.h"

// For 'system' function
#include <process.h>


// umba::md::
namespace umba {
namespace md {

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename FilenameStringType>
void updateGraphVizOptions(const AppConfig<FilenameStringType> &appCfg, const umba::html::HtmlTag &mdHtmlTag, GraphVizOptions &graphVizOptions)
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

    if (appCfg.targetRenderer==TargetRenderer::doxygen)
    {
        if (mdHtmlTag.hasAttr("rtf-dpi"))
        {
            graphVizOptions.setDpi(mdHtmlTag.getAttrValue("rtf-dpi", std::string()));
        }
        if (mdHtmlTag.hasAttr("rtf-target-format"))
        {
            graphVizOptions.setTargetFormat(mdHtmlTag.getAttrValue("rtf-target-format", std::string()));
        }
    }

}

//----------------------------------------------------------------------------
inline
std::string grapvizLabelTextEscape(const std::string &text)
{
    std::string res; res.reserve(text.size());
    for(auto ch : text)
    {
        if (ch=='\\' || ch=='\"' || ch=='\n')
            res.append(1, '\\');
        res.append(1, ch);
    }

    return res;
}

//----------------------------------------------------------------------------
int grapvizGraphGetLineType(std::string line)
{
    umba::string_plus::trim(line);
    if (line.empty())
        return 2;
    if (umba::string_plus::starts_with_and_strip(line, "#") || umba::string_plus::starts_with_and_strip(line, "//")) // single line comment
        return 1;
    if (umba::string_plus::starts_with_and_strip(line, "/*")) // multiline comment
        return -1;

    return 0; // not a comment line
};

//----------------------------------------------------------------------------
inline
void grapvizAddGraphLabel(const std::string &labelText, std::vector<std::string> &tagLines)
{
    if (tagLines.empty() || labelText.empty())
        return;

    const static std::string graphStr = "graph";

    bool waitForBrace = false;
    std::size_t bracePos = std::string::npos;

    std::size_t idx = 0;

    for(; idx!=tagLines.size(); ++idx)
    {
        auto &line = tagLines[idx];

        int gvzLineType = grapvizGraphGetLineType(line);

        if (gvzLineType<0) // многостроч не умеем обрабатывать
            return;

        if (gvzLineType!=0) // single line comment or empty line
            continue;

        // не пустая и не комент строка
        if (!waitForBrace) // ещё ждём не скобку, а начало графа
        {
            // первая не пустая и не комент строка - тут должен начинаться граф
            std::size_t graphPos = line.find(graphStr);

            if (graphPos==line.npos) // Не нашли начало графа - что-то пошло не так
                return;

            bracePos = line.find('{', graphPos+graphStr.size());
            waitForBrace = true;
        }

        if (waitForBrace)
        {
            if (bracePos==std::string::npos) // на строчке с началом графа не нашли курли скобку, ищем с начала текущей
            {
                bracePos = line.find('{');
            }

            if (bracePos==std::string::npos)
                continue;

            // нашли скобку
            ++bracePos; // едем дальше

            bool needSemicolon = umba::html::helpers::skipSpaces(line.begin()+bracePos, line.end())!=line.end();

            std::string textToInsert = " label=\"" + grapvizLabelTextEscape(labelText) + "\"" + (needSemicolon?"; ":"");

            line.insert(bracePos, textToInsert);

            return; // Bingo!
        }

    } // for

}


// graph     :     [ strict ] (graph | digraph) [ ID ] '{' stmt_list '}'
// digraph { a -> b }
// strict graph {
// digraph graphname {

// graph {
//   label="Vincent van Gogh Paintings"

// label is escString - https://graphviz.org/docs/attr-types/escString/


template<typename FilenameStringType>
void processGraphLines( const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType
                      , const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines
                      )
{
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

    // dot -Tsvg -s72 -o test001_72.svg test001.dot

    auto graphVizOptions = appCfg.graphVizOptions;
    updateGraphVizOptions(appCfg, mdHtmlTag, graphVizOptions);

    auto outputFilename = graphVizOptions.generateOutputFilename(appCfg.flattenImageLinks);
    auto tempDotFile    = graphVizOptions.generateInputDotTempFilename();
    auto tempTargetFile = graphVizOptions.generateOutputTempFilename();
    auto hashFile       = graphVizOptions.generateHashFilename();

    auto outputFilenameCanonicalForCompare = umba::filename::makeCanonicalForCompare(outputFilename);


    std::vector<std::string> dotLines;

    std::string graphvizTool;
    std::vector<std::string> graphvizToolArgs;
    if (!graphVizOptions.generateCommandLineArgs(appCfg, graphvizTool, graphvizToolArgs, tempDotFile, tempTargetFile))
    {
        resLines.emplace_back("# Failed to generate DOT command line: possible unknown graph type?");
        return;
    }

    // dotLines.emplace_back("// " + makeSystemFunctionCommandString(graphvizTool, graphvizToolArgs));
    dotLines.emplace_back("// " + umba::shellapi::makeSystemFunctionCommandString(graphvizTool, graphvizToolArgs));
    dotLines.emplace_back(std::string());

    for(auto tagLine : tagLines)
    {
        umba::string_plus::rtrim(tagLine); // Обрезаем справа, чтобы незначащие пробелы не
        dotLines.emplace_back(tagLine);
    }

    if (graphVizOptions.showLabels && mdHtmlTag.hasAttr("title"))
       grapvizAddGraphLabel(mdHtmlTag.getAttrValue("title"), dotLines);


    std::string dotText    = marty_cpp::mergeLines(dotLines, appCfg.outputLinefeed, true  /* addTrailingNewLine */ );
    std::size_t dotHash    = std::hash<std::string>{}(dotText);
    std::string dotHashStr = std::to_string(dotHash);

    bool needDotProcessing = true;

    std::string hashFileText;

    std::vector<std::string> newHashFileLines;
    bool needWriteHashLines = true;
    bool hashFound = false;

    if (!AppConfig<std::string>::readInputFile(hashFile, hashFileText))
    {
        needWriteHashLines = true;
    }
    else
    {
        std::vector<std::string> hashFileLines = marty_cpp::splitToLinesSimple(hashFileText);

        for(auto hashFileLine : hashFileLines)
        {
            if (hashFound)
            {
                newHashFileLines.emplace_back(hashFileLine);
                continue;
            }
            umba::string_plus::trim(hashFileLine);
            if (hashFileLine.empty())
            {
                continue; // пропускаем пустые строки
            }
            if (hashFileLine[0]=='#' || hashFileLine[0]==';')
            {
                newHashFileLines.emplace_back(hashFileLine);
                continue; // пропускаем коменты
            }

            std::string hashStr;
            std::string filenameStr;
            if (!umba::string_plus::split_to_pair(hashFileLine, hashStr, filenameStr, ' '))
            {
                newHashFileLines.emplace_back(hashFileLine);
                continue; // Что-то пошло не так
            }

            umba::string_plus::trim(hashStr);
            umba::string_plus::trim(filenameStr);

            auto cmpName = umba::filename::makeCanonicalForCompare(filenameStr);
            if (cmpName==outputFilenameCanonicalForCompare)
            {
                hashFound = true;

                // Файл - найден
                if (hashStr==dotHashStr) // Хэш - такой же
                {
                    needWriteHashLines = false; // Обновлять не надо
                    if (umba::filesys::isPathExist(filenameStr) && umba::filesys::isPathFile(filenameStr))
                    {
                        needDotProcessing  = false; // Ничего генерить не надо, граф не поменялся, файл на месте
                    }
                    break; // выходим из цикла, потому как newHashFileLines не надо больше обновлять
                }
                else // хэш не сошелся, надо перегенерить картинку и обновить строчку хэша в файле
                {
                    needWriteHashLines = true;
                    needDotProcessing  = true;
                    newHashFileLines.emplace_back(dotHashStr + " " + outputFilename); // Обновляем строчку хэша
                    continue;
                }
            }
            else
            {
                newHashFileLines.emplace_back(hashFileLine);
            }

        } // for(hashFileLine : hashFileLines)

    }

    if (!hashFound)
    {
        newHashFileLines.emplace_back(dotHashStr + " " + outputFilename); // Добавляем строчку хэша
        needWriteHashLines = true;
        needDotProcessing  = true;
    }

    if (needWriteHashLines)
    {
        std::string hashFileText = marty_cpp::mergeLines(newHashFileLines, appCfg.outputLinefeed, true  /* addTrailingNewLine */ );
        umba::filesys::createDirectoryEx( umba::filename::getPath(hashFile), true /* forceCreatePath */ );
        if (!umba::filesys::writeFile(hashFile, hashFileText, true /* overwrite */ ))
        {
            // Плевать на результат
        }
    }

    //bool hasErrorWhileGenerating = false;
    std::string errMsg;

    // Теперь нам надо отпроцессить
    if (needDotProcessing)
    {
        // Старые временные файлы нам не нужны, даже если остались с прошлого запуска
        umba::filesys::deleteFile(tempDotFile);
        umba::filesys::deleteFile(tempTargetFile);

        umba::filesys::createDirectoryEx( umba::filename::getPath(tempDotFile), true /* forceCreatePath */ );
        umba::filesys::createDirectoryEx( umba::filename::getPath(tempTargetFile), true /* forceCreatePath */ );

        if (!umba::filesys::writeFile(tempDotFile, dotText, true /* overwrite */ ))
        {
            // hasErrorWhileGenerating = true;
            errMsg = "Failed to write temporary DOT file";
        }
        else
        {
            // // Записать DOT файл смогли, теперь надо вызвать генерацию
            // std::string graphvizTool, graphvizToolArgs;
            // if (!graphVizOptions.generateCommandLine(graphvizTool, graphvizToolArgs, tempDotFile, tempTargetFile))
            // {
            //     errMsg = "Failed to generate DOT command line: possible unknown graph type?";
            // }
            // else
            {
                std::string toolExeName     = findGraphvizToolExecutableName(appCfg.dontLookupForGraphviz, graphvizTool);
                //std::string toolCommandLine = toolExeName + " " + graphvizToolArgs;

                std::string errMsg;
                //int resCode = system(toolCommandLine.c_str());
                int resCode = umba::shellapi::callSystem(toolExeName, graphvizToolArgs, &errMsg);
                if (resCode!=0)
                {
                    errMsg = "Failed to calling '" + graphvizTool + "', message: " + std::to_string(resCode)
                        + ", command line: " + umba::shellapi::makeSystemFunctionCommandString(toolExeName, graphvizToolArgs);
                }
            }
        }
    }


    if (needDotProcessing && errMsg.empty())
    {
        umba::filesys::createDirectoryEx( umba::filename::getPath(outputFilename), true /* forceCreatePath */ );
        using umba::shellapi::MoveFileFlags;
        if (!umba::shellapi::moveFile(tempTargetFile, outputFilename, MoveFileFlags::copyAllowed|MoveFileFlags::replaceExisting|MoveFileFlags::writeThrough))
        {
            errMsg = "Failed to copy temp file '" + tempTargetFile + "' to target file '" + outputFilename + "', error: " + std::to_string(GetLastError());
        }
    }


    if (!errMsg.empty())
    {
        resLines.emplace_back("# " + errMsg);
    }

    if (errMsg.empty())
    {
        if (!graphVizOptions.keepTempDotFiles)
            umba::filesys::deleteFile(tempDotFile);
        umba::filesys::deleteFile(tempTargetFile);
    }


    std::string imgLink;

    umba::filename::makeRelPath( imgLink
                               , umba::filename::getPath(docFilename)
                               , outputFilename
                               , '/'
                               , std::string(".")
                               , std::string("..")
                               , true // keepLeadingParents
                               , true // tryReverseRelPath
                               );

    if (imgLink.empty())
    {
        imgLink = umba::filename::getFileName(outputFilename);
    }

    resLines.emplace_back("![" + mdHtmlTag.getAttrValue("title", "Graph") + "](" + umba::filename::makeCanonical(imgLink, '/') + ")");

}
//----------------------------------------------------------------------------

} // namespace md
} // namespace umba


