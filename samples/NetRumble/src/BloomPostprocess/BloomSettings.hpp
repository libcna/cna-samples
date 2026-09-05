// SPDX-License-Identifier: MS-PL
#pragma once
#include <array>
#include <string>
namespace NetRumble {
/** @brief Settings used by the bloom postprocessor. */
class BloomSettings {
public:
  /** @brief Creates a bloom preset. @param name Display name. @param
   * bloomThreshold Brightness threshold. @param blurAmount Blur width. @param
   * bloomIntensity Bloom intensity. @param baseIntensity Base intensity. @param
   * bloomSaturation Bloom saturation. @param baseSaturation Base saturation. */
  BloomSettings(std::string name, float bloomThreshold, float blurAmount,
                float bloomIntensity, float baseIntensity,
                float bloomSaturation, float baseSaturation);
  /** @brief Display name of this preset. */
  std::string Name;
  /** @brief Brightness threshold used to extract bloom pixels. */
  float BloomThreshold;
  /** @brief Width of the Gaussian blur. */
  float BlurAmount;
  /** @brief Intensity of the blurred bloom image. */
  float BloomIntensity;
  /** @brief Intensity of the original base image. */
  float BaseIntensity;
  /** @brief Color saturation of the bloom image. */
  float BloomSaturation;
  /** @brief Color saturation of the base image. */
  float BaseSaturation;
  /** @brief Original NetRumble preset table. */ static std::array<BloomSettings, 2>
      PresetSettings;
};
} // namespace NetRumble
