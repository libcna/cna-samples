// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocation.hpp"

namespace Microsoft::Xna::Framework
{
    class GraphicsDeviceManager;
}

namespace Microsoft::Xna::Framework::Audio
{
    class SoundEffect;
    class SoundEffectInstance;
}

namespace Microsoft::Xna::Framework::Media
{
    class Song;
}

namespace SoundAndMusicSample
{
    class Button;
    class UIHelper;

    /** @brief Demonstrates fire-and-forget sounds, sound instances, and songs. */
    class SoundAndMusicSampleGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Latest single touch used by all UI components. */
        std::optional<Microsoft::Xna::Framework::Input::Touch::TouchLocation> touchLocation;

        /** @brief Constructs and configures the phone-oriented sample. */
        SoundAndMusicSampleGame();

        /** @brief Destructor. */
        ~SoundAndMusicSampleGame() override;

        /**
         * @brief Returns the fully-qualified logical type name.
         * @return The type name.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Initializes UI positions and event handlers. */
        void Initialize() override;

        /** @brief Loads the audio and UI content. */
        void LoadContent() override;

        /**
         * @brief Reads gamepad and touch input.
         * @param gameTime Current game timing snapshot.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Draws the static UI and then the button components.
         * @param gameTime Current game timing snapshot.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager> graphics_;
        Button* handleVolumeSong_;
        Button* handleVolumeSound_;
        Button* handlePitchSound_;
        Button* handlePanSound_;

        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> laserSoundEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect> loopedSoundEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance> soundEffectInstance_;
        std::unique_ptr<Microsoft::Xna::Framework::Media::Song> song_;

        std::unique_ptr<UIHelper> uiHelper_;

        void SliderHandlePositionChanged(System::Object* sender, const System::EventArgs& e);
        void ButtonPlayFireForgetTouchDown(System::Object* sender, const System::EventArgs& e);
        void ButtonStopStoredSoundEffectTouchDown(System::Object* sender, const System::EventArgs& e);
        void ButtonPauseStoredSoundEffectTouchDown(System::Object* sender, const System::EventArgs& e);
        void ButtonPlayStoredSoundEffectTouchDown(System::Object* sender, const System::EventArgs& e);
        void ButtonStopSongTouchDown(System::Object* sender, const System::EventArgs& e);
        void ButtonPauseSongTouchDown(System::Object* sender, const System::EventArgs& e);
        void ButtonPlaySongTouchDown(System::Object* sender, const System::EventArgs& e);
    };
}
