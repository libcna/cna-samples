// SPDX-License-Identifier: MS-PL
#include "AudioManager.hpp"

#include "Microsoft/Xna/Framework/Audio/InstancePlayLimitException.hpp"
#include "Microsoft/Xna/Framework/Audio/NoAudioHardwareException.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameComponentCollection.hpp"
#include "Microsoft/Xna/Framework/Media/MediaPlayer.hpp"
#include "System/IO/Path.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Audio;
using namespace Microsoft::Xna::Framework::Media;
std::unique_ptr<AudioManager> AudioManager::audioManagerOwner_;
AudioManager *AudioManager::audioManager_ = nullptr;
std::unordered_map<std::string, SoundEffect> AudioManager::soundList_;
std::optional<Song> AudioManager::soundtrack_;

AudioManager::AudioManager(Game &game,
                           const System::IO::DirectoryInfo &directory)
    : GameComponent(game) {
  try {
    for (const auto &file : directory.GetFiles("*.xnb")) {
      const std::string name =
          System::IO::Path::GetFileNameWithoutExtension(file.getNameProperty());
      SoundEffect sound =
          game.getContentProperty().Load<SoundEffect>("Audio/wav/" + name);
      sound.setNameProperty(name);
      soundList_.insert_or_assign(name, std::move(sound));
    }
    soundtrack_.emplace(
        game.getContentProperty().Load<Song>("One Step Beyond"));
  } catch (const NoAudioHardwareException &) {
  }
}

void AudioManager::Initialize(Game &game,
                              const System::IO::DirectoryInfo &directory) {
  audioManagerOwner_ =
      std::unique_ptr<AudioManager>(new AudioManager(game, directory));
  audioManager_ = audioManagerOwner_.get();
  game.getComponentsProperty().Add(audioManager_);
}
void AudioManager::PlaySoundTrack() {
  if (soundtrack_)
    MediaPlayer::Play(&*soundtrack_);
}
void AudioManager::PlaySoundEffect(const std::string &name) {
  if (!audioManager_)
    return;
  const auto it = soundList_.find(name);
  if (it != soundList_.end())
    (void)it->second.Play();
}
void AudioManager::PlaySoundEffect(
    const std::string &name, bool looped,
    std::unique_ptr<SoundEffectInstance> &instance) {
  instance.reset();
  if (!audioManager_)
    return;
  const auto it = soundList_.find(name);
  if (it == soundList_.end())
    return;
  try {
    instance =
        std::make_unique<SoundEffectInstance>(it->second.CreateInstance());
    instance->setIsLoopedProperty(looped);
    instance->Play();
  } catch (const InstancePlayLimitException &) {
    instance.reset();
  }
}
const std::string &AudioManager::GetTypeName() const {
  static const std::string name = "NetRumble.AudioManager";
  return name;
}
} // namespace NetRumble
