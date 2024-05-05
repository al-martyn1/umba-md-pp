/*! \file
    \brief Всякие тестики, которые раньше тусовались прямо в коде тулзы. Ни как не оформлены, сами по себе не запускаются
 */


    auto testCondition = [&](std::string cond, bool expectedRes)
    {
        std::cout << "Condition: " << cond << "\n";
        try
        {
            bool res = isConditionTrue(appConfig.conditionVars, cond);
            if (res==expectedRes)
            {
                std::cout << " + passed, res: " << (res?"true":"false") << "\n";
            }
            else
            {
                std::cout << " - failed, res: " << (res?"true":"false") << ", expected: " << (expectedRes?"true":"false") << "\n";
            }
        }
        catch(...)
        {
            std::cout << " - unexpected error\n";
        }
    };
    
    appConfig.addConditionVar("A", "A");
    appConfig.addConditionVar("B", "B");
    appConfig.addConditionVar("C:C");
    testCondition("$A==A", true);
    testCondition("$A!=A", false);
    testCondition("$A!=$A", false);
    testCondition("$A!=$B", true);
    testCondition("$C!=C", false);




    auto testGhIdGen = [&](std::string s)
    {
        std::cout << s << ": [" << umba::generateIdFromText_forGitHub(s) << "]\n";
    };
     
    testGhIdGen("Воскл-бакс! \"#$");
    testGhIdGen("Прцнт-апоз%&'");
    testGhIdGen("Опнскб-плюс()*+");
    testGhIdGen("Зпт,");
    testGhIdGen("Минус-");
    testGhIdGen("Тчк.");
    testGhIdGen("Дел-вопр/:;<=>?");
    testGhIdGen("Соб-подч@[\\]^_");
    testGhIdGen("Бктик`");
    testGhIdGen("ФгСкбЛ{");
    testGhIdGen("ФгСкбП}");
    testGhIdGen("Ор|");
    testGhIdGen("Тильда~");
    testGhIdGen("English Title");




    std::string levelStr;
    std::string headerText;
    splitHeaderLine("   ### Some ##", levelStr, headerText);



    std::unordered_set<SnippetOptions> snippetFlagsOptions = appConfig.snippetOptions;
    std::string snippetFile;
    std::string snippetTag ;
    if (!parseSnippetInsertionCommandLine(snippetFlagsOptions, "#!insert{no-lineno} path/to/sample/file.cpp#TAG", snippetFile, snippetTag))
    {
    }




    std::string tempFolder;
    if (createTempFolder(tempFolder, inputFilename, "umba-md-pp-view"))
    {
        std::cout << "Using temp folder: " << tempFolder << "\n";
    }
    else
    {
        std::cout << "Failed to create temp folder\n";
    }



    // auto helloWorld = mtr::tr("Hello world!");
    // std::cout << helloWorld << "\n";
    // std::cout << underwood::tr("Hello non-translated world!") << "\n";



    //mtr::tr_set_def_lang("en-US");

    // auto testFindLang = [&](std::string strLang)
    // {
    //     umba::time_service::TimeTick startTick = umba::time_service::getCurTimeMs();
    //     std::cout << "Taken lang  : " << strLang << ", found id: " << findLangTagByString(strLang) << "\n";
    //     std::cout << "Doxygen lang: " << findGoxygenLanguageByLangTag(findLangTagByString(strLang)) << "\n";
    //     umba::time_service::TimeTick tickDelta = umba::time_service::getCurTimeMs()-startTick;
    //     std::cout << "Time ellapsed: " << tickDelta << "\n\n";
    //  
    // };
    //  
    // testFindLang("Русский/Россия");
    // testFindLang("Русский");
    // testFindLang("en");
    // testFindLang("en-US");
    // testFindLang("English");
    // testFindLang("English/United Kingdom");
    // testFindLang("English/United States");
    // testFindLang("Russian/Russia");


