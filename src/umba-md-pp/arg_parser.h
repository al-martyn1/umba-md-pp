#pragma once

#include <stack>

//#include "app_config.h"
#include "umba/cmd_line.h"
#include "umba/cli_tool_helpers.h"
#include "umba/shellapi.h"
#include "umba/string_plus.h"
#include "app_ver_config.h"

//
#include "marty_cpp/src_normalization.h"



// AppConfig    appConfig;

//extern umba::SimpleFormatter umbaLogStreamMsg;


template<typename StringType>
struct ArgParser
{

    static
    const std::set<std::string>& getWarnOptsSet()
    {
        const static std::set<std::string> s = {"img-copy-target-exist", "same-file", "img-copy", "plantuml", "graphviz"};
        return s;
    }

    static
    const std::set<std::string>& getInfoOptsSet()
    {
        const static std::set<std::string> s /* = */ {"snippet-lookup", "plantuml"};
        return s;
    }

    // constexpr const static std::set<std::string> warnOptsSet /* = */ {"img-copy-target-exist", "same-file", "img-copy", "plantuml", "graphviz"};
    // constexpr const static std::set<std::string> infoOptsSet /* = */ {"snippet-lookup", "plantuml"};


// std::stack<StringType> optFiles;
//
//
// StringType getBasePath() const
// {
//     StringType basePath;
//     if (optFiles.empty())
//         basePath = umba::filesys::getCurrentDirectory<StringType>();
//     else
//         basePath = umba::filename::getPath(optFiles.top());
//
//     return basePath;
// }
//
//
// StringType makeAbsPath( StringType p )
// {
//     //return umba::filename::makeCanonical(umba::filename::makeAbsPath( p, getBasePath() ));
//     return umba::filename::makeAbsPath( p, getBasePath() );
// }


// inline
// std::string makeAllLogOptionsString(const std::set<std::string> &s)
// {
//     return umba::string_plus::merge<std::string, std::set<std::string>::const_iterator>( s.begin(), s.end(), ',');
// }

//    auto addRemoveLogOptions = (std::unordered_map<std::string, bool>& optMap)

// std::unordered_map<std::string, bool>& getWarningDisabledMap()

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
            LOG_ERR<<"invalid (empty) option name\n";
            return -1;
        }

        if (opt.isOption("quet") || opt.isOption('q') || opt.setDescription("Operate quetly"))  // . Short alias for '--verbose=quet'
        {
            argsParser.quet = true;
            //appConfig.setOptQuet(true);
        }

        else if ( opt.setParam("info-type1[,+info-type2,-info-type]",umba::command_line::OptionType::optString)
               || opt.isOption("info")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Make info messages enabled/disabled, '+' (or nothing) - enable message, '-' - disable it. Type is one of: " + umba::log::makeAllWarnInfoLogOptionsString(getInfoOptsSet())
                                    )
                )
        {
            if (argsParser.hasHelpOption) return 0;
        
            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            std::string unknownOpt;
            if (!umba::log::addRemoveInfoOptions(getInfoOptsSet(), strVal, unknownOpt))
            {
                LOG_ERR<<"Unknown info type: '" << unknownOpt << "' (--info)\n";
                return -1;
            }

            return 0;
        }

        #if 1
        else if ( opt.setParam("warn-type1[,+warn-type2,-warn-type]",umba::command_line::OptionType::optString)
               || opt.isOption("warning")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Make warning messages enabled/disabled, '+' (or nothing) - enable message, '-' - disable it. Type is one of: " + umba::log::makeAllWarnInfoLogOptionsString(getWarnOptsSet())
                                    )
                )
        {
            if (argsParser.hasHelpOption) return 0;
        
            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            std::string unknownOpt;
            if (!umba::log::addRemoveWarningOptions(getWarnOptsSet(), strVal, unknownOpt))
            {
                LOG_ERR<<"Unknown warning type: '" << unknownOpt << "' (--warning)\n";
                return -1;
            }

            return 0;
        }
        #endif

        else if (opt.isOption("home") || opt.setDescription("Open homepage"))
        {
            if (argsParser.hasHelpOption) return 0;
            umba::shellapi::openUrl(appHomeUrl);
            return 1;
        }

        else if ( opt.isBuiltinsDisableOptionMain  ()
               // || opt.setDescription( dppof + "main distribution options file `" + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::appGlobal   ) + "`"))
               || opt.setDescription( dppof + "main distribution options file"))
        { } // simple skip - обработка уже сделана

        else if ( opt.isBuiltinsDisableOptionCustom()
               //|| opt.setDescription( dppof + "custom global options file `"     + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::customGlobal) + "`"))
               || opt.setDescription( dppof + "custom global options file"))
        { } // simple skip - обработка уже сделана

        else if ( opt.isBuiltinsDisableOptionUser  ()
               //|| opt.setDescription( dppof + "user local options file `"        + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::userLocal   ) + "`"))
               || opt.setDescription( dppof + "user local options file"))
        { } // simple skip - обработка уже сделана

        else if (opt.isOption("version") || opt.isOption('v') || opt.setDescription("Show version number"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!ignoreInfos)
            {
                umba::cli_tool_helpers::printOnlyVersion(std::cout);
                return 1;
            }
        }

        else if (opt.isOption("version-info") || opt.setDescription("Show version info - app name, version, host platform, build date and time"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!ignoreInfos)
            {
                if (argsParser.getPrintHelpStyle()!=umba::command_line::PrintHelpStyle::md)
                {
                    umba::cli_tool_helpers::printNameVersion(std::cout);
                }
                else
                {
                    umba::cli_tool_helpers::printNameVersion(std::cout);
                }

                return 1;
            }
        }

        else if (opt.isOption("builtin-options-info") || opt.setDescription("Show builtin options files location"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!ignoreInfos)
            {
                argsParser.printBuiltinFileNames(std::cout);
                return 1;
            }
        }

        else if (opt.isOption("where") || opt.setDescription("Show where the executable file is"))
        {
            if (argsParser.hasHelpOption) return 0;

            LOG_MSG << argsParser.programLocationInfo.exeFullName << "\n";
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
                LOG_ERR<<errMsg<<"\n";
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
        //         LOG_ERR<<errMsg<<"\n";
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
        //         LOG_ERR<<errMsg<<"\n";
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
        //             LOG_ERR<<"invalid LANGTAGFORMAT value: '"<<strVal<<"'"<<"\n";
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
        //         LOG_ERR<<errMsg<<"\n";
        //         return -1;
        //     }
        //
        //     if (intVal<0)
        //     {
        //         LOG_ERR<<"invalid option value (--json-indent)"<<"\n";
        //         return -1;
        //     }
        //
        //     jsonIndent = (unsigned)intVal;
        //     return 0;
        // }


        else if ( opt.setParam("?MODE",true)
               || opt.isOption("verbose")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Verbose mode on/off."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            argsParser.quet       = !boolVal;
            appConfig.verboseMode =  boolVal;
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
                LOG_ERR<<errMsg<<"\n";
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
        //         LOG_ERR<<errMsg<<"\n";
        //         return -1;
        //     }
        //
        //     bForce = boolVal;
        //     return 0;
        // }

        else if ( opt.setParam("LINEFEED",umba::command_line::OptionType::optString)
               || opt.isOption("linefeed") || opt.isOption("LF") || opt.isOption('L')
               // || opt.setParam("VAL",true)
               || opt.setDescription("Output linefeed. LINEFEED is one of: `CR`/`LF`/`CRLF`/`LFCR`/`DETECT`."
                                     #if defined(WIN32) || defined(_WIN32)
                                     "Default is `CRLF`."
                                     #else
                                     "Default is `LF`."
                                     #endif
                                    )
                )
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            marty_cpp::ELinefeedType tmp = marty_cpp::enum_deserialize( strVal, marty_cpp::ELinefeedType::invalid );
            if (tmp== marty_cpp::ELinefeedType::invalid)
            {
                LOG_ERR<<"Invalid linefeed option value: "<<strVal<<"\n";
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
        //         LOG_ERR<<"include names mask not taken (--include-names)\n";
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
                LOG_ERR<<"Adding paths for examples searching requires argument (--add-examples-path)\n";
                return -1;
            }

            StringType optArg;
            umba::utfToStringTypeHelper(optArg, opt.optArg);

            if (!appConfig.addSamplesPaths(optArg, argsParser.getBasePath()))
            {
                LOG_ERR<<"Adding paths for examples searching failed, invalid argument: '" << optArg << "'\n";
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
                LOG_ERR<<"Adding lang file extention requires argument (--add-lang-file-extention)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.addLangExtentions(optArg))
            {
                LOG_ERR<<"Adding lang file extention failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            return 0;
        }

        else if ( opt.setParam("LANG:PREFIX")
               || opt.isOption("add-lang-cut-prefix")
               || opt.isOption('P')
               || opt.setDescription("Add prefix for the cut labels in the lang files"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR<<"Adding lang cut prefix requires argument (--add-lang-cut-prefix)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.addCutPrefix(optArg))
            {
                LOG_ERR<<"Addng lang cut prefix failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            return 0;
        }

        else if ( opt.setParam("LANG:PREFIX")
               || opt.isOption("add-lang-separator-prefix")
               || opt.setDescription("Add lang prefix for the separator line"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR<<"Adding lang prefix for the separator line (--add-lang-separator-prefix)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.addSeparatorLinePrefix(optArg))
            {
                LOG_ERR<<"Addng lang prefix for the separator line, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            return 0;
        }

        else if ( opt.setParam("LANG:{}")
               || opt.isOption("set-lang-block-chars")
               || opt.setDescription("Set lang block open and close characters, like \"{}\" for C/C++"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR<<"Setting lang block open and close characters (--set-lang-block-chars)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.setBlockCharacters(optArg))
            {
                LOG_ERR<<"Setting lang block open and close characters, invalid argument: '" << optArg << "'. Argument must be exact the pair of single open and close block characters\n";
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
                LOG_ERR<<"Setting target markdown tag for the code section requires argument (--set-lang-listing-tag)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.setLangListingTag(optArg))
            {
                LOG_ERR<<"Setting target markdown tag for the code section failed, invalid argument: '" << optArg << "'\n";
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
                LOG_ERR<<"Setting default insert options requires argument (--set-insert-options)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.updateInsertOptions(optArg))
            {
                LOG_ERR<<"Setting default insert options failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            return 0;
        }

        else if ( opt.setParam("OPTS")
               || opt.isOption("processing-options")
               || opt.setDescription("Set processing (output generation) options"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR<<"Setting processing options requires argument (--processing-options)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.updateProcessingOptions(optArg))
            {
                LOG_ERR<<"Setting processing options failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            return 0;
        }

        else if ( opt.setParam("VAR:VAL")
               || opt.isOption("set-var") || opt.isOption("set-condition-var") || opt.isOption('C')
               || opt.setDescription("Set variable valie for conditions and substitutions"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR<<"Setting condition variable requires argument (--set-condition-var)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.addConditionVar(optArg))
            {
                LOG_ERR<<"Setting condition variable failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            return 0;
        }

        else if ( opt.setParam("NAME")
               || opt.isOption("target-renderer") || opt.isOption('R')
               || opt.setDescription("Set target renderer (`github`/`doxygen`). "))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR<<"Setting target renderer requires argument (--target-renderer)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            auto renderer = enum_deserialize(optArg, TargetRenderer::invalid);
            if (renderer==TargetRenderer::invalid)
            {
                LOG_ERR<<"Setting target renderer failed, invalid argument: '" << optArg << "'\n";
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
                LOG_ERR<<"Setting target format requires argument (--target-format)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            auto tgFormat = enum_deserialize(optArg, TargetFormat::invalid);
            if (tgFormat==TargetFormat::invalid)
            {
                LOG_ERR<<"Setting target format failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }

            appConfig.targetFormat = tgFormat;

            return 0;
        }

        else if ( opt.setParam("NAME")
               || opt.isOption("target-name") || opt.isOption("rendering-target-name") // || opt.isOption('R')
               || opt.setDescription("Set target name. "))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR<<"Setting target name requires argument (--target-name)\n";
                return -1;
            }

            appConfig.renderingTargetName = opt.optArg;

            return 0;
        }

        else if ( opt.setParam("TAG:REPLACETO")
               || opt.isOption("meta-tag-replace") || opt.isOption('m')
               || opt.setDescription("Add meta tag name replacement."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR<<"Adding meta tag name replacement requires argument (--meta-tag-replace)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.addMetaTagReplace(optArg))
            {
                LOG_ERR<<"Adding meta tag name replacement failed, invalid argument: '" << optArg << "'\n";
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
                LOG_ERR<<"Adding meta tag name for serialization requires argument (--meta-tag-serialize)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.addMetaTagSerialize(optArg))
            {
                LOG_ERR<<"Adding meta tag name for serialization failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }
            return 0;
        }

        else if ( opt.setParam("TYPE:TAG[,TAG...]")
               || opt.isOption("meta-tag-set-type") || opt.isOption("set-meta-tag-type") // || opt.isOption('M')
               || opt.setDescription("Set meta tag type (`TextFirst`/`TextReplace`/`TextMerge`/`List`/`CommaList`/`Set`/`CommaSet`)."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR<<"Setting meta tag type requires argument (--meta-tag-set-type)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.addMetaTagType(optArg))
            {
                LOG_ERR<<"Setting meta tag type failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }
            return 0;
        }

        else if ( opt.setParam("TAG[,TAG...]")
               ||  /* opt.isOption("meta-tag-serialize") || opt.isOption("meta-tag-serialize-list") || */  opt.isOption("serialize-meta-tags") // || opt.isOption('M')
               || opt.setDescription("Set/append meta tag serialize list. Use `+` sign at first position to append tags to list"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR<<"Setting meta tag serialize list requires argument (--meta-tag-serialize)\n";
                return -1;
            }

            auto optArg = opt.optArg;
            if (!appConfig.setMetaTagSerializeList(optArg))
            {
                LOG_ERR<<"Setting meta tag serialize list failed, invalid argument: '" << optArg << "'\n";
                return -1;
            }
            return 0;
        }


        else if (opt.setParam("LEVEL", 0, "0/inf/infinite|"
                                        "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16"
                             )
              || opt.setInitial(0) || opt.isOption("numeric-sections-max-level")
              || opt.setDescription("Set max level for sections numeration")
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
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.numSecMaxLevel = numMaxLevel;
            return 0;
        }

        else if (opt.setParam("LEVEL", 0, "0/inf/infinite|"
                                        "1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16"
                             )
              || opt.setInitial(0) || opt.isOption("toc-max-level")
              || opt.setDescription("Set max section level for table of contents (TOC)")
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
                LOG_ERR<<errMsg<<"\n";
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
                LOG_ERR<<errMsg<<"\n";
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
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.documentDefaultLanguage = strVal;
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
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.documentForceLanguage = strVal;
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
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.batchOutputRoot = strVal;
            return 0;
        }

        else if ( opt.isOption("batch-exclude-files") || opt.isOption('X') || opt.setParam("MASK,...")
               || opt.setDescription("Exclude files from parsing in the batch mode. The `MASK` parameter is a simple file mask, where `*` "
                                     "means any number of any chars, and `?` means exact one of any char. In addition, "
                                     "symbol `^` in front and/or back of the mask means that the mask will be bound to beginning/ending "
                                     "of the tested file name.\n"
                                     "Also, regular expresion syntax allowed in form `" +
                                     umba::regex_helpers::getRawEcmaRegexPrefix<std::string>() + "YOURREGEX`. The regular expresions supports\n"
                                     "See also: C++ Modified ECMA Script regular expression grammar - https://en.cppreference.com/w/cpp/regex/ecmascript"
                                    )
                )
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR<<"exclude files mask not taken (--exclude-files)\n";
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
                LOG_ERR<<"exclude dirs not taken (--batch-exclude-dir)\n";
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
                LOG_ERR<<"scan dirs not taken (--batch-scan)\n";
                return -1;
            }

            std::vector< std::string > lst = umba::string_plus::split(opt.optArg, ',');
            for(auto &p: lst)
            {
                p = argsParser.makeAbsPath(p);
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
                LOG_ERR<<"scan dirs not taken (--batch-scan-recurse)\n";
                return -1;
            }

            std::vector< std::string > lst = umba::string_plus::split(opt.optArg, ',');
            for(auto &p: lst)
            {
                p = argsParser.makeAbsPath(p);
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
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.batchPageIndexFileName = strVal;
            return 0;
        }

        else if ( opt.setParam("FILE",umba::command_line::OptionType::optString)
               || opt.isOption("batch-git-add-file") || opt.isOption("batch-generate-git-add-file")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Generate bat/shell script with git add commands for all generated files."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.gitAddBatchFileName = strVal;
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
                LOG_ERR<<errMsg<<"\n";
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
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.copyImageFiles = boolVal;
            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("flatten-image-links") // || opt.isOption("copy-image-files")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Flatten image links: 'some/path/to/image.png' becomes to 'some_path_to_image.png'. No effect if output path not taken (see --batch-output-root) nor single file output path folder not equal to source path."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.flattenImageLinks = boolVal;
            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("viewer-copy-render-to-source-location") || opt.isOption("viewer-copy-to-source")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Copy generated (rendered) file to source file location."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.viewerCopyToSourceLocation = boolVal;
            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("viewer-render-copy-use-doc-title") || opt.isOption("viewer-copy-use-title")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Copy generated (rendered) file to source file location with file name obtained from the document title."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.viewerCopyFilenameDocTitle = boolVal;
            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("strip-ext") || opt.isOption("strip-extention") || opt.isOption("strip-extentions")
               // || opt.setParam("VAL",true)
               || opt.setDescription("Strip supported extentions in the local links."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.stripExtentions = boolVal;
            return 0;
        }

        else if ( opt.setParam("DPI",umba::command_line::OptionType::optString)
               || opt.isOption("graphviz-dpi")
               || opt.isOption("gviz-dpi")
               || opt.isOption("gv-dpi")
               || opt.setDescription("Set DPI for Graphviz tools output."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            if (!appConfig.graphVizOptions.setDpi(strVal))
            {
                LOG_ERR<<"Setting DPI for Graphviz tools output failed, invalid argument: '" << strVal << "' (--graphviz-dpi)\n";
                return -1;
            }
            return 0;
        }

        else if ( opt.setParam("FORMAT",umba::command_line::OptionType::optString)
               || opt.isOption("graphviz-output-format")
               || opt.isOption("gviz-output-format")
               || opt.isOption("gv-output-format")
               || opt.setDescription("Set Graphviz tools output format (SVG/PNG)."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            if (!appConfig.graphVizOptions.setTargetFormat(strVal))
            {
                LOG_ERR<<"Setting Graphviz tools output format failed, invalid argument: '" << strVal << "' (--graphviz-output-format)\n";
                return -1;
            }
            return 0;
        }

        else if ( opt.setParam("PATH",umba::command_line::OptionType::optString)
               || opt.isOption("graphviz-output-path")
               || opt.isOption("graphviz-output-root")
               || opt.isOption("gviz-output-path")
               || opt.isOption("gviz-output-root")
               || opt.isOption("gv-output-path")
               || opt.isOption("gv-output-root")
               || opt.setDescription("Set Graphviz tools output root path."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.graphVizOptions.savePath = argsParser.makeAbsPath(strVal);

            // if (!appConfig.graphVizOptions.setTargetFormat(optArg))
            // {
            //     LOG_ERR<<"Setting Graphviz tools output format failed, invalid argument: '" << optArg << "' (--graphviz-output-path)\n";
            //     return -1;
            // }

            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("graphviz-show-labels")
               || opt.isOption("gviz-show-labels")
               || opt.isOption("gv-show-labels")
               || opt.setDescription("Show labels on Graphviz graphs."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                return -1;
            }

            appConfig.graphVizOptions.showLabels = boolVal;

            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("graphviz-keep-temp-dot-files")
               || opt.isOption("gviz-keep-temp-dot-files")
               || opt.isOption("gv-keep-temp-dot-files")
               || opt.setDescription("Keep temporary dot files."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                return -1;
            }

            appConfig.graphVizOptions.keepTempDotFiles = boolVal;

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
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            return appConfig.addMdppExtentions(strVal) ?  0 : -1;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("dont-lookup-for-doxygen")
               || opt.setDescription("Do not lookup for Doxygen (in registry)."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                return -1;
            }

            appConfig.dontLookupForDoxygen = boolVal;

            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("dont-lookup-for-graphviz")
               || opt.setDescription("Do not lookup for Graphviz (in registry)."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                return -1;
            }

            appConfig.dontLookupForGraphviz = boolVal;

            return 0;
        }

        else if ( opt.setParam("JAVA_EXE",umba::command_line::OptionType::optString)
               || opt.isOption("java")
               || opt.setDescription("Set Java executable full path name."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.java = strVal;

            return 0;
        }

        else if ( opt.setParam("JAVA_HOME",umba::command_line::OptionType::optString)
               || opt.isOption("java-home")
               || opt.setDescription("Set Java home. Java executable must exist as $(JAVA_HOME)/bin/java"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.javaHome = strVal;

            return 0;
        }

        else if ( opt.setParam("PLANTUML_JAR",umba::command_line::OptionType::optString)
               || opt.isOption("plant-uml")
               || opt.isOption("plantuml")
               || opt.setDescription("Set Plant UML jar full path name."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.plantUml = strVal;

            return 0;
        }

        else if ( opt.setParam("FORMAT",umba::command_line::OptionType::optString)
               || opt.isOption("plant-uml-output-format")
               || opt.isOption("plantuml-output-format")
               || opt.isOption("puml-output-format")
               || opt.setDescription("Set PlantUML output format (SVG/PNG)."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            if (!appConfig.plantUmlOptions.setTargetFormat(strVal))
            {
                LOG_ERR<<"Setting PlantUML tools output format failed, invalid argument: '" << strVal << "' (--graphviz-output-format)\n";
                return -1;
            }
            return 0;
        }

        else if ( opt.setParam("PATH",umba::command_line::OptionType::optString)
               || opt.isOption("plant-uml-output-path")
               || opt.isOption("plant-uml-output-root")
               || opt.isOption("plantuml-output-path")
               || opt.isOption("plantuml-output-root")
               || opt.isOption("puml-output-path")
               || opt.isOption("puml-output-root")
               || opt.setDescription("Set PlantUML output root path."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.plantUmlOptions.savePath = argsParser.makeAbsPath(strVal);

            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("plant-uml-show-labels")
               || opt.isOption("plantuml-show-labels")
               || opt.isOption("puml-show-labels")
               || opt.setDescription("Show labels on PlantUML raphs."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                return -1;
            }

            appConfig.plantUmlOptions.showLabels = boolVal;

            return 0;
        }

        else if ( opt.setParam("PATH",umba::command_line::OptionType::optString)
               || opt.isOption("generated-output-path")
               || opt.isOption("generated-output-root")
               || opt.setDescription("Set output root path for generated files (same as `--graphviz-output-path=PATH --plant-uml-output-path=PATH`)."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR<<errMsg<<"\n";
                return -1;
            }

            appConfig.setGeneratedOutputRoot(argsParser.makeAbsPath(strVal));

            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("clear-generation-cache")
               || opt.setDescription("Clear cached information of the generated files - force regenerate all images on next call. No other actions will be performed if this option was taken."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                return -1;
            }

            appConfig.clearGenerationCaches = boolVal;

            return 0;
        }


        #if defined(UMBA_MD_PP_VIEW)

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("remove-temp")
               || opt.setDescription("Remove application temp folder with all content."))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                return 0;
            }

            std::string tempFolder;
            removeTempFolder(tempFolder);

            return 0;
        }

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
            //         LOG_ERR<<errMsg<<"\n";
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
            //auto appIdNameWide    = umba::fromUtf8(appIdName);

            //auto exeCanonicalNameWide        = umba::fromUtf8(exeCanonicalName);
            //auto exeCanonicalNameWideEscaped = escapeCommandLineArgument(exeCanonicalNameWide);

            #if defined(WIN32) || defined(_WIN32)

            auto exeCanonicalNameEscaped = umba::shellapi::escapeCommandLineArgument(exeCanonicalName);
            auto percent1 = "\"%1\"";

            // if (!regShellExtentionHandlerApplication(appIdNameWide, L"open", exeCanonicalNameWideEscaped + L" " + percent1))
            if (!umba::shellapi::win32::registerShellExtentionHandlerApplication(false/*userOnly*/, appIdName, "open", exeCanonicalNameEscaped + " " + percent1))
            {
                LOG_ERR<<"Failed to register appid"<<"\n";
                return -1;
            }

            //auto wParam = umba::fromUtf8(param);
            //auto extListCommaSepW = umba::fromUtf8(extListCommaSep);
            //if (!regShellExtentionHandlerForExtList(false/*userOnly*/, appIdName, extListCommaSepW))
            if (!umba::shellapi::win32::registerShellExtentionHandlerForExtentionList(false/*userOnly*/, appIdName, extListCommaSep))
            {
                LOG_ERR<<"Failed to register appid as handler"<<"\n";
                return -1;
            }

            #else

                LOG_MSG << "--register-view-handler option not supported\n";

            #endif

            return 1;
        }

        #endif

        //LOG_MSG << argsParser.programLocationInfo.exeFullName << "\n";

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
            return umba::command_line::autocompletionInstaller( pCol, opt, pCol->getPrintHelpStyle(), true, [&]( bool bErr ) -> decltype(auto) { return bErr ? LOG_ERR : LOG_MSG; } );
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
            return umba::command_line::autocompletionInstaller( pCol, opt, pCol->getPrintHelpStyle(), false, [&]( bool bErr ) -> decltype(auto) { return bErr ? LOG_ERR : LOG_MSG; } );
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

                // bool mdMode = argsParser.getPrintHelpStyle()==umba::command_line::PrintHelpStyle::md;
                // os << indent << appFullName << " version ";

//     os << appVersion << "\n";
//     return os;
// }
//
// template<typename StreamType> inline
// StreamType& printNameVersion( StreamType &os, const std::string &indent = "" )
// {
//     os << indent << appFullName << " version ";
//     printOnlyVersion(os);

    // #if !defined(UMBA_PRINT_NAME_VERSION_NO_PLATFORM_ARCHITECTURE)
    // os << umba::getAppPlatformArchitecture() <<"\n";
    // #endif
    //
    // #if !defined(UMBA_PRINT_NAME_VERSION_NO_COMPILER_INFO)
    // std::string compilerFullInfoString = umba::getCompilerNameVersionString();
    // {
    //     std::string compilerSimulateFullInfoString = umba::getCompilerSimulateNameVersionString();
    //     if (!compilerSimulateFullInfoString.empty())
    //     {
    //         compilerFullInfoString += " (as ";
    //         compilerFullInfoString += compilerSimulateFullInfoString;
    //         compilerFullInfoString += ")";
    //     }
    // }
    //
    // os << "Built with " << compilerFullInfoString <<" compiler\n";
    // #endif
    //
    // #if !defined(UMBA_PRINT_NAME_VERSION_NO_BUILD_DATE_TIME)
    // os << "Built at "<< appBuildDate <<" "<< appBuildTime <<"\n";
    // #endif

                //auto helpText = opt.getHelpOptionsString();

                if (pCol && pCol->isNormalPrintHelpStyle() && argsParser.argsNeedHelp.empty())
                {
                    //argsParser.printHelpPage( std::cout, "[OPTIONS] input_file [output_file]", "If output_file not taken, STDOUT used", helpText );
                    auto helpText = opt.getHelpOptionsString();
                    std::cout << "Usage: " << argsParser.programLocationInfo.exeName
                              << " [OPTIONS] input_file [output_file]\n"
                              << "  If output_file not taken, STDOUT used\n"
                              << "\nOptions:\n\n"
                              << helpText;
                              //<< " [OPTIONS] input_file [output_file]\n\nOptions:\n\n"<<helpText;
                }

                if (pCol) // argsNeedHelp
                {
                    argsParser.printHelpPage( std::cout
                                            , "[OPTIONS] input_file [output_file]"
                                            , "If output_file not taken, STDOUT used"
                                            , pCol->makeText( 78, &argsParser.argsNeedHelp )
                                            );
                    // std::cout<<pCol->makeText( 78, &argsParser.argsNeedHelp );
                }

                return 1;

            }

            return 0; // simple skip then parse builtins
        }

        else
        {
            LOG_ERR<<"unknown option: "<<opt.argOrg<<"\n";
            return -1;
        }

        return 0;

    } // if (opt.isOption())

    else if (opt.isResponseFile())
    {
        //std::string

        StringType optName;
        umba::utfToStringTypeHelper(optName, opt.name);
        auto optFileName = argsParser.makeAbsPath(optName);

        if (!argsParser.quet)
        {
            #if !defined(NDEBUG)
            std::cout << "Processing options file: " << optFileName << "\n";
            #endif
        }

        argsParser.pushOptionsFileName(optFileName);
        auto parseRes = argsParser.parseOptionsFile( optFileName );
        argsParser.popOptionsFileName();

        if (!parseRes)
            return -1;

        if (argsParser.mustExit)
            return 1;

        return 0;

    }

    if (inputFilename.empty())
    {
        inputFilename = argsParser.makeAbsPath(a);
    }
    else
    {
        outputFilename = argsParser.makeAbsPath(a);
    }


    return 0;

}

}; // struct ArgParser



class CommandLineOptionCollector : public umba::command_line::CommandLineOptionCollectorImplBase
{
protected:
    virtual void onOptionDup( const std::string &opt ) override
    {
        LOG_ERR<<"Duplicated option key - '"<<opt<<"'\n";
        throw std::runtime_error("Duplicated option key");
    }

};



