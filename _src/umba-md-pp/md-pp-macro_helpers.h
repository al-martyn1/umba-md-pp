#pragma once


#include "umba/env.h"
#include "umba/macros.h"
#include "umba/macro_helpers.h"
#include "umba/string_plus.h"


// umba::md::
namespace umba {
namespace md {


template<typename StringType>
struct MacroTextFromMapOrEnvRef : public umba::macros::IMacroTextGetter<StringType>
{
    const umba::macros::StringStringMap<StringType> &m;
    bool                                            envAllowed = true; // To allow control in runtime

    MacroTextFromMapOrEnvRef(const umba::macros::StringStringMap<StringType> &_m, bool _envAllowed = true) : m(_m), envAllowed(_envAllowed) {}

    virtual bool operator()(const StringType &name, StringType &text) const override
    {
        if (umba::macros::getMacroTextFromMap(m, name, text))
            return true;

        if (umba::macros::getMacroTextFromMap(m, "^"+umba::string_plus::toupper_copy(name), text))
            return true;

        if (!envAllowed)
            return false;

        return umba::env::getVar(name, text);
    }

    const char* getName() const { return "MacroTextFromMapOrEnvRef"; }

}; // struct MacroTextFromMapOrEnvRef


//----------------------------------------------------------------------------

} // namespace md
} // namespace umba

