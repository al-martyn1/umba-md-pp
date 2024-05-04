#pragma once

#include <string>


inline
std::string tr_get_translations_json()
{
    #include "translations.json.h"

    return std::string( &translations_json[0], translations_json_size /* sizeof(translations_json) */ );
}

