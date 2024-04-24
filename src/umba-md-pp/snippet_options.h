#pragma once

#include "enums.h"
#include "umba/string_plus.h"
//
#include <exception>
#include <stdexcept>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <utility>
	


// Опция "делать-что-то" - включает
// Опция "не-делать-что-то" - выключает, если было включено

// По умолчанию никакие опции не установлены
// Где-то в конфигах по умолчанию делаем --set-snippet-options=bla-bla
// Потом можем ещё раз сделать --set-snippet-options=bla-bla, и дополнительно включить или выключить какие-либо опции
// Потом при обработке снипета ещё раз парсим опции, устанавливаем/сбрасываем.

// ifdef:VAL
// ifndef:VAL
// if:VAL==
// if:VAL!=
// if:VAL>=
// if:VAL<=
// if:VAL>
// if:VAL<


//----------------------------------------------------------------------------
inline
std::vector< std::pair<ConditionOperators, std::string> > makeConditionOperatorsInfoVec()
{
    return std::vector< std::pair<ConditionOperators, std::string> >{ { ConditionOperators::equal       , "==" }
                                                                    , { ConditionOperators::notEqual    , "!=" }
                                                                    , { ConditionOperators::greaterEqual, ">=" }
                                                                    , { ConditionOperators::lessEqual   , "<=" }
                                                                    , { ConditionOperators::greater     , ">" }
                                                                    , { ConditionOperators::less        , "<" }
                                                                    };
}

//----------------------------------------------------------------------------
inline
const std::vector< std::pair<ConditionOperators, std::string> >& getConditionOperatorsInfoVec()
{
    static auto vec = makeConditionOperatorsInfoVec();
    return vec;
}

//----------------------------------------------------------------------------
inline
ConditionOperators splitCondition(std::string str, std::string &left, std::string &right)
{
    const auto &condInfoVec = getConditionOperatorsInfoVec();
    for(const auto &condInfo : condInfoVec)
    {
        auto pos = str.find(condInfo.second);
        if (pos!=str.npos)
        {
            left .assign(str, 0, pos);
            right.assign(str, pos+condInfo.second.size(), str.npos);
            umba::string_plus::trim(left);
            umba::string_plus::trim(right);
            return condInfo.first;
        }
    }

    return ConditionOperators::unknown;
}

//----------------------------------------------------------------------------
inline
bool isConditionVar(std::string &condVal)
{
    if (condVal.empty())
        return false;

    if (condVal.front()!='$')
        return false;

    condVal.erase(0u, 1u); // Удаляем бакс
    if (condVal.empty())
        return false;

    if (condVal.front()=='$') // Это литерал бакс, был экранирован баксом
        return false;

    return true; // Начиналось с бакса, оказалось непустым именем переменной, и не экраинированным баксом

}

//----------------------------------------------------------------------------
inline
bool isConditionTrue(const std::unordered_map<std::string, std::string> &condVars, std::string condStr)
{
    umba::string_plus::trim(condStr);

    std::string left ;
    std::string right;

    ConditionOperators condOp = splitCondition(condStr, left, right);
    if (condOp==ConditionOperators::unknown)
    {
        // нет оператора отношения
        if (!isConditionVar(condStr))
        {
            // и это - не переменная, так нельзя
            throw std::runtime_error("Invalid condition");
        }
        else
        {
            // если в if у нас указана переменная, то это аналогично ifdef, то есть, её надо просто поискать в condVars
            return condVars.find(condStr)!=condVars.end();
        }
    }
    else
    {
        // у нас есть выражения справа и слева
        // если участвует неизвестная переменная слева или справа, то результатом будет всегда false

        if (isConditionVar(left))
        {
            std::unordered_map<std::string, std::string>::const_iterator it = condVars.find(left);
            if (it==condVars.end())
                return false;
        }

        if (isConditionVar(right))
        {
            std::unordered_map<std::string, std::string>::const_iterator it = condVars.find(right);
            if (it==condVars.end())
                return false;
        }

        // Теперь слева и справа у нас значения, которые можно сравнивать
        switch(condOp)
        {
            case ConditionOperators::equal       : return left == right;
            case ConditionOperators::notEqual    : return left != right;
            case ConditionOperators::greaterEqual: return left >= right;
            case ConditionOperators::lessEqual   : return left <= right;
            case ConditionOperators::greater     : return left >  right;
            case ConditionOperators::less        : return left <  right;
            case ConditionOperators::unknown     : throw std::runtime_error("Unexpected unknown condition operator");
            default                              : throw std::runtime_error("Unknown condition operator");
        }
    }
}

//----------------------------------------------------------------------------
inline
bool splitToPair(const std::string &str, std::string &first, std::string &second, char ch)
{
    if (umba::string_plus::split_to_pair(str, first, second, ch))
    {
        umba::string_plus::trim(first);
        umba::string_plus::trim(second);
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------
inline
// std::string serializeSnippetOptions(const std::unordered_set<SnippetOptions> &flagOptions, const std::unordered_map<SnippetOptions, int> &intOptions)
std::string serializeSnippetOptions(std::unordered_set<SnippetOptions> flagOptions, const std::unordered_map<SnippetOptions, int> &intOptions)
{
    flagOptions.erase(SnippetOptions::snippetOptions);

    std::vector<std::string> optList;

    for(std::unordered_set<SnippetOptions>::const_iterator fit=flagOptions.begin(); fit!=flagOptions.end(); ++fit)
    {
        optList.emplace_back(enum_serialize(*fit));
    }

    for(std::unordered_map<SnippetOptions, int>::const_iterator fit=intOptions.begin(); fit!=intOptions.end(); ++fit)
    {
        std::string optName = enum_serialize(fit->first);
        std::string optVal  = std::to_string(fit->second);
        optList.emplace_back(optName+"="+optVal);
    }

    return umba::string_plus::merge< std::string, std::vector<std::string>::const_iterator >( optList.begin(), optList.end(), std::string(",") );
}

//----------------------------------------------------------------------------
inline
SnippetOptionsParsingResult deserializeSnippetOptions(const std::string &optListStr, std::unordered_set<SnippetOptions> *pFlagOptions, std::unordered_map<SnippetOptions, int> *pIntOptions=0, const std::unordered_map<std::string, std::string> *pCondVars=0)
{
    std::vector<std::string> optList = marty_cpp::splitToLinesSimple(optListStr, false, ',');

    //const std::unordered_map<std::string, std::string> *pCondVars

    for(auto opt : optList)
    {
        umba::string_plus::trim(opt);

        std::string optName, optVal;

        // ifdef:VAL
        // ifndef:VAL
        // if:$VAL==
        if (splitToPair(opt, optName, optVal, ':') && (optName=="ifdef" || optName=="ifndef" || optName=="if"))
        {
            if (pCondVars)
            {
                const std::unordered_map<std::string, std::string> &condVars = *pCondVars;
                if (optName=="ifdef")
                {
                    return condVars.find(optVal)==condVars.end() ? SnippetOptionsParsingResult::okButCondition : SnippetOptionsParsingResult::ok;
                }
                else if (optName=="ifndef")
                {
                    return condVars.find(optVal)!=condVars.end() ? SnippetOptionsParsingResult::okButCondition : SnippetOptionsParsingResult::ok;
                }
                else // if (optName=="if")
                {
                    try
                    {
                        return isConditionTrue(condVars, optVal) ? SnippetOptionsParsingResult::ok : SnippetOptionsParsingResult::okButCondition;
                    }
                    catch(...)
                    {
                        //bool isConditionTrue(const std::unordered_map<std::string, std::string> &condVars, std::string condStr)
                        return SnippetOptionsParsingResult::fail;
                    }
                }
            }
        }
        else if (splitToPair(opt, optName, optVal, '='))
        {
            if (pIntOptions)
            {
                umba::string_plus::trim(optName);
                umba::string_plus::trim(optVal);
    
                auto optId = enum_deserialize(optName, SnippetOptions::invalid);
                if (optId==SnippetOptions::invalid)
                {
                    return SnippetOptionsParsingResult::fail;
                }
    
                if ((((std::uint32_t)optId)&0xF000u)!=0x2000u)
                {
                    // Not a numeric option
                    return SnippetOptionsParsingResult::fail;
                }
    
                try
                {
                    int iVal = std::stoi(optVal);
                    std::unordered_map<SnippetOptions, int> &intOptions = *pIntOptions;
                    intOptions[optId] = iVal;
                }
                catch(...)
                {
                    return SnippetOptionsParsingResult::fail;
                }
            }
        }
        else // flag option
        {
            if (pFlagOptions)
            {
                std::unordered_set<SnippetOptions> &flagOptions = *pFlagOptions;

                auto optId = enum_deserialize(opt, SnippetOptions::invalid);
                if (optId==SnippetOptions::invalid)
                {
                    return SnippetOptionsParsingResult::fail;
                }
    
                if ((((std::uint32_t)optId)&0xF000u)!=0x1000u)
                {
                    // Not a flag option
                    return SnippetOptionsParsingResult::fail;
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
    }

    return SnippetOptionsParsingResult::ok;
}

//----------------------------------------------------------------------------
inline
bool testFlagSnippetOption(const std::unordered_set<SnippetOptions> &flagOptions, SnippetOptions opt)
{
    auto baseOpt = (SnippetOptions)(((std::uint32_t)opt)|0x0001u);

    if (flagOptions.find(baseOpt)!=flagOptions.end())
        return true;

    return false;
}

//----------------------------------------------------------------------------
inline
SnippetOptionsParsingResult parseSnippetInsertionCommandLine( std::unordered_set<SnippetOptions>       &snippetFlagsOptions
                                                            , std::unordered_map<SnippetOptions, int>  &snippetIntOptions
                                                            , const std::unordered_map<std::string, std::string> &condVars
                                                            , std::string line, std::string &snippetFile, std::string &snippetTag
                                                            )
{
    umba::string_plus::trim(line);

    if (!umba::string_plus::starts_with_and_strip(line, ("#!insert")))
        return SnippetOptionsParsingResult::fail;

    umba::string_plus::trim(line);

    if (line.empty())
        return SnippetOptionsParsingResult::fail;

    if (line[0]=='{')
    {
        std::string::size_type idx = 1u;
        for(; idx!=line.size() && line[idx]!='}'; ++idx) { }
        if (idx==line.size())
            return SnippetOptionsParsingResult::fail; // не нашли завершающую фигурную скобку - что-то пошло не так

        auto optionsString = std::string(line, (std::string::size_type)1u, idx-1u);
        line.erase(0u, idx+1);
        umba::string_plus::trim(line);

        // обновляем переданные нам дефолтные опции
        auto res = deserializeSnippetOptions(optionsString, &snippetFlagsOptions, &snippetIntOptions, &condVars);
        if (res!=SnippetOptionsParsingResult::ok)
            return res; // что-то пошло не так
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

    return SnippetOptionsParsingResult::ok;
}








        //     auto docGenCommand = dotNutDocGen::enum_deserialize(strVal, dotNutDocGen::DocGenCommand::invalid);
        //     if (docGenCommand==dotNutDocGen::DocGenCommand::invalid)
        //     {
        //         LOG_ERR_OPT<<"Invalid value in "<<opt.argOrg<<" option"<<", allowed vals: translation,tr/markdown,md/listing,lst/document,doc\n";
        //         return -1;
        //     }
