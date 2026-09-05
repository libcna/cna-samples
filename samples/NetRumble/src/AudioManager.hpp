// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffectInstance.hpp"
#include "Microsoft/Xna/Framework/GameComponent.hpp"
#include "Microsoft/Xna/Framework/Media/Song.hpp"
#include "System/IO/DirectoryInfo.hpp"

namespace NetRumble {
/** @brief Loads and plays the game's named audio assets. */
class AudioManager final : public Microsoft::Xna::Framework::GameComponent {
public:
  /** @brief Creates and registers the singleton audio component. @param game
   * Owning game. @param audioDirectory Directory containing compiled sounds. */
  static void Initialize(Microsoft::Xna::Framework::Game &game,
                         const System::IO::DirectoryInfo &audioDirectory);
  /** @brief Starts the looping soundtrack. */
  static void PlaySoundTrack();
  /** @brief Plays a fire-and-forget sound by content name. @param soundName
   * Sound name. */
  static void PlaySoundEffect(const std::string &soundName);
  /** @brief Plays a sound instance. @param soundName Sound name. @param looped
   * Loop state. @param instance Receives the instance or null. */
  static void PlaySoundEffect(
      const std::string &soundName, bool looped,
      std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance>
          &instance);
  /** @brief Returns logical runtime type name. @return Type name. */
  CNAEXT [[nodiscard]] const std::string &GetTypeName() const override;

private:
  AudioManager(Microsoft::Xna::Framework::Game &game,
               const System::IO::DirectoryInfo &audioDirectory);
  static std::unique_ptr<AudioManager> audioManagerOwner_;
  static AudioManager *audioManager_;
  static std::unordered_map<std::string,
                            Microsoft::Xna::Framework::Audio::SoundEffect>
      soundList_;
  static std::optional<Microsoft::Xna::Framework::Media::Song> soundtrack_;
};
} // namespace NetRumble
