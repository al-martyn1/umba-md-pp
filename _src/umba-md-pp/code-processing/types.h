#pragma once


#include "umba/tokenizer.h"
//
#include <string>
#include <vector>
#include <variant>

//
//#include "../code-options-database.h"
//
// #include "../snippet_options.h"

// 
#include "../enums.h"

#include "enums.h"


//----------------------------------------------------------------------------
#define MDPP_CODE_PROCESSING_TOKEN_KEYWORD_TEMPLATE    ((UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST) + 0x01)
#define MDPP_CODE_PROCESSING_TOKEN_KEYWORD_DEFAULT     ((UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST) + 0x02)
#define MDPP_CODE_PROCESSING_TOKEN_KEYWORD_DELETE      ((UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST) + 0x03)
#define MDPP_CODE_PROCESSING_TOKEN_KEYWORD_VIRTUAL     ((UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST) + 0x04)
#define MDPP_CODE_PROCESSING_TOKEN_KEYWORD_PUBLIC      ((UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST) + 0x05)
#define MDPP_CODE_PROCESSING_TOKEN_KEYWORD_PROTECTED   ((UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST) + 0x06)
#define MDPP_CODE_PROCESSING_TOKEN_KEYWORD_PRIVATE     ((UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST) + 0x07)
#define MDPP_CODE_PROCESSING_TOKEN_KEYWORD_CLASS       ((UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST) + 0x08)
#define MDPP_CODE_PROCESSING_TOKEN_KEYWORD_STRUCT      ((UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST) + 0x09)
#define MDPP_CODE_PROCESSING_TOKEN_KEYWORD_CONCEPT     ((UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST) + 0x0A)
#define MDPP_CODE_PROCESSING_TOKEN_KEYWORD_REQUIRES    ((UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST) + 0x0B)
//#define MDPP_CODE_PROCESSING_TOKEN_KEYWORD_    ((UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST) + 0x01)
//#define MDPP_CODE_PROCESSING_TOKEN_KEYWORD_    ((UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST) + 0x01)
//#define MDPP_CODE_PROCESSING_TOKEN_KEYWORD_    ((UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST) + 0x01)


// Для упрощения форматирования заранее помечаем идентификатор перед последней группой круглых скобок данным токеном
#define MDPP_CODE_PROCESSING_TOKEN_FUNCTION_NAME       ((UMBA_TOKENIZER_TOKEN_BASE_USER_FIRST)+0x01)
//#define UMBA_TOKENIZER_TOKEN_BASE_USER_LAST                                           0x00FFu     


//----------------------------------------------------------------------------
// umba::mdpp::code::

namespace umba {

namespace md {
class CodeOptions;
} // namespace md



namespace mdpp {
namespace code {

namespace utils {

auto makeTokenText(umba::tokenizer::payload_type tokenType, umba::iterator::TextPositionCountingIterator<char> b, umba::iterator::TextPositionCountingIterator<char> e);

} // namespace utils


//----------------------------------------------------------------------------

// void (C::* p)(int) = &C::f;

typedef std::vector<std::string> (*simpleCodeLinesProcessingFnPtr)( const umba::md::CodeOptions                     &langOpts
                                                                  , const std::string                               &lang
                                                                  , bool                                            keepComments
                                                                  , const std::unordered_set<SnippetOptions>        &snippetFlagsOptions
                                                                  , const std::unordered_map<SnippetOptions, int>   &snippetIntOptions
                                                                  , const std::vector<std::string>                  &lines
                                                                  );

std::unordered_map<std::string, simpleCodeLinesProcessingFnPtr> makeSimpleProcessors();
const std::unordered_map<std::string, simpleCodeLinesProcessingFnPtr>& getSimpleProcessors();
simpleCodeLinesProcessingFnPtr getSimpleProcessorFuction(std::string fnName);


//----------------------------------------------------------------------------
struct SimpleTokenInfo
{
    umba::tokenizer::payload_type   tokenType = UMBA_TOKENIZER_TOKEN_NUL;
    std::string                     tokenText;


    SimpleTokenInfo() = default;
    SimpleTokenInfo(const SimpleTokenInfo &) = default;
    SimpleTokenInfo(SimpleTokenInfo &&) = default;
    SimpleTokenInfo& operator=(const SimpleTokenInfo &) = default;
    SimpleTokenInfo& operator=(SimpleTokenInfo &&) = default;

    SimpleTokenInfo(umba::tokenizer::payload_type tp, const std::string &tx) : tokenType(tp), tokenText(tx) {}

    template<typename IterType>
    SimpleTokenInfo(umba::tokenizer::payload_type tp, IterType b, IterType e) : tokenType(tp), tokenText(utils::makeTokenText(tp, b, e)) {}

}; // struct SimpleTokenInfo

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct ArgumentData
{
    std::string typeName;
    std::string ptrRef  ; // pointer or reference or other modifiers
    std::string argName ;
    std::string valueSep; // usualy '='
    std::string defValue;
    std::string comment ;


    void clear()
    {
        typeName.clear();
        ptrRef  .clear();
        argName .clear();
        valueSep.clear();
        defValue.clear();
        comment .clear();
    }

    void reset() { reset(); }

    bool empty() const
    {
        return typeName.empty()
            && ptrRef  .empty()
            && argName .empty()
            && valueSep.empty()
            && defValue.empty()
            // && comment .empty() // незначащие данные
             ;
    }

    void trim()
    {
        umba::string_plus::rtrim(typeName);
        umba::string_plus::rtrim(ptrRef  );
        umba::string_plus::rtrim(argName );
        umba::string_plus::rtrim(valueSep);
        umba::string_plus::rtrim(defValue);
        umba::string_plus::rtrim(comment );
    }

};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct FunctionArguments
{
    std::vector<ArgumentData>   args;

    void                clear()      { args.clear(); }
    std::size_t         size()       { return args.size(); }
    bool                empty()      { return args.empty(); }
    void                push_back(const ArgumentData &ad) { return args.push_back(ad); }
    ArgumentData&       back()       { return args.back(); }
    const ArgumentData& back() const { return args.back(); }

    ArgumentData&       operator[](std::size_t i)       { return args[i]; }
    const ArgumentData& operator[](std::size_t i) const { return args[i]; }

};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct TemplateArguments
{
    std::vector<ArgumentData>   args;

    void                clear()      { args.clear(); }
    std::size_t         size()       { return args.size(); }
    bool                empty()      { return args.empty(); }
    void                push_back(const ArgumentData &ad) { return args.push_back(ad); }
    ArgumentData&       back()       { return args.back(); }
    const ArgumentData& back() const { return args.back(); }

    ArgumentData&       operator[](std::size_t i)       { return args[i]; }
    const ArgumentData& operator[](std::size_t i) const { return args[i]; }

};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
using TokenInfoVariant = std::variant< SimpleTokenInfo
                                     , FunctionArguments
                                     , TemplateArguments
                                     >;

//----------------------------------------------------------------------------

// using std::vector<SimpleTokenInfo>  SimpleTokenInfoVector;


// Примеры наблонов - https://en.cppreference.com/w/cpp/language/template_specialization.html
// Members of specializations
//----------------------------------------------------------------------------
// struct FunctionPrototypeInfo
// {
//     SimpleTokenInfoVector                       templateSpecifiers       ;
//     bool                                        bTemplate         = false;
//     std::vector<SimpleTokenInfoVector>          templateArgs             ;
//     SimpleTokenInfoVector                       functionSpecifiers       ;
//     SimpleTokenInfo                             functionName             ;
//     std::vector<SimpleTokenInfoVector>          functionTplSpecializers  ;
//     SimpleTokenInfo                             functionArgsOpen         ;
//     SimpleTokenInfo                             functionArgsClose        ;
//     std::vector<SimpleTokenInfoVector>          functionArgs             ;
//     SimpleTokenInfoVector                       functionPostSpec         ;
//  
// }; // struct FunctionPrototypeInfo

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

} // namespace code
} // namespace mdpp
} // namespace umba

// umba::mdpp::code::
