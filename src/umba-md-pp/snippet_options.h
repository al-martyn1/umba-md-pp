#pragma once

#include "enums.h"
#include "umba/string_plus.h"
//
#include "umba/macros.h"
#include "umba/macro_helpers.h"
//
#include <exception>
#include <stdexcept>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <utility>
	
//
#include "umba/container.h"



// umba::md::
namespace umba {
namespace md {


/*

 1) Запретить в метках/тэгах выкусывания фрагментов кода символ '-' - он только для разделения частей тэга
 2) Конечного тэга нет - ориентируемся по скобкам: "#start_tag-{}" - при этом скобки реально используются те, которые в конфиге для языка заданы, а не именно что фигурные
 3) Конечного номера строки нет - ориентируемся по скобкам: "#NNN-{}" - при этом скобки реально используются те, которые в конфиге для языка заданы, а не именно что фигурные
 4) У нас нет тэга, мы хотим выцепить по сигнатуре текста, возможно, многострочного: "#`inline\nvoid\ndoSomething()`-{}". 
    Сигнатуру можно искать только после определённого номера строки: "#NNN`inline\nvoid\ndoSomething()`-{}".
 5) Когда ищем скобки - строковые литералы не сканируем - слишком геморно пока, да и для разных языков оно разное, таким образом, может получится косяк распознования блока.
 6) Хотим закончить по generic stop маркеру: "#start_tag-(-)"/"#NNN-(-)"
 7) Хотим закончить по N пустых строк: "#start_tag-(N)"/"#NNN-(N)"

 Алгоритм работы текстовой сигнатуры.
 1) Парсим с учётом escape-последовательности
 2) Разбиваем на строки по переводу строки
 3) Каждую строку "нормализуем" - удаляем все пробелы
 4) Строки текстовой сигнатуры не обязательно будут встречаться на разных строках - их могут переформатировать туда-обратно. Надо просекать такой вариант, и на каждом шаге склеивать нужное количество строк перед проверкой

*/


struct TextSignature
{

    using options_type  = umba::container::small_vector_options< umba::container::growth_factor<umba::container::growth_factor_50>, umba::container::inplace_alignment<16> >::type;

    //using signature_lines_vector_type = umba::container::small_vector<std::string, 4, void, umba::container::small_vector_option_inplace_alignment_16_t, umba::container::small_vector_option_growth_50_t >;
    using signature_lines_vector_type = umba::container::small_vector<std::string, 4, void, options_type >;

    signature_lines_vector_type    signatureLinesVector;
    std::string                    normalizedSignature ;

}; // struct TextSignature


struct SnippetTagInfo
{
    using options_type  = umba::container::small_vector_options< umba::container::growth_factor<umba::container::growth_factor_50>, umba::container::inplace_alignment<16> >::type;

    using text_signature_vector = umba::container::small_vector<TextSignature, 4, void, options_type >;

    SnippetTagType             startType               = SnippetTagType::invalid;
    std::size_t                startNumber             = 0; // line number
    text_signature_vector      startTagOrSignaturePath ; // start tag or text signatures path. For start tag only one element of the  vector used

    SnippetTagType             endType                 = SnippetTagType::invalid;
    std::size_t                endNumber               = 0; // end line number or number of empty lines to stop
    TextSignature              endSignature            ;    // paths not supported here

}; // struct SnippetTagInfo



// enum class SnippetTagType : std::uint32_t
// {
//     invalid             = (std::uint32_t)(-1),
//     unknown             = (std::uint32_t)(-1),
//     normalTag           = 0x0000 /*!< Allowed for start/end */,
//     lineNumber          = 0x0001 /*!< Allowed for start/end */,
//     textSignature       = 0x0002 /*!< Allowed for start/end - end signature not included to code snippet */,
//     block               = 0x0003 /*!< Allowed for end only - signals that we need to cat code block in block symbols */,
//     genericStopMarker   = 0x0004 /*!< Allowed for end only */,
//     stopOnEmptyLines    = 0x0005 /*!< Allowed for end only */
//  
// }; // enum class SnippetTagType : std::uint32_t










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
bool isConditionTrue(const umba::macros::StringStringMap<std::string> &condVars, std::string condStr)
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
            umba::macros::StringStringMap<std::string>::const_iterator it = condVars.find(left);
            if (it==condVars.end())
                return false;
        }

        if (isConditionVar(right))
        {
            umba::macros::StringStringMap<std::string>::const_iterator it = condVars.find(right);
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

    std::set<std::string> flagOptionStrings;

    for(std::unordered_set<SnippetOptions>::const_iterator fit=flagOptions.begin(); fit!=flagOptions.end(); ++fit)
    {
        flagOptionStrings.insert(enum_serialize(*fit));
        //optList.emplace_back(enum_serialize(*fit));
    }

    std::map<std::string,std::string> intOptionStrings;
    for(std::unordered_map<SnippetOptions, int>::const_iterator fit=intOptions.begin(); fit!=intOptions.end(); ++fit)
    {
        std::string optName = enum_serialize(fit->first);
        std::string optVal  = std::to_string(fit->second);
        //optList.emplace_back(optName+"="+optVal);
        intOptionStrings[optName] = optVal;
    }

    std::vector<std::string> optList;
    for(auto opt: flagOptionStrings)
        optList.emplace_back(opt);

    for(auto it=intOptionStrings.begin(); it!=intOptionStrings.end(); ++it)
    {
        optList.emplace_back(it->first+"="+it->second);
    }

    return umba::string_plus::merge< std::string, std::vector<std::string>::const_iterator >( optList.begin(), optList.end(), std::string(",") );
}

//----------------------------------------------------------------------------
inline
SnippetOptionsParsingResult deserializeSnippetOptions(const std::string &optListStr, std::unordered_set<SnippetOptions> *pFlagOptions, std::unordered_map<SnippetOptions, int> *pIntOptions=0, const umba::macros::StringStringMap<std::string> *pCondVars=0)
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
                const umba::macros::StringStringMap<std::string> &condVars = *pCondVars;
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
SnippetOptionsParsingResult deserializeProcessingOptions(const std::string &optListStr, std::unordered_set<ProcessingOptions> &flagOptions)
{
    std::vector<std::string> optList = marty_cpp::splitToLinesSimple(optListStr, false, ',');

    for(auto opt : optList)
    {
        umba::string_plus::trim(opt);

        auto optId = enum_deserialize(opt, ProcessingOptions::invalid);
        if (optId==ProcessingOptions::invalid)
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
    
        auto baseOpt = (ProcessingOptions)(((std::uint32_t)optId)|0x0001u);
    
        if (isOff)
        {
            flagOptions.erase(baseOpt);
        }
        else
        {
            flagOptions.insert(baseOpt);
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
SnippetOptionsParsingResult parseSnippetInsertionCommandLine( std::unordered_set<SnippetOptions>               &snippetFlagsOptions
                                                            , std::unordered_map<SnippetOptions, int>          &snippetIntOptions
                                                            , const umba::macros::StringStringMap<std::string> &condVars
                                                            , std::string line
                                                            , std::string &snippetFile, std::string &snippetTag
                                                            )
{
    umba::string_plus::trim(line);

    if (!umba::string_plus::starts_with_and_strip(line, ("#!insert")) && !umba::string_plus::starts_with_and_strip(line, ("#$insert")))
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


//----------------------------------------------------------------------------

} // namespace md
} // namespace umba




