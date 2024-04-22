#pragma once


//#include "app_config.h"
#include "umba/cmd_line.h"
// #include "textUtils.h"
//  
// #if defined(WIN32) || defined(_WIN32)
//     #include <shellapi.h>
// #endif

#include <stack>


// AppConfig    appConfig;


struct ArgParser
{

std::stack<std::string> optFiles;


std::string makeAbsPath( std::string p )
{
    std::string basePath;

    if (optFiles.empty())
        basePath = umba::filesys::getCurrentDirectory<std::string>();
    else
        basePath = umba::filename::getPath(optFiles.top());


    return umba::filename::makeAbsPath( p, basePath );

}



// 0 - ok, 1 normal stop, -1 - error
template<typename ArgsParser>
int operator()( const std::string                               &a           //!< строка - текущий аргумент
              , umba::command_line::CommandLineOption           &opt         //!< Объект-опция, содержит разобранный аргумент и умеет отвечать на некоторые вопросы
              , ArgsParser                                      &argsParser  //!< Класс, который нас вызывает, содержит некоторый контекст
              , umba::command_line::ICommandLineOptionCollector *pCol        //!< Коллектор опций - собирает инфу по всем опциям и готов вывести справку
              , bool fBuiltin
              , bool ignoreInfos
              )
{
    //using namespace marty::clang::helpers;
    UMBA_USED(a);
    UMBA_USED(fBuiltin);

    std::string dppof = "Don't parse predefined options from ";

    if (opt.isOption())
    {
        std::string errMsg;
        std::string strVal;
        int intVal;
        //unsigned uintVal;
        std::size_t szVal;
        bool boolVal;

        UMBA_USED(intVal);
        UMBA_USED(szVal);

        if (opt.name.empty())
        {
            LOG_ERR_OPT<<"invalid (empty) option name\n";
            return -1;
        }

       if (opt.isOption("quet") || opt.isOption('q') || opt.setDescription("Operate quetly"))  // . Short alias for '--verbose=quet'
        {
            argsParser.quet = true;
            //appConfig.setOptQuet(true);
        }

        else if ( opt.isBuiltinsDisableOptionMain  () 
               || opt.setDescription( dppof + "main distribution options file '" + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::appGlobal   ) + "'"))
        { } // simple skip - обработка уже сделана

        else if ( opt.isBuiltinsDisableOptionCustom() 
               || opt.setDescription( dppof + "custom global options file '"     + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::customGlobal) + "'"))
        { } // simple skip - обработка уже сделана

        else if ( opt.isBuiltinsDisableOptionUser  () 
               || opt.setDescription( dppof + "user local options file '"        + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::userLocal   ) + "'"))
        { } // simple skip - обработка уже сделана

        else if (opt.isOption("version") || opt.isOption('v') || opt.setDescription("Show version info"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!ignoreInfos)
            {
                umba::cli_tool_helpers::printOnlyVersion(umbaLogStreamMsg);
                return 1;
            }

            return 0;
        }

        else if (opt.isOption("where") || opt.setDescription("Show where the executable file is"))
        {
            if (argsParser.hasHelpOption) return 0;

            LOG_MSG_OPT << argsParser.programLocationInfo.exeFullName << "\n";
            return 0;
        }

        else if (opt.setParam("CLR", 0, "no/none/file|" 
                                        "ansi/term|" 
                                        #if defined(WIN32) || defined(_WIN32)
                                        "win32/win/windows/cmd/console"
                                        #endif
                             )
              || opt.setInitial(-1) || opt.isOption("color") 
              || opt.setDescription("Force set console output coloring")
              /* ", can be:\nno, none, file - disable coloring\nansi, term - set ansi terminal coloring\nwin32, win, windows, cmd, console - windows console specific coloring method" */
              )
        {
            if (argsParser.hasHelpOption) return 0;

            umba::term::ConsoleType res;
            auto mapper = [](int i) -> umba::term::ConsoleType
                          {
                              switch(i)
                              {
                                  case 0 : return umba::term::ConsoleType::file;
                                  case 1 : return umba::term::ConsoleType::ansi_terminal;
                                  case 2 : return umba::term::ConsoleType::windows_console;
                                  default: return umba::term::ConsoleType::file;
                              };
                          };
            if (!opt.getParamValue( res, errMsg, mapper ) )
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            coutWriter.forceSetConsoleType(res);
            cerrWriter.forceSetConsoleType(res);
            return 0;
        }

        else if ( opt.setParam("LANG", umba::command_line::OptionType::optString)
               || opt.isOption("lang")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Set target language."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.generationOptions.mdLang = strVal;

            return 0;
        }

        else if ( opt.setParam("LANG", umba::command_line::OptionType::optString)
               || opt.isOption("listings-lang-tag")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Set listings language spec (for code tag)."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.generationOptions.mdCodeTag = strVal;

            return 0;
        }

        else if ( opt.setParam("EXT", umba::command_line::OptionType::optString)
               || opt.isOption("target-ext")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Set target extention (default is '.md')."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.generationOptions.mdFileExt = strVal;

            return 0;
        }

        else if ( opt.setParam("FILE", umba::command_line::OptionType::optString)
               || opt.isOption("translations")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Read translations from file."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.translationsFile = strVal;

            return 0;
        }

        else if ( opt.setParam("FILENAME", umba::command_line::OptionType::optString)
               || opt.isOption("bat-file")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Generate '.bat' file (git add $(Name) etc). Used while generating docs not translations."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.batFile = strVal;

            return 0;
        }


        else if ( opt.setParam("FILENAME", umba::command_line::OptionType::optString)
               || opt.isOption("api-def")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Add API definition file for processing."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.apiDefFiles.emplace_back(strVal);

            return 0;
        }


        else if ( opt.setParam("TPL", umba::command_line::OptionType::optString)
               || opt.isOption("bat-file-line-template")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Template for '.bat' line (git add $(Name) etc). Used while generating docs not translations."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.batFileLineTemplate = strVal;

            return 0;
        }

        else if ( opt.setParam("FILENAME", umba::command_line::OptionType::optString)
               || opt.isOption("index-file")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Generate index '.md' file. Used while generating docs not translations."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.indexFile = strVal;

            return 0;
        }

        else if ( opt.setParam("PATH", umba::command_line::OptionType::optString)
               || opt.isOption("sources-root")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Set root folder for all relative references to source code files."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.sourcesRoot = strVal;

            return 0;
        }

        else if ( opt.setParam("ADD",false)
               || opt.isOption("md-add-vals")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Add numeric values to generated docs."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.generationOptions.mdAddVals = boolVal;

            return 0;
        }

        else if ( opt.setParam("LEVEL")
               || opt.isOption("md-section-level")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Set markdown sections level."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(intVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            if (intVal<1 || intVal>6)
            {
                LOG_ERR_OPT<<"Invalid markdown sections level ("<<opt.argOrg<<")"<<"\n";
                return -1;
            }

            appConfig.generationOptions.mdSectionLevel = (std::size_t)intVal;

            return 0;
        }

        // else if ( opt.setParam("MODE", umba::command_line::OptionType::optString) // , "translation/tr/markdown/md"
        //        || opt.isOption("mode")
        //        // || opt.setParam("VAL",true)
        //        || opt.setDescription("Set generation mode, translation/tr/markdown/md."))
        // {
        //     if (argsParser.hasHelpOption) return 0;
        //  
        //     if (!opt.getParamValue(strVal,errMsg))
        //     {
        //         LOG_ERR_OPT<<errMsg<<"\n";
        //         return -1;
        //     }
        //  
        //     auto docGenCommand = dotNutDocGen::enum_deserialize(strVal, dotNutDocGen::DocGenCommand::invalid);
        //     if (docGenCommand==dotNutDocGen::DocGenCommand::invalid)
        //     {
        //         LOG_ERR_OPT<<"Invalid value in "<<opt.argOrg<<" option"<<", allowed vals: translation,tr/markdown,md/listing,lst/document,doc\n";
        //         return -1;
        //     }
        //  
        //     appConfig.docGenCommand = docGenCommand;
        //  
        //     return 0;
        // }
        else if ( opt.setParam("MODE", 0, "translation/tr|markdown/md|listing/lst|document/doc")
               || opt.setInitial(-1) || opt.isOption("mode")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Set generation mode."))
        {
            if (argsParser.hasHelpOption) return 0;

            auto docGenCommand = dotNutDocGen::DocGenCommand::unknown;
            auto mapper = [](int i) -> dotNutDocGen::DocGenCommand
                          {
                              switch(i)
                              {
                                  case 0 : return dotNutDocGen::DocGenCommand::translation;
                                  case 1 : return dotNutDocGen::DocGenCommand::markdown;
                                  case 2 : return dotNutDocGen::DocGenCommand::listing;
                                  default: return dotNutDocGen::DocGenCommand::document;
                              };
                          };
            if (!opt.getParamValue( docGenCommand, errMsg, mapper ) )
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.docGenCommand = docGenCommand;

            return 0;
        }

        else if ( opt.setParam("MODE", 0, "keep/k|remove/r|empty/empty-line/e")
               || opt.setInitial(-1) || opt.isOption("listing-nested-tags-mode")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Set processing nested tags mode in listings."))
        {
            if (argsParser.hasHelpOption) return 0;

            auto listingNestedTagsMode  = marty_simplesquirrel::ListingNestedTagsMode::unknown;
            auto mapper = [](int i) -> marty_simplesquirrel::ListingNestedTagsMode
                          {
                              switch(i)
                              {
                                  case 0 : return marty_simplesquirrel::ListingNestedTagsMode::keep;
                                  case 1 : return marty_simplesquirrel::ListingNestedTagsMode::remove;
                                  default: return marty_simplesquirrel::ListingNestedTagsMode::emptyLine;
                              };
                          };
            if (!opt.getParamValue( listingNestedTagsMode, errMsg, mapper ) )
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.generationOptions.listingNestedTagsMode = listingNestedTagsMode;

            return 0;
        }

        else if ( opt.setParam("MODE", 0, "none/n|update/u|update-only/o")
               || opt.setInitial(-1) || opt.isOption("listing-tr-update-mode")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Set mode for translations updating, none - don't update TR, update - update TR, update-only - update TR, but don't generate output"))
        {
            if (argsParser.hasHelpOption) return 0;

            auto listingUpdateTrMode  = dotNutDocGen::ListingUpdateTrMode::unknown;
            auto mapper = [](int i) -> dotNutDocGen::ListingUpdateTrMode
                          {
                              switch(i)
                              {
                                  case 0 : return dotNutDocGen::ListingUpdateTrMode::none;
                                  case 1 : return dotNutDocGen::ListingUpdateTrMode::update;
                                  default: return dotNutDocGen::ListingUpdateTrMode::updateOnly;
                              };
                          };
            if (!opt.getParamValue( listingUpdateTrMode, errMsg, mapper ) )
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.listingUpdateTrMode = listingUpdateTrMode;

            return 0;
        }

        // 
        else if ( opt.setParam("FILE", umba::command_line::OptionType::optString)
               || opt.isOption("template-file")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Input document template file."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.docTemplateFile = strVal;

            return 0;
        }

        else if ( opt.setParam("PREFIX", umba::command_line::OptionType::optString)
               || opt.isOption("code-tag-prefix")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Listing labels prefix."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.generationOptions.codeTagPrefix = strVal;

            return 0;
        }

        else if ( opt.setParam("TAG", umba::command_line::OptionType::optString)
               || opt.isOption("code-tag")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Listing tag for cutting part of code."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.codeTag = strVal;

            return 0;
        }

        else if ( opt.setParam("ID", umba::command_line::OptionType::optString)
               || opt.isOption("listing-tr-base-id")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Base ID for listing translations title and text."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.listingTrBaseId = strVal;

            return 0;
        }

        else if ( opt.setParam("FILENAME", umba::command_line::OptionType::optString)
               || opt.isOption("listing-code-file")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Source code file for listing generation. Allowed adding code tag (instead of using --code-tag option) with '#' as filename and tag separator."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.listingCodeFile = strVal;

            return 0;
        }

        else if ( opt.setParam("FILENAME", umba::command_line::OptionType::optString)
               || opt.isOption("output-file")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Set output file for listing/article modes."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.outputFile = strVal;

            return 0;
        }

        //--output-file

        else if ( opt.setParam("?SPACING",true)
               || opt.isOption("alter-types-spacing") || opt.isOption('A') 
               // || opt.setParam("VAL",true)
               || opt.setDescription("Add extra space while formatting alter types."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.generationOptions.mdAlterTypesSpacing = boolVal;
            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("overwrite") || opt.isOption('Y') 
               // || opt.setParam("VAL",true)
               || opt.setDescription("Allow overwrite existing file."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.bOverwrite = boolVal;
            return 0;
        }

        else if ( opt.setParam("?SORT",true)
               || opt.isOption("sort-classes") || opt.isOption('S') 
               // || opt.setParam("VAL",true)
               || opt.setDescription("Sort classes in namespaces."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.generationOptions.mdSortNamespaceClasses = boolVal;
            return 0;
        }

        else if ( opt.isOption("autocomplete-install") 
               || opt.setDescription("Install autocompletion to bash"
                                     #if defined(WIN32) || defined(_WIN32)
                                         "/clink(cmd)"
                                     #endif
                                    )
               )
        {
            if (argsParser.hasHelpOption) return 0;

            //return autocomplete(opt, true);
            return umba::command_line::autocompletionInstaller( pCol, opt, pCol->getPrintHelpStyle(), true, [&]( bool bErr ) -> decltype(auto) { return bErr ? LOG_ERR_OPT : LOG_MSG_OPT; } );
        }
        else if ( opt.isOption("autocomplete-uninstall") 
               || opt.setDescription("Remove autocompletion from bash"
                                     #if defined(WIN32) || defined(_WIN32)
                                         "/clink(cmd)"
                                     #endif
                                    )
                )
        {
            if (argsParser.hasHelpOption) return 0;

            //return autocomplete(opt, false);
            return umba::command_line::autocompletionInstaller( pCol, opt, pCol->getPrintHelpStyle(), false, [&]( bool bErr ) -> decltype(auto) { return bErr ? LOG_ERR_OPT : LOG_MSG_OPT; } );
        }

        else if (opt.isHelpStyleOption())
        {
            // Job is done in isHelpStyleOption
            return 0;
        }
        else if (opt.isHelpOption()) // if (opt.infoIgnore() || opt.isOption("help") || opt.isOption('h') || opt.isOption('?') || opt.setDescription(""))
        {
            if (!ignoreInfos)
            {
                if (pCol && !pCol->isNormalPrintHelpStyle())
                    argsParser.quet = true;
                //printNameVersion();
                if (!argsParser.quet)
                {
                    umba::cli_tool_helpers::printNameVersion(umbaLogStreamMsg);
                    umba::cli_tool_helpers::printBuildDateTime(umbaLogStreamMsg);
                    umba::cli_tool_helpers::printCommitHash(umbaLogStreamMsg);
                    std::cout<<"\n";
                //printHelp();
                }

                if (pCol && pCol->isNormalPrintHelpStyle() && argsParser.argsNeedHelp.empty())
                {
                    auto helpText = opt.getHelpOptionsString();
                    std::cout << "Usage: " << argsParser.programLocationInfo.exeName
                              << " [OPTIONS] target_name\n"
                              << "  If output_file not taken, STDOUT used\n"
                              << "\nOptions:\n\n"
                              << helpText;
                              //<< " [OPTIONS] input_file [output_file]\n\nOptions:\n\n"<<helpText;

                }
                
                if (pCol) // argsNeedHelp
                    std::cout<<pCol->makeText( 78, &argsParser.argsNeedHelp );

                return 1;

            }

            return 0; // simple skip then parse builtins
        }
        else
        {
            LOG_ERR_OPT<<"unknown option: "<<opt.argOrg<<"\n";
            return -1;
        }

        return 0;

    } // if (opt.isOption())
    else if (opt.isResponseFile())
    {
        std::string optFileName = makeAbsPath(opt.name);

        optFiles.push(optFileName);

        auto parseRes = argsParser.parseOptionsFile( optFileName );

        optFiles.pop();

        if (!parseRes)
            return -1;

        if (argsParser.mustExit)
            return 1;

        return 0;
    
    }

    #if !defined(UMBA_2RCFS)

        appConfig.targetName = makeAbsPath(a);

        // if (inputFilename.empty())
        // {
        //     inputFilename = makeAbsPath(a);
        // }
        // else
        // {
        //     outputFilename = makeAbsPath(a);
        // }

    #else

        // outputFilename = makeAbsPath(a);

    #endif

    return 0;

}

}; // struct ArgParser



class CommandLineOptionCollector : public umba::command_line::CommandLineOptionCollectorImplBase
{
protected:
    virtual void onOptionDup( const std::string &opt ) override
    {
        LOG_ERR_OPT<<"Duplicated option key - '"<<opt<<"'\n";
        throw std::runtime_error("Duplicated option key");
    }

};



