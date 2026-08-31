// SPDX-License-Identifier: MS-PL
#pragma once

// AudioManager.hpp -- C++ port of Misc/AudioManager.cs (XNA 4.0
// CardsStarterKit sample). Static-singleton component managing sound/music
// playback. Follows the same pattern already established for NinjAcademy's
// AudioManager.hpp in this repo.
//
// LoadMusic()/PlayMusic() are ported faithfully but unreachable: the C#
// original's BlackjackGame.LoadContent() only ever calls LoadSounds(), never
// LoadMusic() -- and no music WAV/OGG assets ship in BlackjackHiDefContent
// either, so this is dead code in the original sample itself, not something
// introduced or fixed by this port.

#include <string>
#include <memory>
#include <unordered_map>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameComponent.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffectInstance.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundState.hpp"
#include "Microsoft/Xna/Framework/Media/MediaPlayer.hpp"
#include "Microsoft/Xna/Framework/Media/MediaState.hpp"
#include "Microsoft/Xna/Framework/Media/Song.hpp"
#include "CNA/CNAHelper.hpp"

#include "BlackjackCommon.hpp"

namespace Blackjack {

using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameComponent;
using Microsoft::Xna::Framework::Audio::SoundEffect;
using Microsoft::Xna::Framework::Audio::SoundEffectInstance;
using Microsoft::Xna::Framework::Audio::SoundState;
using Microsoft::Xna::Framework::Media::MediaPlayer;
using Microsoft::Xna::Framework::Media::MediaState;
using Microsoft::Xna::Framework::Media::Song;

class AudioManager : public GameComponent {
public:
    static AudioManager* Instance() { return instance_.get(); }

    static void Initialize(Game& game) {
        instance_.reset(new AudioManager(game));
        game.getComponentsProperty().Add(instance_.get());
    }

    static void Shutdown(Game& game) {
        if (!instance_)
            return;
        (void)game.getComponentsProperty().Remove(instance_.get());
        instance_->GameComponent::Dispose();
        instance_.reset();
    }

    static void LoadSound(const std::string& contentName, const std::string& alias) {
        SoundEffect soundEffect =
            instance_->getGameProperty().getContentProperty().Load<SoundEffect>(SoundAssetLocation + contentName);
        SoundEffectInstance soundEffectInstance = soundEffect.CreateInstance();
        if (instance_->soundBank_.find(alias) == instance_->soundBank_.end())
            instance_->soundBank_.emplace(alias, std::move(soundEffectInstance));
    }

    static void LoadSong(const std::string& contentName, const std::string& alias) {
        Song song = instance_->getGameProperty().getContentProperty().Load<Song>(SoundAssetLocation + contentName);
        if (instance_->musicBank_.find(alias) == instance_->musicBank_.end())
            instance_->musicBank_.emplace(alias, song);
    }

    static void LoadSounds() {
        LoadSound("Bet", "Bet");
        LoadSound("CardFlip", "Flip");
        LoadSound("CardsShuffle", "Shuffle");
        LoadSound("Deal", "Deal");
    }

    // See file header: never actually called by BlackjackGame, kept for
    // parity with the original (which is equally unreachable there).
    static void LoadMusic() {
        LoadSong("InGameSong_Loop", "InGameSong_Loop");
        LoadSong("MenuMusic_Loop", "MenuMusic_Loop");
    }

    SoundEffectInstance* operator[](const std::string& soundName) {
        auto it = soundBank_.find(soundName);
        return it == soundBank_.end() ? nullptr : &it->second;
    }

    static void PlaySound(const std::string& soundName) {
        auto it = instance_->soundBank_.find(soundName);
        if (it != instance_->soundBank_.end())
            it->second.Play();
    }

    static void PlaySound(const std::string& soundName, bool isLooped) {
        auto it = instance_->soundBank_.find(soundName);
        if (it == instance_->soundBank_.end())
            return;
        if (it->second.getIsLoopedProperty() != isLooped)
            it->second.setIsLoopedProperty(isLooped);
        it->second.Play();
    }

    static void PlaySound(const std::string& soundName, bool isLooped, float volume) {
        auto it = instance_->soundBank_.find(soundName);
        if (it == instance_->soundBank_.end())
            return;
        if (it->second.getIsLoopedProperty() != isLooped)
            it->second.setIsLoopedProperty(isLooped);
        it->second.setVolumeProperty(volume);
        it->second.Play();
    }

    static void StopSound(const std::string& soundName) {
        auto it = instance_->soundBank_.find(soundName);
        if (it != instance_->soundBank_.end())
            it->second.Stop();
    }

    static void StopSounds() {
        for (auto& [name, sound] : instance_->soundBank_) {
            if (sound.getStateProperty() != SoundState::Stopped)
                sound.Stop();
        }
    }

    static void PauseResumeSounds(bool resumeSounds) {
        SoundState state = resumeSounds ? SoundState::Paused : SoundState::Playing;
        for (auto& [name, sound] : instance_->soundBank_) {
            if (sound.getStateProperty() != state)
                continue;
            if (resumeSounds)
                sound.Resume();
            else
                sound.Pause();
        }
    }

    static void PlayMusic(const std::string& musicSoundName) {
        auto it = instance_->musicBank_.find(musicSoundName);
        if (it != instance_->musicBank_.end()) {
            if (MediaPlayer::getStateProperty() != MediaState::Stopped)
                MediaPlayer::Stop();
            MediaPlayer::setIsRepeatingProperty(true);
            MediaPlayer::Play(&it->second);
        }
    }

    static void StopMusic() {
        if (MediaPlayer::getStateProperty() != MediaState::Stopped)
            MediaPlayer::Stop();
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "Blackjack.AudioManager";
        return name;
    }

private:
    static constexpr const char* SoundAssetLocation = "Sounds/";

    explicit AudioManager(Game& game) : GameComponent(game) {}

    static std::unique_ptr<AudioManager> instance_;

    std::unordered_map<std::string, SoundEffectInstance> soundBank_;
    std::unordered_map<std::string, Song> musicBank_;

protected:
    void Dispose(bool disposing) override {
        if (disposing) {
            soundBank_.clear();
        }
        GameComponent::Dispose(disposing);
    }
};

inline std::unique_ptr<AudioManager> AudioManager::instance_;

} // namespace Blackjack
