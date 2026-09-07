#pragma once

// XmlSaveHooks.hpp -- CNAEXT.
//
// Opts the two XNA value types the save file contains -- Point and Vector2 -- and this game's
// Direction enum into System::Xml::Serialization. XmlSerializer finds these by argument-dependent
// lookup, the same customization point the module documents, so no CNA header changes. The member
// names and the enum text are what the XNA 4.0 runtime writes, measured rather than assumed.

#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"

#include "Direction.hpp"

namespace Microsoft::Xna::Framework {

inline constexpr const char* SharpXmlRootName(const Point*) { return "Point"; }
inline constexpr auto SharpXmlMembers(const Point*) {
    return std::make_tuple(
        ::System::Xml::Serialization::detail::MakeMember("X", &Point::X),
        ::System::Xml::Serialization::detail::MakeMember("Y", &Point::Y));
}

inline constexpr const char* SharpXmlRootName(const Vector2*) { return "Vector2"; }
inline constexpr auto SharpXmlMembers(const Vector2*) {
    return std::make_tuple(
        ::System::Xml::Serialization::detail::MakeMember("X", &Vector2::X),
        ::System::Xml::Serialization::detail::MakeMember("Y", &Vector2::Y));
}

} // namespace Microsoft::Xna::Framework

namespace RolePlayingGameData {

SHARP_XML_ENUM(Direction, SHARP_XML_E(Direction, North), SHARP_XML_E(Direction, NorthEast),
               SHARP_XML_E(Direction, East), SHARP_XML_E(Direction, SouthEast),
               SHARP_XML_E(Direction, South), SHARP_XML_E(Direction, SouthWest),
               SHARP_XML_E(Direction, West), SHARP_XML_E(Direction, NorthWest))

} // namespace RolePlayingGameData
