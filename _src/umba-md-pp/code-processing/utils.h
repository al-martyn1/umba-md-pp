#pragma once

//
#include <string>
#include <vector>

// 
#include "types.h"

//
#include "umba/tokenizer/lexers/cpp.h"
//
#include "umba/string.h"
//

#include <unordered_set>
#include <utility>
#include <exception>
#include <stdexcept>


#if defined(min)
    #undef min
#endif

#if defined(max)
    #undef max
#endif



namespace umba {
namespace mdpp {
namespace code {
namespace utils {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
std::unordered_map<std::string, umba::tokenizer::payload_type> makeStdTokensMap()
{
    return std::unordered_map<std::string, umba::tokenizer::payload_type>
    { { "template"  , MDPP_CODE_PROCESSING_TOKEN_KEYWORD_TEMPLATE   }
    , { "default"   , MDPP_CODE_PROCESSING_TOKEN_KEYWORD_DEFAULT    }
    , { "delete"    , MDPP_CODE_PROCESSING_TOKEN_KEYWORD_DELETE     }
    , { "virtual"   , MDPP_CODE_PROCESSING_TOKEN_KEYWORD_VIRTUAL    }
    , { "public"    , MDPP_CODE_PROCESSING_TOKEN_KEYWORD_PUBLIC     }
    , { "protected" , MDPP_CODE_PROCESSING_TOKEN_KEYWORD_PROTECTED  }
    , { "private"   , MDPP_CODE_PROCESSING_TOKEN_KEYWORD_PRIVATE    }
    , { "class"     , MDPP_CODE_PROCESSING_TOKEN_KEYWORD_CLASS      }
    , { "struct"    , MDPP_CODE_PROCESSING_TOKEN_KEYWORD_STRUCT     }
    , { "concept"   , MDPP_CODE_PROCESSING_TOKEN_KEYWORD_CONCEPT    }
    , { "requires"  , MDPP_CODE_PROCESSING_TOKEN_KEYWORD_REQUIRES   }
    };
}

//----------------------------------------------------------------------------
inline
const std::unordered_map<std::string, umba::tokenizer::payload_type>& getStdTokensMap()
{
    static auto m = makeStdTokensMap();
    return m;
}

//----------------------------------------------------------------------------
inline
auto makeTokenText(umba::tokenizer::payload_type tokenType, umba::iterator::TextPositionCountingIterator<char> b, umba::iterator::TextPositionCountingIterator<char> e)
{
    if (tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
    {
        return std::string("\n");
    }

    if (tokenType==UMBA_TOKENIZER_TOKEN_LINE_CONTINUATION)
    {
        return std::string("\\\n");
    }

    if (tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_START)
    {
        return marty_cpp::normalizeCrLfToLf(umba::iterator::makeString(b, e));
    }

    return umba::iterator::makeString(b, e);
}

//----------------------------------------------------------------------------
enum class EBracketTypeIndex
{
    curly  = 0,
    round  = 1,
    angle  = 2,
    square = 3

};

//----------------------------------------------------------------------------
inline
std::size_t tokenTypeToBracketIndex(umba::tokenizer::payload_type tokenType)
{
    switch(tokenType)
    {
        case UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_OPEN          : return std::size_t(EBracketTypeIndex::curly );
        case UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_CLOSE         : return std::size_t(EBracketTypeIndex::curly );
                                                                                                    
        case UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN          : return std::size_t(EBracketTypeIndex::round );
        case UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE         : return std::size_t(EBracketTypeIndex::round );
                                                                                                    
        case UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN          : return std::size_t(EBracketTypeIndex::angle );
        case UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE         : return std::size_t(EBracketTypeIndex::angle );
        case UMBA_TOKENIZER_TOKEN_OPERATOR_LESS               : return std::size_t(EBracketTypeIndex::angle );
        case UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER            : return std::size_t(EBracketTypeIndex::angle );
        case UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT: return std::size_t(EBracketTypeIndex::angle );
                                                                                                    
        case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN         : return std::size_t(EBracketTypeIndex::square);
        case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE        : return std::size_t(EBracketTypeIndex::square);

        default : return std::size_t(-1);
    }
}

//----------------------------------------------------------------------------
//! Less, Greater or Right Shift
inline
bool tokenTypeIsLessGreaterShift(umba::tokenizer::payload_type tokenType)
{
    switch(tokenType)
    {
        case UMBA_TOKENIZER_TOKEN_OPERATOR_LESS               : return true;
        case UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER            : return true;
        case UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT: return true;
        default : return false;
    }
}

//----------------------------------------------------------------------------
inline
int tokenTypeToBracketKindCount(umba::tokenizer::payload_type tokenType)
{
    switch(tokenType)
    {
        case UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_OPEN          : return  1; // Открывашка
        case UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_CLOSE         : return -1; // Закрывашка
                                                                       
        case UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN          : return  1;
        case UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE         : return -1;
                                                                       
        case UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN          : return  1;
        case UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE         : return -1;
        case UMBA_TOKENIZER_TOKEN_OPERATOR_LESS               : return  1;
        case UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER            : return -1;
        case UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT: return -2;
                                                                       
        case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN         : return  1;
        case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE        : return -1;

        default : return 0;
    }
}

//----------------------------------------------------------------------------
inline
bool tokenTypeIsNumberLiteral(umba::tokenizer::payload_type tokenType)
{
    return tokenType>=UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FIRST && tokenType<=UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_LAST;
}

//----------------------------------------------------------------------------
inline
bool tokenTypeIsStringLiteral(umba::tokenizer::payload_type tokenType)
{
    return tokenType>=UMBA_TOKENIZER_TOKEN_STRING_LITERAL_FIRST && tokenType<=UMBA_TOKENIZER_TOKEN_STRING_LITERAL_LAST;
}

//----------------------------------------------------------------------------
inline
bool tokenTypeIsLiteral(umba::tokenizer::payload_type tokenType)
{
    return tokenTypeIsNumberLiteral(tokenType) || tokenTypeIsStringLiteral(tokenType);
}

//----------------------------------------------------------------------------
inline
bool tokenTypeIsAnyFqIdent(umba::tokenizer::payload_type tokenType)
{
    switch(tokenType)
    {
        case UMBA_TOKENIZER_TOKEN_FQIDENT         : return true;
        case UMBA_TOKENIZER_TOKEN_FQIDENT_SCOPERES: return true;
        case UMBA_TOKENIZER_TOKEN_FQIDENT_AOPEN   : return true;
        case UMBA_TOKENIZER_TOKEN_FQIDENT_ACLOSE  : return true;
        case UMBA_TOKENIZER_TOKEN_FQIDENT_COMMA   : return true;
        default : return false;
    }
}

//----------------------------------------------------------------------------
inline
bool tokenTypeIsIdent(umba::tokenizer::payload_type tokenType)
{
    return tokenType==UMBA_TOKENIZER_TOKEN_IDENTIFIER || tokenTypeIsAnyFqIdent(tokenType);
}

//----------------------------------------------------------------------------
inline
bool tokenTypeIsKeyword(umba::tokenizer::payload_type tokenType)
{
    return tokenType>=UMBA_TOKENIZER_TOKEN_KEYWORDS_FIRST && tokenType<=UMBA_TOKENIZER_TOKEN_KEYWORDS_LAST;
}

//----------------------------------------------------------------------------
inline
bool tokenTypeIsIdentOrKeyword(umba::tokenizer::payload_type tokenType)
{
    return tokenTypeIsIdent(tokenType) || tokenTypeIsKeyword(tokenType);
}

//----------------------------------------------------------------------------
inline
bool tokenTypeIsOperator(umba::tokenizer::payload_type tokenType)
{
    return tokenType>=UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST && tokenType<=UMBA_TOKENIZER_TOKEN_OPERATOR_LAST ;
}

//----------------------------------------------------------------------------
inline
bool tokenTypeIsComment(umba::tokenizer::payload_type tokenType)
{
    return tokenType>=UMBA_TOKENIZER_TOKEN_COMMENT_FIRST && tokenType<=UMBA_TOKENIZER_TOKEN_COMMENT_LAST;
}

//----------------------------------------------------------------------------
inline
bool tokenTypeIsSingleLineComment(umba::tokenizer::payload_type tokenType)
{
    return tokenType>=UMBA_TOKENIZER_TOKEN_COMMENT_SINGLE_LINE_FIRST && tokenType<=UMBA_TOKENIZER_TOKEN_COMMENT_SINGLE_LINE_LAST;
}

//----------------------------------------------------------------------------
inline
bool tokenTypeIsSpaceToken(umba::tokenizer::payload_type tokenType)
{
    return tokenType==UMBA_TOKENIZER_TOKEN_SPACE && tokenType<=UMBA_TOKENIZER_TOKEN_TAB;
}

//----------------------------------------------------------------------------
inline
bool tokenTypeIsLinefeedToken(umba::tokenizer::payload_type tokenType)
{
    return tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED && tokenType<=UMBA_TOKENIZER_TOKEN_FORM_FEED;
}

//----------------------------------------------------------------------------
inline
bool tokenTypeIsAnySpaceToken(umba::tokenizer::payload_type tokenType)
{
    return tokenTypeIsLinefeedToken(tokenType) || tokenTypeIsSpaceToken(tokenType);
}

//----------------------------------------------------------------------------
inline
bool isIndentifierChar(char ch)
{
    return (ch>='a' && ch<='z')
        || (ch>='A' && ch<='Z')
        || (ch>='0' && ch<='9')
        || (ch=='_')
           ;
}

//----------------------------------------------------------------------------
inline
bool isAnyBracketChar(char ch)
{
    return ch=='<' || ch=='>'
        || ch=='[' || ch==']'
        || ch=='(' || ch==')'
        || ch=='{' || ch=='}'
         ;
}

//----------------------------------------------------------------------------
inline
bool isAnyOperatorChar(char ch)
{
    return ch=='%' || ch=='='
        || ch=='*' || ch=='+'
        || ch=='/' || ch=='-'
        || ch=='&' || ch=='|'
        || ch=='^' || ch=='~'
         ;
}

//----------------------------------------------------------------------------
inline
bool isAnyOperatorString(const std::string &str)
{
    for(auto ch: str)
    {
        if (!isAnyOperatorChar(ch))
            return false;
    }

    return true;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct ParsingBasicContext
{
    int bracketCounters[4] = { 0, 0, 0, 0 };
    bool hasErrors         = false;

    void reset()
    {
        hasErrors = false;
        bracketCounters[0] = bracketCounters[1] = bracketCounters[2] = bracketCounters[3] = 0;
    }

    bool testForZeros(std::size_t exceptIdx) const
    {
        for(std::size_t idx=0; idx!=4; ++idx)
        {
            if (idx==exceptIdx)
                continue;

            if (bracketCounters[idx]!=0)
                return false;
        }

        return true;
    }

    bool testForZeros(EBracketTypeIndex ebr) const
    {
        return testForZeros(std::size_t(ebr));
    }

    bool testForZeros() const
    {
        return testForZeros(std::size_t(-1));
    }

}; // struct ParsingBasicContext

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename TokenizerType
        , typename InputIteratorType
        >
std::vector<std::string>
processCodeImpl( TokenizerType &tokenizer, InputIteratorType itBegin, InputIteratorType itEnd
               , ParsingBasicContext               &ctxBasic
               , const std::vector<std::string>    &lines
               , std::string                       &resText
               )
{

    using messages_string_type   = typename TokenizerType::messages_string_type;

    tokenizer.unexpectedHandler = [&](auto &tokenizer, InputIteratorType it, InputIteratorType itEnd, const char* srcFile, int srcLine) -> bool
                             {
                                 UMBA_USED(tokenizer); UMBA_USED(it); UMBA_USED(itEnd); UMBA_USED(srcFile); UMBA_USED(srcLine);
                                 // printError(std::cout, inputFilename, UMBA_TOKENIZER_TOKEN_UNEXPECTED, it, itEnd, srcFile, srcLine);
                                 ctxBasic.hasErrors = true;
                                 return false;
                             };
     
    tokenizer.reportUnknownOperatorHandler = [&](auto &tokenizer, InputIteratorType b, InputIteratorType e)
                             {
                                 //cout << "Possible unknown operator: '" << umba::iterator::makeString(b, e) << "'\n";
                                 UMBA_USED(b); UMBA_USED(e);
                                 ctxBasic.hasErrors = true;
                             };
     
    tokenizer.reportStringLiteralMessageHandler = [&](auto &tokenizer, bool bErr, InputIteratorType it, const messages_string_type &msg)
                             {
                                 UMBA_USED(bErr); UMBA_USED(it); UMBA_USED(msg);
                                 ctxBasic.hasErrors = true;
                             };
     
    bool bOk = true;
     
    // auto itBegin = InputIteratorType(text.data(), text.size());
    // auto itEnd   = InputIteratorType();
    tokenizer.tokenizeInit();
    InputIteratorType it = itBegin;
    for(; it!=itEnd && bOk; ++it)
    {
        if (!tokenizer.tokenize(it, itEnd))
        {
            bOk = false;
        }
    }
     
    if (bOk)
    {
        bOk = tokenizer.tokenizeFinalize(it, itEnd);
    }

    if (!bOk || ctxBasic.hasErrors)
        return lines;

    return marty_cpp::splitToLinesSimple(resText, false, '\n');
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template< typename TokenizerBuilderType
        >
std::vector<SimpleTokenInfo>
makeSimpleTokenInfoVec( TokenizerBuilderType  &tokenizerBuilder
                      , const std::string     &text
                      , ParsingBasicContext   &ctxBasic
                      )
{

    // auto  tokenizerBuilder       = umba::tokenizer::makeTokenizerBuilderCpp<char>();
    // using TokenizerBuilderType   = decltype(tokenizerBuilder);
    //  
    // //using tokenizer_type        = std::decay<decltype(tokenizer)>;

    using tokenizer_type         = typename TokenizerBuilderType::tokenizer_type;
    using InputIteratorType      = typename tokenizer_type::iterator_type;
    using tokenizer_char_type    = typename tokenizer_type::value_type;
    using messages_string_type   = typename tokenizer_type::messages_string_type;
    using token_parsed_data_type = typename tokenizer_type::token_parsed_data_type;
    using messages_string_type   = typename tokenizer_type::messages_string_type;

    // bool inPreprocessor = false;
    ctxBasic.reset();

    std::vector<SimpleTokenInfo> resTokenInfoVec;


    auto tokenHandler = [&](auto& tokenizer
        , bool bLineStart, umba::tokenizer::payload_type tokenType
        , InputIteratorType b, InputIteratorType e
        , token_parsed_data_type parsedData // std::basic_string_view<tokenizer_char_type> parsedData
        , messages_string_type& errMsg
        ) -> bool
    {
        UMBA_USED(tokenizer);
        UMBA_USED(parsedData);
        UMBA_USED(errMsg);

#if defined(_DEBUG)

        auto dataPtr = b != e ? static_cast<const char*>(b) : static_cast<const char*>(0);
        auto tokenText = b != e ? utils::makeTokenText(tokenType, b, e) : std::string();

        UMBA_USED(dataPtr);
        UMBA_USED(tokenText);

#endif

        if (tokenType == UMBA_TOKENIZER_TOKEN_CTRL_RST || tokenType == UMBA_TOKENIZER_TOKEN_CTRL_FIN)
            return true;

        // else if (tokenType==UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_START)
        // {
        //     inPreprocessor = true;
        //     return true;
        // }
        // else if (tokenType==UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_END)
        // {
        //     inPreprocessor = false;
        //     return true;
        // }
        //  
        // // else if (tokenType==UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_DEFINE)
        // // {
        // //     return true;
        // // }
        // // else if (tokenType==UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_INCLUDE)
        // // {
        // //     return true;
        // // }

        if (tokenType & UMBA_TOKENIZER_TOKEN_CTRL_FLAG)
        {
            return true; // Управляющий токен, не надо выводить, никакой нагрузки при нём нет
        }

        resTokenInfoVec.emplace_back(tokenType, b, e);

        return true;
    };


    auto tokenizer = umba::tokenizer::cpp::makeTokenizer( tokenizerBuilder
                                                      , tokenHandler
                                                      );


    tokenizer.unexpectedHandler = [&](auto &tokenizer, InputIteratorType it, InputIteratorType itEnd, const char* srcFile, int srcLine) -> bool
                             {
                                 UMBA_USED(tokenizer); UMBA_USED(it); UMBA_USED(itEnd); UMBA_USED(srcFile); UMBA_USED(srcLine);
                                 // printError(std::cout, inputFilename, UMBA_TOKENIZER_TOKEN_UNEXPECTED, it, itEnd, srcFile, srcLine);
                                 ctxBasic.hasErrors = true;
                                 return false;
                             };
     
    tokenizer.reportUnknownOperatorHandler = [&](auto &tokenizer, InputIteratorType b, InputIteratorType e)
                             {
                                 //cout << "Possible unknown operator: '" << umba::iterator::makeString(b, e) << "'\n";
                                 UMBA_USED(b); UMBA_USED(e);
                                 ctxBasic.hasErrors = true;
                             };
     
    tokenizer.reportStringLiteralMessageHandler = [&](auto &tokenizer, bool bErr, InputIteratorType it, const messages_string_type &msg)
                             {
                                 UMBA_USED(bErr); UMBA_USED(it); UMBA_USED(msg);
                                 ctxBasic.hasErrors = true;
                             };
     
    bool bOk = true;
     
    auto itBegin = InputIteratorType(text.data(), text.size());
    auto itEnd   = InputIteratorType();
    tokenizer.tokenizeInit();
    InputIteratorType it = itBegin;
    for(; it!=itEnd && bOk; ++it)
    {
        if (!tokenizer.tokenize(it, itEnd))
        {
            bOk = false;
        }
    }
     
    if (bOk)
    {
        bOk = tokenizer.tokenizeFinalize(it, itEnd);
    }

    if (!bOk || ctxBasic.hasErrors)
        return std::vector<SimpleTokenInfo>(); // При ошибках возвращаем пустой вектор токенов

    return resTokenInfoVec;
}

//----------------------------------------------------------------------------
/*
    Склеиваем имена, разделённые токеном NS-separator.
    Также, если задано allowAngleBrackets=true, приклеиваем к ним шаблонные скобки и их содержимое.
    Внутри определения шаблона на уровне вложенности угловых скобок равном одному склеиваем после запятых.
    Вне определения шаблона по запятым склеиваем всегда при уровне вложенности угловых скобок больше нуля.
*/
template<typename InputIteratorType>
InputIteratorType mergeFullQualifiedIdentifiers(std::vector<SimpleTokenInfo> &resVec, InputIteratorType tiBegin, InputIteratorType tiEnd, bool allowAngleBrackets=false)
{
    // std::vector<SimpleTokenInfo> resVec; resVec.reserve(tiVec.size());

    std::size_t resVecOrgSize = resVec.size();

    enum State
    {
        stNormal          ,
        stWaitBracketOpen ,
        stWaitBracketClose
    };

    State st = stNormal;
    int openCount = 0;

    auto isInTemplateDefinition              = [&]()  { return st==stWaitBracketClose; };
    auto isInTemplateDefinitionRoot          = [&]()  { return st==stWaitBracketClose && openCount==1; };
    auto getCommaGlueBracketLevelLimit       = [&]() -> int { return isInTemplateDefinition() ? 1 : 0; };
    auto getOpenAngleGlueBracketLevelLimit   = [&]() -> int { return isInTemplateDefinition() ? 1 : 0; };
    auto getCloseAngleGlueBracketLevelLimit  = [&]() -> int { return isInTemplateDefinition() ? 1 : 0; };
    auto isNeedGlueAfterComma                = [&]() -> bool { return openCount > getCommaGlueBracketLevelLimit(); };
    auto isNeedGlueAfterOpenAngle            = [&]() -> bool { return openCount > getCommaGlueBracketLevelLimit(); };
    auto isNeedGlueComma                     = [&]() -> bool { return openCount > getCommaGlueBracketLevelLimit(); };
    auto isKindOfAngleBracket = [](auto tt) -> bool
    {
        if (tt == UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN
            || tt == UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE
            || tt == UMBA_TOKENIZER_TOKEN_OPERATOR_LESS
            || tt == UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER
            || tt == UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT
            )
            return true;

        return false;
    };


    //for(const auto &ti: tiVec)
    for(; tiBegin!=tiEnd; ++tiBegin)
    {
        const auto &ti = *tiBegin;

        switch(st)
        {
            case stNormal          :
            {
                if (ti.tokenType==MDPP_CODE_PROCESSING_TOKEN_KEYWORD_TEMPLATE)
                {
                    st = stWaitBracketOpen;
                    resVec.emplace_back(ti);
                    continue; // Ключевое слово template никак не обрабатывается дополнительно
                    // Оно может быть как началом шаблона, так и указанием использования шаблонной функции в некоторых случаях
                }
                break;
            }

            case stWaitBracketOpen :
            {
                if (tokenTypeIsAnySpaceToken(ti.tokenType))
                    continue;

                if (ti.tokenType==UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN || ti.tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LESS)
                    st = stWaitBracketClose;
                else
                    st = stNormal; // Что-то пошло не так, это не объявление шаблона
                break;
            }

            case stWaitBracketClose:
                break;

            default: break;
        }


        if (resVec.empty())
        {
            resVec.emplace_back(ti);
            continue;
        }


        // У нас на входе идентификатор?
        if ( ti.tokenType==UMBA_TOKENIZER_TOKEN_IDENTIFIER
          || ti.tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT
           )
        {
            if ( resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_SCOPE_RESOLUTION
              || resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT_SCOPERES
               )
            {
                // У нас в последнем токене результата что-то, что имеет в конце оператор скоупа
                // Можно приклеивать идентификатор
                resVec.back().tokenType = UMBA_TOKENIZER_TOKEN_FQIDENT;
                resVec.back().tokenText.append(ti.tokenText);
            }

            else if (resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT_AOPEN)
            {
                // У нас в последнем токене результата что-то, заканчивающееся на открывающую угловую слобку
                if (isNeedGlueAfterOpenAngle())
                {
                    // Можно приклеивать идентификатор
                    resVec.back().tokenType = UMBA_TOKENIZER_TOKEN_FQIDENT;
                    resVec.back().tokenText.append(ti.tokenText);
                }
                else
                {
                    resVec.emplace_back(ti);
                }
            }

            else if (isNeedGlueAfterComma())
            {
                // Если у нас удовлетворяется условие по склейке после запятой, то проверяем наличие приклеенной запятой
                if ( resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT_COMMA )
                {
                    // У нас в последнем токене результата что-то, заканчивающееся на запятую
                    resVec.back().tokenType = UMBA_TOKENIZER_TOKEN_FQIDENT;
                    resVec.back().tokenText.append(ti.tokenText);
                }
                else
                {
                    resVec.emplace_back(ti);
                }
            }

            else
            {
                resVec.emplace_back(ti);
            }

            continue;
        }

        // У нас на входе оператор скоупа?
        if (ti.tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_SCOPE_RESOLUTION)
        {
            if ( resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_IDENTIFIER
              || resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT
               )
            {
                // Скоуп оператор приклеивается после идентификатора
                resVec.back().tokenType = UMBA_TOKENIZER_TOKEN_FQIDENT_SCOPERES;
                resVec.back().tokenText.append(ti.tokenText);
            }

            else if ( resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT_AOPEN )
            {
                if (isInTemplateDefinitionRoot())
                {
                    // Идентификатор НЕ приклеиваем к угловой скобке в корне определения шаблона
                    resVec.emplace_back(ti);
                    resVec.back().tokenType = UMBA_TOKENIZER_TOKEN_FQIDENT_SCOPERES;
                }
                else
                {
                    // Вне определения шаблона или на вложенном уровне - идентификатор приклеиваем к угловой скобке
                    resVec.back().tokenType = UMBA_TOKENIZER_TOKEN_FQIDENT_SCOPERES;
                    resVec.back().tokenText.append(ti.tokenText);
                }
            }

            else if ( resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT_ACLOSE )
            {
                resVec.emplace_back(ti);
            }

            else
            {
                resVec.emplace_back(ti);
            }

            continue;
        }

        if (allowAngleBrackets && isKindOfAngleBracket(ti.tokenType))
        {
            if (ti.tokenType==UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN || ti.tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LESS)
            {
                ++openCount;
                // if (st==stWaitBracketClose)
                // {
                // }

                // Открывающая угловая скобка приклеивается только к идентификатору
                if ( resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_IDENTIFIER
                  || resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT
                   )
                {
                    // Но только вне шаблона или внутри него
                    if (openCount>=getOpenAngleGlueBracketLevelLimit())
                    {
                        resVec.back().tokenType = UMBA_TOKENIZER_TOKEN_FQIDENT_AOPEN;
                        resVec.back().tokenText.append(ti.tokenText);
                    }
                    else
                    {
                        resVec.emplace_back(ti);
                    }
                }
                else
                {
                    resVec.emplace_back(ti);
                }
            }
            else if (ti.tokenType==UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE || ti.tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER)
            {
                // Закрывающая угловая скобка приклеивается к идентификатору и закрывающей же скобке
                if ( resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_IDENTIFIER
                  || resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT
                  || resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT_ACLOSE
                   )
                {
                    // Но только вне шаблона или внутри него
                    if (openCount>getCloseAngleGlueBracketLevelLimit())
                    {
                        resVec.back().tokenType = UMBA_TOKENIZER_TOKEN_FQIDENT_ACLOSE;
                        resVec.back().tokenText.append(ti.tokenText);
                    }
                    else
                    {
                        resVec.emplace_back(ti);
                    }
                }
                else
                {
                    resVec.emplace_back(ti);
                }

                --openCount;
                if (openCount<=0)
                    st = stNormal;
            }
            else // if (ti.tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT)
            {
                // Закрывающая угловая скобка приклеивается к идентификатору и закрывающей же скобке
                if ( resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_IDENTIFIER
                  || resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT
                  || resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT_ACLOSE
                   )
                {
                    // Но только вне шаблона или внутри него
                    if (openCount>getCloseAngleGlueBracketLevelLimit())
                    {
                        
                        resVec.back().tokenType = UMBA_TOKENIZER_TOKEN_FQIDENT_ACLOSE;
                        resVec.back().tokenText.append(ti.tokenText);
                    }
                    else
                    {
                        resVec.emplace_back(ti);
                    }
                }
                else
                {
                    resVec.emplace_back(ti);
                }

                --openCount;
                if (openCount<=0)
                    st = stNormal;

                // Закрывающая угловая скобка приклеивается к идентификатору и закрывающей же скобке
                if ( resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_IDENTIFIER
                  || resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT
                  || resVec.back().tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT_ACLOSE
                   )
                {
                    // Но только вне шаблона или внутри него
                    if (openCount>getCloseAngleGlueBracketLevelLimit())
                    {
                        resVec.back().tokenType = UMBA_TOKENIZER_TOKEN_FQIDENT_ACLOSE;
                        resVec.back().tokenText.append(ti.tokenText);
                    }
                    else
                    {
                        resVec.emplace_back(ti);
                    }
                }
                else
                {
                    resVec.emplace_back(ti);
                }

                --openCount;
                if (openCount<=0)
                    st = stNormal;
            }

            continue;
        }


        if (ti.tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_COMMA)
        {
             if (!isNeedGlueComma())
             {
                 resVec.emplace_back(ti);
             }
             else
             {
                 resVec.back().tokenType = UMBA_TOKENIZER_TOKEN_FQIDENT_COMMA;
                 resVec.back().tokenText.append(ti.tokenText);
             }

            continue;
        }

        // else
        {
            if (!tokenTypeIsAnySpaceToken(ti.tokenType))
            {
                resVec.emplace_back(ti);
            }
            else // У нас - пробельный символ
            {
                if ( resVec.back().tokenType!=UMBA_TOKENIZER_TOKEN_IDENTIFIER 
                  && resVec.back().tokenType!=UMBA_TOKENIZER_TOKEN_FQIDENT 
                  && resVec.back().tokenType!=UMBA_TOKENIZER_TOKEN_FQIDENT_SCOPERES
                  && resVec.back().tokenType!=UMBA_TOKENIZER_TOKEN_OPERATOR_SCOPE_RESOLUTION
                  && resVec.back().tokenType!=UMBA_TOKENIZER_TOKEN_FQIDENT_AOPEN 
                  && resVec.back().tokenType!=UMBA_TOKENIZER_TOKEN_FQIDENT_ACLOSE
                  && resVec.back().tokenType!=UMBA_TOKENIZER_TOKEN_FQIDENT_COMMA
                   )
                {
                    if (isInTemplateDefinition()) // isInTemplateDefinitionRoot
                    {}
                    else if (openCount>0)
                    {}
                    else
                    {
                        resVec.emplace_back(ti);
                    }
                }
            }
        }
    
    }

// UMBA_TOKENIZER_TOKEN_OPERATOR_LESS
// UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER
// UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT
// #define UMBA_TOKENIZER_TOKEN_FQIDENT_AOPEN                                            0x000Au /* For internal usage Full qualified identifier with '<' at end */
// #define UMBA_TOKENIZER_TOKEN_FQIDENT_ACLOSE                                           0x000Bu /* For internal usage Full qualified identifier with '>' at end */
// UMBA_TOKENIZER_TOKEN_FQIDENT_COMMA
// UMBA_TOKENIZER_TOKEN_OPERATOR_COMMA

    // auto isInTemplateDefinition        = [&]()  { return st==stWaitBracketClose; };
    // auto isInTemplateDefinitionRoot    = [&]()  { return st==stWaitBracketClose && openCount==1; };
    // auto getCommaGlueBracketLevelLimit = [&]() -> int { return isInTemplateDefinition() > 1 : 0; };
    // auto isNeedGlueAfterComma          = [&]() -> bool { return openCount > getCommaGlueBracketLevelLimit(); };
    // auto isNeedGlueAfterOpenAngle      = [&]() -> bool { return openCount!=1 && isInTemplateDefinition()(); };
    // auto isKindOfAngleBracket          = [](auto tt) -> bool



// #define UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN                                       0x0041u
// #define UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE                                      0x0042u

// #define UMBA_TOKENIZER_TOKEN_FQIDENT_AOPEN                                            0x000Au /* For internal usage Full qualified identifier with '<' at end */
// #define UMBA_TOKENIZER_TOKEN_FQIDENT_ACLOSE                                           0x000Bu /* For internal usage Full qualified identifier with '>' at end */

    for(std::size_t idx=resVecOrgSize; idx!=resVec.size(); ++idx)
    {
        auto &ti = resVec[idx];

        if ( ti.tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT_SCOPERES
          || ti.tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT_AOPEN 
          || ti.tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT_ACLOSE
          || ti.tokenType==UMBA_TOKENIZER_TOKEN_FQIDENT_COMMA
           )
        {
            ti.tokenType = UMBA_TOKENIZER_TOKEN_FQIDENT;
        }
    }

    //return resVec;
    return tiBegin;
}

//----------------------------------------------------------------------------
inline
std::vector<SimpleTokenInfo> mergeFullQualifiedIdentifiers(const std::vector<SimpleTokenInfo> &tiVec, bool allowAngleBrackets=false)
{
    std::vector<SimpleTokenInfo> resVec; resVec.reserve(tiVec.size());
    mergeFullQualifiedIdentifiers(resVec, tiVec.begin(), tiVec.end(), allowAngleBrackets);
    return resVec;
}

//----------------------------------------------------------------------------
template<typename InputIteratorType>
std::size_t countTopLevelRoundBracketChunks(InputIteratorType tiBegin, InputIteratorType tiEnd)
{
    // По идее, не особо надо отвлекаться на другие типы скобок,
    // но потом может переделаю
    std::size_t cnt = 0;
    int bracketLevel = 0;

    for(; tiBegin!=tiEnd; ++tiBegin)
    {
        auto tt = tiBegin->tokenType;
        if (tt==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN)
        {
            ++bracketLevel;
        }
        else if (tt==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE)
        {
            --bracketLevel;
            if (bracketLevel==0)
            {
                ++cnt;
            }
        }
    }

    return cnt;
}

// enum class EBracketTypeIndex
// {
//     curly ,
//     round ,
//     angle ,
//     square
//  
// };
//  
// //----------------------------------------------------------------------------
// inline
// std::size_t tokenTypeToBracketIndex(umba::tokenizer::payload_type tokenType)

// struct ParsingBasicContext
// {
//     int bracketCounters[4] = { 0, 0, 0, 0 };
//     bool hasErrors         = false;
//  
//     void reset()
//     bool testForZeros(std::size_t exceptIdx) const
//     bool testForZeros(EBracketTypeIndex ebr) const
//     bool testForZeros() const
//  
// }; // struct ParsingBasicContext

//----------------------------------------------------------------------------
template<typename InputIteratorType>
InputIteratorType mergeRoundBracketsHelper(std::string &res, InputIteratorType tiBegin, InputIteratorType tiEnd)
{
    int bracketLevel = 0;

    for(; tiBegin!=tiEnd; ++tiBegin)
    {
        auto tt = tiBegin->tokenType;

        if (tokenTypeIsComment(tt))
            continue; // Коментарии отбрасываем

        if (tt==UMBA_TOKENIZER_TOKEN_FQIDENT || tt==UMBA_TOKENIZER_TOKEN_IDENTIFIER)
        {
            if (res.empty())
            {
                res.append(tiBegin->tokenText);
            }
            else 
            {
                char backCh = res.back();
                bool isBackOp = false;
                if ( isAnyBracketChar(backCh) || isAnyOperatorChar(backCh)
                  || backCh==',' || backCh=='.' || backCh==':' || backCh==';'
                  || backCh=='?' || backCh=='@' || backCh=='\\'
                   )
                {
                    isBackOp = true;
                }

                if (!isBackOp)
                {
                    // К операторам и различным скобкам можно приклеивать идентификаторы непосредственно
                    // Для остального надо добавить пробел
                    res.append(1, ' ');
                }

                res.append(tiBegin->tokenText);
            }

            continue;
        }

        if (tokenTypeIsAnySpaceToken(tt))
        {
            // Пробелы элиминируем в один, заменяя переводы строк и табуляции на пробел
            if (res.empty())
                continue;

            if (res.back()==' ' || res.back()=='\n' || res.back()=='\r' || res.back()=='\t')
                continue;

            res.append(1, ' ');

            continue;
        }

        if (tt==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN)
        {
            ++bracketLevel;
        }
        else if (tt==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE)
        {
            --bracketLevel;
            if (bracketLevel<=0)
            {
                res.append(tiBegin->tokenText);
                return ++tiBegin;
            }
        }

        res.append(tiBegin->tokenText);
    }

    return tiBegin;

// bool tokenTypeIsComment(umba::tokenizer::payload_type tokenType)
// bool tokenTypeIsSingleLineComment(umba::tokenizer::payload_type tokenType)
// bool tokenTypeIsSpaceToken(umba::tokenizer::payload_type tokenType)
// bool tokenTypeIsLinefeedToken(umba::tokenizer::payload_type tokenType)
// bool tokenTypeIsAnySpaceToken(umba::tokenizer::payload_type tokenType)

}

//----------------------------------------------------------------------------
template<typename InputIteratorType>
InputIteratorType mergeRoundBrackets(std::vector<SimpleTokenInfo> &resVec, InputIteratorType tiBegin, InputIteratorType tiEnd, bool lastMergeInners=true)
{
    std::size_t totalRounds  = countTopLevelRoundBracketChunks(tiBegin, tiEnd);
    std::size_t curRoundsCnt = 0;

    for(; tiBegin!=tiEnd; )
    {
        auto tt = tiBegin->tokenType;

        if (tt==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN)
        {
            ++curRoundsCnt;

            if (curRoundsCnt>=totalRounds && lastMergeInners)
            {
                // тут мержим те, что внутри встречаются

                resVec.push_back(*tiBegin);
                ++tiBegin;
                tiBegin = mergeRoundBrackets(resVec, tiBegin, tiEnd, false  /* !lastMergeInners */ );
                if (tiBegin!=tiEnd)
                {
                    resVec.push_back(*tiBegin); // Добавляем закрывающую скобку, если она есть
                    ++tiBegin;
                }
            }
            else
            {
                // Тут мержим текущие ()

                std::string mergedRounds;
                tiBegin = mergeRoundBracketsHelper(mergedRounds, tiBegin, tiEnd);
                // Тут tiBegin указывает на то, что следует за закрывающей скобкой

                resVec.emplace_back(UMBA_TOKENIZER_TOKEN_FQIDENT, mergedRounds);

                // InputIteratorType mergeRoundBracketsHelper(std::string &res, InputIteratorType tiBegin, InputIteratorType tiEnd)
            }
        }
        else
        {
            resVec.push_back(*tiBegin);
            ++tiBegin;
        }
    
    }

    return tiBegin;

}

//----------------------------------------------------------------------------
inline
std::vector<SimpleTokenInfo> mergeRoundBrackets(const std::vector<SimpleTokenInfo> &tiVec, bool lastMergeInners=true)
{
    std::vector<SimpleTokenInfo> resVec; resVec.reserve(tiVec.size());
    mergeRoundBrackets(resVec, tiVec.begin(), tiVec.end(), lastMergeInners);
    return resVec;
}


//----------------------------------------------------------------------------
inline
std::vector<SimpleTokenInfo> removeAnySpaces(const std::vector<SimpleTokenInfo> &tiVec)
{
    std::vector<SimpleTokenInfo> resVec; resVec.reserve(tiVec.size());

    for(const auto &ti: tiVec)
    {
        if (tokenTypeIsAnySpaceToken(ti.tokenType))
            continue;
        resVec.emplace_back(ti);
    }

    return resVec;
}

//----------------------------------------------------------------------------
inline
void replaceTokenIdByTextEq(std::vector<SimpleTokenInfo> &tiVec, bool caseSens, std::string textToCompare, umba::tokenizer::payload_type newTokenId)
{
    if (!caseSens)
        umba::string_plus::tolower(textToCompare);

    for(auto &ti: tiVec)
    {
        if (ti.tokenText==textToCompare)
            ti.tokenType = newTokenId;
    }
}

//----------------------------------------------------------------------------
inline
void replaceTokenIdByTextEq(std::vector<SimpleTokenInfo> &tiVec, bool caseSens, std::string textToCompare, umba::tokenizer::payload_type newTokenId, umba::tokenizer::payload_type cmpTokenId)
{
    if (!caseSens)
        umba::string_plus::tolower(textToCompare);

    for(auto &ti: tiVec)
    {
        if (ti.tokenType==cmpTokenId && ti.tokenText==textToCompare)
            ti.tokenType = newTokenId;
    }
}

//----------------------------------------------------------------------------
inline
void replaceTokenIds(std::vector<SimpleTokenInfo> &tiVec, bool caseSens, const std::unordered_map<std::string, umba::tokenizer::payload_type> &tokens)
{
    for(auto &ti: tiVec)
    {
        auto text = ti.tokenText;
        if (!caseSens)
            umba::string_plus::tolower(text);
    
        auto it = tokens.find(text);
        if (it==tokens.end())
            continue;

        ti.tokenType = it->second;
    }
}

//----------------------------------------------------------------------------
inline
void replaceTokenIds(std::vector<SimpleTokenInfo> &tiVec, bool caseSens, const std::unordered_map<std::string, umba::tokenizer::payload_type> &tokens, umba::tokenizer::payload_type cmpTokenId)
{
    for(auto &ti: tiVec)
    {
        if (ti.tokenType!=cmpTokenId)
            continue;

        auto text = ti.tokenText;
        if (!caseSens)
            umba::string_plus::tolower(text);
    
        auto it = tokens.find(text);
        if (it==tokens.end())
            continue;

        ti.tokenType = it->second;
    }
}

//----------------------------------------------------------------------------
inline
std::vector<SimpleTokenInfo> simpleConvertAngleBrackets(const std::vector<SimpleTokenInfo> &tiVec, char charBlockOpen, char charBlockClose, int blockTestLevel=0)
{
    std::vector<SimpleTokenInfo> resVec; resVec.reserve(tiVec.size());

    int openCount = 0;

    for(const auto &ti: tiVec)
    {
        if (ti.tokenText.size()==1)
        {
            if (ti.tokenText[0]==charBlockOpen)
            {
                ++openCount;
                resVec.emplace_back(ti);
                continue;
            }
            else if (ti.tokenText[0]==charBlockClose)
            {
                --openCount;
                resVec.emplace_back(ti);
                continue;
            }
        }

        // Если баланс блоковых скобок не нулевой, то ничего не обрабатываем
        if (openCount>blockTestLevel)
        {
            resVec.emplace_back(ti);
            continue;
        }

        if (ti.tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_LESS)
        {
            //ti.tokenType = UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN;
            resVec.emplace_back(UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN, ti.tokenText);
            continue;
        }

        if (ti.tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER)
        {
            // ti.tokenType = UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE;
            // resVec.emplace_back(ti);
            resVec.emplace_back(UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE, ti.tokenText);
            continue;
        }

        if (ti.tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT)
        {
            resVec.emplace_back(UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE, ">");
            resVec.emplace_back(UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE, ">");
            continue;
        }

        resVec.emplace_back(ti);
    }

    return resVec;

}

//----------------------------------------------------------------------------
inline
bool testForTokenType(const std::vector<SimpleTokenInfo> &tiVec, umba::tokenizer::payload_type testToken)
{
    for(const auto &ti: tiVec)
    {
        if (ti.tokenType==testToken)
            return true;
    }

    return false;
}

//----------------------------------------------------------------------------
template<typename IteratorType>
bool parseTemplateArg(ArgumentData &ad, const umba::md::CodeOptions &langOpts, IteratorType b, IteratorType e)
{
    UMBA_ARG_USED(langOpts);

    using TokenType = std::decay_t<decltype(b->tokenType)>;
    auto prevTt = TokenType(-1);

    ad.clear();

    auto checkReadComment = [&](auto tt) -> bool
    {
        if (tokenTypeIsComment(b->tokenType))
        {
            // TODO: тут надо проверять, является ли коментарий коментарием спец вида. Пока складываем любые коментарии
            ad.comment.assign(b->tokenText);
            return true; // Коментарии могут быть внутри параметра, но остаётся только последний
        }
    
        return false;
    };

    // typename StringType = std::string<CharType, std::char_traits<CharType>, std::allocator<CharType> > //!< Тип строки
    //                    ^  ^                                                                            ^
    //                    |  |                                                                            |

    // Комент перед началом аргумента пропускаем
    for(; b!=e; ++b)
    {
        if (tokenTypeIsAnySpaceToken(b->tokenType))
            continue;
        if (!tokenTypeIsComment(b->tokenType))
            break;
        ++b;
        break;
    }

    for(; b!=e; ++b)
    {
        if (checkReadComment(b->tokenType))
            continue;

        if (b->tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_ASSIGNMENT)
        {
            ad.valueSep.assign(b->tokenText);
            ++b;
            break;
        }

        if (tokenTypeIsIdentOrKeyword(prevTt) && tokenTypeIsIdentOrKeyword(b->tokenType))
            ad.typeName.append(1, ' ');

        prevTt = b->tokenType;

        ad.typeName.append(b->tokenText);
    }

    prevTt = TokenType(-1); // инит считывания значения по умолчанию

    // скипаем незначащее
    for(; b!=e; ++b)
    {
        if (checkReadComment(b->tokenType))
            continue;
        if (tokenTypeIsAnySpaceToken(b->tokenType))
            continue;
        break;
    }

    if (b==e)
        return true;


    if (!ad.valueSep.empty())
    {
        for(; b!=e; ++b)
        {
            if (checkReadComment(b->tokenType))
                continue;
    
            if (tokenTypeIsIdentOrKeyword(prevTt) && tokenTypeIsIdentOrKeyword(b->tokenType))
                ad.typeName.append(1, ' ');
    
            prevTt = b->tokenType;
    
            ad.defValue.append(b->tokenText);
        }
    }

    for(; b!=e; ++b)
    {
        if (checkReadComment(b->tokenType) || tokenTypeIsAnySpaceToken(b->tokenType))
            continue;
    }

    return true;
}

//----------------------------------------------------------------------------
template<typename IteratorType>
bool parseFunctionArg(ArgumentData &ad, const umba::md::CodeOptions &langOpts, IteratorType b, IteratorType e)
{
    UMBA_ARG_USED(langOpts);

    using TokenType = std::decay_t<decltype(b->tokenType)>;
    auto prevTt = TokenType(-1);

    ad.clear();

    auto checkReadComment = [&](auto tt) -> bool
    {
        if (tokenTypeIsComment(b->tokenType))
        {
            // TODO: тут надо проверять, является ли коментарий коментарием спец вида. Пока складываем любые коментарии
            ad.comment.assign(b->tokenText);
            return true; // Коментарии могут быть внутри параметра, но остаётся только последний
        }
    
        return false;
    };

    // const std::string<char> & str2 = std::string("aaa") /* AAA */
    //                        ^  ^    ^ ^                  ^
    //                        |  |    | |                  |

    // Комент перед началом аргумента пропускаем
    for(; b!=e; ++b)
    {
        if (tokenTypeIsAnySpaceToken(b->tokenType))
            continue;
        if (!tokenTypeIsComment(b->tokenType))
            break;
        ++b;
        break;
    }

    for(; b!=e; ++b)
    {
        if (checkReadComment(b->tokenType))
            continue;

        if (b->tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_ASSIGNMENT)
        {
            ad.valueSep.assign(b->tokenText);
            ++b;
            break;
        }

        if (tokenTypeIsOperator(b->tokenType)) // ссылки-указатели и подобное
        {
            ad.ptrRef.assign(b->tokenText);
            ++b;
            break;
        }

        if (tokenTypeIsIdentOrKeyword(prevTt) && tokenTypeIsIdentOrKeyword(b->tokenType))
            ad.typeName.append(1, ' ');

        prevTt = b->tokenType;

        ad.typeName.append(b->tokenText);

    }

    prevTt = TokenType(-1);

    if (b==e)
        return true;

    if (!ad.ptrRef.empty()) // читаем ссылки-указатели
    {
        for(; b!=e; ++b)
        {
            if ( /* tokenTypeIsComment(b->tokenType) ||  */ tokenTypeIsAnySpaceToken(b->tokenType))
                continue;

            if (!tokenTypeIsOperator(b->tokenType))
                break;

            ad.ptrRef.append(b->tokenText);
        }

        // Тут читаем имя аргумента
        // Иначе, если не было ссылки/указателя/ещё чего-то, то имя аргумента приклеено к типу аргумента

        for(; b!=e; ++b)
        {
            if (checkReadComment(b->tokenType) || tokenTypeIsAnySpaceToken(b->tokenType))
                continue;
    
            if (b->tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_ASSIGNMENT)
            {
                ad.valueSep.assign(b->tokenText);
                ++b;
                break;
            }
    
            // // Имя аргумента - только один токен
            // if (!ad.argName.empty())
            //     break;

            //if (tokenTypeIsIdentOrKeyword(prevTt) && tokenTypeIsIdentOrKeyword(b->tokenType))
            if (!ad.argName.empty() && tokenTypeIsIdentOrKeyword(b->tokenType))
                ad.argName.append(1, ' ');
    
            prevTt = b->tokenType;

            ad.argName.append(b->tokenText);
        }
    }

    for(; b!=e; ++b)
    {
        if (checkReadComment(b->tokenType))
            continue;
        if (tokenTypeIsAnySpaceToken(b->tokenType))
            continue;
        break;
    }

    if (!ad.valueSep.empty())
    {
        for(; b!=e; ++b)
        {
            if (checkReadComment(b->tokenType))
                continue;
    
            if (tokenTypeIsIdentOrKeyword(prevTt) && tokenTypeIsIdentOrKeyword(b->tokenType))
                ad.typeName.append(1, ' ');
    
            prevTt = b->tokenType;
    
            ad.defValue.append(b->tokenText);
        }
    }


    for(; b!=e; ++b)
    {
        if (checkReadComment(b->tokenType))
            continue;
        if (tokenTypeIsAnySpaceToken(b->tokenType))
            continue;
        break;
    }

    return true;

}

//----------------------------------------------------------------------------
void checkExtractArgName(ArgumentData &ad)
{
    if (!ad.argName.empty() || !ad.ptrRef.empty())
        return;

    auto pos = ad.typeName.rfind(' ');
    if (pos==ad.typeName.npos)
    {
        ad.trim();
        return;
    }

    auto s1 = std::string(ad.typeName, 0, pos);
    auto s2 = std::string(ad.typeName, pos+1 );
    umba::string_plus::rtrim(s1 /* , umba::string_plus::is_one_of<char>(" ") */ );
    umba::string_plus::rtrim(s2 /* , umba::string_plus::is_one_of<char>(" ") */ );

    if (s1.empty() || s2.empty())
    {
        ad.trim();
        return;
    }

    ad.typeName = s1;
    ad.argName  = s2;

    ad.trim();
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// namespace format {


// TODO: Не поддерживаются C++20 requires

// TODO: При бэк-сканировании до начала блока стоп производится в том числе по закрывающей блочной скобке
// При использовании requires requires могут быть блочные скобки в заголовке функции, и блок будет выцеплен некорректно
// Надо что-то сделать, возможно, отключать опцией остановку по символу блока

inline
std::vector<TokenInfoVariant> 
parseSimpleTokensFunctionPrototypeGenericCc(const umba::md::CodeOptions &langOpts, std::vector<SimpleTokenInfo> tokenInfoVec)
{
    replaceTokenIdByTextEq(tokenInfoVec, langOpts.getCaseSens(), langOpts.getSatementSeparator(), UMBA_TOKENIZER_TOKEN_OPERATOR_SEMICOLON );
    replaceTokenIdByTextEq(tokenInfoVec, langOpts.getCaseSens(), langOpts.getAssignOperator()   , UMBA_TOKENIZER_TOKEN_OPERATOR_ASSIGNMENT);
    replaceTokenIdByTextEq(tokenInfoVec, langOpts.getCaseSens(), "template"                     , MDPP_CODE_PROCESSING_TOKEN_KEYWORD_TEMPLATE);

    tokenInfoVec = mergeFullQualifiedIdentifiers(tokenInfoVec, true); // allow merge angle brackets
    tokenInfoVec = mergeRoundBrackets(tokenInfoVec, true /* lastMergeInners */ );
    tokenInfoVec = removeAnySpaces(tokenInfoVec);

    auto blockCharsStr = langOpts.getBlockCharacters();
    if (blockCharsStr.size()==2)
    {
        tokenInfoVec = simpleConvertAngleBrackets(tokenInfoVec, blockCharsStr[0], blockCharsStr[1]);
    }

    {
        auto tokenInfoVecCopy = tokenInfoVec;
        tokenInfoVec.clear();
        for(const auto &t : tokenInfoVecCopy)
        {
            if (langOpts.testPrototypeSkip(t.tokenText))
                continue;
            tokenInfoVec.push_back(t);
        }
    }


    std::vector<TokenInfoVariant>  resVec;

    if (tokenInfoVec.empty())
        return resVec;

    enum State
    {
        stNormal              ,
        stWaitTemplateArgs    ,
        //stWaitTemplateArgValue,
        stReadTemplateArg     ,
        //stWaitFunctionArgValue,
        stReadFunctionArg
    };

    State st = stNormal;
    TemplateArguments tplArgs;
    FunctionArguments fnArgs ;

    for(auto it=tokenInfoVec.begin(); it!=tokenInfoVec.end(); )
    {
                // UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN
                // UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE
                // UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN 
                // UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE
        switch(st)
        {
            case stNormal              : // [X]
            {
                if (tokenTypeIsComment(it->tokenType) || tokenTypeIsAnySpaceToken(it->tokenType))
                    {} // коментарии и пробелы просто пропускаем

                else if (it->tokenType==MDPP_CODE_PROCESSING_TOKEN_KEYWORD_TEMPLATE)
                    st = stWaitTemplateArgs; // переход к ожиданию угловой скобки

                else if (it->tokenType==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN)
                {
                    fnArgs.clear();
                    st = stReadFunctionArg;
                }

                else
                    resVec.emplace_back(*it);

                ++it;

            }
            break;

            case stWaitTemplateArgs    : // [X] ждём угловую скобку
            {
                if (tokenTypeIsComment(it->tokenType) || tokenTypeIsAnySpaceToken(it->tokenType))
                    {} // коментарии и пробелы просто пропускаем

                else if (it->tokenType==UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN)
                {
                    tplArgs.clear();
                    st = stReadTemplateArg; // Переход к чтению параметров шаблона
                }

                else if (it->tokenType==MDPP_CODE_PROCESSING_TOKEN_KEYWORD_TEMPLATE)
                {
                    // неожиданное появление 'template', предыдущий 'template' был хз о чем
                    // предыдущий кладём в результат
                    // состояние не меняем
                    auto tmpIt = it;
                    --tmpIt;
                    resVec.emplace_back(*tmpIt);
                }
                else
                {
                    // хз что, кладём предыдущий 'template' в результат
                    // также кладём текущий элемент в результат,
                    // и возврат в нормальное состояние
                    auto tmpIt = it;
                    --tmpIt;
                    resVec.emplace_back(*tmpIt);
                    resVec.emplace_back(*it);
                    st = stNormal;
                }
                ++it;

            }
            break;

            case stReadTemplateArg: // stWaitTemplateArgValue: // [ ] читаем аргументы шаблона
            {
                auto argStartIt = it;

                bool argStarted = false;

                while(it!=tokenInfoVec.end())
                {
                    if (tokenTypeIsAnySpaceToken(it->tokenType))
                    {
                        ++it; // пробелы просто пропускаем
                        //argStartIt = it;
                    }

                    else if (tokenTypeIsComment(it->tokenType))
                    {
                        // коментарий докидываем к предыдущему элементу, если он есть и он без коментария
                        // TODO: Тут, наверное, надо удалить символы коментария в начале и в конце, а также
                        // доп символы, которые использует доксиген: '*'/'!'/'<'
                        if (!argStarted)
                        {
                            if (!tplArgs.args.empty() && tplArgs.args.back().comment.empty())
                                 tplArgs.args.back().comment = it->tokenText;
                        }
                        ++it;
                        //argStartIt = it;
                    }

                    else if (it->tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_COMMA || it->tokenType==UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE)
                    {
                        ArgumentData ad;
                        //if (parseTemplateArg(ad, argStartIt, it))
                        parseTemplateArg(ad, langOpts, argStartIt, it);
                        if (!ad.empty())
                        {
                            checkExtractArgName(ad);
                            tplArgs.push_back(ad);
                            argStarted = false;
                        }

                        bool bBreak = it->tokenType==UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE;
                        ++it;
                        argStartIt = it;

                        if (bBreak)
                            break;
                    }

                    else
                    {
                        argStarted = true;
                        ++it; // двигаемся по текущему аргументу дальше
                    }

                } // while

                // if (it!=tokenInfoVec.end())
                //     ++it; // Пропускаем закрывающую скобку

                //if (!tplArgs.empty())
                    resVec.push_back(tplArgs);

                tplArgs.clear();

                st = stNormal;

            }
            break;

            // case stReadTemplateArgValue: // [ ]
            // {
            // }
            // break;

            // case stWaitFunctionArgValue: // [ ]
            // {
            // }
            // break;

            case stReadFunctionArg: // [ ]
            {
                auto argStartIt = it;

                bool argStarted = false;

                while(it!=tokenInfoVec.end())
                {
                    if (tokenTypeIsAnySpaceToken(it->tokenType))
                    {
                        ++it; // пробелы просто пропускаем
                        //argStartIt = it;
                    }

                    else if (tokenTypeIsComment(it->tokenType))
                    {
                        // коментарий докидываем к предыдущему элементу, если он есть и он без коментария
                        // TODO: Тут, наверное, надо удалить символы коментария в начале и в конце, а также
                        // доп символы, которые использует доксиген: '*'/'!'/'<'
                        if (!argStarted)
                        {
                            if (!fnArgs.args.empty() && fnArgs.args.back().comment.empty())
                                 fnArgs.args.back().comment = it->tokenText;
                        }
                        ++it;
                        //argStartIt = it;
                    }

                    else if (it->tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_COMMA || it->tokenType==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE)
                    {
                        ArgumentData ad;
                        //if (parseTemplateArg(ad, argStartIt, it))
                        parseFunctionArg(ad, langOpts, argStartIt, it);
                        if (!ad.empty())
                        {
                            checkExtractArgName(ad);
                            fnArgs.push_back(ad);
                            argStarted = false;
                        }

                        bool bBreak = it->tokenType==UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE;
                        ++it;
                        argStartIt = it;

                        if (bBreak)
                            break;
                    }

                    else
                    {
                        argStarted = true;
                        ++it; // двигаемся по текущему аргументу дальше
                    }

                } // while

                // if (it!=tokenInfoVec.end())
                //     ++it; // Пропускаем закрывающую скобку

                //if (!fnArgs.empty())
                    resVec.push_back(fnArgs);

                fnArgs.clear();

                st = stNormal;
 
            }
            break;

            // case :

        } // switch(st)

    } // for


    // MDPP_CODE_PROCESSING_TOKEN_FUNCTION_NAME
    // tokenTypeIsIdent(umba::tokenizer::payload_type tokenType)

    return resVec;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> formatArguments( const umba::md::CodeOptions     &langOpts
                                        , std::vector<ArgumentData>       adList
                                        , std::size_t                     argFillExtra
                                        , std::size_t                     defFillExtra
                                        , bool tableMode      = true
                                        , bool tableModeDefs  = true
                                        , bool addComments    = false
                                        )
{
    UMBA_ARG_USED(langOpts);

    #if 0
    {
        auto tmp = adList;
        adList.clear();
        for(const auto &ad: tmp)
        {
            if (!ad.empty())
                adList.emplace_back(ad);
        }

        using std::swap;

        swap(tmp, adList);
    }
    #endif

    auto checkFillSize = [](std::size_t sz) -> std::size_t
    {
        if (sz==0)
            return 1;
        if (sz>64)
            return 64;

        return sz;
    };

    if (argFillExtra<1) argFillExtra = 1;
    if (defFillExtra<1) defFillExtra = 1;

    std::size_t argFillSize = 0;
    std::size_t defFillSize = 0;

    for(const auto &ad : adList)
    {
        argFillSize = std::max(argFillSize, ad.typeName.size());
        defFillSize = std::max(defFillSize, ad.ptrRef.size() + ad.argName.size());
    }

    if (tableMode)
    {
        if (argFillSize!=0)
            argFillSize += argFillExtra;
    }
    else
    {
        if (argFillSize!=0)
            argFillSize = 1u;
    }

    // For default values
    if (tableModeDefs)
    {
        if (defFillSize!=0)
            defFillSize += defFillExtra;
    }
    else
    {
        if (defFillSize!=0)
            defFillSize = 1u;
    }

    argFillSize = checkFillSize(argFillSize);
    defFillSize = checkFillSize(defFillSize);


    // std::string argFillStr = std::string(argFillSize, ' ');
    // std::string defFillStr = std::string(defFillSize, ' ');

    std::vector<std::string> resVec;
    for(const auto &ad : adList)
    {
        std::size_t afsz = 1u;
        if (argFillSize>=ad.typeName.size())
            afsz = argFillSize - ad.typeName.size();
        afsz = checkFillSize(afsz);
        std::string str = ad.typeName + std::string(afsz, ' ') + ad.ptrRef + ad.argName;
        if (!ad.valueSep.empty() && !ad.defValue.empty())
        {
            std::size_t dfsz = 1u;
            if (defFillSize>=(ad.ptrRef.size() + ad.argName.size()))
                dfsz = defFillSize - (ad.ptrRef.size() + ad.argName.size());
            dfsz = checkFillSize(dfsz);
            str += std::string(dfsz, ' ') + ad.valueSep + " " + ad.defValue;
        }

        resVec.push_back(str);
    }

    if (addComments)
    {
        std::size_t idx = 0;
        for(const auto &ad : adList)
        {
            if (!ad.comment.empty())
            {
                resVec[idx].append(1, ' ');
                resVec[idx].append(ad.comment);
            }
            ++idx;
        }
    }

    return resVec;
}

//----------------------------------------------------------------------------
/*
linux информация о системе и дистрибутиве
https://www.google.com/search?q=linux+byajhvfwbz+j+cbcntvt+b+lbcnhb%2Cenbdt&oq=linux+byajhvfwbz+j+cbcntvt+b+lbcnhb%2Cenbdt&gs_lcrp=EgZjaHJvbWUyBggAEEUYOTIHCAEQABjvBTIHCAIQABjvBTIHCAMQABjvBTIHCAQQABjvBdIBCTE0MDMxajBqN6gCALACAA&sourceid=chrome&ie=UTF-8

-a, --all
       print all information, in the following order, except omit -p and -i if unknown:

-s, --kernel-name
       print the kernel name

-n, --nodename
       print the network node hostname

-r, --kernel-release
       print the kernel release

-v, --kernel-version
       print the kernel version

-m, --machine
       print the machine hardware name

-p, --processor
       print the processor type (non-portable)

-i, --hardware-platform
       print the hardware platform (non-portable)

-o, --operating-system
       print the operating system

1. uname -r:
Отображает версию ядра Linux. Например, 5.4.0-74-generic. Это базовый уровень информации о ядре.
2. cat /etc/os-release:
Отображает подробную информацию о дистрибутиве, включая название, версию, ID и другие данные. Это основной способ узнать информацию о дистрибутиве.
3. lsb_release -a:
Предоставляет расширенный отчет о характеристиках операционной системы, включая название дистрибутива, его версию и кодовое название. Это более удобный способ, чем чтение файла /etc/os-release напрямую.
4. hostnamectl:
Отображает информацию о хосте, включая название дистрибутива, его версию и кодовое имя, а также версию ядра. Это удобный способ получить всю основную информацию в одном месте.
5. cat /proc/version:
Выводит информацию о версии ядра Linux.
6. lshw:
Показывает подробную информацию об аппаратном обеспечении компьютера.


*/


//----------------------------------------------------------------------------
/*
Варианты форматирования:
1) msdn - Как в документации MS, например - CreateFileA function (fileapi.h) - https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea

HANDLE CreateFileA(
  [in]           LPCSTR                lpFileName,
  [in]           DWORD                 dwDesiredAccess,
  [in]           DWORD                 dwShareMode,
  [in, optional] LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  [in]           DWORD                 dwCreationDisposition,
  [in]           DWORD                 dwFlagsAndAttributes,
  [in, optional] HANDLE                hTemplateFile
);

BOOL CloseHandle(
  [in] HANDLE hObject
);

_Post_equals_last_error_ DWORD GetLastError();

Аргументы прижаты к левому краю, с небольшим отступом (2/4 пробела)

Наше отличие: все спецификаторы, а также тип возвращаемого значения выводятся по одному в строке

Если аргументов нет, скобки выводятся в одну строку


2) man/linux/unix/bsd - как в манах/доках по *nix - https://man7.org/linux/man-pages/man2/select.2.html

int select(int nfds,
           fd_set *_Nullable restrict readfds,
           fd_set *_Nullable restrict writefds,
           fd_set *_Nullable restrict exceptfds,
           struct timeval *_Nullable restrict timeout);

В моей версии так:
int
select(int nfds,
      fd_set *_Nullable restrict readfds,
      fd_set *_Nullable restrict writefds,
      fd_set *_Nullable restrict exceptfds,
      struct timeval *_Nullable restrict timeout);

3) mdr/umba/dotnut

int
select( int nfds
      , fd_set         *_Nullable restrict readfds
      , fd_set         *_Nullable restrict writefds
      , fd_set         *_Nullable restrict exceptfds
      , struct timeval *_Nullable restrict timeout
      );

*/

//----------------------------------------------------------------------------
template<typename SnippetOptionsType, typename SomeIntegralOrEnumType>
SomeIntegralOrEnumType getIntSnippetOption(const std::unordered_map<SnippetOptionsType, int> &intOptions, SnippetOptionsType opt, SomeIntegralOrEnumType defVal)
{
    auto baseOpt = (SnippetOptionsType)(((std::uint32_t)opt)|0x0001u);

    auto it = intOptions.find(baseOpt);
    if (it==intOptions.end())
        return defVal;

    return SomeIntegralOrEnumType(it->second);
}

//----------------------------------------------------------------------------
inline
void formatFunctionPrototypeGenericCcAppendSimpleTokenImpl( const SimpleTokenInfo       &arg
                                                 , const umba::md::CodeOptions &langOpts
                                                 , bool                        fnArgsPassed
                                                 , std::vector<std::string>    &resVec
                                                 , std::size_t                 &lastNameLen
                                                 )
{
    bool tryAppend = fnArgsPassed || isAnyOperatorString(arg.tokenText);
    if (tryAppend && !resVec.empty())
    {
        if (arg.tokenText!=langOpts.getSatementSeparator() && !resVec.back().empty())
            resVec.back().append(" ");
        resVec.back().append(arg.tokenText);
    }
    else
    {
        resVec.emplace_back(arg.tokenText);
    }
    
    lastNameLen = arg.tokenText.size();
}

//----------------------------------------------------------------------------
inline
void emplaceOrAppend(std::vector<std::string> &resVec, const std::string &str)
{
    if (resVec.empty())
        resVec.emplace_back(str);
    else
        resVec.back().append(str);
}

//----------------------------------------------------------------------------
inline
bool formatFunctionPrototypeGenericCcTestAppendEmptyArgListStr(std::vector<std::string> &resVec, const std::vector<std::string> &formattedArgs, const std::string &strAppend)
{
    if (formattedArgs.empty())
    {
        //resVec.emplace_back(strAppend);
        emplaceOrAppend(resVec, strAppend);
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> formatFunctionPrototypeGenericCcMsdn(const std::vector<TokenInfoVariant> &tiVec, const umba::md::CodeOptions &langOpts)
{
     std::vector<std::string> resVec;
     bool fnArgsPassed = false;
     std::size_t lastNameLen = 0;

     std::size_t tiIdx = 0;

     for(const auto &ti : tiVec)
     {
         ++tiIdx;
         std::visit( [&](auto&& arg)
                     {
                         using T = std::decay_t<decltype(arg)>;

                         if constexpr (std::is_same_v<T, SimpleTokenInfo>)
                         {
                             formatFunctionPrototypeGenericCcAppendSimpleTokenImpl(arg, langOpts, fnArgsPassed, resVec, lastNameLen);
                         }

                         else if constexpr (std::is_same_v<T, TemplateArguments>)
                         {
                             auto formattedArgs = formatArguments(langOpts, arg.args, 1, 1, true /* tableMode */, true /* tableMode, Defs */, false /* addComments */ );
    
                             if (!formatFunctionPrototypeGenericCcTestAppendEmptyArgListStr(resVec, formattedArgs, "template<>"))
                             {
                                 resVec.emplace_back("template<");
                                 std::size_t argIdx = 0;
                                 for(auto &&fa: formattedArgs)
                                 {
                                     ++argIdx;
                                     resVec.emplace_back(std::string(2, ' ') + fa);
                                     if (argIdx<formattedArgs.size())
                                         resVec.back().append(",");
                                 }
                                 resVec.emplace_back(">");
                             }
                         }

                         else if constexpr (std::is_same_v<T, FunctionArguments>)
                         {
                             auto formattedArgs = formatArguments(langOpts, arg.args, 1, 1, true  /* tableMode */, true /* tableMode, Defs */, false /* addComments */ );
    
                             if (!formatFunctionPrototypeGenericCcTestAppendEmptyArgListStr(resVec, formattedArgs, "()"))
                             {
                                 emplaceOrAppend(resVec, "(");

                                 std::size_t argIdx = 0;
                                 for(auto &&fa: formattedArgs)
                                 {
                                     ++argIdx;
                                     resVec.emplace_back(std::string(2, ' ') + fa);
                                     if (argIdx<formattedArgs.size())
                                         resVec.back().append(",");
                                 }
                                 resVec.emplace_back(")");
                                 if (tiIdx<(tiVec.size()-1))
                                    resVec.emplace_back(std::string());
                             }
    
                             fnArgsPassed = true;
                         }

                         else
                         {
                             static_assert(false, "non-exhaustive visitor!");
                         }
                     }
                   , ti
                   );
     }

    return resVec;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> formatFunctionPrototypeGenericCcMan(const std::vector<TokenInfoVariant> &tiVec, const umba::md::CodeOptions &langOpts, bool tabbed)
{
     std::vector<std::string> resVec;
     bool fnArgsPassed = false;
     std::size_t lastNameLen = 0;

     std::size_t tiIdx = 0;

     for(const auto &ti : tiVec)
     {
         ++tiIdx;
         std::visit( [&](auto&& arg)
                     {
                         using T = std::decay_t<decltype(arg)>;

                         if constexpr (std::is_same_v<T, SimpleTokenInfo>)
                         {
                             formatFunctionPrototypeGenericCcAppendSimpleTokenImpl(arg, langOpts, fnArgsPassed, resVec, lastNameLen);
                         }

                         else if constexpr (std::is_same_v<T, TemplateArguments>)
                         {
                             auto formattedArgs = formatArguments(langOpts, arg.args, 1, 1, tabbed /* tableMode */, tabbed /* tableMode, Defs */, false /* addComments */ );
    
                             if (!formatFunctionPrototypeGenericCcTestAppendEmptyArgListStr(resVec, formattedArgs, "template<>"))
                             {
                                 resVec.emplace_back("template<");
                                 lastNameLen = 8u;
                                 std::size_t argIdx = 0;
                                 for(auto &&fa: formattedArgs)
                                 {
                                     if (argIdx)
                                         resVec.emplace_back(std::string(lastNameLen+1u, ' '));
                                     ++argIdx;
                                     emplaceOrAppend(resVec, fa);
                                     //resVec.emplace_back("  " + fa);
                                     if (argIdx<formattedArgs.size())
                                         resVec.back().append(",");
                                 }
                                 emplaceOrAppend(resVec, ">");
                                 // resVec.emplace_back(">");
                             }
                         }

                         else if constexpr (std::is_same_v<T, FunctionArguments>)
                         {
                             auto formattedArgs = formatArguments(langOpts, arg.args, 1, 1, tabbed  /* tableMode */, tabbed /* tableMode, Defs */, false /* addComments */ );
    
                             if (!formatFunctionPrototypeGenericCcTestAppendEmptyArgListStr(resVec, formattedArgs, "()"))
                             {
                                 emplaceOrAppend(resVec, "(");

                                 std::size_t argIdx = 0;
                                 for(auto &&fa: formattedArgs)
                                 {
                                     if (argIdx)
                                         resVec.emplace_back(std::string(lastNameLen+1u, ' '));
                                     ++argIdx;
                                     emplaceOrAppend(resVec, fa);
                                     if (argIdx<formattedArgs.size())
                                         resVec.back().append(",");
                                 }
                                 emplaceOrAppend(resVec, ")");
                                 if (tiIdx<(tiVec.size()-1))
                                    resVec.emplace_back(std::string());
                             }
    
                             fnArgsPassed = true;
                         }

                         else
                         {
                             static_assert(false, "non-exhaustive visitor!");
                         }
                     }
                   , ti
                   );
     }

    return resVec;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> formatFunctionPrototypeGenericCcUmba(const std::vector<TokenInfoVariant> &tiVec, const umba::md::CodeOptions &langOpts)
{
     std::vector<std::string> resVec;
     bool fnArgsPassed = false;
     std::size_t lastNameLen = 0;

     std::size_t tiIdx = 0;

     for(const auto &ti : tiVec)
     {
         ++tiIdx;
         std::visit( [&](auto&& arg)
                     {
                         using T = std::decay_t<decltype(arg)>;

                         if constexpr (std::is_same_v<T, SimpleTokenInfo>)
                         {
                             formatFunctionPrototypeGenericCcAppendSimpleTokenImpl(arg, langOpts, fnArgsPassed, resVec, lastNameLen);
                         }

                         else if constexpr (std::is_same_v<T, TemplateArguments>)
                         {
                             auto formattedArgs = formatArguments(langOpts, arg.args, 1, 1, true /* tableMode */, true /* tableMode, Defs */, false /* addComments */ );
    
                             if (!formatFunctionPrototypeGenericCcTestAppendEmptyArgListStr(resVec, formattedArgs, "template<>"))
                             {
                                 resVec.emplace_back("template<");
                                 lastNameLen = 8u;
                                 std::size_t argIdx = 0;
                                 for(auto &&fa: formattedArgs)
                                 {
                                     if (argIdx)
                                     {
                                         resVec.emplace_back(std::string(lastNameLen, ' '));
                                         emplaceOrAppend(resVec, ", ");
                                     }
                                     else
                                     {
                                         emplaceOrAppend(resVec, " ");
                                     }

                                     ++argIdx;
                                     emplaceOrAppend(resVec, fa);
                                     //resVec.emplace_back("  " + fa);
                                     // if (idx<formattedArgs.size())
                                     //     resVec.back().append(",");
                                 }
                                 //resVec.emplace_back(std::string(lastNameLen, ' '));
                                 if (formattedArgs.size()>1)
                                     resVec.emplace_back(std::string(lastNameLen, ' '));
                                 else
                                     emplaceOrAppend(resVec, " ");
                                 emplaceOrAppend(resVec, ">");
                             }
                         }

                         else if constexpr (std::is_same_v<T, FunctionArguments>)
                         {
                             auto formattedArgs = formatArguments(langOpts, arg.args, 1, 1, true  /* tableMode */, true /* tableMode, Defs */, false /* addComments */ );
    
                             if (!formatFunctionPrototypeGenericCcTestAppendEmptyArgListStr(resVec, formattedArgs, "()"))
                             {
                                 emplaceOrAppend(resVec, "(");

                                 std::size_t argIdx = 0;
                                 for(auto &&fa: formattedArgs)
                                 {
                                     if (argIdx) // Ненулевой аргумент, делает отступ и запятую
                                     {
                                         resVec.emplace_back(std::string(lastNameLen, ' '));
                                         emplaceOrAppend(resVec, ", ");
                                     }
                                     else // Нулевой аргумент, делаем отступ от открывающей скобки
                                     {
                                         emplaceOrAppend(resVec, " ");
                                     }

                                     ++argIdx;
                                     emplaceOrAppend(resVec, fa);
                                 }

                                 if (formattedArgs.size()>1)
                                     resVec.emplace_back(std::string(lastNameLen, ' '));
                                 else
                                     emplaceOrAppend(resVec, " ");
                                 emplaceOrAppend(resVec, ")");
                                 if (tiIdx<(tiVec.size()-1))
                                    resVec.emplace_back(std::string());
                             }
    
                             fnArgsPassed = true;
                         }

                         else
                         {
                             static_assert(false, "non-exhaustive visitor!");
                         }
                     }
                   , ti
                   );
     }

    return resVec;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> formatFunctionPrototypeGenericCc(const std::vector<TokenInfoVariant> &tiVec, const umba::md::CodeOptions &langOpts, PrototypeFormatStyle fmtStyle)
{
    if (fmtStyle==PrototypeFormatStyle::msdn)
        return formatFunctionPrototypeGenericCcMsdn(tiVec, langOpts);

    else if (fmtStyle==PrototypeFormatStyle::man)
        return formatFunctionPrototypeGenericCcMan(tiVec, langOpts, false);

    else if (fmtStyle==PrototypeFormatStyle::tabMan)
        return formatFunctionPrototypeGenericCcMan(tiVec, langOpts, true);

    else if (fmtStyle==PrototypeFormatStyle::umba)
        return formatFunctionPrototypeGenericCcUmba(tiVec, langOpts);

    else 
        throw std::invalid_argument("Unknown formatting style");

}

// std::vector<std::string> formatArguments(const umba::md::CodeOptions &langOpts, const std::vector<ArgumentData> &adList, bool tableMode=true, bool addComments=false)
//  
// enum class PrototypeFormatStyle : int
// {
//     invalid   = -1 /*!< ! */,
//     unknown   = -1 /*!< ! */,
//     none      = 0x00 /*!< Don't format prototypes */,
//     msdn      = 0x01 /*!< Microsoft MSDN style */,
//     ms        = 0x01 /*!< Microsoft MSDN style */,
//     man       = 0x02 /*!< Linux/Unix man style */,
//     linux     = 0x02 /*!< Linux/Unix man style */,
//     unix      = 0x02 /*!< Linux/Unix man style */,
//     bsd       = 0x02 /*!< Linux/Unix man style */,
//     mdr       = 0x03 /*!< Our own style, close similar to MSDN-style */,
//     umba      = 0x03 /*!< Our own style, close similar to MSDN-style */,
//     dotnut    = 0x03 /*!< Our own style, close similar to MSDN-style */
//  
// }; // enum 



    // tokenTypeIsIdent(umba::tokenizer::payload_type tokenType)



// std::vector<TokenInfoVariant> 


// struct ArgumentData
// {
//     std::string typeName;
//     std::string ptrRef  ; // pointer or reference or other modifiers
//     std::string argName ;
//     std::string valueSep; // usualy '='
//     std::string defValue;
//     std::string comment ;





// } // namespace format



// struct FunctionArguments
// {
//     std::vector<ArgumentData>   args;
// };
//  
// struct TemplateArguments
// {
//     std::vector<ArgumentData>   args;
// };
//  
//  
// using TokenInfoVariant = std::variant< SimpleTokenInfo
//                                      , FunctionArguments
//                                      , TemplateArguments
//                                      >;



#if 0
inline
void splitArgument( std::string &typeName
                  , std::string &ptrRef
                  , std::string &argName
                  , std::string &valueSep
                  , std::string &defValue
                  , std::string argStr
                  , const std::string &assignOpStr = "="
                  , const std::string &ptrRefChars = "&*"
                  )
{
    auto pos = argStr.find(assignOpStr);
    if (pos!=argStr.npos)
    {
        valueSep = assignOpStr;
        defValue.assign(argStr, pos+assignOpStr.size(), assignOpStr.npos);
        argStr.erase(pos, assignOpStr.npos);
    }

    pos = argStr.size();
    for (;pos-->0;)
    {
        if (!isIndentifierChar(argStr[pos]))
            break;
    }

    if (pos!=0 && pos!=argStr.size())
    {
        argName.assign(argStr, pos+1, argStr.npos);
    }


}

//----------------------------------------------------------------------------
inline
void splitArgument( ArgumentData &ad
                  , const std::string &argStr
                  , const std::string &assignOpStr = "="
                  , const std::string &ptrRefChars = "&*"
                  )
{
    splitArgument( ad.typeName
                 , ad.ptrRef
                 , ad.argName
                 , ad.valueSep
                 , ad.defValue
                 , argStr
                 , assignOpStr
                 , ptrRefChars
                 );
}
#endif
// //----------------------------------------------------------------------------
// struct ArgumentData
// {
//     std::string typeName;
//     std::string ptrRef  ; // pointer or reference or other modifiers
//     std::string argName ;
//     std::string valueSep; // usualy '='
//     std::string defValue;
//     std::string comment ;
//  
// };


// template< typename TokenizerType
//         , typename InputIteratorType
//         >
// std::vector<std::string>
// processCodeImpl( TokenizerType &tokenizer, InputIteratorType itBegin, InputIteratorType itEnd
//                , ParsingBasicContext               &ctxBasic
//                , const std::vector<std::string>    &lines
//                , std::string                       &resText
//                )



//----------------------------------------------------------------------------


} // namespace utils
} // namespace code
} // namespace mdpp
} // namespace umba
