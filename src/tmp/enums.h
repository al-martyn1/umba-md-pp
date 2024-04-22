#pragma once

#include "marty_cpp/marty_enum.h"

#include <exception>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>



namespace dotNutDocGen{

enum class DocGenCommand : std::uint32_t
{
    invalid       = (std::uint32_t)(-1),
    unknown       = (std::uint32_t)(-1),
    translation   = 0x00,
    tr            = 0x00,
    markdown      = 0x01,
    md            = 0x01,
    listing       = 0x02,
    lst           = 0x02,
    document      = 0x03,
    doc           = 0x03

}; // enum class DocGenCommand : std::uint32_t

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(DocGenCommand)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( DocGenCommand, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( DocGenCommand::invalid       , "Invalid"     );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( DocGenCommand::document      , "Document"    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( DocGenCommand::markdown      , "Markdown"    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( DocGenCommand::translation   , "Translation" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( DocGenCommand::listing       , "Listing"     );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( DocGenCommand, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( DocGenCommand, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( DocGenCommand::invalid       , "invalid"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( DocGenCommand::invalid       , "unknown"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( DocGenCommand::document      , "document"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( DocGenCommand::document      , "doc"         );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( DocGenCommand::markdown      , "md"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( DocGenCommand::markdown      , "markdown"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( DocGenCommand::translation   , "translation" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( DocGenCommand::translation   , "tr"          );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( DocGenCommand::listing       , "listing"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( DocGenCommand::listing       , "lst"         );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( DocGenCommand, std::map, 1 )


enum class ListingNestedTagsMode : std::uint32_t
{
    invalid     = (std::uint32_t)(-1),
    unknown     = (std::uint32_t)(-1),
    keep        = 0x00,
    remove      = 0x01,
    empty       = 0x02,
    emptyLine   = 0x02

}; // enum class ListingNestedTagsMode : std::uint32_t

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(ListingNestedTagsMode)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( ListingNestedTagsMode, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ListingNestedTagsMode::keep      , "Keep"    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ListingNestedTagsMode::invalid   , "Invalid" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ListingNestedTagsMode::remove    , "Remove"  );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ListingNestedTagsMode::empty     , "Empty"   );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( ListingNestedTagsMode, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( ListingNestedTagsMode, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingNestedTagsMode::keep      , "keep"      );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingNestedTagsMode::invalid   , "invalid"   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingNestedTagsMode::invalid   , "unknown"   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingNestedTagsMode::remove    , "remove"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingNestedTagsMode::empty     , "empty"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingNestedTagsMode::empty     , "emptyline" );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( ListingNestedTagsMode, std::map, 1 )


enum class ListingUpdateTrMode : std::uint32_t
{
    invalid      = (std::uint32_t)(-1),
    unknown      = (std::uint32_t)(-1),
    none         = 0x00,
    update       = 0x01,
    updateOnly   = 0x02

}; // enum class ListingUpdateTrMode : std::uint32_t

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(ListingUpdateTrMode)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( ListingUpdateTrMode, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ListingUpdateTrMode::invalid      , "Invalid"    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ListingUpdateTrMode::none         , "None"       );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ListingUpdateTrMode::update       , "Update"     );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( ListingUpdateTrMode::updateOnly   , "UpdateOnly" );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( ListingUpdateTrMode, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( ListingUpdateTrMode, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingUpdateTrMode::invalid      , "invalid"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingUpdateTrMode::invalid      , "unknown"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingUpdateTrMode::none         , "none"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingUpdateTrMode::update       , "update"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( ListingUpdateTrMode::updateOnly   , "updateonly" );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( ListingUpdateTrMode, std::map, 1 )


enum class MdExtraProcessingTags : std::uint32_t
{
    invalid   = (std::uint32_t)(-1),
    unknown   = (std::uint32_t)(-1),
    none      = 0x00,
    code      = 0x01,
    c         = 0x01,
    typeref   = 0x02,
    r         = 0x02

}; // enum class MdExtraProcessingTags : std::uint32_t

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(MdExtraProcessingTags)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( MdExtraProcessingTags, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( MdExtraProcessingTags::invalid   , "Invalid" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( MdExtraProcessingTags::none      , "None"    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( MdExtraProcessingTags::code      , "Code"    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( MdExtraProcessingTags::typeref   , "Typeref" );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( MdExtraProcessingTags, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( MdExtraProcessingTags, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( MdExtraProcessingTags::invalid   , "invalid" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( MdExtraProcessingTags::invalid   , "unknown" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( MdExtraProcessingTags::none      , "none"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( MdExtraProcessingTags::code      , "code"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( MdExtraProcessingTags::code      , "c"       );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( MdExtraProcessingTags::typeref   , "typeref" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( MdExtraProcessingTags::typeref   , "r"       );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( MdExtraProcessingTags, std::map, 1 )

} // namespace dotNutDocGen

