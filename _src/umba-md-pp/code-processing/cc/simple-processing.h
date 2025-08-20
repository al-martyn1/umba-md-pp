#pragma once

#include "../types.h"
//
#include "../utils.h"
//
//#include "../code-options-database.h"

//
#include "umba/tokenizer/lexers/cpp.h"
//
#include "umba/string.h"
//

#include <unordered_set>


namespace umba {
namespace mdpp {
namespace code {
namespace cc {


// Извлечение прототипа функции
// Извлечение прототипа класса - оставляем только прототипы функций, удаляем их тела, если есть
// Форматирование прототипа функции
// Форматирование прототипа класса


//----------------------------------------------------------------------------
inline
const std::unordered_set<std::string>& getCppKeywords()
{
    static std::unordered_set<std::string> s = 
    { "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept", "auto"
    , "bitand", "bitor", "bool", "break"
    , "case", "catch", "char", "char8_t", "char16_t", "char32_t", "class", "compl", "concept", "const", "consteval"
    , "constexpr", "constinit", "const_cast", "continue", "co_await", "co_return", "co_yield"
    , "decltype", "default", "delete", "do", "double", "dynamic_cast"
    , "else", "enum", "explicit", "export", "extern"
    , "false", "float", "for", "friend"
    , "goto"
    , "if", "inline", "int"
    , "long", "mutable"
    , "namespace", "new", "noexcept", "not", "not_eq", "nullptr"
    , "operator", "or", "or_eq", "private", "protected", "public"
    , "reflexpr", "register", "reinterpret_cast", "requires", "return"
    , "short", "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "synchronized"
    , "template", "this", "thread_local", "throw", "true", "try", "typedef", "typeid", "typename"
    , "union", "unsigned", "using", "virtual", "void", "volatile"
    , "wchar_t", "while", "xor", "xor_eq"
    // identifiers with special meaning
    , "final", "override", "transaction_safe", "transaction_safe_dynamic", "import", "module"
    };

    return s;
}

//----------------------------------------------------------------------------
//! Временная функция, пока у нас нет опций для настройки скип набора
inline
void updatePrototypeSkipSet(std::unordered_set<std::string> &s)
{
    // s.insert("inline");
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> extractFunctionPrototype( const umba::md::CodeOptions                     &langOpts
                                                 , const std::string                               &lang
                                                 , bool                                            keepComments
                                                 , const std::unordered_set<SnippetOptions>        &snippetFlagsOptions
                                                 , const std::unordered_map<SnippetOptions, int>   &snippetIntOptions
                                                 , const std::vector<std::string>                  &lines
                                                 )
{
    UMBA_ARG_USED(langOpts);
    UMBA_ARG_USED(lang);
    UMBA_ARG_USED(snippetFlagsOptions);
    UMBA_ARG_USED(lines);

    auto text = umba::string_plus::merge<std::string, std::vector<std::string>::const_iterator>( lines.begin(), lines.end(), '\n' );
    UMBA_ARG_USED(text);

    std::string resText;
    bool        processingStopped = false;

    auto  tokenizerBuilder       = umba::tokenizer::cpp::makeTokenizerBuilder<char>();
    using TokenizerBuilderType   = decltype(tokenizerBuilder);

    //using tokenizer_type        = std::decay<decltype(tokenizer)>;
    using tokenizer_type         = typename TokenizerBuilderType::tokenizer_type;
    using InputIteratorType      = typename tokenizer_type::iterator_type;
    using tokenizer_char_type    = typename tokenizer_type::value_type;
    using messages_string_type   = typename tokenizer_type::messages_string_type;
    using token_parsed_data_type = typename tokenizer_type::token_parsed_data_type;

    bool inPreprocessor = false;

    utils::ParsingBasicContext ctxBasic;

    auto areBracketCountersZero = [&]() // except curly
    {
        return ctxBasic.bracketCounters[1]==0 && ctxBasic.bracketCounters[2]==0 && ctxBasic.bracketCounters[3]==0;
    };


    auto tokenHandler = [&]( auto &tokenizer
                           , bool bLineStart, umba::tokenizer::payload_type tokenType
                           , InputIteratorType b, InputIteratorType e
                           , token_parsed_data_type parsedData // std::basic_string_view<tokenizer_char_type> parsedData
                           , messages_string_type &errMsg
                           ) -> bool
    {
        UMBA_USED(parsedData);

        #if defined(_DEBUG)

        auto dataPtr   = b!=e ? static_cast<const char*>(b) : static_cast<const char*>(0);
        auto tokenText = b!=e ? utils::makeTokenText(tokenType, b, e) : std::string();

        UMBA_USED(dataPtr);
        UMBA_USED(tokenText);

        #endif

        if (processingStopped)
            return true;


        if (tokenType==UMBA_TOKENIZER_TOKEN_CTRL_RST || tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
            return true;

        else if (tokenType==UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_START)
        {
            inPreprocessor = true;
            return true;
        }
        else if (tokenType==UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_END)
        {
            inPreprocessor = false;
            return true;
        }
        // else if (tokenType==UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_DEFINE)
        // {
        //     return true;
        // }
        // else if (tokenType==UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_INCLUDE)
        // {
        //     return true;
        // }

        if (tokenType&UMBA_TOKENIZER_TOKEN_CTRL_FLAG)
        {
            return true; // Управляющий токен, не надо выводить, никакой нагрузки при нём нет
        }

        if (utils::tokenTypeIsComment(tokenType))
        {
            if (!keepComments)
            {
                return true; // Коментарий не надо выводить
            }
            else
            {
                if (utils::tokenTypeIsSingleLineComment(tokenType))
                {
                    // UMBA_TOKENIZER_TOKEN_LINEFEED // Не надо добавлять перевод строки, достаточно не обрезать его при добавлении сепаратора ниже
                    resText.append(utils::makeTokenText(tokenType, b, e));
                }
                else
                {
                    resText.append(utils::makeTokenText(tokenType, b, e));
                }
            }
        }


        int incVal = utils::tokenTypeToBracketKindCount(tokenType);

        std::size_t bracketIndex = utils::tokenTypeToBracketIndex(tokenType);

        if (bracketIndex==std::size_t(-1))
        {
            // Не скобка

            auto appText = utils::makeTokenText(tokenType, b, e);
            
            if (appText!=langOpts.getSatementSeparator())
            {
                resText.append(appText);
            }
            else // наткнулись на разделитель
            {
                if (!areBracketCountersZero()) // Внутри скобок могут быть какие-то лямбды с кодом, и, соответственно, с разделителем выражений
                {
                    resText.append(appText);
                }
                else
                {
                    if (!keepComments)
                        umba::string_plus::rtrim(resText, umba::string_plus::is_one_of<char>(" \t\n"));
                    else
                        umba::string_plus::rtrim(resText, umba::string_plus::is_one_of<char>(" \t"));
                    resText.append(appText);
                    processingStopped = true;
                }
            }

            return true;
        }

        if (bracketIndex==std::size_t(utils::EBracketTypeIndex::curly)) // Не важно, открывашка или закрывашка
        {
            if (areBracketCountersZero())
            {
                if (!keepComments)
                    umba::string_plus::rtrim(resText, umba::string_plus::is_one_of<char>(" \t\n"));
                else
                    umba::string_plus::rtrim(resText, umba::string_plus::is_one_of<char>(" \t"));
                resText.append(langOpts.getSatementSeparator());
                processingStopped = true;
            }
            else
            {
                resText.append(utils::makeTokenText(tokenType, b, e));
            }

            return true;
        }

        ctxBasic.bracketCounters[bracketIndex] += incVal;
        resText.append(utils::makeTokenText(tokenType, b, e));

        return true;
    };


    auto tokenizer = umba::tokenizer::cpp::makeTokenizer( tokenizerBuilder
                                                      , tokenHandler
                                                      );
    UMBA_ARG_USED(tokenizer);

    return utils::processCodeImpl(tokenizer, InputIteratorType(text.data(), text.size()), InputIteratorType(), ctxBasic, lines, resText);

}

//----------------------------------------------------------------------------
inline
std::vector<std::string> extractClassPrototype( const umba::md::CodeOptions                     &langOpts
                                              , const std::string                               &lang
                                              , bool                                            keepComments
                                              , const std::unordered_set<SnippetOptions>        &snippetFlagsOptions
                                              , const std::unordered_map<SnippetOptions, int>   &snippetIntOptions
                                              , const std::vector<std::string>                  &lines
                                              )
{
    UMBA_ARG_USED(langOpts);
    UMBA_ARG_USED(lang);
    UMBA_ARG_USED(snippetFlagsOptions);
    UMBA_ARG_USED(lines);

    auto text = umba::string_plus::merge<std::string, std::vector<std::string>::const_iterator>( lines.begin(), lines.end(), '\n' );
    UMBA_ARG_USED(text);

    return lines;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> formatFunctionPrototype( const umba::md::CodeOptions                     &langOpts
                                                , const std::string                               &lang
                                                , bool                                            keepComments
                                                , const std::unordered_set<SnippetOptions>        &snippetFlagsOptions
                                                , const std::unordered_map<SnippetOptions, int>   &snippetIntOptions
                                                , const std::vector<std::string>                  &lines
                                                )
{
    UMBA_ARG_USED(langOpts);
    UMBA_ARG_USED(lang);
    UMBA_ARG_USED(snippetFlagsOptions);
    UMBA_ARG_USED(lines);

    if (lines.empty())
        return lines;

    auto text = umba::string_plus::merge<std::string, std::vector<std::string>::const_iterator>( lines.begin(), lines.end(), '\n' );
    //UMBA_ARG_USED(text);

    auto tokenizerBuilder       = umba::tokenizer::cpp::makeTokenizerBuilder<char>();
    utils::ParsingBasicContext ctxBasic;

    std::vector<SimpleTokenInfo> 
    tokenInfoVec = utils::makeSimpleTokenInfoVec( tokenizerBuilder, text, ctxBasic);

    if (tokenInfoVec.empty())
        return lines;

    std::vector<TokenInfoVariant> tvVec = utils::parseSimpleTokensFunctionPrototypeGenericCc(langOpts, tokenInfoVec);

    auto fmtStyle = utils::getIntSnippetOption(snippetIntOptions, SnippetOptions::prototypeFormat, PrototypeFormatStyle::unknown);
    if (fmtStyle==PrototypeFormatStyle::unknown || fmtStyle==PrototypeFormatStyle::none)
        return lines;

    try
    {
        return utils::formatFunctionPrototypeGenericCc(tvVec, langOpts, fmtStyle);
    }
    catch(const std::invalid_argument &)
    {
        return lines;
    }

    



    // utils::replaceTokenIdByTextEq(tokenInfoVec, langOpts.getCaseSens(), langOpts.getSatementSeparator(), UMBA_TOKENIZER_TOKEN_OPERATOR_SEMICOLON);
    // utils::replaceTokenIdByTextEq(tokenInfoVec, langOpts.getCaseSens(), "template"                     , MDPP_CODE_PROCESSING_TOKEN_KEYWORD_TEMPLATE);
    //  
    // tokenInfoVec = utils::mergeFullQualifiedIdentifiers(tokenInfoVec, true); // allow merge angle brackets
    // tokenInfoVec = utils::mergeRoundBrackets(tokenInfoVec, true /* lastMergeInners */ );
    // tokenInfoVec = utils::removeAnySpaces(tokenInfoVec);
    //  
    // auto blockCharsStr = langOpts.getBlockCharacters();
    // if (blockCharsStr.size()==2)
    // {
    //     tokenInfoVec = utils::simpleConvertAngleBrackets(tokenInfoVec, blockCharsStr[0], blockCharsStr[1]);
    // }
    //  
    // if (tokenInfoVec.empty())
    //     return lines;


    // auto makeTokenText(umba::tokenizer::payload_type tokenType, umba::iterator::TextPositionCountingIterator<char> b, umba::iterator::TextPositionCountingIterator<char> e)
    // std::size_t tokenTypeToBracketIndex(umba::tokenizer::payload_type tokenType)
    // bool tokenTypeIsLessGreaterShift(umba::tokenizer::payload_type tokenType)
    // int tokenTypeToBracketKindCount(umba::tokenizer::payload_type tokenType)
    // bool tokenTypeIsComment(umba::tokenizer::payload_type tokenType)
    // bool tokenTypeIsSingleLineComment(umba::tokenizer::payload_type tokenType)

    /*
        1) У нас есть возможные элементы до шаблона
        2) Опциональный шаблон
           + один или больше параметров шаблона
        3) Возвращаемый тип и прочие квалификаторы функции
        4) Имя функции
        5) Аргументы функции - ноль или больше
        6) Возможные атрибуты функции
        7) Символ завершения выражения (для плюсов - точка с запятой)
    */

    // // std::string resText; resText.reserve(lines.size()*16);
    // // UMBA_ARG_USED(resText);
    // //  
    // // std::vector<std::string> tplArgComments;
    // // std::vector<std::string> fnArgComments ;
    // // UMBA_ARG_USED(tplArgComments);
    // // UMBA_ARG_USED(fnArgComments );

    //auto prototypeSkipSet = langOpts.getPrototypeSkip();
    //updatePrototypeSkipSet(prototypeSkipSet);

    // ArgumentData ad;
    // UMBA_ARG_USED(ad);
    // utils::splitArgument(ad, "std::vector<std::pair<ConditionOperators,StringType>> &*str");
    // utils::splitArgument(ad, "std::vector<std::pair<ConditionOperators,StringType>> &*");
    // utils::splitArgument(ad, "std::vector<std::pair<ConditionOperators,StringType>>");
    

#if 0
    // bool langOpts.testPrototypeSkip(std::string testFor, const std::unordered_set<std::string> &s) const

    // bool testForTokenType(const std::vector<SimpleTokenInfo> &tiVec, umba::tokenizer::payload_type testToken)

    std::size_t idx = 0;

    if (utils::testForTokenType(tokenInfoVec, MDPP_CODE_PROCESSING_TOKEN_KEYWORD_TEMPLATE))
    {
        for(; idx!=tokenInfoVec.size(); ++idx)
        {
            const auto &ti = tokenInfoVec[idx];

            if (utils::tokenTypeIsComment(ti.tokenType))
                continue;

            if (ti.tokenType==MDPP_CODE_PROCESSING_TOKEN_KEYWORD_TEMPLATE)
                break;
        
            if ((ti.tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT || ti.tokenType==UMBA_TOKENIZER_TOKEN_IDENTIFIER) && langOpts.testPrototypeSkip(ti.tokenText, prototypeSkipSet))
                continue;

            if (!resText.empty())
                resText.append(1, '\n');
            resText.append(ti.tokenText);
            
        }

        if (idx==tokenInfoVec.size())
            return lines; // Что-то пошло не так

        // Процессим объявление шаблона
        
        std::size_t argNum     = 0;
        bool        argStarted = false;

        UMBA_ARG_USED(argNum);
        UMBA_ARG_USED(argStarted);

        ++idx; // скипаем `template`
        if (idx==tokenInfoVec.size())
            return lines; // Что-то пошло не так

        if (!resText.empty())
            resText.append(1, '\n');
        resText.append("template");

        int angleLevel = 0;
        UMBA_ARG_USED(angleLevel);
        




    }



    for(; idx!=tokenInfoVec.size(); ++idx)
    {
        const auto &ti = tokenInfoVec[idx];
        if (utils::tokenTypeIsComment(ti.tokenType))
            continue;

        if (ti.tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_SEMICOLON || ti.tokenType==MDPP_CODE_PROCESSING_TOKEN_KEYWORD_TEMPLATE)
            continue;
    }

    int angleCount = 0;
    UMBA_ARG_USED(angleCount);

#endif


    // return lines;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> formatClassPrototype( const umba::md::CodeOptions                     &langOpts
                                             , const std::string                               &lang
                                             , bool                                            keepComments
                                             , const std::unordered_set<SnippetOptions>        &snippetFlagsOptions
                                             , const std::unordered_map<SnippetOptions, int>   &snippetIntOptions
                                             , const std::vector<std::string>                  &lines
                                             )
{
    UMBA_ARG_USED(langOpts);
    UMBA_ARG_USED(lang);
    UMBA_ARG_USED(snippetFlagsOptions);
    UMBA_ARG_USED(lines);

    auto text = umba::string_plus::merge<std::string, std::vector<std::string>::const_iterator>( lines.begin(), lines.end(), '\n' );
    UMBA_ARG_USED(text);

    return lines;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace cc
} // namespace code
} // namespace mdpp
} // namespace umba

