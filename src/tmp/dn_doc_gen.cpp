/*! \file
    \brief DotNut Docs Generator
*/

#if 0
#include "umba/umba.h"
//
#include "umba/simple_formatter.h"

//
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <queue>
#include <deque>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <random>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <functional>
#include <utility>
#include <array>
#include <optional>

//
#include "umba/filename.h"
#include "umba/filesys.h"

//
#include "simplesquirrel/simplesquirrel.hpp"
#include "marty_simplesquirrel/simplesquirrel.h"

//
#include "marty_draw_context/bindings/simplesquirrel.h"
#endif

#include "umba/umba.h"
//
#include "umba/string_plus.h"
//
#include "umba/macro_helpers.h"
#include "umba/macros.h"
//
#include "umba/debug_helpers.h"
#include "umba/cli_tool_helpers.h"
#include "umba/filename.h"
#include "umba/filesys.h"
//
#include "marty_cpp/marty_cpp.h"
#include "marty_cpp/src_normalization.h"
//
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <queue>
#include <deque>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <random>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <functional>
#include <utility>
#include <array>
#include <optional>


//----------------------------------------------------------------------------
// 
#include "encoding/encoding.h"

// #pragma warning(disable:4717)
#include "umba/simple_formatter.h"
//
#include "dotNutBase/dotNutBase.h"
//

#include "simplesquirrel/simplesquirrel.hpp"
#include "marty_simplesquirrel/script_loader.h"
#include "marty_simplesquirrel/simplesquirrel.h"
#include "marty_simplesquirrel/squirrel_closure.h"
#include "marty_simplesquirrel/squirrel_interface.h"
#include "marty_simplesquirrel/class_info.h"

//
#include "dotNut/bindings/simplesquirrel.h" 
//
#include "marty_draw_context/bindings/simplesquirrel.h"
//
#include "enums.h"
#include "marty_simplesquirrel/types.h"

//
#include "dn_doc_gen_app_config.h"
#include "cppHelpers.h"


#ifdef _MSC_VER
#pragma comment(lib, "_squirrel.lib")
#pragma comment(lib, "_simplesquirrel.lib")
#endif


// #if defined(WIN32) || defined(_WIN32)
//     #include <windows.h>
// #endif


umba::StdStreamCharWriter coutWriter(std::cout);
umba::StdStreamCharWriter cerrWriter(std::cerr);
umba::NulCharWriter       nulWriter;

umba::SimpleFormatter umbaLogStreamErr(&cerrWriter);
umba::SimpleFormatter umbaLogStreamMsg(&cerrWriter);
umba::SimpleFormatter umbaLogStreamNul(&nulWriter);

bool umbaLogGccFormat   = false; // true;
bool umbaLogSourceInfo  = false;

#include "dn_doc_gen_app_ver_config.h"

#include "log.h"
#include "dn_doc_gen_app_config.h"

AppConfig     appConfig;

#include "dn_doc_gen_arg_parser.h"

#include "utils.h"



// Нам нужно:

// 1) Сгенерировать JSON с текстами описаний типов enum/флагов, и их значений.
//    Для этого мы считываем заданный файл в трансляцию, дополняем трансляцию, и сохраняем в тот же файл
//    Для каждого языка надо производить отдельный запуск утилиты

//    Надо задать опции:
//    - имя файла трансляции
//    - язык


// 2) Сгенерировать текст документации
//    Генератор генерит все разделы в один текст, разделяя заданным разделителем
//    Для простоты сразу после разделителя добавляет имя типа отдельной строкой, чтобы было проще парсить.
//    Или будем извлекать из названия секции?
//    Надо разделить этот текст на фрагменты, и записать всё в отдельные файлы
//    Полное имя типа преобразуем в путь при помощи tr_fix_category, 
//    фиксим также заданный язык при помощи tr_fix_lang_tag_format (или не надо?),
//    и добавляем это к заданному корневому пути, добавляем расширение .MD
//    и в этот файл сохраняем содержимое секции

//    Надо задать опции:
//    - путь к корню генерируемой документации
//    - язык



int main(int argc, char* argv[])
{


    #if 1
    {
        using namespace marty_simplesquirrel;

        #if 0
        FunctionParameterInfo fpi = FunctionParameterInfo("array of [int, string]   ar[]");
        std::cout << fpi.type << ": " << fpi.name << "\n";
        std::cout << "---" << "\n";


        std::string fnDefStr1 = "array of [int, string] doSomething ( A(a) a,B b,C c(ww), D d<>[] , array of [string, string] aE ) const";

        std::string type, name, suffix;
        std::vector<std::string> params;
        split_utils::splitFunctionPrototype(fnDefStr1, type, name, params, suffix);
        std::cout << type << ": " << name << "\n";
        std::cout << suffix << "\n";
        if (!params.empty())
        {
            std::cout << params[0] << "\n";
        }
        std::cout << "---" << "\n";


        auto 
        fnDef = FunctionInfo(fnDefStr1);
        //std::cout << fnDef.type << ": " << fnDef.name << "\n";
        std::cout << fnDef.formatFunctionPrototypeSquirrelLike(2u, 4u, 6u) << "\n";
        std::cout << "---" << "\n";

        fnDef = FunctionInfo("table{array,int,string} doSomething2 ( A a, Drawing.Context B,, array of [string, string] aE ) const");
        //std::cout << fnDef.type << ": " << fnDef.name << "\n";
        std::cout << fnDef.formatFunctionPrototypeSquirrelLike(2u, 4u, 6u) << "\n";
        std::cout << "---" << "\n";

        fnDef = FunctionInfo("table {array, int ,string} doSomething3 ( Drawing.Context B,, array of [string, nil] aE ) const");
        //std::cout << fnDef.type << ": " << fnDef.name << "\n";
        std::cout << fnDef.formatFunctionPrototypeSquirrelLike(2u, 4u, 6u) << "\n";
        std::cout << "---" << "\n";
        #endif


        #if 0
        std::cout << utils::convertTypeNameToPath("DotNut.FileSystem::FileSystem") << "\n";
        std::cout << "---" << "\n";

        auto testSplitTypeToParts = [&](std::string str)
        {
             std::vector<std::string> parts = split_utils::splitTypeToParts(str);
             std::cout << str << ":\n";
             for(auto p: parts)
             {
                 std::cout << "  |" << p << "|\n";
             }
        };

        testSplitTypeToParts("int, array of string");
        testSplitTypeToParts("array of [int, string]");
        testSplitTypeToParts("array of [int, array of [bool, float]]");
        testSplitTypeToParts("table{status,message}");
        std::cout << "---" << "\n";


        auto makeCheckStr = [](bool chk)
        {
            return std::string(chk ? "[V]" : "[ ]");
        };

        auto testIs = [&](std::string s)
        {
            std::size_t maxLen = 20u;
            maxLen = std::max(maxLen, s.size());
            std::size_t nFill = 0;
            if (maxLen>s.size())
            {
                nFill = maxLen - s.size();
            }

            std::cout << s << std::string(nFill, ' ') << "\n";
            std::cout << "  " << "isKeyword   : " << makeCheckStr(utils::isSquirrelTypeAllKeywords(s)) << "\n";
            std::cout << "  " << "isVoid      : " << makeCheckStr(utils::isSquirrelTypeVoidKeywords(s)) << "\n";
            std::cout << "  " << "isIntegral  : " << makeCheckStr(utils::isSquirrelTypeIntegralKeywords(s)) << "\n";
            std::cout << "  " << "isContainer : " << makeCheckStr(utils::isSquirrelTypeContainerKeywords(s)) << "\n";
            std::cout << "  " << "isIgnore    : " << makeCheckStr(utils::isSquirrelTypeIgnoreKeywords(s)) << "\n";
        };

        testIs("umba");
        testIs("DotNut");
        testIs("DotNut.FileSystem");
        testIs("int");
        testIs("array");
        testIs("integer");
        testIs("bool");
        testIs("string");
        testIs("float");
        std::cout << "---" << "\n";

        #endif // 0

        #if 0
        auto testSplitTypeToAlters = [&](std::string str)
        {
             std::vector<std::string> parts = split_utils::splitTypeToAlters(str);
             std::cout << str << ":\n";
             for(auto p: parts)
             {
                 std::cout << "  <" << p << ">\n";
             }
        };

        testSplitTypeToAlters("bool|integer|{string|table}");
        std::cout << "---" << "\n";
        #endif

    }
    #endif


    auto argsParser = umba::command_line::makeArgsParser( ArgParser()
                                                        , CommandLineOptionCollector()
                                                        , argc, argv
                                                        , umba::program_location::getProgramLocation
                                                            ( argc, argv
                                                            , false // useUserFolder = false
                                                            //, "" // overrideExeName
                                                            )
                                                        );

    // Force set CLI arguments while running under debugger
    if (umba::isDebuggerPresent())
    {
        argsParser.args.clear();

        // For translations test
        #if 0
        argsParser.args.push_back("--mode=translation");
        //argsParser.args.push_back("--translations=./translations/en-US.json");
        argsParser.args.push_back("--translations=./translations/ru-RU.json");
        argsParser.args.push_back("--md-add-vals=true");
        argsParser.args.push_back("../doc/refman.md/en-US/");
        #endif


        // For MD generation test
        #if 1
        
        argsParser.args.push_back("--api-def=..\\..\\_libs/dotNut/bindings/DotNut.sqapi.txt");
        argsParser.args.push_back("--overwrite=true");
        argsParser.args.push_back("--lang=ru-RU");
        argsParser.args.push_back("--translations=..\\..\\tests\\.\\tr\\ru-RU.json");
        argsParser.args.push_back("--mode=markdown");
        argsParser.args.push_back("--md-add-vals=true");
        argsParser.args.push_back("--bat-file-line-template=git add $(Name)");
        argsParser.args.push_back("--bat-file=..\\..\\tests\\.\\refman\\ru-RU\\git_add.bat");
        argsParser.args.push_back("--sources-root=..\\..\\nuts");
        argsParser.args.push_back("..\\..\\tests\\.\\refman\\ru-RU");

        #endif

        // Listing
        #if 0
        argsParser.args.push_back("--overwrite=true");
        argsParser.args.push_back("--lang=en-US");
        argsParser.args.push_back("--translations=..\\..\\tests\\.\\tr\\en-US.json");
        argsParser.args.push_back("--mode=listing");
        argsParser.args.push_back("--listing-nested-tags-mode=remove");
        argsParser.args.push_back("--listing-tr-update-mode=update");
        argsParser.args.push_back("--code-tag-prefix=//!#");
        //argsParser.args.push_back("--listing-tr-base-id=boomAnimatePaintId");
        argsParser.args.push_back("--code-tag=boomAnimatePaintBlaBla");
        argsParser.args.push_back("--listing-code-file=..\\..\\tests\\..\\nuts\\TestDrawContext.nut#boomAnimatePaint");
        argsParser.args.push_back("--output-file=..\\..\\tests\\test_lst.md");
        argsParser.args.push_back("..\\..\\tests\\.\\refman\\en-US");
        #endif

        // Doc template
        #if 0
        argsParser.args.push_back("--overwrite=true");
        argsParser.args.push_back("--lang=en-US");
        argsParser.args.push_back("--translations=..\\..\\tests\\.\\tr\\en-US.json");
        argsParser.args.push_back("--mode=document");
        argsParser.args.push_back("--listing-nested-tags-mode=remove");
        argsParser.args.push_back("--listing-tr-update-mode=update");
        argsParser.args.push_back("--code-tag-prefix=//!#");
        //argsParser.args.push_back("--listing-tr-base-id=boomAnimatePaintId");
        argsParser.args.push_back("--template-file=..\\..\\tests\\sample_doc.md");
        argsParser.args.push_back("--output-file=sample_doc.md");
        argsParser.args.push_back("..\\..\\tests\\.\\refman\\en-US");
        #endif

        //argsParser.args.push_back("");

        // --md-add-vals=true ..\doc\refman.md\en-US\
        // --lang=ru-RU 
        // --translations=.\translations\en-US.json
        // argsParser.args.push_back("--overwrite");
    }

    //programLocationInfo = argsParser.programLocationInfo;

    // try
    // {
        // Job completed - may be, --where option found
        if (argsParser.mustExit)
            return 0;
       
        // if (!argsParser.quet)
        // {
        //     printNameVersion();
        // }
       
        if (!argsParser.parseStdBuiltins())
            return 1;
        if (argsParser.mustExit)
            return 0;
       
        if (!argsParser.parse())
            return 1;
        if (argsParser.mustExit)
            return 0;
    // }
    // catch(const std::exception &e)
    // {
    //     LOG_ERR_OPT << e.what() << "\n";
    //     return -1;
    // }
    // catch(const std::exception &e)
    // {
    //     LOG_ERR_OPT << "command line arguments parsing error" << "\n";
    //     return -1;
    // }


    //if (hasHelpOption)

    if (!argsParser.quet  /* && !hasHelpOption */ )
    {
        //printNameVersion();
        //LOG_MSG_OPT<<"\n";
        umba::cli_tool_helpers::printNameVersion(umbaLogStreamMsg);
    }



    if (appConfig.generationOptions.mdLang.empty())
    {
        LOG_ERR_OPT<<"No target lang taken\n";
        return 1;
    }

    marty_tr::tr_set_lang_tag_format(marty_tr::ELangTagFormat::langTag); // langTagNeutral/langTagNeutralAuto/langId/langIdFull/langIdX/langIdFullX
    marty_tr::tr_set_def_lang(appConfig.generationOptions.mdLang);

    if (!argsParser.quet)
    {
        LOG_MSG_OPT << "Language: " << marty_tr::tr_fix_lang_tag_format(appConfig.generationOptions.mdLang) << "\n";
    }

    auto generationOptions   = appConfig.generationOptions;

    auto callGeneration = [&]()
    {
        auto dotNutClassesInfo = marty_simplesquirrel::ClassInfo(true); // true - NS
        {
            dotNutClassesInfo.merge(dotNut::simplesquirrel::getDotNutCoreClassesInfo());
        }

        for(auto apiDefFile: appConfig.apiDefFiles)
        {
            std::string apiDefText;
            if (!umba::filesys::readFile(apiDefFile, apiDefText))
            {
                LOG_ERR_OPT<< appConfig.listingCodeFile<<": "<<"Failed to read source code file for listing"<< "\n";
                // return 1; // в лямбде нельзя выскочить из функции, в которой она задана, поэтому эта ошибка не останавливает обработку
                continue;
            }

            apiDefText = autoConvertToUtf8(apiDefText);

            try
            {
                dotNutClassesInfo.merge(parseApiDefData(apiDefText, apiDefFile));
            }
            catch(...)
            {
            }
        }


        auto drawingClassesInfo = marty_simplesquirrel::ClassInfo(true); // true - NS
        {
            drawingClassesInfo.merge(marty_draw_context::simplesquirrel::getDrawingCoreClassesInfo());
        }
        
        auto dotNutClassDocs  = marty_simplesquirrel::generateClassDocs(dotNutClassesInfo, generationOptions);
        auto drawingClassDocs = marty_simplesquirrel::generateClassDocs(drawingClassesInfo, generationOptions);
        auto enumDocs  = // marty_vk::simplesquirrel::enumsExposeMakeScript("Vk", generationOptions)
                        /* + */ marty_draw_context::simplesquirrel::enumsExposeMakeScript( generationOptions // singleLineScript
                                                                                  ,  /* const std::string &prefix = */  "Drawing"
                                                                                  )
                       // + _SC("\n\n\n")
                       + dotNut::simplesquirrel::dotNutCoreEnumsExposeMakeScript(false /* createTable */ , generationOptions)
                       ;

        return enumDocs + dotNutClassDocs + drawingClassDocs; // + enumDocs;
    };

    //appConfig.translationsFile



    if (appConfig.docGenCommand==dotNutDocGen::DocGenCommand::unknown)
    {
        LOG_ERR_OPT<<"No generation mode taken "<<"(--mode)"<<"\n";
        return 1;
    }

    else if (appConfig.docGenCommand==dotNutDocGen::DocGenCommand::translation)
    {

        if (!readUpdateTranslations(appConfig.translationsFile, false /* !allowNoTranslations */ ))
        {
            // return 1; // Не надо тут паниковать
        }

        if (appConfig.targetName.empty())
        {
            LOG_ERR_OPT<<"No target file name taken\n";
            return 1;
        }

        generationOptions.generationType = marty_simplesquirrel::EnumScriptGenerationType::trTemplate;

        // Если текст пустой, то будет как будто сообщения нет
        marty_tr::tr_set_empty_msg_not_exist(true);

        auto targetName = umba::filename::makeAbsPath(appConfig.targetName);


        //------------------------------
        auto addMdCommonStr = [=]( std::string name, std::string val)
        {
            if (!marty_tr::tr_has_msg(name, std::string("_md-common"), generationOptions.mdLang))
            {
                marty_tr::tr_add_if_empty(name, val, std::string("_md-common"), generationOptions.mdLang);
            }
        };

        auto addSqCommonStr = [=]( std::string name, std::string val)
        {
            if (!marty_tr::tr_has_msg(name, std::string("_sq-common"), generationOptions.mdLang))
            {
                marty_tr::tr_add_if_empty(name, val, std::string("_sq-common"), generationOptions.mdLang);
            }
        };

        //----------------------------------------------------------------------------
        
        if (generationOptions.mdLang=="en-US")
        {
            #include "addCommonStringsEn.h"
        }
         
        if (generationOptions.mdLang=="ru-RU")
        {
            #include "addCommonStringsRu.h"
        }


        callGeneration();

        ///impl_helpers
        //tr_get_all_translations

        std::string resultJson = marty_tr::tr_serialize_translations(marty_tr::tr_get_all_translations(), 2 /* indent */);
        umba::filesys::createDirectoryEx( umba::filename::getPath(targetName), true /* forceCreatePath */  );
        umba::filesys::writeFile(targetName, resultJson, appConfig.bOverwrite ); // overwrite

        return 0;
    }

    else if (appConfig.docGenCommand==dotNutDocGen::DocGenCommand::listing)
    {
        // [V] appConfig.translationsFile
        if (!readUpdateTranslations(appConfig.translationsFile, false /* !allowNoTranslations */ ))
        {
            // return 1; // Не надо тут паниковать
        }

        // [V] appConfig.targetName
        if (appConfig.targetName.empty())
        {
            LOG_ERR_OPT<<"No target path taken\n";
            return 1;
        }

        // [V] appConfig.listingCodeFile
        if (appConfig.listingCodeFile.empty())
        {
            LOG_ERR_OPT<<"No code file name taken (--listing-code-file)\n";
            return 1;
        }

        // [V] inplace tag
        {
            std::string::size_type hashPos = appConfig.listingCodeFile.find('#');
            if (hashPos!=appConfig.listingCodeFile.npos)
            {
                appConfig.codeTag        .assign(appConfig.listingCodeFile, hashPos+1); // Если метка задана с именем файла, то перетираем то, что задано опцией
                appConfig.listingCodeFile.assign(appConfig.listingCodeFile, 0, hashPos);
            }
        }

        // [V] appConfig.outputFile
        if (appConfig.outputFile.empty())
        {
            appConfig.outputFile = replaceFileExt(appConfig.listingCodeFile, appConfig.generationOptions.mdFileExt);
        }


        // [V] appConfig.listingTrBaseId
        if (appConfig.listingTrBaseId.empty())
        {
            auto listingTrBaseId = umba::filename::makeCanonical(appConfig.listingCodeFile);
            listingTrBaseId.append(1,'#');
            listingTrBaseId.append(appConfig.codeTag);
            appConfig.listingTrBaseId = listingTrBaseId;
        }


        if (!appConfig.codeTag.empty()) // выкусываем только часть сорца, поэтому нужен codeTagPrefix
        {
            // [V] appConfig.codeTagPrefix
            if (appConfig.generationOptions.codeTagPrefix.empty())
            {
                LOG_ERR_OPT<<"No code tags prefix taken (--code-tag-prefix)\n";
                return 1;
            }
        }

        // Если текст пустой, то будет как будто сообщения нет
        marty_tr::tr_set_empty_msg_not_exist(true);

        std::string allCodeText;

        auto listingCodeFile = appConfig.listingCodeFile; // umba::filename::makeCanonical(umba::filename::makeAbsPath(appConfig.listingCodeFile));
        if (!umba::filesys::readFile(listingCodeFile, allCodeText))
        {
            LOG_ERR_OPT<< appConfig.listingCodeFile<<": "<<"Failed to read source code file for listing"<< "\n";
            return 1;
        }

        std::string allCodeTextNormalized = marty_cpp::normalizeCrLfToLf(allCodeText);

        auto listingCode = extractCodeFragment( allCodeTextNormalized, appConfig.codeTag, appConfig.generationOptions.codeTagPrefix, appConfig.generationOptions.listingNestedTagsMode );

        std::string resBuf;
        auto resWritter = umba::StringRefCharWriter(resBuf);
        auto res = umba::SimpleFormatter(&resWritter);


        const std::string catId   = "_sq-listings";
        const std::string titleId = appConfig.listingTrBaseId + "-title";
        const std::string textId  = appConfig.listingTrBaseId + "-text";

        auto sectionTitle = umba::filename::makeCanonical(appConfig.listingCodeFile, '/'); // appConfig.listingTrBaseId;

        if (appConfig.generationOptions.hasTr(titleId, catId))
        {
            sectionTitle = appConfig.generationOptions.getTr(titleId, catId);
        }
        else
        {
            appConfig.generationOptions.addTr(titleId, catId);
        }

        // С заголовком вопрос порешали
        res << appConfig.generationOptions.makeSectionHeaderMdString(sectionTitle);
        res << "\n";


        if (appConfig.generationOptions.hasTr(textId, catId))
        {
            res << appConfig.generationOptions.getTr(textId, catId) << "\n\n";
        }
        else
        {
            appConfig.generationOptions.addTr(textId, catId);
        }

        res << appConfig.generationOptions.getCodeStartMdString();
        res << listingCode << "\n";
        res << appConfig.generationOptions.getCodeEndMdString();
        res << "\n";

        res.flush();

        if (appConfig.listingUpdateTrMode != dotNutDocGen::ListingUpdateTrMode::updateOnly)
        {
            auto fileText = marty_cpp::converLfToOutputFormat(resBuf, marty_cpp::ELinefeedType::detect);

            auto fullFileNameWithExt = umba::filename::appendPath(appConfig.targetName, appConfig.outputFile);
             
            if (!umba::filesys::writeFile(fullFileNameWithExt, fileText, appConfig.bOverwrite ))
            {
                LOG_ERR_OPT << "failed to write output file: '" << fullFileNameWithExt << "'\n";
                return 1;
            }
        
        }

        // Тут надо сохранить переводы
        if (appConfig.listingUpdateTrMode != dotNutDocGen::ListingUpdateTrMode::none)
        {
            std::string resultJson = marty_tr::tr_serialize_translations(marty_tr::tr_get_all_translations(), 2 /* indent */);
            umba::filesys::createDirectoryEx( umba::filename::getPath(appConfig.translationsFile), true /* forceCreatePath */  );
            umba::filesys::writeFile(appConfig.translationsFile, resultJson, appConfig.bOverwrite ); // overwrite
        }

        return 0;
    }

    else if (appConfig.docGenCommand==dotNutDocGen::DocGenCommand::document)
    {
        // [V] appConfig.translationsFile
        if (!readUpdateTranslations(appConfig.translationsFile, false /* !allowNoTranslations */ ))
        {
            // return 1; // Не надо тут паниковать
        }

        // [V] appConfig.targetName
        if (appConfig.targetName.empty())
        {
            LOG_ERR_OPT<<"No target path taken\n";
            return 1;
        }

        // [V] appConfig.listingCodeFile
        if (appConfig.docTemplateFile.empty())
        {
            LOG_ERR_OPT<<"No document template file name taken (--template-file)\n";
            return 1;
        }

        //!!! Тут надо поработать

        // [V] appConfig.outputFile
        if (appConfig.outputFile.empty())
        {
            appConfig.outputFile = replaceFileExt(appConfig.docTemplateFile, appConfig.generationOptions.mdFileExt);
        }


        // [V] appConfig.codeTagPrefix
        if (appConfig.generationOptions.codeTagPrefix.empty())
        {
            LOG_ERR_OPT<<"No code tags prefix taken (--code-tag-prefix)\n";
            return 1;
        }

        // Если текст пустой, то будет как будто сообщения нет
        marty_tr::tr_set_empty_msg_not_exist(true);

        std::string tplText;
         
        //auto listingCodeFile = appConfig.listingCodeFile; // umba::filename::makeCanonical(umba::filename::makeAbsPath(appConfig.listingCodeFile));
        if (!umba::filesys::readFile(appConfig.docTemplateFile, tplText))
        {
            LOG_ERR_OPT<< appConfig.docTemplateFile<<": "<<"Failed to read document template file for listing"<< "\n";
            return 1;
        }

        std::string processedText;
        
        try
        {
            processedText = processDocTemplate(appConfig.generationOptions, marty_cpp::normalizeCrLfToLf(tplText), appConfig.docTemplateFile, appConfig.outputFile);
        }
        catch(...)
        {
            return 1;
        }
        
         
        //if (appConfig.listingUpdateTrMode != dotNutDocGen::ListingUpdateTrMode::updateOnly)
        {
            auto fileText = marty_cpp::converLfToOutputFormat(processedText, marty_cpp::ELinefeedType::detect);
         
            auto fullFileNameWithExt = umba::filename::appendPath(appConfig.targetName, appConfig.outputFile);
             
            if (!umba::filesys::writeFile(fullFileNameWithExt, fileText, appConfig.bOverwrite ))
            {
                LOG_ERR_OPT << "failed to write output file: '" << fullFileNameWithExt << "'\n";
                return 1;
            }
         
        }
        //  
        // // Тут надо сохранить переводы
        // if (appConfig.listingUpdateTrMode != dotNutDocGen::ListingUpdateTrMode::none)
        // {
        //     std::string resultJson = marty_tr::tr_serialize_translations(marty_tr::tr_get_all_translations(), 2 /* indent */);
        //     umba::filesys::createDirectoryEx( umba::filename::getPath(appConfig.translationsFile), true /* forceCreatePath */  );
        //     umba::filesys::writeFile(appConfig.translationsFile, resultJson, appConfig.bOverwrite ); // overwrite
        // }

        return 0;
    }

    else if (appConfig.docGenCommand==dotNutDocGen::DocGenCommand::markdown)
    {
        if (!readUpdateTranslations(appConfig.translationsFile, false /* !allowNoTranslations */ ))
        {
            // return 1; // Не надо тут паниковать
        }

        if (appConfig.targetName.empty())
        {
            LOG_ERR_OPT<<"No target path taken\n";
            return 1;
        }

        if (appConfig.generationOptions.mdSectionLevel<1u || appConfig.generationOptions.mdSectionLevel>6)
        {
            LOG_ERR_OPT<<"Invalid markdown sections level ("<<"--md-section-level"<<")"<<"\n";
            return -1;
        }

        // Если текст пустой, то будет как будто сообщения нет
        marty_tr::tr_set_empty_msg_not_exist(true);

        generationOptions.generationType = marty_simplesquirrel::EnumScriptGenerationType::mdDoc;
        generationOptions.mdSectionSep   = "<<-{[!-<<>>-!]}->>";

        auto generatedMd = umba::toUtf8(callGeneration());

        std::string batFileText;

        //using FullQualifiedTypeNameMap = std::map< std::string, typename TVal, utils::FullQualifiedTypeNameLess >;

        marty_simplesquirrel::FullQualifiedTypeNameMap<std::string> allNamesMap;

        auto splittedMd = marty_cpp::simple_string_split(generatedMd, generationOptions.mdSectionSep + "\n");
        for(auto mdSec : splittedMd)
        {
            // У нас все статьи/секции идут скопом при генерации, для их разделения используется маркер странного вида, который маловероятно встретится в тексте
            // И первой строчкой у нас идёт описание того, что в статье
            // namespace Bla.Bla
            // enum Bla.Bla
            // flags Bla.Bla
            // method Bla.bla

            std::string::size_type lfPos = mdSec.find('\n');
            if (lfPos==std::string::npos)
            {
                continue;
            }

            std::string firstLine = std::string(mdSec, 0, lfPos);
            std::string secText   = std::string(mdSec, lfPos+1);

            // Выцепили первую строчку, и текст секции

            std::string::size_type spPos = firstLine.find(' ');
            if (spPos==std::string::npos)
            {
                continue; // в первой строке секции (с метаданными) - ошибка, пропускаем
            }

            // первым идёт тип (namespace/class/enum/flags/etc), вторым - имя описуемого
            std::string typenameType     = std::string(firstLine, 0, spPos);
            std::string typeName         = std::string(firstLine, spPos+1);

            allNamesMap[typeName] = typenameType;

            std::string typeNamePath = marty_simplesquirrel::utils::convertTypeNameToPath(typeName);

            // Тут уже вроде всё выцепили без ошибок, можно и отпроцессить на предмет спецссылок
            // std::string processDocTemplate( const marty_simplesquirrel::EnumScriptGenerationOptions genOptions
            //                   , std::string tpl
            //                   , const std::string &tplFilename
            //                   , const std::string &outputFilename
            //                   )

            // вообще это надо вынести за цикл
            auto tmpCurProcessedFile = umba::filename::appendPath(appConfig.sourcesRoot, std::string("dummy.txt"));

            secText = processDocTemplate( generationOptions
                                        , secText
                                        , tmpCurProcessedFile // typeNamePath // оно нужно, чтобы относительно пути к нему вычислять пути к сорцам для выкусывания фрагментов
                                        , typeNamePath
                                        );

            #if 0
            std::string::size_type secTextEstraMacroPos = secText.find("!#");
            if (secTextEstraMacroPos!=secText.npos)
            {
                LOG_ERR_OPT<<"Markdown subst error in : " << secText << "\n";

                secText = processDocTemplate( generationOptions
                                            , secText
                                            , tmpCurProcessedFile // typeNamePath // оно нужно, чтобы относительно пути к нему вычислять пути к сорцам для выкусывания фрагментов
                                            , typeNamePath
                                            );
            }
            #endif

            std::string fullFileName        = umba::filename::appendPath(appConfig.targetName  , typeNamePath);
            std::string fullFileNameWithExt = umba::filename::appendExt (fullFileName, generationOptions.mdFileExt);
            std::string fileNameWithExt     = umba::filename::appendExt (typeNamePath, generationOptions.mdFileExt);

            if (!appConfig.batFileLineTemplate.empty())
            {
                auto macros = umba::macros::StringStringMap<std::string>{ {"FullName"     , fullFileNameWithExt}
                                                                        , {"fullname"     , fullFileNameWithExt}
                                                                        , {"FullFileName" , fullFileNameWithExt}
                                                                        , {"FullFilename" , fullFileNameWithExt}
                                                                        , {"fullfilename" , fullFileNameWithExt}
                                                                        , {"Name"         , fileNameWithExt}
                                                                        , {"name"         , fileNameWithExt}
                                                                        , {"FileName"     , fileNameWithExt}
                                                                        , {"Filename"     , fileNameWithExt}
                                                                        , {"filename"     , fileNameWithExt}
                                                                        };
                auto batLine = umba::macros::substMacros( appConfig.batFileLineTemplate
                                                        , umba::macros::MacroTextFromMapRef<std::string>(macros)
                                                        , umba::macros::keepUnknownVars
                                                        );
                batLine.append(1u, '\n');
                batFileText.append(batLine);
            }

            //TODO: !!! Add type name to type map here


            std::string filePath = umba::filename::getPath(fullFileNameWithExt);
            std::string filePathCanonical = umba::filename::makeCanonical(filePath);
            umba::filesys::createDirectoryEx( filePathCanonical, true /* forceCreatePath */ );

            auto fileText = marty_cpp::converLfToOutputFormat(secText, marty_cpp::ELinefeedType::detect);

            if (!umba::filesys::writeFile(fullFileNameWithExt, fileText, appConfig.bOverwrite ))
            {
                LOG_ERR_OPT << "failed to write output file: '" << fullFileNameWithExt << "'\n";
                return 1;
            }

        } // for(auto mdSec : splittedMd)

        if (!appConfig.indexFile.empty())
        {
            //TODO: !!! Generate index file here
        }

        if (!appConfig.batFile.empty())
        {
            // batFileText
            std::string batFilePath = umba::filename::getPath(appConfig.batFile);
            umba::filesys::createDirectoryEx( batFilePath, true /* forceCreatePath */ );

            auto batFileTextFinal = marty_cpp::converLfToOutputFormat(batFileText, marty_cpp::ELinefeedType::detect);

            if (!umba::filesys::writeFile(appConfig.batFile, batFileTextFinal, appConfig.bOverwrite ))
            {
                LOG_ERR_OPT << "failed to write output BAT file: '" << appConfig.batFile << "'\n";
                return 1;
            }

        }

        if (!appConfig.indexFile.empty())
        {
            std::string resBuf;
            auto resWritter = umba::StringRefCharWriter(resBuf);
            auto res = umba::SimpleFormatter(&resWritter);

            const auto &genOptions = appConfig.generationOptions;

            if (genOptions.hasTr("index-section-title", "_md-common"))
            {
                res << genOptions.makeSectionHeaderMdString(genOptions.getTr("index-section-title", "_md-common"))<<"\n";
            }

            marty_simplesquirrel::FullQualifiedTypeNameMap<std::string>::const_iterator anmIt = allNamesMap.begin();
            for(; anmIt!=allNamesMap.end(); ++anmIt)
            {
                auto refText = marty_simplesquirrel::formatExternalMemberRef( genOptions, anmIt->first, anmIt->first, "index" ); 

                if (anmIt->second=="method" || anmIt->second=="function") //  || anmIt->second==
                {
                    continue;
                }

                res << "- " << genOptions.getTrSubst(anmIt->second + "-index-text", std::string("_md-common"), {{ "Name", anmIt->first }, { "RefText", refText }}) << "\n";
            }

            res.flush();
            auto indexFileTextFinal = marty_cpp::converLfToOutputFormat(resBuf, marty_cpp::ELinefeedType::detect);

            std::string fullIndexFileName    = umba::filename::appendPath(appConfig.targetName  , appConfig.indexFile);
            // std::string fullFileNameWithExt = umba::filename::appendExt (fullFileName, generationOptions.mdFileExt);
            // std::string fileNameWithExt     = umba::filename::appendExt (typeNamePath, generationOptions.mdFileExt);

            if (!umba::filesys::writeFile(fullIndexFileName, indexFileTextFinal, appConfig.bOverwrite ))
            {
                LOG_ERR_OPT << "failed to write output file: '" << fullIndexFileName << "'\n";
                return 1;
            }

        }

    }


    return 0;
}



