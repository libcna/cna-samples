// SPDX-License-Identifier: MS-PL

#include "Sound.hpp"

#include "SpacewarGame.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioEngine.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioStopOptions.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundBank.hpp"
#include "Microsoft/Xna/Framework/Audio/WaveBank.hpp"

#include <array>

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework::Audio;

    std::unique_ptr<AudioEngine> Sound::engine_;
    std::unique_ptr<WaveBank> Sound::waveBank_;
    std::unique_ptr<SoundBank> Sound::soundBank_;

    namespace
    {
        constexpr std::array<const char*, 29> CueNames{
            "tx0_fire", "hyperspace_activate", "hyperspace_return", "pdp3_fire",
            "pdp3_explode", "hax2_fire", "menu_select", "menu_advance", "menu_back",
            "menu_bad_select", "menu_scroll", "menu_select2", "menu_select3",
            "weapon_pickup", "countdown_expire", "countdown_warning", "phase_activate",
            "phase_expire", "accel_player1", "accel_player2", "title_music", "menu_music",
            "points_tally", "explosion_generic", "pdp1_fire", "pdp2_fire", "hax2_impact",
            "damage_ship", "explosion_ship",
        };

        const char* CueName(Sounds sound)
        {
            return CueNames.at(static_cast<std::size_t>(sound));
        }
    }

    std::unique_ptr<Cue> Sound::Play(Sounds sound)
    {
        auto cue = std::unique_ptr<Cue>(soundBank_->GetCue(CueName(sound)));
        cue->Play();
        return cue;
    }

    void Sound::PlayCue(Sounds sound)
    {
        soundBank_->PlayCue(CueName(sound));
    }

    void Sound::Update()
    {
        engine_->Update();
    }

    void Sound::Stop(Cue* cue)
    {
        cue->Stop(AudioStopOptions::Immediate);
    }

    void Sound::Initialize()
    {
        const std::string mediaPath = SpacewarGame::getSettingsProperty().MediaPath;
        engine_ = std::make_unique<AudioEngine>(mediaPath + "audio/spacewar.xgs");
        waveBank_ = std::make_unique<WaveBank>(engine_.get(), mediaPath + "audio/spacewar.xwb");
        soundBank_ = std::make_unique<SoundBank>(engine_.get(), mediaPath + "audio/spacewar.xsb");
    }

    void Sound::Shutdown()
    {
        if (soundBank_) soundBank_->Dispose();
        if (waveBank_) waveBank_->Dispose();
        if (engine_) engine_->Dispose();
        soundBank_.reset();
        waveBank_.reset();
        engine_.reset();
    }
}
