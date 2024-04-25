#pragma once

#include "marty_cpp/marty_enum.h"

#include <exception>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>




enum class SnippetOptions : std::uint32_t
{
    invalid            = (std::uint32_t)(-1),
    unknown            = (std::uint32_t)(-1),
    noLineNo           = 0x1010 /*!< [x] Do not add line numbers to generated listing */,
    lineNo             = 0x1011 /*!< [x] Add line numbers to generated listing */,
    noTrim             = 0x1020 /*!< [x] Do not trim left inserted code */,
    noTrimLeft         = 0x1020 /*!< [x] Do not trim left inserted code */,
    trim               = 0x1021 /*!< [x] Trim left inserted code */,
    trimLeft           = 0x1021 /*!< [x] Trim left inserted code */,
    noTag              = 0x1030 /*!< Do not add language tag */,
    noLangTag          = 0x1030 /*!< Do not add language tag */,
    tag                = 0x1031 /*!< Add language tag */,
    langTag            = 0x1031 /*!< Add language tag */,
    noFilename         = 0x1040 /*!< Do not add filename to listing */,
    filename           = 0x1041 /*!< Add filename to listing */,
    noPath             = 0x1050 /*!< Do not add full path to filename (filename option) */,
    path               = 0x1051 /*!< Add full path to filename (filename option) */,
    noFail             = 0x1060 /*!< [x] If insertion file not found, don't add insert command text to result - used for C++ __has_include emulation. If there is some other failures occurs (such invalif options), wrong line will be always inserted */,
    fail               = 0x1061 /*!< [x] If insertion file not found, add insert command text to result. If there is some other failures occurs (such invalif options), wrong line will be always inserted. Default is set in basic configs */,
    noKeepCutTags      = 0x1070 /*!< [x] Don't keep nested cut tags/markers */,
    keepCutTags        = 0x1071 /*!< [x] Keep nested cut tags/markers */,
    noFilenameLineNo   = 0x1080 /*!< Do not add line number to filename before liting */,
    filenameLineNo     = 0x1081 /*!< Add line number to filename before liting */,
    noSnippetOptions   = 0x1090 /*!< Do not add actual snippet options line before listing */,
    snippetOptions     = 0x1091 /*!< Add actual snippet options line before listing */,
    noTrimArround      = 0x10A0 /*!< Do not trim empty lines before and after listing */,
    trimArround        = 0x10A1 /*!< Trim empty lines before and after listing */,
    noDoc              = 0x10F0 /*!< -doc */,
    doc                = 0x10F1 /*!< +doc */,
    raise              = 0x2011 /*!< Numeric option */

}; // enum class SnippetOptions : std::uint32_t

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(SnippetOptions)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( SnippetOptions, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::snippetOptions     , "SnippetOptions"   );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::invalid            , "Invalid"          );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noFilename         , "NoFilename"       );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noLineNo           , "NoLineNo"         );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::lineNo             , "LineNo"           );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noTag              , "NoTag"            );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noTrim             , "NoTrim"           );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::trim               , "Trim"             );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::tag                , "Tag"              );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::filename           , "Filename"         );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noPath             , "NoPath"           );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::path               , "Path"             );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noFilenameLineNo   , "NoFilenameLineNo" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noFail             , "NoFail"           );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::fail               , "Fail"             );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noKeepCutTags      , "NoKeepCutTags"    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::keepCutTags        , "KeepCutTags"      );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::filenameLineNo     , "FilenameLineNo"   );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noTrimArround      , "NoTrimArround"    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noSnippetOptions   , "NoSnippetOptions" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::trimArround        , "TrimArround"      );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::noDoc              , "NoDoc"            );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::doc                , "Doc"              );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptions::raise              , "Raise"            );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( SnippetOptions, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( SnippetOptions, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::snippetOptions     , "snippet-options"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::snippetOptions     , "snippet_options"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::snippetOptions     , "snippetoptions"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::invalid            , "invalid"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::invalid            , "unknown"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFilename         , "no-filename"         );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFilename         , "no_filename"         );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFilename         , "nofilename"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noLineNo           , "no-line-no"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noLineNo           , "no_line_no"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noLineNo           , "nolineno"            );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::lineNo             , "line-no"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::lineNo             , "line_no"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::lineNo             , "lineno"              );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTag              , "no-tag"              );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTag              , "no_tag"              );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTag              , "notag"               );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTag              , "no-lang-tag"         );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTag              , "no_lang_tag"         );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTag              , "nolangtag"           );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrim             , "no_trim_left"        );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrim             , "no-trim"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrim             , "notrimleft"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrim             , "no_trim"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrim             , "notrim"              );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrim             , "no-trim-left"        );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::trim               , "trim"                );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::trim               , "trim-left"           );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::trim               , "trim_left"           );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::trim               , "trimleft"            );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::tag                , "langtag"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::tag                , "tag"                 );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::tag                , "lang-tag"            );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::tag                , "lang_tag"            );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::filename           , "filename"            );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noPath             , "no-path"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noPath             , "no_path"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noPath             , "nopath"              );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::path               , "path"                );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFilenameLineNo   , "no-filename-line-no" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFilenameLineNo   , "no_filename_line_no" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFilenameLineNo   , "nofilenamelineno"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFail             , "no-fail"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFail             , "no_fail"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noFail             , "nofail"              );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::fail               , "fail"                );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noKeepCutTags      , "no-keep-cut-tags"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noKeepCutTags      , "no_keep_cut_tags"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noKeepCutTags      , "nokeepcuttags"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::keepCutTags        , "keep-cut-tags"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::keepCutTags        , "keepcuttags"         );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::keepCutTags        , "keep_cut_tags"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::filenameLineNo     , "filename-line-no"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::filenameLineNo     , "filename_line_no"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::filenameLineNo     , "filenamelineno"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrimArround      , "no-trim-arround"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrimArround      , "no_trim_arround"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noTrimArround      , "notrimarround"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noSnippetOptions   , "no-snippet-options"  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noSnippetOptions   , "no_snippet_options"  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noSnippetOptions   , "nosnippetoptions"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::trimArround        , "trim-arround"        );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::trimArround        , "trim_arround"        );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::trimArround        , "trimarround"         );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noDoc              , "no-doc"              );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noDoc              , "no_doc"              );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::noDoc              , "nodoc"               );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::doc                , "doc"                 );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptions::raise              , "raise"               );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( SnippetOptions, std::map, 1 )


enum class SnippetOptionsParsingResult : std::uint32_t
{
    invalid          = (std::uint32_t)(-1),
    unknown          = (std::uint32_t)(-1),
    fail             = 0x0000 /*!< Parsing failed */,
    ok               = 0x0001 /*!< Parsing ok, conditions met */,
    okButCondition   = 0x0002 /*!< Parsing ok, but conditions not met */

}; // enum class SnippetOptionsParsingResult : std::uint32_t

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(SnippetOptionsParsingResult)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( SnippetOptionsParsingResult, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptionsParsingResult::invalid          , "Invalid"        );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptionsParsingResult::fail             , "Fail"           );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptionsParsingResult::ok               , "Ok"             );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( SnippetOptionsParsingResult::okButCondition   , "OkButCondition" );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( SnippetOptionsParsingResult, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( SnippetOptionsParsingResult, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptionsParsingResult::invalid          , "invalid"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptionsParsingResult::invalid          , "unknown"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptionsParsingResult::fail             , "fail"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptionsParsingResult::ok               , "ok"               );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptionsParsingResult::okButCondition   , "ok-but-condition" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptionsParsingResult::okButCondition   , "ok_but_condition" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( SnippetOptionsParsingResult::okButCondition   , "okbutcondition"   );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( SnippetOptionsParsingResult, std::map, 1 )


enum class ConditionOperators : std::uint32_t
{
    invalid        = (std::uint32_t)(-1),
    unknown        = (std::uint32_t)(-1),
    equal          = 0x0000 /*!< == */,
    notEqual       = 0x0001 /*!< != */,
    greaterEqual   = 0x0002 /*!< >= */,
    lessEqual      = 0x0003 /*!< <= */,
    greater        = 0x0004 /*!< > */,
    less           = 0x0005 /*!< < */

}; // enum class ConditionOperators : std::uint32_t

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(ConditionOperators)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( ConditionOperators, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ConditionOperators::less           , "Less"         );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ConditionOperators::lessEqual      , "LessEqual"    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ConditionOperators::invalid        , "Invalid"      );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ConditionOperators::equal          , "Equal"        );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ConditionOperators::notEqual       , "NotEqual"     );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ConditionOperators::greaterEqual   , "GreaterEqual" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ConditionOperators::greater        , "Greater"      );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( ConditionOperators, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( ConditionOperators, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ConditionOperators::less           , "less"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ConditionOperators::lessEqual      , "less-equal"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ConditionOperators::lessEqual      , "less_equal"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ConditionOperators::lessEqual      , "lessequal"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ConditionOperators::invalid        , "invalid"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ConditionOperators::invalid        , "unknown"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ConditionOperators::equal          , "equal"         );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ConditionOperators::notEqual       , "not-equal"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ConditionOperators::notEqual       , "not_equal"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ConditionOperators::notEqual       , "notequal"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ConditionOperators::greaterEqual   , "greater-equal" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ConditionOperators::greaterEqual   , "greater_equal" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ConditionOperators::greaterEqual   , "greaterequal"  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ConditionOperators::greater        , "greater"       );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( ConditionOperators, std::map, 1 )


enum class ListingNestedTagsMode : std::uint32_t
{
    invalid     = (std::uint32_t)(-1),
    unknown     = (std::uint32_t)(-1),
    keep        = 0x0000 /*!< keep lines with tags */,
    remove      = 0x0001 /*!< remove lines with tags */,
    empty       = 0x0002 /*!< replace lines with tags with empty lines */,
    emptyLine   = 0x0002 /*!< replace lines with tags with empty lines */

}; // enum class ListingNestedTagsMode : std::uint32_t

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(ListingNestedTagsMode)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( ListingNestedTagsMode, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ListingNestedTagsMode::keep      , "Keep"    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ListingNestedTagsMode::invalid   , "Invalid" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ListingNestedTagsMode::remove    , "Remove"  );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ListingNestedTagsMode::empty     , "Empty"   );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( ListingNestedTagsMode, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( ListingNestedTagsMode, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingNestedTagsMode::keep      , "keep"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingNestedTagsMode::invalid   , "invalid"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingNestedTagsMode::invalid   , "unknown"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingNestedTagsMode::remove    , "remove"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingNestedTagsMode::empty     , "empty"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingNestedTagsMode::empty     , "empty-line" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingNestedTagsMode::empty     , "empty_line" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingNestedTagsMode::empty     , "emptyline"  );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( ListingNestedTagsMode, std::map, 1 )


enum class ProcessingOptions : std::uint32_t
{
    invalid               = (std::uint32_t)(-1),
    unknown               = (std::uint32_t)(-1),
    noNumericSections     = 0x1010,
    numericSections       = 0x1011,
    noGenerateSectionId   = 0x1020,
    generateSectionId     = 0x1021

}; // enum class ProcessingOptions : std::uint32_t

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(ProcessingOptions)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( ProcessingOptions, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ProcessingOptions::generateSectionId     , "GenerateSectionId"   );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ProcessingOptions::invalid               , "Invalid"             );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ProcessingOptions::noNumericSections     , "NoNumericSections"   );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ProcessingOptions::numericSections       , "NumericSections"     );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ProcessingOptions::noGenerateSectionId   , "NoGenerateSectionId" );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( ProcessingOptions, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( ProcessingOptions, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ProcessingOptions::generateSectionId     , "generate-section-id"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ProcessingOptions::generateSectionId     , "generate_section_id"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ProcessingOptions::generateSectionId     , "generatesectionid"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ProcessingOptions::invalid               , "invalid"                );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ProcessingOptions::invalid               , "unknown"                );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ProcessingOptions::noNumericSections     , "no-numeric-sections"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ProcessingOptions::noNumericSections     , "no_numeric_sections"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ProcessingOptions::noNumericSections     , "nonumericsections"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ProcessingOptions::numericSections       , "numeric-sections"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ProcessingOptions::numericSections       , "numeric_sections"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ProcessingOptions::numericSections       , "numericsections"        );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ProcessingOptions::noGenerateSectionId   , "no-generate-section-id" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ProcessingOptions::noGenerateSectionId   , "no_generate_section_id" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ProcessingOptions::noGenerateSectionId   , "nogeneratesectionid"    );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( ProcessingOptions, std::map, 1 )


