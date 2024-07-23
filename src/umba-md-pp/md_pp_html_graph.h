#pragma once

#include "umba/umba.h"
#include "umba/filename.h"
#include "umba/filesys.h"
//
#include "md_pp_html.h"
#include "graph_viz_options.h"

#if defined(WIN32) || defined(_WIN32)
    #include "umba/win32_utils.h"
#endif


// For 'system' function
#include <process.h>


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
    updateGraphVizOptions(mdHtmlTag, graphVizOptions);

    auto outputFilename = graphVizOptions.generateOutputFilename();
    auto tempDotFile    = graphVizOptions.generateInputDotTempFilename();
    auto tempTargetFile = graphVizOptions.generateOutputTempFilename();
    auto hashFile       = graphVizOptions.generateHashFilename();

    auto outputFilenameCanonicalForCompare = umba::filename::makeCanonicalForCompare(outputFilename);


    std::vector<std::string> dotLines;
    for(auto tagLine : tagLines)
    {
        umba::string_plus::rtrim(tagLine);
        dotLines.emplace_back(tagLine);
    }

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
        umba::filesys::createDirectoryEx<std::string>( umba::filename::getPath(hashFile), true /* forceCreatePath */ );
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

        umba::filesys::createDirectoryEx<std::string>( umba::filename::getPath(tempDotFile), true /* forceCreatePath */ );
        umba::filesys::createDirectoryEx<std::string>( umba::filename::getPath(tempTargetFile), true /* forceCreatePath */ );

        if (!umba::filesys::writeFile(tempDotFile, dotText, true /* overwrite */ ))
        {
            // hasErrorWhileGenerating = true;
            errMsg = "Failed to write temporary DOT file";
        }
        else
        {
            // Записать DOT файл смогли, теперь надо вызвать генерацию
            std::string tool, args;
            if (!graphVizOptions.generateCommandLine(tool, args, tempDotFile, tempTargetFile))
            {
                errMsg = "Failed to generate DOT command line: possible unknown graph type?";
            }
            else
            {
                std::string toolExeName = tool;
                #if defined(WIN32) || defined(_WIN32)
                std::string graphvizBinPath;
                if (!umba::win32_utils::regQueryAppInstallLocationBin(std::string("Graphviz"), graphvizBinPath))
                {
                }
                toolExeName = escapeCommandLineArgument(umba::filename::appendPath(graphvizBinPath, umba::filename::appendExt(tool, std::string("exe"))));
                #endif

                std::string toolCommandLine = toolExeName + " " + args;
            
                int resCode = system(toolCommandLine.c_str());
                if (resCode!=0)
                {
                    errMsg = "Failed to calling '" + tool + "', result code: " + std::to_string(resCode) + ", command line: " + toolCommandLine;
                }
            }
        }
    }


    if (needDotProcessing && errMsg.empty())
    {
        umba::filesys::createDirectoryEx<std::string>( umba::filename::getPath(outputFilename), true /* forceCreatePath */ );
        if (!MoveFileExA( tempTargetFile.c_str(), outputFilename.c_str()
                        , MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH
                        )
           )
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

    resLines.emplace_back("![Graph](" + umba::filename::makeCanonical(imgLink, '/') + ")");

}
//----------------------------------------------------------------------------

} // namespace md
} // namespace umba


