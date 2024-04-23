#pragma once

#include "enums.h"
#include "umba/string_plus.h"
//
#include <unordered_set>
#include <unordered_map>
#include <string>


// Опция "делать-что-то" - включает
// Опция "не-делать-что-то" - выключает, если было включено

// По умолчанию никакие опции не установлены
// Где-то в конфигах по умолчанию делаем --set-snippet-options=bla-bla
// Потом можем ещё раз сделать --set-snippet-options=bla-bla, и дополнительно включить или выключить какие-либо опции
// Потом при обработке снипета ещё раз парсим опции, устанавливаем/сбрасываем.


inline
bool deserializeSnippetOptions(const std::string &optListStr, std::unordered_set<SnippetOptions> &flagOptions, std::unordered_map<SnippetOptions, int> *pIntOptions=0)
{
    std::vector<std::string> optList = marty_cpp::splitToLinesSimple(optListStr, false, ',');

    for(auto opt : optList)
    {
        umba::string_plus::trim(opt);

        std::string optName, optVal;
        if (umba::string_plus::split_to_pair(opt, optName, optVal, '='))
        {
            if (!pIntOptions)
            {
                return false; // numeric options not allowed
            }

            umba::string_plus::trim(optName);
            umba::string_plus::trim(optVal);

            auto optId = enum_deserialize(optName, SnippetOptions::invalid);
            if (optId==SnippetOptions::invalid)
            {
                return false;
            }

            if ((((std::uint32_t)optId)&0xF000u)!=0x2000u)
            {
                // Not a numeric option
                return false;
            }

            try
            {
                int iVal = std::stoi(optVal);
                std::unordered_map<SnippetOptions, int> &intOptions = *pIntOptions;
                intOptions[optId] = iVal;
            }
            catch(...)
            {
                return false;
            }

        }
        else // flag option
        {
            auto optId = enum_deserialize(opt, SnippetOptions::invalid);
            if (optId==SnippetOptions::invalid)
            {
                return false;
            }

            if ((((std::uint32_t)optId)&0xF000u)!=0x1000u)
            {
                // Not a flag option
                return false;
            }

            bool isOff = false;
            if ((((std::uint32_t)optId)&0x0001u)==0x0000u)
            {
                isOff = true;
            }

            auto baseOpt = (SnippetOptions)(((std::uint32_t)optId)|0x0001u);

            if (isOff)
            {
                flagOptions.erase(baseOpt);
            }
            else
            {
                flagOptions.insert(baseOpt);
            }
        
        }
    
    }

    return true;
}

inline
bool testFlagSnippetOption(const std::unordered_set<SnippetOptions> &flagOptions, SnippetOptions opt)
{
    auto baseOpt = (SnippetOptions)(((std::uint32_t)opt)|0x0001u);

    if (flagOptions.find(baseOpt)!=flagOptions.end())
        return true;

    return false;
}

inline
bool parseSnippetInsertionCommandLine(std::unordered_set<SnippetOptions> &snippetFlagsOptions, std::string line, std::string &snippetFile, std::string &snippetTag)
{
    umba::string_plus::trim(line);

    if (!umba::string_plus::starts_with_and_strip(line, ("#!snippet")))
        return false;

    umba::string_plus::trim(line);

    if (line.empty())
        return false;

    if (line[0]=='{')
    {
        std::string::size_type idx = 1u;
        for(; idx!=line.size() && line[idx]!='}'; ++idx) { }
        if (idx==line.size())
            return false; // не нашли завершающую фигурную скобку - что-то пошло не так

        auto optionsString = std::string(line, (std::string::size_type)1u, idx); //!!!
        line.erase(0u, idx); //!!!
        umba::string_plus::trim(line);

        // обновляем переданные нам дефолтные опции
        if (deserializeSnippetOptions(optionsString, snippetFlagsOptions))
            return false; // что-то пошло не так

    }

    auto hashPos = line.find('#');

    if (hashPos==line.npos)
    {
        // Нет тэга - включаем весь файл
        snippetTag.clear();
        snippetFile = line;
    }
    else
    {
        snippetTag .assign(line, hashPos+1u, line.npos);
        snippetFile.assign(line, 0u, hashPos);
    }

    return true;
}








        //     auto docGenCommand = dotNutDocGen::enum_deserialize(strVal, dotNutDocGen::DocGenCommand::invalid);
        //     if (docGenCommand==dotNutDocGen::DocGenCommand::invalid)
        //     {
        //         LOG_ERR_OPT<<"Invalid value in "<<opt.argOrg<<" option"<<", allowed vals: translation,tr/markdown,md/listing,lst/document,doc\n";
        //         return -1;
        //     }
