// SPDX-License-Identifier: MS-PL
#include "BloomPostprocess/BloomSettings.hpp"
#include <utility>
namespace NetRumble {
BloomSettings::BloomSettings(std::string n, float t, float b, float bi,
                             float bai, float bs, float bas)
    : Name(std::move(n)), BloomThreshold(t), BlurAmount(b), BloomIntensity(bi),
      BaseIntensity(bai), BloomSaturation(bs), BaseSaturation(bas) {}
std::array<BloomSettings, 2> BloomSettings::PresetSettings = {
    BloomSettings("NetRumble", .25f, 4, 1.25f, 1, 1, 1),
    BloomSettings("VectorRumble", 0, 2, 3, 1, 2, 1)};
} // namespace NetRumble
