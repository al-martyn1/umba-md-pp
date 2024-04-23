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
    noLineNo     = 0x1010 /*!< Do not add line numbers to generated listing */,
    lineNo       = 0x1011 /*!< Add line numbers to generated listing */,
    noTrim       = 0x1020 /*!< Do not trim left inserted code */,
    noTrimLeft   = 0x1020 /*!< Do not trim left inserted code */,
    trim         = 0x1021 /*!< Trim left inserted code */,
    trimLeft     = 0x1021 /*!< Trim left inserted code */,
    noTag        = 0x1030 /*!< Do not add language tag */,
    noLangTag    = 0x1030 /*!< Do not add language tag */,
    tag          = 0x1031 /*!< Add language tag */,
    langTag      = 0x1031 /*!< Add language tag */,
    noFilename   = 0x1040 /*!< Do not add filename to listing */,
    filename     = 0x1041 /*!< Add filename to listing */,
    noPath       = 0x1050 /*!< Do not add full path to filename (filename option) */,
    path         = 0x1051 /*!< Add full path to filename (filename option) */,
    noFail       = 0x1060 /*!< If insert failed, don't add insert command text to result */,
    fail         = 0x1061 /*!< If insert failed, add command insert command text to result */,
    noDoc        = 0x10F0 /*!< -doc */,
    doc          = 0x10F1 /*!< +doc */,
    raise        = 0x2011 /*!< Numeric option */

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
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noPath       , "NoPath"     );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::path         , "Path"       );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noFail       , "NoFail"     );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::fail         , "Fail"       );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noDoc        , "NoDoc"      );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::doc          , "Doc"        );
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
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noPath       , "no-path"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noPath       , "no_path"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noPath       , "nopath"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::path         , "path"         );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFail       , "no-fail"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFail       , "no_fail"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFail       , "nofail"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::fail         , "fail"         );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noDoc        , "no-doc"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noDoc        , "no_doc"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noDoc        , "nodoc"        );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::doc          , "doc"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::raise        , "raise"        );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( SnippetOptions, std::map, 1 )


