#pragma once

#include "umba/umba.h"
#include "umba/filename.h"
#include "umba/filesys.h"
#include "umba/shellapi.h"

//
#include "md_pp_html.h"
#include "plantuml_options.h"

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
        if (mdHtmlTag.hasAttr("rtf-scale"))
        {
            plantUmlOptions.setScale(mdHtmlTag.getAttrValue("rtf-scale", std::string()));
        }
        if (mdHtmlTag.hasAttr("rtf-target-format"))
        {
            plantUmlOptions.setTargetFormat(mdHtmlTag.getAttrValue("rtf-target-format", std::string()));
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
            LOG_INFO("plantuml") << "Found '@startXXX' expression in input lines\n";
            hasStart = true;
            ++insertPos; // Вставляем после текущей строки, которая @startXXX
        }

        if (!labelText.empty())
        {
            std::string textToInsert = plantUmlLabelTextEscape(labelText);
            tagLines.insert(tagLines.begin()+insertPos, textToInsert); 
            ++insertPos;
            LOG_INFO("plantuml") << "Inserting label to diagram: " << textToInsert << "\n";
        }

        if (scale!=100)
        {
            std::string scaleStr = "scale " + std::to_string(scale) + "/100";
            tagLines.insert(tagLines.begin()+insertPos, scaleStr); 
            LOG_INFO("plantuml") << "Inserting scale to diagram: " << scaleStr << "\n";
        }

        if (!hasStart)
        {
            std::string diagramTypeStr = enum_serialize(diagramType);
            auto strStart = "@start"+diagramTypeStr;
            auto strEnd   = "@end"+diagramTypeStr;
            tagLines.insert(tagLines.begin(), strStart); 
            tagLines.insert(tagLines.end()  , strEnd  ); 
            LOG_INFO("plantuml") << "Inserting '" << strStart << "' and '" << strEnd << "' to diagram\n";
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
    LOG_INFO("plantuml") << "-----------------------------------------" << "\n";
    LOG_INFO("plantuml") << "PlantUML generator configuration (Global)" << "\n";
    plantUmlOptions.logConfig();

    LOG_INFO("plantuml") << "-----------------------------------------" << "\n";

    if (mdHtmlTag.hasAttr("title"))
       LOG_INFO("plantuml") << "Diagram: " << mdHtmlTag.getAttrValue("title", std::string()) << "\n";
    else
       LOG_INFO("plantuml") << "Diagram: Anonimous Diagram" << "\n";

    if (mdHtmlTag.hasAttr("file"))
       LOG_INFO("plantuml") << "From file: " << mdHtmlTag.getAttrValue("file", std::string()) << "\n";


    //------------------------------
    updatePlantUmlOptions(appCfg, mdHtmlTag, plantUmlOptions);
    //------------------------------


    LOG_INFO("plantuml") << "-----------------------------------------" << "\n";
    LOG_INFO("plantuml") << "PlantUML generator configuration (Diagram)" << "\n";
    plantUmlOptions.logConfig();


    LOG_INFO("plantuml") << "-----------------------------------------" << "\n";
    auto outputFilename   = plantUmlOptions.generateOutputFilename(appCfg.flattenImageLinks);
    auto tempPumlFile     = plantUmlOptions.generateInputTempFilename();
    auto tempTargetFolder = plantUmlOptions.generateOutputTempFolderName();
    auto hashFile         = plantUmlOptions.generateHashFilename();

    LOG_INFO("plantuml") << "Output Filename    : " << outputFilename << "\n";
    LOG_INFO("plantuml") << "Temp Puml File     : " << tempPumlFile<< "\n";
    LOG_INFO("plantuml") << "Temp Target Folder : " << tempTargetFolder << "\n";
    LOG_INFO("plantuml") << "Hash File          : " << hashFile << "\n";
    LOG_INFO("plantuml") << "-----------------------------------------" << "\n";


    auto outputFilenameCanonicalForCompare = umba::filename::makeCanonicalForCompare(outputFilename);


    std::string pumlTool;
    std::vector<std::string> pumlToolArgs;
    if (!plantUmlOptions.generateCommandLineArgs(appCfg, pumlTool, pumlToolArgs, tempPumlFile, tempTargetFolder))
    {
        resLines.emplace_back("# Failed to generate PlantUML command line: possible unknown diagram type, or JAVA/JAVA_HOME or PLANTUML/PLANTUML_JAR environment variables not set");
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
        if (mdHtmlTag.hasAttr("title"))
            labelText = mdHtmlTag.getAttrValue("title");
        if (!plantUmlOptions.showLabels)
            labelText.clear();
        plantUmlAddDiagramLabelScaleAndPlantTags(labelText, plantUmlOptions.scale, plantUmlOptions.diagramType, pumlLines);
    }

    LOG_INFO("plantuml") << "------- PlantUML Diagram text" << "\n";
    for(const auto &l : pumlLines)
    {
        LOG_INFO("plantuml") << l << "\n";
    }
    LOG_INFO("plantuml") << "-------" << "\n";

    std::string pumlText    = marty_cpp::mergeLines(pumlLines, appCfg.outputLinefeed, true  /* addTrailingNewLine */ );
    std::size_t pumlHash    = std::hash<std::string>{}(pumlText);
    std::string pumlHashStr = std::to_string(pumlHash);

    bool needPumlProcessing = true;

    std::string hashFileText;

    std::vector<std::string> newHashFileLines;
    bool needWriteHashLines = true;
    bool hashFound = false;

    if (!AppConfig<std::string>::readInputFile(hashFile, hashFileText))
    {
        needWriteHashLines = true;
        LOG_INFO("plantuml") << "Failed to read hash file: '" << hashFile << "'\n";
    }
    else
    {
        LOG_INFO("plantuml") << "Hashes readed from '" << hashFile << "'\n";
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
                if (hashStr==pumlHashStr) // Хэш - такой же
                {
                    needWriteHashLines = false; // Обновлять не надо
                    if (umba::filesys::isPathExist(filenameStr) && umba::filesys::isPathFile(filenameStr))
                    {
                        needPumlProcessing  = false; // Ничего генерить не надо, граф не поменялся, файл на месте
                    }
                    break; // выходим из цикла, потому как newHashFileLines не надо больше обновлять
                }
                else // хэш не сошелся, надо перегенерить картинку и обновить строчку хэша в файле
                {
                    needWriteHashLines = true;
                    needPumlProcessing  = true;
                    newHashFileLines.emplace_back(pumlHashStr + " " + outputFilename); // Обновляем строчку хэша
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
        LOG_INFO("plantuml") << "Diagram hash not found, need to generate image\n";
        newHashFileLines.emplace_back(pumlHashStr + " " + outputFilename); // Добавляем строчку хэша
        needWriteHashLines = true;
        needPumlProcessing  = true;
    }

    if (needWriteHashLines)
    {
        LOG_INFO("plantuml") << "Saving hashes to '" << hashFile << "'\n";
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
    if (needPumlProcessing)
    {

    // auto outputFilename   = plantUmlOptions.generateOutputFilename(appCfg.flattenImageLinks);
    // auto tempPumlFile     = plantUmlOptions.generateInputTempFilename();
    // auto tempTargetFolder = plantUmlOptions.generateOutputTempFolderName();
    // auto hashFile         = plantUmlOptions.generateHashFilename();

        LOG_INFO("plantuml") << "Deleting old temp files and folders\n";
        LOG_INFO("plantuml") << "Deleting file  : '" << tempPumlFile << "'\n";
        // Старые временные файлы нам не нужны, даже если остались с прошлого запуска
        if (!umba::filesys::deleteFile(tempPumlFile))
        {
            auto lastErr = umba::shellapi::getLastError();
            if (umba::filesys::isPathExist(tempPumlFile))
            {
                LOG_WARN("plantuml") << "Failed to delete file: '" << tempPumlFile << "': " << umba::shellapi::getErrorMessage(lastErr) << "\n";
            }
        }
        
        LOG_INFO("plantuml") << "Deleting folder: '" << tempTargetFolder << "'\n";
        //umba::filesys::deleteFile(tempTargetFile);
        if (!umba::shellapi::deleteDirectory(tempTargetFolder))
        {
            auto lastErr = umba::shellapi::getLastError();
            if (umba::filesys::isPathExist(tempTargetFolder))
            {
                LOG_WARN("plantuml") << "Failed to delete folder: '" << tempTargetFolder << "': " << umba::shellapi::getErrorMessage(lastErr) << "\n";
            }
        }

        LOG_INFO("plantuml") << "Creating temp files and folders\n";
        {
            auto tempPumlFolder = umba::filename::getPath(tempPumlFile);
            LOG_INFO("plantuml") << "Creating folder: '" << tempPumlFolder << "'\n";
    
            if (!umba::filesys::createDirectoryEx( tempPumlFolder, true /* forceCreatePath */ ))
            {
                auto lastErr = umba::shellapi::getLastError();
                if (!umba::filesys::isPathExist(tempPumlFolder))
                {
                    LOG_WARN("plantuml") << "Failed to creating folder: '" << tempPumlFolder << "': " << umba::shellapi::getErrorMessage(lastErr) << "\n";
                }
            }
    
            LOG_INFO("plantuml") << "Creating folder: '" << tempTargetFolder << "'\n";
            if (!umba::filesys::createDirectoryEx( tempTargetFolder, true /* forceCreatePath */ ))
            {
                auto lastErr = umba::shellapi::getLastError();
                if (!umba::filesys::isPathExist(tempTargetFolder))
                {
                    LOG_WARN("plantuml") << "Failed to creating folder: '" << tempTargetFolder << "': " << umba::shellapi::getErrorMessage(lastErr) << "\n";
                }
            }
        }


        LOG_INFO("plantuml") << "Writing temporary PUML file: '" << tempPumlFile << "'\n";
        if (!umba::filesys::writeFile(tempPumlFile, pumlText, true /* overwrite */ ))
        {
            // hasErrorWhileGenerating = true;
            errMsg = "Failed to write temporary PUML file";
            LOG_WARN("plantuml") << "Failed to write temporary PUML file: '" << tempPumlFile << "'\n";
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


                std::string cmdLine = umba::shellapi::makeSystemFunctionCommandString(pumlTool, pumlToolArgs);

                LOG_INFO("plantuml") << "Calling PlantUML diagram generator, command line: " << cmdLine << "\n";

                std::string errMsg;
                //int resCode = system(toolCommandLine.c_str());
                int resCode = umba::shellapi::callSystem(pumlTool, pumlToolArgs, &errMsg);
                if (resCode!=0)
                {
                    LOG_WARN("plantuml") << "Failed to call PlantUML diagram generator, command line: " << cmdLine << "\n";
                    errMsg = "Failed to calling JAVA for PlantUML , message: " + std::to_string(resCode)
                        + ", command line: " + cmdLine;
                }
            }
        }
    }

    std::vector<std::string> resultFileNames;
    std::size_t resulFileCopyErrCount = 0;

    if (needPumlProcessing && errMsg.empty())
    {
        LOG_INFO("plantuml") << "-----------------------------------------" << "\n";
        LOG_INFO("plantuml") << "Looking for generated files\n";
    
        // Тут надо пройтись по tempTargetFolder и собрать все появившиеся там файлы
        std::vector<std::string> foundNewFiles;
        umba::filesys::enumerateDirectory( tempTargetFolder
                                         , [&](const std::string &fileName, const umba::filesys::FileStat &fileStat)
                                           {
                                               if (fileStat.isFile())
                                               {
                                                   auto newFile = umba::filename::appendPath(tempTargetFolder, fileName);
                                                   foundNewFiles.emplace_back(newFile);
                                                   LOG_INFO("plantuml") << "Found generated file: " << newFile << "\n";
                                               }

                                               return true; // continue file enumeration
                                           }
                                         );

        if (foundNewFiles.empty())
        {
            LOG_WARN("plantuml") << "No generated files found\n";
            errMsg = "No output files found";
        }
        else
        {
            // !!!
            // Разобрать имя выходного файла и для каждого найденного файла генерить выходное имя с индексом

            auto outputPathFile = umba::filename::getPathFile(outputFilename);
            auto outputExt      = umba::filename::getExt(outputFilename);
            std::size_t idx     = 0;
            std::size_t numDigitsMax = 1;
            if (foundNewFiles.size()>1000)
                numDigitsMax = 4;
            else if (foundNewFiles.size()>100)
                numDigitsMax = 3;
            else if (foundNewFiles.size()>10)
                numDigitsMax = 2;


            LOG_INFO("plantuml") << "-----------------------------------------" << "\n";

            auto targetFilesFolder = umba::filename::getPath(outputFilename);
            LOG_INFO("plantuml") << "Creating target folder: " << targetFilesFolder << "\n";
            if (!umba::filesys::createDirectoryEx( targetFilesFolder, true /* forceCreatePath */ ))
            {
                auto lastErr = umba::shellapi::getLastError();
                if (!umba::filesys::isPathExist(targetFilesFolder))
                {
                    LOG_WARN("plantuml") << "Failed to create target folder: '" << targetFilesFolder << "': " << umba::shellapi::getErrorMessage(lastErr) << "\n";
                }
            }

            for(const auto &newTmpFile : foundNewFiles)
            {
                auto resultFileName = outputFilename;

                std::string strIdx;
                if (foundNewFiles.size()>1)
                {
                    strIdx = std::to_string(idx);
                    if (strIdx.size()<numDigitsMax)
                    {
                        strIdx = std::string(numDigitsMax-strIdx.size(), '0') + strIdx;
                    }

                    resultFileName = umba::filename::appendExt(outputPathFile+"_"+strIdx, outputExt);
                }

                LOG_INFO("plantuml") << "Moving file to target location: " << newTmpFile << " -> " << resultFileName << "\n";
                using umba::shellapi::MoveFileFlags;
                if (!umba::shellapi::moveFile(newTmpFile, resultFileName, MoveFileFlags::copyAllowed|MoveFileFlags::replaceExisting|MoveFileFlags::writeThrough))
                {
                    //errMsg = "Failed to copy temp file '" + tempTargetFile + "' to target file '" + outputFilename + "', error: " + std::to_string(GetLastError());
                    ++resulFileCopyErrCount;
                    LOG_INFO("plantuml") << "Failed to move file to target location: " << newTmpFile << " -> " << resultFileName << "\n";
                }
                else
                {
                    resultFileNames.emplace_back(resultFileName);
                }

            } // for(const auto &newTmpFile : foundNewFiles)

        } // if (foundNewFiles.empty())

    } // if (needPumlProcessing && errMsg.empty())
    else
    {
        if (!needPumlProcessing)
        {
            LOG_INFO("plantuml") << "No need to call PlantUML generator - no changes found in diagram\n";
        }
        else if (!errMsg.empty())
        {
            LOG_INFO("plantuml") << "PlantUML generator not called - there is some errors occured\n";
        }
    }

            // auto lastErr = umba::shellapi::getLastError();
            // if (umba::filesys::isPathExist(tempPumlFile))
            // {
            //     LOG_WARN("plantuml") << "Failed to delete file: '" << tempPumlFile << "': " << umba::shellapi::getErrorMessage(lastErr) << "\n";
            // }

    LOG_INFO("plantuml") << "-----------------------------------------" << "\n";
    // Тут надо удалить tempTargetFolder со всем его содержимым
    LOG_INFO("plantuml") << "Deleting temp files and folders\n";
    LOG_INFO("plantuml") << "Deleting file  : '" << tempPumlFile << "'\n";
    // Старые временные файлы нам не нужны, даже если остались с прошлого запуска
    if (!umba::filesys::deleteFile(tempPumlFile))
    {
        auto lastErr = umba::shellapi::getLastError();
        if (umba::filesys::isPathExist(tempPumlFile))
        {
            LOG_WARN("plantuml") << "Failed to delete file: '" << tempPumlFile << "': " << umba::shellapi::getErrorMessage(lastErr) << "\n";
        }
    }
    
    LOG_INFO("plantuml") << "Deleting folder: '" << tempTargetFolder << "'\n";
    //umba::filesys::deleteFile(tempTargetFile);
    if (!umba::shellapi::deleteDirectory(tempTargetFolder))
    {
        auto lastErr = umba::shellapi::getLastError();
        if (umba::filesys::isPathExist(tempTargetFolder))
        {
            LOG_WARN("plantuml") << "Failed to delete folder: '" << tempTargetFolder << "': " << umba::shellapi::getErrorMessage(lastErr) << "\n";
        }
    }

    if (errMsg.empty() && resulFileCopyErrCount)
    {
        errMsg = "Some errors occurs while copying files to final location";
    }

    if (!errMsg.empty())
    {
        resLines.emplace_back("# " + errMsg);
    }

    LOG_INFO("plantuml") << "-----------------------------------------" << "\n";
    LOG_INFO("plantuml") << "Generating links\n";

    auto docPath = umba::filename::getPath(docFilename);
    for(const auto &resultFile : resultFileNames)
    {
        std::string imgLink;

        LOG_INFO("plantuml") << "Generating link for '" << resultFile << "'\n";
    
        umba::filename::makeRelPath( imgLink
                                   , docPath
                                   , resultFile
                                   , '/'
                                   , std::string(".")
                                   , std::string("..")
                                   , true // keepLeadingParents
                                   , true // tryReverseRelPath
                                   );
    
        if (imgLink.empty())
        {
            LOG_WARN("plantuml") << "Failed to make relative link, using full file name: '" << resultFile << "'\n";
            imgLink = umba::filename::getFileName(outputFilename);
        }
    
        auto linkText = mdHtmlTag.getAttrValue("title", "Diagram");
        LOG_INFO("plantuml") << "Adding link, text: '" << linkText << "', link: '" << imgLink << "'\n";
        resLines.emplace_back("![" + linkText + "](" + umba::filename::makeCanonical(imgLink, '/') + ")");
    
    }

    LOG_INFO("plantuml") << "----------------------------------------------------------------------------------" << "\n";

}
//----------------------------------------------------------------------------

} // namespace md
} // namespace umba

