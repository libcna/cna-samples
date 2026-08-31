// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <string>
#include <unordered_map>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameComponent.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffectInstance.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundState.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameComponent;
using Microsoft::Xna::Framework::Audio::SoundEffect;
using Microsoft::Xna::Framework::Audio::SoundEffectInstance;
using Microsoft::Xna::Framework::Audio::SoundState;

class AudioManager : public GameComponent {
public:
    static void Initialize(Game& game) {
        audioManager_ = new AudioManager(game);
        game.getComponentsProperty().Add(audioManager_);
    }

    static void LoadSounds() {
        static constexpr std::array<std::array<const char*, 2>, 6> soundNames{{
            {{"CatapultExplosion", "catapultExplosion"}},
            {{"Lose", "gameOver_Lose"}},
            {{"Win", "gameOver_Win"}},
            {{"BoulderHit", "boulderHit"}},
            {{"CatapultFire", "catapultFire"}},
            {{"RopeStretch", "ropeStretch"}},
        }};

        for (const auto& names : soundNames) {
            auto [effect, inserted] = audioManager_->soundEffects_.try_emplace(
                names[1], audioManager_->getGameProperty().getContentProperty().Load<SoundEffect>(
                    std::string("Sounds/") + names[0]));
            (void)inserted;
            audioManager_->soundBank_.emplace(names[1], effect->second.CreateInstance());
        }
    }

    static void PlaySound(const std::string& soundName) {
        const auto sound = audioManager_->soundBank_.find(soundName);
        if (sound != audioManager_->soundBank_.end()) sound->second.Play();
    }

    static void PlaySound(const std::string& soundName, bool isLooped) {
        const auto sound = audioManager_->soundBank_.find(soundName);
        if (sound != audioManager_->soundBank_.end()) {
            if (sound->second.getIsLoopedProperty() != isLooped)
                sound->second.setIsLoopedProperty(isLooped);
            sound->second.Play();
        }
    }

    static void StopSound(const std::string& soundName) {
        const auto sound = audioManager_->soundBank_.find(soundName);
        if (sound != audioManager_->soundBank_.end()) sound->second.Stop();
    }

    static void StopSounds() {
        for (auto& [name, sound] : audioManager_->soundBank_) {
            (void)name;
            if (sound.getStateProperty() != SoundState::Stopped) sound.Stop();
        }
    }

    // This intentionally preserves the original sample's inverted-sounding parameter semantics.
    static void PauseResumeSounds(bool isPause) {
        const SoundState state = isPause ? SoundState::Paused : SoundState::Playing;
        for (auto& [name, sound] : audioManager_->soundBank_) {
            (void)name;
            if (sound.getStateProperty() != state) continue;
            if (isPause)
                sound.Play();
            else
                sound.Pause();
        }
    }

    static void PlayMusic(const std::string& musicSoundName) {
        if (audioManager_->musicSound_ != nullptr) audioManager_->musicSound_->Stop(true);

        const auto sound = audioManager_->soundBank_.find(musicSoundName);
        if (sound != audioManager_->soundBank_.end()) {
            audioManager_->musicSound_ = &sound->second;
            if (!audioManager_->musicSound_->getIsLoopedProperty())
                audioManager_->musicSound_->setIsLoopedProperty(true);
            audioManager_->musicSound_->Play();
        }
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CatapultGame.AudioManager";
        return name;
    }

protected:
    void Dispose(bool disposing) override {
        if (disposing) {
            for (auto& [name, sound] : soundBank_) {
                (void)name;
                sound.Dispose();
            }
            soundBank_.clear();
            soundEffects_.clear();
            musicSound_ = nullptr;
        }
        GameComponent::Dispose(disposing);
    }

private:
    explicit AudioManager(Game& game) : GameComponent(game) {}

    inline static AudioManager* audioManager_ = nullptr;
    std::unordered_map<std::string, SoundEffect> soundEffects_;
    std::unordered_map<std::string, SoundEffectInstance> soundBank_;
    SoundEffectInstance* musicSound_ = nullptr;
};

} // namespace CatapultGame
