#pragma once

#include "../types.h"

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
std::vector<std::string> extractFunctionPrototype(const std::vector<std::string> &lines)
{
    return lines;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> extractClassPrototype(const std::vector<std::string> &lines)
{
    return lines;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> formatFunctionPrototype(const std::vector<std::string> &lines)
{
    return lines;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> formatClassPrototype(const std::vector<std::string> &lines)
{
    return lines;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace cc
} // namespace code
} // namespace mdpp
} // namespace umba

