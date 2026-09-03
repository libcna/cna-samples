// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>
#include <memory>
#include <string>

namespace Microsoft::Xna::Framework::Audio
{
    class AudioEngine;
    struct AudioCategory;
    class Cue;
    class SoundBank;
    class WaveBank;
}

namespace RacingGame
{
    class RacingGameManager;
}

namespace RacingGame::GameLogic
{
    enum class BrakeSoundType;
    enum class CheckpointSoundType;
    enum class PlayerSound;
}

namespace RacingGame::Sounds
{
    /** @brief Identifies every original Racing cue other than gear loops. */
    enum class SoundCue
    {
        ButtonClick,
        ScreenClick,
        ScreenBack,
        Highlight,
        Beep,
        Bleep,
        BrakeCurveMajor,
        BrakeCurveMinor,
        BrakeMajor,
        BrakeMinor,
        CarCrashMinor,
        CarCrashTotal,
        CheckpointBetter,
        CheckpointWorse,
        Victory,
        CarLose,
        MenuMusic,
        GameMusic,
    };

    /** @brief Owns and updates the authentic XACT audio graph used by Racing. */
    class Sound final
    {
    public:
        /**
         * @brief Loads the original XGS, XWB and XSB products.
         * @param game Running Racing game used for timing and menu state.
         * @param audioDirectory Directory containing the authentic audio products.
         * @param soundVolume Initial effects and engine volume.
         * @param musicVolume Initial music volume.
         */
        Sound(RacingGameManager& game, const std::string& audioDirectory,
              float soundVolume, float musicVolume);
        /** @brief Releases all active cues and XACT banks. */
        ~Sound();

        Sound(const Sound&) = delete;
        Sound& operator=(const Sound&) = delete;

        /** @brief Plays one original named cue. */
        void Play(SoundCue cue);
        /** @brief Stops all music immediately through the original replacement-cue technique. */
        void StopMusic();
        /** @brief Plays a rate-limited original braking cue. */
        void PlayBrakeSound(GameLogic::BrakeSoundType type);
        /** @brief Plays a rate-limited original crash cue. */
        void PlayCrashSound(bool totalCrash);
        /** @brief Plays the original checkpoint comparison cue. */
        void PlayCheckpointSound(GameLogic::CheckpointSoundType type);
        /** @brief Plays the original victory or loss cue. */
        void PlayPlayerSound(GameLogic::PlayerSound sound);
        /** @brief Starts the first looping engine-gear cue. */
        void StartGearSound();
        /** @brief Stops all current gear and transition cues immediately. */
        void StopGearSound();
        /** @brief Updates the engine gear, volume and Pitch XACT variable. */
        void UpdateGearSound(float speed, float acceleration);
        /** @brief Advances cue lifetime and sound cooldowns by one game frame. */
        void Update();
        /** @brief Applies effects and music category volumes. */
        void SetVolumes(float soundVolume, float musicVolume);

        /** @brief Gets whether all three authentic XACT products loaded. */
        [[nodiscard]] bool getIsInitializedProperty() const;
        /** @brief Gets the number of accepted named cue play requests. */
        [[nodiscard]] int getPlayRequestCountProperty() const;
        /** @brief Gets the number of started gear or gear-transition cues. */
        [[nodiscard]] int getGearCueStartCountProperty() const;

    private:
        static constexpr int NumberOfGears = 5;
        static constexpr int GearChangeSoundLengthInMs = 1200;
        static constexpr float StayingVolume = 0.5f;

        RacingGameManager& game;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::AudioEngine>
            audioEngine;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::WaveBank> waveBank;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundBank> soundBank;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::AudioCategory>
            defaultCategory;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::AudioCategory>
            gearsCategory;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::AudioCategory>
            musicCategory;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::Cue> currentGearCue;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::Cue>
            currentGearChangeCue;
        float soundVolume = 0.8f;
        float brakeSoundStillPlayingMilliseconds = 1000.0f;
        float crashSoundStillPlayingMilliseconds = 2000.0f;
        float gearChangeSoundInitiatedMilliseconds = 0.0f;
        float lastGearVolume = StayingVolume;
        float lastGearPitch = 0.0f;
        int currentGear = 0;
        int playRequestCount = 0;
        int gearCueStartCount = 0;
        bool audioHardwareAvailable = true;

        static const std::array<float, NumberOfGears> GearVolumes;
        static const std::array<float, NumberOfGears> MinimumPitch;
        static const std::array<float, NumberOfGears> MaximumPitch;

        static const char* GetCueName(SoundCue cue);
        void Play(const std::string& cueName);
        void PlayGearSound(const std::string& cueName);
        void UpdateGearVolumeAndPitch(
            const std::string& gearCueName, float volume, float pitch);
        void DisableUnavailableAudio();
    };
}
