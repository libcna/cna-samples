#pragma once

// AudioManager.hpp -- C++ port of AudioManager.cs.
//
// The original wraps XACT (AudioEngine/SoundBank/WaveBank/Cue) and loads the compiled
// .xgs/.xwb/.xsb project shipped beside the game's content; CNA implements that same API, and
// this sample's Content/Audio carries the official compiled banks, so this is the original's
// component rather than a substitute. GameComponent registration is the original's too.

#include <memory>
#include <stack>
#include <string>

#include "Microsoft/Xna/Framework/Audio/AudioEngine.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioStopOptions.hpp"
#include "Microsoft/Xna/Framework/Audio/Cue.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundBank.hpp"
#include "Microsoft/Xna/Framework/Audio/WaveBank.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameComponent.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameComponent;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Audio::AudioEngine;
using Microsoft::Xna::Framework::Audio::AudioStopOptions;
using Microsoft::Xna::Framework::Audio::Cue;
using Microsoft::Xna::Framework::Audio::SoundBank;
using Microsoft::Xna::Framework::Audio::WaveBank;

// Component that manages audio playback for all cues.
class AudioManager : public GameComponent {
public:
    // Declared, not defaulted, because the singleton below is a unique_ptr<AudioManager> held
    // inside AudioManager itself: its deleter needs the complete type, which only exists after
    // the class. Defined at the bottom of this header.
    ~AudioManager() override;

    // Initialize the static AudioManager functionality.
    static void Initialize(Game& game, const std::string& settingsFile,
                           const std::string& waveBankFile, const std::string& soundBankFile) {
        audioManager_ = std::unique_ptr<AudioManager>(
            new AudioManager(game, settingsFile, waveBankFile, soundBankFile));
        game.getComponentsProperty().Add(audioManager_.get());
    }

    // Retrieve a cue by name.
    static Cue* GetCue(const std::string& cueName) {
        if (cueName.empty() || audioManager_ == nullptr ||
            audioManager_->audioEngine_ == nullptr || audioManager_->soundBank_ == nullptr ||
            audioManager_->waveBank_ == nullptr) {
            return nullptr;
        }
        return audioManager_->soundBank_->GetCue(cueName);
    }

    // Plays a cue by name.
    static void PlayCue(const std::string& cueName) {
        if (audioManager_ != nullptr && audioManager_->audioEngine_ != nullptr &&
            audioManager_->soundBank_ != nullptr && audioManager_->waveBank_ != nullptr) {
            audioManager_->soundBank_->PlayCue(cueName);
        }
    }

    // Plays the desired music, clearing the stack of music cues.
    static void PlayMusic(const std::string& cueName) {
        // start the new music cue
        if (audioManager_ != nullptr) {
            audioManager_->musicCueNameStack_ = {};
            PushMusic(cueName);
        }
    }

    // Plays the music for this game, adding it to the music stack.
    static void PushMusic(const std::string& cueName) {
        // start the new music cue
        if (audioManager_ != nullptr && audioManager_->audioEngine_ != nullptr &&
            audioManager_->soundBank_ != nullptr && audioManager_->waveBank_ != nullptr) {
            audioManager_->musicCueNameStack_.push(cueName);
            if (audioManager_->musicCue_ == nullptr ||
                audioManager_->musicCue_->getNameProperty() != cueName) {
                if (audioManager_->musicCue_ != nullptr) {
                    audioManager_->musicCue_->Stop(AudioStopOptions::AsAuthored);
                    audioManager_->musicCue_->Dispose();
                    audioManager_->musicCue_ = nullptr;
                }
                audioManager_->musicCue_ = GetCue(cueName);
                if (audioManager_->musicCue_ != nullptr) {
                    audioManager_->musicCue_->Play();
                }
            }
        }
    }

    // Stops the current music and plays the previous music on the stack.
    static void PopMusic() {
        // start the new music cue
        if (audioManager_ != nullptr && audioManager_->audioEngine_ != nullptr &&
            audioManager_->soundBank_ != nullptr && audioManager_->waveBank_ != nullptr) {
            std::string cueName;
            if (!audioManager_->musicCueNameStack_.empty()) {
                audioManager_->musicCueNameStack_.pop();
                if (!audioManager_->musicCueNameStack_.empty()) {
                    cueName = audioManager_->musicCueNameStack_.top();
                }
            }
            if (audioManager_->musicCue_ == nullptr ||
                audioManager_->musicCue_->getNameProperty() != cueName) {
                if (audioManager_->musicCue_ != nullptr) {
                    audioManager_->musicCue_->Stop(AudioStopOptions::AsAuthored);
                    audioManager_->musicCue_->Dispose();
                    audioManager_->musicCue_ = nullptr;
                }
                if (!cueName.empty()) {
                    audioManager_->musicCue_ = GetCue(cueName);
                    if (audioManager_->musicCue_ != nullptr) {
                        audioManager_->musicCue_->Play();
                    }
                }
            }
        }
    }

    // Stop music playback, clearing the cue.
    static void StopMusic() {
        if (audioManager_ != nullptr) {
            audioManager_->musicCueNameStack_ = {};
            if (audioManager_->musicCue_ != nullptr) {
                audioManager_->musicCue_->Stop(AudioStopOptions::AsAuthored);
                audioManager_->musicCue_->Dispose();
                audioManager_->musicCue_ = nullptr;
            }
        }
    }

    // Update the audio manager, particularly the engine.
    void Update(GameTime& gameTime) override {
        // update the audio engine
        if (audioEngine_ != nullptr) {
            audioEngine_->Update();
        }

        GameComponent::Update(gameTime);
    }

private:
    // Constructs the manager for audio playback of all cues.
    AudioManager(Game& game, const std::string& settingsFile, const std::string& waveBankFile,
                 const std::string& soundBankFile)
        : GameComponent(game) {
        try {
            audioEngine_ = std::make_unique<AudioEngine>(settingsFile);
            waveBank_ = std::make_unique<WaveBank>(audioEngine_.get(), waveBankFile);
            soundBank_ = std::make_unique<SoundBank>(audioEngine_.get(), soundBankFile);
        } catch (const std::exception&) {
            // silently fall back to silence
            audioEngine_.reset();
            waveBank_.reset();
            soundBank_.reset();
        }
    }

    // The singleton for this type. Defined after the class: a static inline unique_ptr<Self>
    // declared inside the class instantiates its deleter where Self is still incomplete.
    static std::unique_ptr<AudioManager> audioManager_;

    // The audio engine used to play all cues.
    std::unique_ptr<AudioEngine> audioEngine_;

    // The soundbank that contains all cues.
    std::unique_ptr<SoundBank> soundBank_;

    // The wavebank with all wave files for this game.
    std::unique_ptr<WaveBank> waveBank_;

    // The cue for the music currently playing, if any.
    Cue* musicCue_ = nullptr;

    // Stack of music cue names, for layered music playback.
    std::stack<std::string> musicCueNameStack_;
};

inline AudioManager::~AudioManager() = default;

inline std::unique_ptr<AudioManager> AudioManager::audioManager_;

} // namespace RolePlaying
