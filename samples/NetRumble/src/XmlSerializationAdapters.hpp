// SPDX-License-Identifier: MS-PL
#pragma once

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "System/Xml/Serialization/detail/XmlMember.hpp"
#include <tuple>
namespace Microsoft::Xna::Framework {
CNAEXT inline constexpr const char *SharpXmlRootName(const Vector2 *) {
  return "Vector2";
}
CNAEXT inline constexpr auto SharpXmlMembers(const Vector2 *) {
  return std::make_tuple(
      System::Xml::Serialization::detail::MakeMember("X", &Vector2::X),
      System::Xml::Serialization::detail::MakeMember("Y", &Vector2::Y));
}
CNAEXT inline constexpr const char *SharpXmlRootName(const Vector4 *) {
  return "Vector4";
}
CNAEXT inline constexpr auto SharpXmlMembers(const Vector4 *) {
  return std::make_tuple(
      System::Xml::Serialization::detail::MakeMember("X", &Vector4::X),
      System::Xml::Serialization::detail::MakeMember("Y", &Vector4::Y),
      System::Xml::Serialization::detail::MakeMember("Z", &Vector4::Z),
      System::Xml::Serialization::detail::MakeMember("W", &Vector4::W));
}
} // namespace Microsoft::Xna::Framework
