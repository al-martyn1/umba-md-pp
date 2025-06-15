#pragma once

//
#include <string>
#include <vector>


namespace umba {
namespace mdpp {
namespace code {

//----------------------------------------------------------------------------

// void (C::* p)(int) = &C::f;

typedef std::vector<std::string> (*simpleCodeLinesProcessingFnPtr)(const std::vector<std::string> &lines);



//----------------------------------------------------------------------------

} // namespace code
} // namespace mdpp
} // namespace umba
