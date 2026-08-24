// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Audio/Cue.hpp"

#include <memory>

namespace Microsoft::Xna::Framework::Audio
{
    class AudioEngine;
    class WaveBank;
    class SoundBank;
}

namespace Spacewar
{
    enum class Sounds
    {
        PeashooterFire,
        HyperspaceActivate,
        HyperspaceReturn,
        RocketFire,
        RocketExplode,
        BFGFire,
        MenuSelect,
        MenuAdvance,
        MenuBack,
        MenuBadSelect,
        MenuScroll,
        MenuSelect2,
        MenuSelect3,
        WeaponPickup,
        CountDownExpire,
        CountDownWarning,
        PhaseActivate,
        PhaseExpire,
        ThrustPlayer1,
        ThrustPlayer2,
        TitleMusic,
        MenuMusic,
        PointsTally,
        Explosion,
        MachineGunFire,
        DoubleMachineGunFire,
        BFGExplode,
        DamageShip,
        ExplodeShip,
    };

    class Sound
    {
    public:
        [[nodiscard]] static std::unique_ptr<Microsoft::Xna::Framework::Audio::Cue> Play(Sounds sound);
        static void PlayCue(Sounds sound);
        static void Update();
        static void Stop(Microsoft::Xna::Framework::Audio::Cue* cue);
        static void Initialize();
        static void Shutdown();

    private:
        static std::unique_ptr<Microsoft::Xna::Framework::Audio::AudioEngine> engine_;
        static std::unique_ptr<Microsoft::Xna::Framework::Audio::WaveBank> waveBank_;
        static std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundBank> soundBank_;
    };
}
