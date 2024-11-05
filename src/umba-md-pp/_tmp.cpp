document.h:37
    std::vector<std::string> getTocLines() const

[X]
enums.h
    noGenerateToc                       = 0x1030,
    generateToc                         = 0x1031,

processing.h:329
            if (isTocCommand(line))
            {
                if (handler(LineHandlerEvent::tocCommand, resLines, line, idx, lastLineIdx))


processing.h:763
            case LineHandlerEvent::tocCommand:    checkPrintLineIfContainsPngExt(event, line);
                                                  return true;

processing.h:1649
template<typename FilenameStringType> inline
std::vector<std::string> processTocCommands(const AppConfig<FilenameStringType> &appCfg, const FilenameStringType &curFilename, Document &doc, const std::vector<std::string> &lines, bool &tocAdded)


processing.h:1926
    bool tocAdded = false;
    resLines = processTocCommands(appCfg, curFilename, doc, resLines, tocAdded);

    if (!tocAdded && appCfg.testProcessingOption(ProcessingOptions::generateToc))
    {


processing_utils.h:48
    m["#!toc"] = PreprocessorDirective::toc;
    m["#$toc"] = PreprocessorDirective::toc;


processing_utils.h:125
inline
bool isTocCommand(std::string line)
{






--------------
umba_md_processing_utils.h:34
template<typename StringType>
bool isUrlAbsolute(const StringType &strUrl)

template<typename StringType>
bool isUrlAbsoluteHostPath(StringType urlPath)

- заюзать при отбрасывании расширений

