#pragma once

#include <stack>

//#include "app_config.h"
#include "umba/cmd_line.h"
#include "umba/cli_tool_helpers.h"
#include "app_ver_config.h"


#if defined(WIN32) || defined(_WIN32)
    #include <shellapi.h>
#endif

// AppConfig    appConfig;

template<typename StringType>
struct ArgParser
{

std::stack<StringType> optFiles;


StringType getBasePath() const
{
    StringType basePath;
    if (optFiles.empty())
        basePath = umba::filesys::getCurrentDirectory<StringType>();
    else
        basePath = umba::filename::getPath(optFiles.top());

    return basePath;
}


StringType makeAbsPath( StringType p )
{
    //return umba::filename::makeCanonical(umba::filename::makeAbsPath( p, getBasePath() ));
    return umba::filename::makeAbsPath( p, getBasePath() );
}



// 0 - ok, 1 normal stop, -1 - error
template<typename ArgsParser>
int operator()( const StringType                                &a           //!< строка - текущий аргумент
              , umba::command_line::CommandLineOption           &opt         //!< Объект-опция, содержит разобранный аргумент и умеет отвечать на некоторые вопросы
              , ArgsParser                                      &argsParser  //!< Класс, который нас вызывает, содержит некоторый контекст
              , umba::command_line::ICommandLineOptionCollector *pCol        //!< Коллектор опций - собирает инфу по всем опциям и готов вывести справку
              , bool fBuiltin
              , bool ignoreInfos
              )
{
    //using namespace marty::clang::helpers;

    std::string dppof = "Don't parse predefined options from ";

    if (opt.isOption())
    {
        std::string errMsg;
        std::string strVal;
        //int intVal;
        //unsigned uintVal;
        std::size_t szVal;
        bool boolVal;

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

        #if defined(WIN32) || defined(_WIN32)
        else if (opt.isOption("home") || opt.setDescription("Open homepage"))
        {
            if (argsParser.hasHelpOption) return 0;
            ShellExecuteA( 0, "open", appHomeUrl, 0, 0, SW_SHOW );
            return 1;
        }
        #endif

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
                umba::cli_tool_helpers::printOnlyVersion(std::cout);
                return 1;
            }
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

        // else if ( opt.setParam("CATID",true)
        //        || opt.isOption("subst-category") || opt.isOption('S') 
        //        // || opt.setParam("VAL",true)
        //        || opt.setDescription("Subst category CATID with file relative path. By default, category with empty ID will be renamed."))
        // {
        //     if (argsParser.hasHelpOption) return 0;
        //  
        //     if (!opt.getParamValue(strVal,errMsg))
        //     {
        //         LOG_ERR_OPT<<errMsg<<"\n";
        //         return -1;
        //     }
        //     
        //     substCategoryName = strVal;
        //     return 0;
        // }

        // else if ( opt.setParam("LANGTAGFORMAT",true)
        //        || opt.isOption("lang-tag-format") || opt.isOption('T') 
        //        // || opt.setParam("VAL",true)
        //        || opt.setDescription( "Set language tag format for output. LANGTAGFORMAT can be one of:\n"
        //                               "LangTag - en-US\n"
        //                               "LangId - 409\n"
        //                               "LangIdFull - 0409\n"
        //                               "LangIdX - 0x409\n"
        //                               "LangIdFullX - 0x0409\n"
        //                             )
        //         )
        // {
        //     if (argsParser.hasHelpOption) return 0;
        //  
        //     if (!opt.getParamValue(strVal,errMsg))
        //     {
        //         LOG_ERR_OPT<<errMsg<<"\n";
        //         return -1;
        //     }
        //     
        //     marty_tr::ELangTagFormat tmp = marty_tr::enum_deserialize(strVal, marty_tr::ELangTagFormat::invalid);
        //     switch(tmp)
        //     {
        //         //case marty_tr::ELangTagFormat::invalid           : break;
        //         case marty_tr::ELangTagFormat::langTag           : break;
        //         //case marty_tr::ELangTagFormat::langTagNeutral    : break;
        //         //case marty_tr::ELangTagFormat::langTagNeutralAuto: break;
        //         case marty_tr::ELangTagFormat::langId            : break;
        //         case marty_tr::ELangTagFormat::langIdFull        : break;
        //         case marty_tr::ELangTagFormat::langIdX           : break;
        //         case marty_tr::ELangTagFormat::langIdFullX       : break;
        //         default:
        //             LOG_ERR_OPT<<"invalid LANGTAGFORMAT value: '"<<strVal<<"'"<<"\n";
        //             return -1;
        //     }
        //  
        //     langTagFormat = tmp;
        //  
        //     return 0;
        // }

        // else if ( opt.setParam("INDENT",2)
        //        || opt.isOption("json-indent") || opt.isOption("indent")
        //        // || opt.setParam("VAL",true)
        //        || opt.setDescription("Set indent for nesting levels in final JSON."))
        // {
        //     if (argsParser.hasHelpOption) return 0;
        //  
        //     if (!opt.getParamValue(intVal,errMsg))
        //     {
        //         LOG_ERR_OPT<<errMsg<<"\n";
        //         return -1;
        //     }
        //  
        //     if (intVal<0)
        //     {
        //         LOG_ERR_OPT<<"invalid option value (--json-indent)"<<"\n";
        //         return -1;
        //     }
        //     
        //     jsonIndent = (unsigned)intVal;
        //     return 0;
        // }

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
            
            bOverwrite = boolVal;
            return 0;
        }

        // else if ( opt.setParam("?MODE",true)
        //        || opt.isOption("force-generate") || opt.isOption("force") || opt.isOption('F') 
        //        // || opt.setParam("VAL",true)
        //        || opt.setDescription("Force generate output ignoring all input errors."))
        // {
        //     if (argsParser.hasHelpOption) return 0;
        //  
        //     if (!opt.getParamValue(boolVal,errMsg))
        //     {
        //         LOG_ERR_OPT<<errMsg<<"\n";
        //         return -1;
        //     }
        //     
        //     bForce = boolVal;
        //     return 0;
        // }

        else if ( opt.setParam("LINEFEED",umba::command_line::OptionType::optString)
               || opt.isOption("linefeed") || opt.isOption("LF") || opt.isOption('L')
               // || opt.setParam("VAL",true)
               || opt.setDescription("Output linefeed. LINEFEED is one of: CR/LF/CRLF/LFCR/DETECT. "
                                     #if defined(WIN32) || defined(_WIN32)
                                     "Default is CRLF."
                                     #else
                                     "Default is LF."
                                     #endif
                                    )
                )
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            marty_cpp::ELinefeedType tmp = marty_cpp::enum_deserialize( strVal, marty_cpp::ELinefeedType::invalid );
            if (tmp== marty_cpp::ELinefeedType::invalid)
            {
                LOG_ERR_OPT<<"Invalid linefeed option value: "<<strVal<<"\n";
                return -1;
            }

            appConfig.outputLinefeed = tmp;

            return 0;
        }

        //  
        // else if ( opt.isOption("include-files") || opt.isOption('I') || opt.setParam("MASK,...")
        //        || opt.setDescription("Include C/C++ names for output. Only files which file name matched any of taken masks, will be added to output.\n"
        //                              "Note: exclude masks also performed on included names\n"
        //                              "For details about 'MASK' parameter see '--exclude-files' option description.\n"
        //                              "Use\n"
        //                              "'--include-files=*.json^,*.jsn^,*.yaml^,*.yml^' to process resources\n"
        //                             )
        //         )
        // {
        //     if (argsParser.hasHelpOption) return 0;
        //     
        //     if (!opt.hasArg())
        //     {
        //         LOG_ERR_OPT<<"include names mask not taken (--include-names)\n";
        //         return -1;
        //     }
        //  
        //     std::vector< std::string > lst = umba::string_plus::split(opt.optArg, ',');
        //     appConfig.includeFilesMaskList.insert(appConfig.includeFilesMaskList.end(), lst.begin(), lst.end());
        //  
        //     return 0;
        // }

        else if ( opt.setParam("PATH")
               || opt.isOption("add-examples-path") || opt.isOption('I')
               || opt.setDescription("Add paths list for examples searching"))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Adding paths for examples searching requires argument (--add-examples-path)\n";
                return -1;
            }

            StringType optArg;
            umba::utfToStringTypeHelper(optArg, opt.optArg);
            if (!appConfig.addSamplesPaths(optArg, getBasePath()))
            {
                LOG_ERR_OPT<<"Adding paths for examples searching failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            return 0;
        }

        else if ( opt.setParam("LANG:EXT[,EXT...]")
               || opt.isOption("add-lang-file-extention")
               || opt.isOption("add-lang-file-extentions")
               || opt.isOption('E')
               || opt.setDescription("Add file extention for the lang for lang detection"))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Adding lang file extention requires argument (--add-lang-file-extention)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.addLangExtentions(optArg))
            {
                LOG_ERR_OPT<<"Adding lang file extention failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            return 0;
        }

        else if ( opt.setParam("LANG:PREFIX")
               || opt.isOption("set-lang-cut-prefix")
               || opt.isOption('P')
               || opt.setDescription("Set prefix for the cut labels in the lang files"))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Setting lang cut prefix requires argument (--set-lang-cut-prefix)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.setLangCutPrefix(optArg))
            {
                LOG_ERR_OPT<<"Setting lang cut prefix failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            return 0;
        }

        else if ( opt.setParam("LANG:TAG")
               || opt.isOption("set-lang-listing-tag") || opt.isOption('T')
               || opt.setDescription("Set target markdown tag for the code section"))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Setting target markdown tag for the code section requires argument (--set-lang-listing-tag)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.setLangListingTag(optArg))
            {
                LOG_ERR_OPT<<"Setting target markdown tag for the code section failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            return 0;
        }

        else if ( opt.setParam("OPTS")
               || opt.isOption("set-insert-options") || opt.isOption('O')
               || opt.setDescription("Set default insert options"))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Setting default insert options requires argument (--set-insert-options)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.updateInsertOptions(optArg))
            {
                LOG_ERR_OPT<<"Setting default insert options failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            return 0;
        }

        else if ( opt.setParam("OPTS")
               || opt.isOption("processing-options") // || opt.isOption('P') // conflict with set-lang-cut-prefix option
               || opt.setDescription("Set processing (output generation) options"))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Setting processing options requires argument (--processing-options)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.updateProcessingOptions(optArg))
            {
                LOG_ERR_OPT<<"Setting processing options failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            return 0;
        }

        else if ( opt.setParam("VAR:VAL")
               || opt.isOption("set-condition-var") || opt.isOption('C')
               || opt.setDescription("Set condition variable"))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Setting condition variable requires argument (--set-condition-var)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.addConditionVar(optArg))
            {
                LOG_ERR_OPT<<"Setting condition variable failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            return 0;
        }

        else if ( opt.setParam("NAME")
               || opt.isOption("target-renderer") || opt.isOption('R')
               || opt.setDescription("Set target renderer (github/doxygen). "))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Setting target renderer requires argument (--target-renderer)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            auto renderer = enum_deserialize(optArg, TargetRenderer::invalid);
            if (renderer==TargetRenderer::invalid)
            {
                LOG_ERR_OPT<<"Setting target renderer failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            appConfig.targetRenderer = renderer;

            return 0;
        }

        else if ( opt.setParam("NAME")
               || opt.isOption("target-format") // || opt.isOption('R')
               || opt.setDescription("Set target format (md/html/rtf/pdf). "))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Setting target format requires argument (--target-format)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            auto tgFormat = enum_deserialize(optArg, TargetFormat::invalid);
            if (tgFormat==TargetFormat::invalid)
            {
                LOG_ERR_OPT<<"Setting target format failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            appConfig.targetFormat = tgFormat;

            return 0;
        }

        else if ( opt.setParam("TAG:REPLACETO")
               || opt.isOption("meta-tag-replace") || opt.isOption('m')
               || opt.setDescription("Add meta tag name replacement."))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Adding meta tag name replacement requires argument (--meta-tag-replace)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.addMetaTagReplace(optArg))
            {
                LOG_ERR_OPT<<"Adding meta tag name replacement failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }
            return 0;
        }

        else if ( opt.setParam("TAG:SERIALIZETO")
               || opt.isOption("meta-tag-serialize") || opt.isOption('M')
               || opt.setDescription("Add meta tag name for serialization."))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Adding meta tag name for serialization requires argument (--meta-tag-serialize)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.addMetaTagSerialize(optArg))
            {
                LOG_ERR_OPT<<"Adding meta tag name for serialization failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }
            return 0;
        }

        else if ( opt.setParam("TYPE:TAG[,TAG...]")
               || opt.isOption("meta-tag-set-type") || opt.isOption("set-meta-tag-type") // || opt.isOption('M')
               || opt.setDescription("Set meta tag type."))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Setting meta tag type requires argument (--meta-tag-set-type)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.addMetaTagType(optArg))
            {
                LOG_ERR_OPT<<"Setting meta tag type failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }
            return 0;
        }

        else if ( opt.setParam("TAG[,TAG...]")
               ||  /* opt.isOption("meta-tag-serialize") || opt.isOption("meta-tag-serialize-list") || */  opt.isOption("serialize-meta-tags") // || opt.isOption('M')
               || opt.setDescription("Set/append meta tag serialize list."))
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"Setting meta tag serialize list requires argument (--meta-tag-serialize)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.setMetaTagSerializeList(optArg))
            {
                LOG_ERR_OPT<<"Setting meta tag serialize list failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }
            return 0;
        }


        else if (opt.setParam("LEVEL", 0, "0/inf/infinite|" 
                                        "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16" 
                             )
              || opt.setInitial(0) || opt.isOption("numeric-sections-max-level") 
              || opt.setDescription("Set max level for numeration")
              )
        {
            if (argsParser.hasHelpOption) return 0;

            unsigned numMaxLevel = 0;
            auto mapper = [](int i) -> unsigned
                          {
                              return (unsigned)i;
                          };
            if (!opt.getParamValue( numMaxLevel, errMsg, mapper ) )
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.numSecMaxLevel = numMaxLevel;
            return 0;
        }

        else if (opt.setParam("LEVEL", 0, "0/inf/infinite|" 
                                        "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16" 
                             )
              || opt.setInitial(0) || opt.isOption("toc-max-level") 
              || opt.setDescription("Set max level for table of contents (TOC)")
              )
        {
            if (argsParser.hasHelpOption) return 0;

            unsigned numMaxLevel = 0;
            auto mapper = [](int i) -> unsigned
                          {
                              return (unsigned)i;
                          };
            if (!opt.getParamValue( numMaxLevel, errMsg, mapper ) )
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.tocMaxLevel = numMaxLevel;
            return 0;
        }


        else if ( opt.setParam("RAISE", (size_t )0, (size_t )0, (size_t)4) || opt.setInitial((size_t)0) || opt.isOption("restrict-path-raise") // || opt.isOption('i') 
               || opt.setDescription("Raise insert restriction path up to 4 levels max"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue( szVal, errMsg ))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.restrictPathRaise = szVal;

            return 0;
        }

        else if ( opt.setParam("LANG",umba::command_line::OptionType::optString)
               || opt.isOption("document-language")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Set default document language, which used if no language tag in document meta info."))
        {
            if (argsParser.hasHelpOption) return 0;
         
            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.documentDefaultLanguage = umba::toUtf8(strVal);
            return 0;
        }
        // else if ( opt.setParam("LINEFEED",umba::command_line::OptionType::optString)
        //        || opt.isOption("linefeed") || opt.isOption("LF") || opt.isOption('L')
        //        // || opt.setParam("VAL",true)
        //        || opt.setDescription("Output linefeed. LINEFEED is one of: CR/LF/CRLF/LFCR/DETECT."))
        // {
        //     if (argsParser.hasHelpOption) return 0;
        //  
        //     if (!opt.getParamValue(strVal,errMsg))
        //     {

        else if ( opt.setParam("LANG",umba::command_line::OptionType::optString)
               || opt.isOption("force-document-language")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Force set document language, override document meta info."))
        {
            if (argsParser.hasHelpOption) return 0;
         
            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.documentForceLanguage = umba::toUtf8(strVal);
            return 0;
        }

        else if ( opt.setParam("PATH",umba::command_line::OptionType::optString)
               || opt.isOption("batch-output-root")
               || opt.isOption("batch-output-path")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Set output root path for batch mode."))
        {
            if (argsParser.hasHelpOption) return 0;
         
            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.batchOutputRoot = strVal;
            return 0;
        }

        else if ( opt.isOption("batch-exclude-files") || opt.isOption('X') || opt.setParam("MASK,...")
               || opt.setDescription("Exclude files from parsing in the batch mode. The 'MASK' parameter is a simple file mask, where '*' "
                                     "means any number of any chars, and '?' means exact one of any char. In addition, "
                                     "symbol '^' in front and/or back of the mask means that the mask will be bound to beginning/ending "
                                     "of the tested file name.\n"
                                     "Also, regular expresion syntax allowed in form '" + 
                                     umba::regex_helpers::getRawEcmaRegexPrefix<std::string>() + "YOURREGEX'. The regular expresions supports\n"
                                     "See also: C++ Modified ECMA Script regular expression grammar - https://en.cppreference.com/w/cpp/regex/ecmascript"
                                    )
                )
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"exclude files mask not taken (--exclude-files)\n";
                return -1;
            }
         
            std::vector< std::string > lst = umba::string_plus::split(opt.optArg, ',');
            appConfig.batchExcludeFilesMaskList.insert(appConfig.batchExcludeFilesMaskList.end(), lst.begin(), lst.end());
         
            return 0;
        }

        else if ( opt.isOption("batch-exclude-dir") || opt.isOption("batch-exclude-dirs") || opt.setParam("DIRNAME,...")
               || opt.setDescription("Exclude dirs from scaning in the batch mode. The 'DIRNAME' parameter is a simple directory name, not a mask"
                                    )
                )
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"exclude dirs not taken (--batch-exclude-dir)\n";
                return -1;
            }
         
            std::vector< std::string > lst = umba::string_plus::split(opt.optArg, ',');
            appConfig.batchExcludeDirs.insert(appConfig.batchExcludeDirs.end(), lst.begin(), lst.end());
         
            return 0;
        }

        else if ( opt.isOption("batch-scan") || opt.setParam("DIRNAME,...")
               || opt.setDescription("Perform batch job on taken directories"
                                    )
                )
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"scan dirs not taken (--batch-scan)\n";
                return -1;
            }
         
            std::vector< std::string > lst = umba::string_plus::split(opt.optArg, ',');
            for(auto &p: lst)
            {
                p = makeAbsPath(p);
            }
            //appConfig.batchScanPaths.insert(appConfig.batchScanPaths.end(), lst.begin(), lst.end());
            appConfig.addBatchScanPaths(lst, false); // no recurse
         
            return 0;
        }

        else if ( opt.isOption("batch-scan-recurse") || opt.isOption("batch-rscan") || opt.setParam("DIRNAME,...")
               || opt.setDescription("Perform batch job on taken directories with recursion scan"
                                    )
                )
        {
            if (argsParser.hasHelpOption) return 0;
            
            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"scan dirs not taken (--batch-scan-recurse)\n";
                return -1;
            }
         
            std::vector< std::string > lst = umba::string_plus::split(opt.optArg, ',');
            for(auto &p: lst)
            {
                p = makeAbsPath(p);
            }
            //appConfig.batchScanPaths.insert(appConfig.batchScanPaths.end(), lst.begin(), lst.end());
            appConfig.addBatchScanPaths(lst, true); // recurse
         
            return 0;
        }

        else if ( opt.setParam("FILE",umba::command_line::OptionType::optString)
               || opt.isOption("batch-page-index-file") || opt.isOption("batch-pages-index-file")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Generate index of processed pages."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.batchPageIndexFileName = strVal;
            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("batch-split-page-index-file") || opt.isOption("batch-split-pages-index-file")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Generate index of processed pages."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.batchSplitPageIndex = boolVal;
            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("copy-images") || opt.isOption("copy-image-files")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Copy image files to output folder, if it taken. No effect if output path not taken (see --batch-output-root) nor single file output path folder not equal to source path."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.copyImageFiles = boolVal;
            return 0;
        }

        else if ( opt.setParam("EXT[,EXT...]",umba::command_line::OptionType::optString)
               || opt.isOption("add-mdpp-extention")
               || opt.isOption("add-mdpp-extentions")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Add file extentions of the MD-PP files."))
        {
            if (argsParser.hasHelpOption) return 0;
         
            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            return appConfig.addMdppExtentions(strVal) ?  0 : -1;
        }


        #if defined(UMBA_MD_PP_VIEW)

        else if ( opt.setParam("COMMAEXTLIST", std::string()/* umba::command_line::OptionType::optString */ )
               || opt.isOption("register-view-handler")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Register this application as view handler in OS."))
        {
            if (argsParser.hasHelpOption) return 0;


            std::string extListCommaSep = appConfig.getSupportedExtentionsString();

            // auto paramStr = opt.optArg;
            // if (!param.empty())
            // {
            //     if (!opt.getParamValue(strVal,errMsg))
            //     {
            //         LOG_ERR_OPT<<errMsg<<"\n";
            //         return -1;
            //     }
            //  
            //     param = strVal;
            // }
            // else
            // {
            //     ///param = "md,md_,_md,markdown,_markdown,markdown_";
            //  
            // }

            if (!opt.optArg.empty())
            {
                extListCommaSep += "," + opt.optArg;
            }

            auto exeFullName      = argsParser.programLocationInfo.exeFullName;
            auto exeCanonicalName = umba::filename::makeCanonical(exeFullName);
            auto appIdName        = umba::filename::getName(exeFullName);
            auto appIdNameWide    = umba::fromUtf8(appIdName);

            auto exeCanonicalNameWide        = umba::fromUtf8(exeCanonicalName);
            auto exeCanonicalNameWideEscaped = escapeCommandLineArgument(exeCanonicalNameWide);
            auto percent1                    = L"\"%1\"";

            if (!regShellExtentionHandlerApplication(appIdNameWide, L"open", exeCanonicalNameWideEscaped + L" " + percent1))
            {
                LOG_ERR_OPT<<"Failed to register appid"<<"\n";
                return -1;
            }

            //auto wParam = umba::fromUtf8(param);
            auto extListCommaSepW = umba::fromUtf8(extListCommaSep);
            if (!regShellExtentionHandlerForExtList(appIdNameWide, extListCommaSepW))
            {
                LOG_ERR_OPT<<"Failed to register appid as handler"<<"\n";
                return -1;
            }

            return 0;
        }

        #endif

        //LOG_MSG_OPT << argsParser.programLocationInfo.exeFullName << "\n";

        // else if ( opt.isOption("all")
        //        || opt.setDescription("In scan mode, if no --exclude-files nor --include-files mask are taken, --all option required to confirm processing all files")
        //         )
        // {
        //     if (argsParser.hasHelpOption) return 0;
        //     
        //     appConfig.allFiles = true;
        //  
        //     return 0;
        // }
        //  
        else if ( opt.isOption("gcc")
               || opt.setDescription("GCC messages format instead of MSVC format")
                )
        {
            if (argsParser.hasHelpOption) return 0;
            
            umbaLogGccFormat = true;

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
                    umba::cli_tool_helpers::printNameVersion(std::cout);
                    //umba::cli_tool_helpers::printBuildDateTime();
                    umba::cli_tool_helpers::printCommitHash(std::cout);
                    std::cout<<"\n";
                //printHelp();
                }

                if (pCol && pCol->isNormalPrintHelpStyle() && argsParser.argsNeedHelp.empty())
                {
                    auto helpText = opt.getHelpOptionsString();
                    std::cout << "Usage: " << umba::toUtf8(argsParser.programLocationInfo.exeName)
                              << " [OPTIONS] input_file [output_file]\n"
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
        //std::string

        StringType optName;
        umba::utfToStringTypeHelper(optName, opt.name);
        auto optFileName = makeAbsPath(optName);

        if (!argsParser.quet)
        {
            #if !defined(NDEBUG)
            std::cout << "Processing options file: " << optFileName << "\n";
            #endif
        }

        optFiles.push(optFileName);

        auto parseRes = argsParser.parseOptionsFile( optFileName );

        optFiles.pop();

        if (!parseRes)
            return -1;

        if (argsParser.mustExit)
            return 1;

        return 0;
    
    }

    if (inputFilename.empty())
    {
        inputFilename = makeAbsPath(a);
    }
    else
    {
        outputFilename = makeAbsPath(a);
    }
    

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



