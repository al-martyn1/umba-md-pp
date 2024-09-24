#pragma once

#include "umba/umba.h"
#include "umba/filename.h"
#include "umba/filesys.h"
#include "umba/shellapi.h"

//
#include "md_pp_html.h"
#include "plantuml_options.h"

#include "extern_tools.h"

// For 'system' function
#include <process.h>


// umba::md::
namespace umba {
namespace md {

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename FilenameStringType>
void updatePlantUmlOptions(const AppConfig<FilenameStringType> &appCfg, const umba::html::HtmlTag &mdHtmlTag, PlantUmlOptions &plantUmlOptions)
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
        plantUmlOptions.setSaveFileName(appendPath(getPath(attrVal), getName(attrVal)));
    }

    if (mdHtmlTag.hasAttr("save-as"))
    {
        auto attrVal = mdHtmlTag.getAttrValue("save-as", std::string());
        plantUmlOptions.setSaveFileName(appendPath(getPath(attrVal), getName(attrVal)));
    }

    if (mdHtmlTag.hasAttr("scale"))
    {
        plantUmlOptions.setScale(mdHtmlTag.getAttrValue("scale", std::string()));
    }

    if (mdHtmlTag.hasAttr("type"))
    {
        plantUmlOptions.setDiagramType(mdHtmlTag.getAttrValue("type", std::string()));
    }

    if (appCfg.targetRenderer==TargetRenderer::doxygen)
    {
        // if (mdHtmlTag.hasAttr("rtf-dpi"))
        // {
        //     graphVizOptions.setDpi(mdHtmlTag.getAttrValue("rtf-dpi", std::string()));
        // }
        if (mdHtmlTag.hasAttr("rtf-target-format"))
        {
            graphVizOptions.setTargetFormat(mdHtmlTag.getAttrValue("rtf-target-format", std::string()));
        }
    }

}

//----------------------------------------------------------------------------
inline
std::string plantUmlLabelTextEscape(const std::string &text)
{
    std::string res; res.reserve(text.size());
    for(auto ch : text)
    {
        if (ch=='\\'  /* || ch=='\"' */  || ch=='\n')
            res.append(1, '\\');
        res.append(1, ch);
    }

    return res;
}

//----------------------------------------------------------------------------
int plantUmlDiagramGetLineType(std::string line)
{
    umba::string_plus::trim(line);
    if (line.empty())
        return 2;
    if (umba::string_plus::starts_with_and_strip(line, "'")) // single line comment
        return 1;
    if (umba::string_plus::starts_with_and_strip(line, "/'")) // multiline comment
        return -1;

    return 0; // not a comment line
};

//----------------------------------------------------------------------------
inline
void plantUmlAddDiagramLabelScaleAndPlantTags(const std::string &labelText, unsigned scale, PlantUmlDiagramType diagramType, std::vector<std::string> &tagLines)
{
    if (tagLines.empty())
        return;

    const static std::string diagramStartStr = "@start";

    std::size_t idx = 0;

    for(; idx!=tagLines.size(); ++idx)
    {
        auto &line = tagLines[idx];

        int pumlLineType = plantUmlDiagramGetLineType(line);

        if (pumlLineType<0) // многостроч не умеем обрабатывать
            return;

        if (pumlLineType!=0) // single line comment or empty line
            continue;


        bool hasStart = false;
        std::size_t insertPos = idx; // вставляем перед текущей строкой
        //scale
        if (umba::string_plus::starts_with(line, diagramStartStr))
        {
            hasStart = true;
            ++insertPos; // Вставляем после текущей строки, которая @startXXX
        }

        if (!labelText.empty())
        {
            std::string textToInsert = plantUmlLabelTextEscape(labelText);
            tagLines.insert(tagLines.begin()+insertPos, textToInsert); 
            ++insertPos;
        }

        if (scale!=100)
        {
            std::string scaleStr = "scale " + std::to_string(scale) + "/100";
            tagLines.insert(tagLines.begin()+insertPos, textToInsert); 
        }

        if (!hasStart)
        {
            std::string diagramTypeStr = enum_serialize(diagramType);
            tagLines.insert(tagLines.begin(), "@start"+diagramTypeStr); 
            tagLines.insert(tagLines.end()  , "@end"+diagramTypeStr); 
        }

        return;
    } // for
}

template<typename FilenameStringType>
void processDiagramLines( const AppConfig<FilenameStringType> &appCfg, umba::html::HtmlTag &mdHtmlTag, MdPpTag tagType
                        , const FilenameStringType &docFilename, const std::vector<std::string> &tagLines, std::vector<std::string> &resLines
                        )
{
    // Копируем опции plantUmlOptions из appCfg
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

    auto plantUmlOptions = appCfg.plantUmlOptions;
    updatePlantUmlOptions(appCfg, mdHtmlTag, plantUmlOptions);

    auto outputFilename   = plantUmlOptions.generateOutputFilename(appCfg.flattenImageLinks);
    auto tempPumlFile     = plantUmlOptions.generateInputTempFilename();
    auto tempTargetFolder = plantUmlOptions.generateOutputTempFolderName();
    auto hashFile         = plantUmlOptions.generateHashFilename();

    auto outputFilenameCanonicalForCompare = umba::filename::makeCanonicalForCompare(outputFilename);


    std::string pumlTool;
    std::vector<std::string> pumlToolArgs;
    if (!plantUmlOptions.generateCommandLineArgs(appCfg, pumlTool, pumlToolArgs, tempPumlFile, tempTargetFolder))
    {
        resLines.emplace_back("# Failed to generate PlantUML command line: possible unknown diagram type?");
        return;
    }

    std::vector<std::string> pumlLines;

    // dotLines.emplace_back("// " + makeSystemFunctionCommandString(graphvizTool, graphvizToolArgs));
    pumlLines.emplace_back("' " + umba::shellapi::makeSystemFunctionCommandString(pumlTool, pumlToolArgs));
    pumlLines.emplace_back(std::string());

    for(auto tagLine : tagLines)
    {
        umba::string_plus::rtrim(tagLine); // Обрезаем справа, чтобы незначащие пробелы не
        pumlLines.emplace_back(tagLine);
    }

    {
        std::string labelText;
        if (mdHtmlTag.hasAttr("text"))
            labelText = mdHtmlTag.getAttrValue("text");
        if (!graphVizOptions.showLabels)
            labelText.clear();
        plantUmlAddDiagramLabelScaleAndPlantTags(labelText, plantUmlOptionsscale, plantUmlOptions.diagramType, pumlLines);
    }


    std::string pumlText    = marty_cpp::mergeLines(pumlLines, appCfg.outputLinefeed, true  /* addTrailingNewLine */ );
    std::size_t pumlHash    = std::hash<std::string>{}(pumlText);
    std::string pumlHashStr = std::to_string(dotHash);

    bool needPumlProcessing = true;

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

    // auto outputFilename   = plantUmlOptions.generateOutputFilename(appCfg.flattenImageLinks);
    // auto tempPumlFile     = plantUmlOptions.generateInputTempFilename();
    // auto tempTargetFolder = plantUmlOptions.generateOutputTempFolderName();
    // auto hashFile         = plantUmlOptions.generateHashFilename();

        // Старые временные файлы нам не нужны, даже если остались с прошлого запуска
        umba::filesys::deleteFile(tempPumlFile);
        //umba::filesys::deleteFile(tempTargetFile);
        umba::shellapi::deleteDirectory(tempTargetFolder);

        umba::filesys::createDirectoryEx( umba::filename::getPath(tempPumlFile), true /* forceCreatePath */ );
        umba::filesys::createDirectoryEx( tempTargetFolder, true /* forceCreatePath */ );

        if (!umba::filesys::writeFile(tempPumlFile, pumlText, true /* overwrite */ ))
        {
            // hasErrorWhileGenerating = true;
            errMsg = "Failed to write temporary PUML file";
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
                //std::string toolExeName     = findGraphvizToolExecutableName(appCfg.dontLookupForGraphviz, graphvizTool);
                //std::string toolCommandLine = toolExeName + " " + graphvizToolArgs;

                

                std::string errMsg;
                //int resCode = system(toolCommandLine.c_str());
                int resCode = umba::shellapi::callSystem(pumlTool, pumlToolArgs, &errMsg);
                if (resCode!=0)
                {
                    errMsg = "Failed to calling JAVA for PlantUML , message: " + std::to_string(resCode)
                        + ", command line: " + umba::shellapi::makeSystemFunctionCommandString(pumlTool, pumlToolArgs);
                }
            }
        }
    }


    if (needDotProcessing && errMsg.empty())
    {
        // Тут надо пройтись по tempTargetFolder и собрать все появившиеся там файлы

        std::vector<std::string> foundNewFiles;

        umba::filesys::enumerateDirectory( tempTargetFolder
                                         , [&](const std::string &fileName, const umba::filesys::FileStat &fileStat)
                                           {
                                               if (fileStat.isFile())
                                               {
                                                   foundNewFiles.emplace_back(umba::filename::appendPath(tempTargetFolder, fileName));
                                               }

                                               return true; // continue file enumeration
                                           }
                                         );

        if (foundNewFiles.empty())                                 
        {
            errMsg = "No output files found";
        }
        else
        {
            // !!!
            // Разобрать имя выходного файла и для каждого найденного файла генерить выходное имя с индексом
            umba::filesys::createDirectoryEx( umba::filename::getPath(outputFilename), true /* forceCreatePath */ );
            using umba::shellapi::MoveFileFlags;
            if (!umba::shellapi::moveFile(tempTargetFile, outputFilename, MoveFileFlags::copyAllowed|MoveFileFlags::replaceExisting|MoveFileFlags::writeThrough))
            {
                errMsg = "Failed to copy temp file '" + tempTargetFile + "' to target file '" + outputFilename + "', error: " + std::to_string(GetLastError());
            }
        }
    }

    // Тут надо удалить tempTargetFolder со всем его содержимым


    if (!errMsg.empty())
    {
        resLines.emplace_back("# " + errMsg);
    }

    if (errMsg.empty())
    {
        // if (!graphVizOptions.keepTempDotFiles)
        //     umba::filesys::deleteFile(tempDotFile);
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

    resLines.emplace_back("![" + mdHtmlTag.getAttrValue("text", "Graph") + "](" + umba::filename::makeCanonical(imgLink, '/') + ")");

}
//----------------------------------------------------------------------------

} // namespace md
} // namespace umba


