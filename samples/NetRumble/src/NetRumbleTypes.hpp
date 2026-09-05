// SPDX-License-Identifier: MS-PL
#pragma once
#include "System/Xml/Serialization/detail/XmlMember.hpp"
namespace NetRumble {
/** @brief Selects the SpriteBatch blend pass used for particles. */
enum class SpriteBlendMode {
  /** @brief Adds particle color to the existing framebuffer color. */
  Additive,
  /** @brief Uses conventional source-alpha blending. */
  AlphaBlend
};
SHARP_XML_ENUM(SpriteBlendMode, SHARP_XML_E(SpriteBlendMode, Additive),
               SHARP_XML_E(SpriteBlendMode, AlphaBlend))
/** @brief Identifies one registered particle-effect graph. */
enum class ParticleEffectType {
  /** @brief Effect emitted when a ship spawns. */
  ShipSpawn,
  /** @brief Effect emitted when a ship explodes. */
  ShipExplosion,
  /** @brief Effect emitted behind a flying rocket. */
  RocketTrail,
  /** @brief Effect emitted when a rocket explodes. */
  RocketExplosion,
  /** @brief Effect emitted when a mine explodes. */
  MineExplosion,
  /** @brief Effect emitted when a laser impacts. */
  LaserExplosion
};
} // namespace NetRumble
