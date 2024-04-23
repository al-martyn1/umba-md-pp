#pragma once

#include "marty_cpp/marty_enum.h"

#include <exception>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>




enum class SnippetOptions : std::uint32_t
{
    invalid      = (std::uint32_t)(-1),
    unknown      = (std::uint32_t)(-1),
    noLineNo     = 0x0010 /*!< Do not add line numbers to generated listing */,
    lineNo       = 0x0011 /*!< Add line numbers to generated listing */,
    noTrim       = 0x0020 /*!< Do not trim left inserted code */,
    noTrimLeft   = 0x0020 /*!< Do not trim left inserted code */,
    trim         = 0x0021 /*!< Trim left inserted code */,
    trimLeft     = 0x0021 /*!< Trim left inserted code */,
    noTag        = 0x0030 /*!< Do not add language tag */,
    noLangTag    = 0x0030 /*!< Do not add language tag */,
    tag          = 0x0031 /*!< Add language tag */,
    langTag      = 0x0031 /*!< Add language tag */,
    noFilename   = 0x0040 /*!< Do not add filename to listing */,
    filename     = 0x0041 /*!< Add filename to listing */,
    raise        = 0x1001 /*!< Numeric option */

}; // enum class SnippetOptions : std::uint32_t

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(SnippetOptions)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( SnippetOptions, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::invalid      , "Invalid"    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noFilename   , "NoFilename" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noLineNo     , "NoLineNo"   );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::lineNo       , "LineNo"     );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noTag        , "NoTag"      );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noTrim       , "NoTrim"     );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::trim         , "Trim"       );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::tag          , "Tag"        );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::filename     , "Filename"   );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::raise        , "Raise"      );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( SnippetOptions, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( SnippetOptions, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::invalid      , "invalid"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::invalid      , "unknown"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFilename   , "no-filename"  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFilename   , "no_filename"  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFilename   , "nofilename"   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noLineNo     , "no-line-no"   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noLineNo     , "no_line_no"   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noLineNo     , "nolineno"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::lineNo       , "line-no"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::lineNo       , "line_no"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::lineNo       , "lineno"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTag        , "no-tag"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTag        , "no_tag"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTag        , "notag"        );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTag        , "no-lang-tag"  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTag        , "no_lang_tag"  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTag        , "nolangtag"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrim       , "no_trim_left" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrim       , "no-trim"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrim       , "notrimleft"   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrim       , "no_trim"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrim       , "notrim"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrim       , "no-trim-left" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::trim         , "trim"         );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::trim         , "trim-left"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::trim         , "trim_left"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::trim         , "trimleft"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::tag          , "langtag"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::tag          , "tag"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::tag          , "lang-tag"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::tag          , "lang_tag"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::filename     , "filename"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::raise        , "raise"        );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( SnippetOptions, std::map, 1 )


