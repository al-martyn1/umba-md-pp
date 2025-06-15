#pragma once

#include "umba/string.h"
//
#include "types.h"
#include "cc/simple-processing.h"

//
#include <unordered_map>

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// umba::mdpp::code::
namespace umba {
namespace mdpp {
namespace code {

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/*
--set-code-handler=LANG:HANDLER_TYPE:HANDLER_NAME

*/
//----------------------------------------------------------------------------
inline
std::unordered_map<std::string, simpleCodeLinesProcessingFnPtr>
makeSimpleProcessors()
{
    std::unordered_map<std::string, simpleCodeLinesProcessingFnPtr> m;
    m["cc-fn-ptt-extract"  ] = &cc::extractFunctionPrototype;
    m["cc-cls-ptt-extract" ] = &cc::extractClassPrototype   ;
    m["cc-fn-ptt-format"   ] = &cc::formatFunctionPrototype ;
    m["cc-cls-ptt-format"  ] = &cc::formatClassPrototype    ;
    return m;
}

//----------------------------------------------------------------------------
inline
const std::unordered_map<std::string, simpleCodeLinesProcessingFnPtr>&
getSimpleProcessors()
{
    static auto m = makeSimpleProcessors();
    return m;
}

//----------------------------------------------------------------------------
inline
simpleCodeLinesProcessingFnPtr getSimpleProcessorFuction(std::string fnName)
{
    const auto &m = getSimpleProcessors();
    umba::string_plus::tolower(fnName);

    auto it = m.find(fnName);
    if (it==m.end())
        return 0;

    return it->second;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace code
} // namespace mdpp
} // namespace umba

