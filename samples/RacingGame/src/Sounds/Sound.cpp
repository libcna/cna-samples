// SPDX-License-Identifier: MS-PL

#include "Sounds/Sound.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <thread>

#include "GameLogic/CarPhysics.hpp"
#include "GameLogic/Player.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioCategory.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioEngine.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioStopOptions.hpp"
#include "Microsoft/Xna/Framework/Audio/Cue.hpp"
#include "Microsoft/Xna/Framework/Audio/NoAudioHardwareException.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundBank.hpp"
#include "Microsoft/Xna/Framework/Audio/WaveBank.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "RacingGameManager.hpp"

namespace RacingGame::Sounds
{
    using Microsoft::Xna::Framework::Audio::AudioCategory;
    using Microsoft::Xna::Framework::Audio::AudioEngine;
    using Microsoft::Xna::Framework::Audio::AudioStopOptions;
    using Microsoft::Xna::Framework::Audio::Cue;
    using Microsoft::Xna::Framework::Audio::NoAudioHardwareException;
    using Microsoft::Xna::Framework::Audio::SoundBank;
    using Microsoft::Xna::Framework::Audio::WaveBank;
    using Microsoft::Xna::Framework::MathHelper;

    const std::array<float, Sound::NumberOfGears> Sound::GearVolumes{
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    const std::array<float, Sound::NumberOfGears> Sound::MinimumPitch{
        -0.375f, -0.375f, -0.345f, -0.25f, -0.205f};
    const std::array<float, Sound::NumberOfGears> Sound::MaximumPitch{
        0.24f, 0.17f, 0.17f, 0.145f, 0.10f};

    Sound::Sound(
        RacingGameManager& setGame, const std::string& audioDirectory,
        const float initialSoundVolume, const float initialMusicVolume)
        : game(setGame), soundVolume(initialSoundVolume)
    {
        try
        {
            const std::filesystem::path directory(audioDirectory);
            audioEngine = std::make_unique<AudioEngine>(
                (directory / "RacingGameManager.xgs").string());
            waveBank = std::make_unique<WaveBank>(
                audioEngine.get(), (directory / "Wave Bank.xwb").string());
            soundBank = std::make_unique<SoundBank>(
                audioEngine.get(), (directory / "Sound Bank.xsb").string());
            defaultCategory = std::make_unique<AudioCategory>(
                audioEngine->GetCategory("Default"));
            gearsCategory = std::make_unique<AudioCategory>(
                audioEngine->GetCategory("Gears"));
            musicCategory = std::make_unique<AudioCategory>(
                audioEngine->GetCategory("Music"));
            SetVolumes(initialSoundVolume, initialMusicVolume);
        }
        catch (const NoAudioHardwareException&)
        {
            DisableUnavailableAudio();
        }
    }

    Sound::~Sound()
    {
        StopGearSound();
        soundBank.reset();
        waveBank.reset();
        audioEngine.reset();
    }

    const char* Sound::GetCueName(const SoundCue cue)
    {
        switch (cue)
        {
        case SoundCue::ButtonClick: return "ButtonClick";
        case SoundCue::ScreenClick: return "ScreenClick";
        case SoundCue::ScreenBack: return "ScreenBack";
        case SoundCue::Highlight: return "Highlight";
        case SoundCue::Beep: return "Beep";
        case SoundCue::Bleep: return "Bleep";
        case SoundCue::BrakeCurveMajor: return "BrakeCurveMajor";
        case SoundCue::BrakeCurveMinor: return "BrakeCurveMinor";
        case SoundCue::BrakeMajor: return "BrakeMajor";
        case SoundCue::BrakeMinor: return "BrakeMinor";
        case SoundCue::CarCrashMinor: return "CarCrashMinor";
        case SoundCue::CarCrashTotal: return "CarCrashTotal";
        case SoundCue::CheckpointBetter: return "CheckpointBetter";
        case SoundCue::CheckpointWorse: return "CheckpointWorse";
        case SoundCue::Victory: return "Victory";
        case SoundCue::CarLose: return "CarLose";
        case SoundCue::MenuMusic: return "MenuMusic";
        case SoundCue::GameMusic: return "GameMusic";
        }
        return "";
    }

    void Sound::Play(const std::string& cueName)
    {
        if (!soundBank || !audioHardwareAvailable) return;
        try
        {
            soundBank->PlayCue(cueName);
            ++playRequestCount;
        }
        catch (const NoAudioHardwareException&)
        {
            DisableUnavailableAudio();
        }
    }

    void Sound::Play(const SoundCue cue)
    {
        Play(GetCueName(cue));
    }

    void Sound::StopMusic()
    {
        if (!soundBank || !audioHardwareAvailable) return;
        try
        {
            std::unique_ptr<Cue> musicCue(soundBank->GetCue("MenuMusic"));
            musicCue->Play();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            musicCue->Stop(AudioStopOptions::Immediate);
        }
        catch (const NoAudioHardwareException&)
        {
            DisableUnavailableAudio();
        }
    }

    void Sound::PlayBrakeSound(const GameLogic::BrakeSoundType type)
    {
        if (brakeSoundStillPlayingMilliseconds > 0.0f || game.IsInMenu())
            return;
        switch (type)
        {
        case GameLogic::BrakeSoundType::BrakeMinor:
            Play(SoundCue::BrakeMinor);
            brakeSoundStillPlayingMilliseconds = 750.0f;
            break;
        case GameLogic::BrakeSoundType::BrakeMajor:
            Play(SoundCue::BrakeMajor);
            brakeSoundStillPlayingMilliseconds = 2500.0f;
            break;
        case GameLogic::BrakeSoundType::BrakeCurveMinor:
            Play(SoundCue::BrakeCurveMinor);
            brakeSoundStillPlayingMilliseconds = 1250.0f;
            break;
        case GameLogic::BrakeSoundType::BrakeCurveMajor:
            Play(SoundCue::BrakeCurveMajor);
            brakeSoundStillPlayingMilliseconds = 3500.0f;
            break;
        }
    }

    void Sound::PlayCrashSound(const bool totalCrash)
    {
        if (crashSoundStillPlayingMilliseconds > 0.0f || game.IsInMenu())
            return;
        Play(totalCrash ? SoundCue::CarCrashTotal : SoundCue::CarCrashMinor);
        crashSoundStillPlayingMilliseconds = totalCrash ? 3456.0f : 2345.0f;
    }

    void Sound::PlayCheckpointSound(const GameLogic::CheckpointSoundType type)
    {
        Play(type == GameLogic::CheckpointSoundType::Better
                 ? SoundCue::CheckpointBetter
                 : SoundCue::CheckpointWorse);
    }

    void Sound::PlayPlayerSound(const GameLogic::PlayerSound sound)
    {
        Play(sound == GameLogic::PlayerSound::Victory
                 ? SoundCue::Victory : SoundCue::CarLose);
    }

    void Sound::PlayGearSound(const std::string& cueName)
    {
        if (!soundBank || !audioHardwareAvailable) return;
        try
        {
            if (cueName.find("To") != std::string::npos)
            {
                currentGearChangeCue.reset(soundBank->GetCue(cueName));
                currentGearChangeCue->Play();
                gearChangeSoundInitiatedMilliseconds =
                    static_cast<float>(GearChangeSoundLengthInMs);
                currentGearCue.reset();
            }
            else
            {
                currentGearCue.reset(soundBank->GetCue(cueName));
                currentGearCue->Play();
                currentGearChangeCue.reset();
            }
            ++gearCueStartCount;
        }
        catch (const NoAudioHardwareException&)
        {
            DisableUnavailableAudio();
        }
    }

    void Sound::UpdateGearVolumeAndPitch(
        const std::string& gearCueName, float volume, float pitch)
    {
        if (!audioEngine || !audioHardwareAvailable) return;
        if (gearChangeSoundInitiatedMilliseconds > 0.0f)
        {
            gearChangeSoundInitiatedMilliseconds -=
                game.GetElapsedMilliseconds();
            if (gearChangeSoundInitiatedMilliseconds <= 0.0f)
            {
                gearChangeSoundInitiatedMilliseconds = 0.0f;
                PlayGearSound(gearCueName);
                volume = lastGearVolume = 1.0f;
                pitch = lastGearPitch = -0.3f;
            }
        }
        gearsCategory->SetVolume(MathHelper::Clamp(volume, 0.0f, 1.0f) *
                                 soundVolume);
        if (currentGearCue)
        {
            currentGearCue->SetVariable(
                "Pitch", 55.0f * MathHelper::Clamp(pitch, -1.0f, 1.0f));
        }
    }

    void Sound::StartGearSound()
    {
        currentGear = 0;
        PlayGearSound("Gear1");
        UpdateGearVolumeAndPitch("Gear1", StayingVolume, MinimumPitch[0]);
    }

    void Sound::StopGearSound()
    {
        currentGear = 0;
        if (currentGearChangeCue)
            currentGearChangeCue->Stop(AudioStopOptions::Immediate);
        currentGearChangeCue.reset();
        if (currentGearCue)
            currentGearCue->Stop(AudioStopOptions::Immediate);
        currentGearCue.reset();
        gearChangeSoundInitiatedMilliseconds = 0.0f;
    }

    void Sound::UpdateGearSound(float speed, const float acceleration)
    {
        int newGear = static_cast<int>(
            NumberOfGears * speed / GameLogic::CarPhysics::MaxPossibleSpeed);
        newGear = std::clamp(newGear, 0, NumberOfGears - 1);
        if (gearChangeSoundInitiatedMilliseconds <= 0.0f)
        {
            if (newGear > currentGear)
            {
                PlayGearSound("Gear" + std::to_string(newGear) + "ToGear" +
                              std::to_string(newGear + 1));
                lastGearVolume = 1.0f;
                lastGearPitch = 0.0f;
            }
            else if (newGear < currentGear)
            {
                PlayGearSound("Gear" + std::to_string(newGear + 1));
                lastGearVolume = 1.0f;
                lastGearPitch = MaximumPitch[static_cast<std::size_t>(newGear)];
            }
            currentGear = newGear;
        }

        if (speed < 0.0f)
        {
            speed = MathHelper::Clamp(
                std::abs(speed), 0.0f,
                GameLogic::CarPhysics::MaxPossibleSpeed / NumberOfGears);
        }
        float gearPercentage = static_cast<float>(
            static_cast<int>((speed / GameLogic::CarPhysics::MaxPossibleSpeed) *
                             499.0f) %
            (500 / NumberOfGears)) / 100.0f;
        gearPercentage = MathHelper::Clamp(gearPercentage, 0.0f, 1.0f);
        const float minimumVolume = currentGear > 0
            ? GearVolumes[static_cast<std::size_t>(currentGear - 1)]
            : StayingVolume;
        const float maximumVolume =
            GearVolumes[static_cast<std::size_t>(currentGear)];
        float volume = MathHelper::Lerp(
            minimumVolume, maximumVolume, gearPercentage);
        float pitch = MathHelper::Lerp(
            MinimumPitch[static_cast<std::size_t>(currentGear)],
            MaximumPitch[static_cast<std::size_t>(currentGear)],
            gearPercentage);
        if (gearChangeSoundInitiatedMilliseconds > 0.0f) pitch = 0.0f;
        if (acceleration > 0.25f)
        {
            volume = 1.0f;
        }
        else
        {
            volume /= 1.75f;
            pitch = std::min(-0.025f, pitch / 1.25f);
            if (lastGearPitch > pitch)
                lastGearPitch = lastGearPitch * 0.9f + pitch * 0.1f;
        }
        const float interpolation = 5.0f * game.GetMoveFactorPerSecond();
        lastGearVolume = MathHelper::Lerp(
            lastGearVolume, volume, interpolation);
        lastGearPitch = MathHelper::Lerp(
            lastGearPitch, pitch, interpolation);
        UpdateGearVolumeAndPitch(
            "Gear" + std::to_string(currentGear + 1),
            lastGearVolume, lastGearPitch);
    }

    void Sound::Update()
    {
        if (brakeSoundStillPlayingMilliseconds > 0.0f)
            brakeSoundStillPlayingMilliseconds -= game.GetElapsedMilliseconds();
        if (crashSoundStillPlayingMilliseconds > 0.0f)
            crashSoundStillPlayingMilliseconds -= game.GetElapsedMilliseconds();
        if (audioEngine && audioHardwareAvailable) audioEngine->Update();
    }

    void Sound::SetVolumes(
        const float setSoundVolume, const float setMusicVolume)
    {
        soundVolume = setSoundVolume;
        if (!audioEngine || !audioHardwareAvailable) return;
        defaultCategory->SetVolume(setSoundVolume);
        musicCategory->SetVolume(setMusicVolume);
    }

    bool Sound::getIsInitializedProperty() const
    {
        return audioEngine && waveBank && soundBank && defaultCategory &&
               gearsCategory && musicCategory;
    }

    int Sound::getPlayRequestCountProperty() const
    {
        return playRequestCount;
    }

    int Sound::getGearCueStartCountProperty() const
    {
        return gearCueStartCount;
    }

    void Sound::DisableUnavailableAudio()
    {
        audioHardwareAvailable = false;
        currentGearCue.reset();
        currentGearChangeCue.reset();
    }
}
