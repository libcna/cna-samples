// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>

#include "Button.hpp"
#include "System/EventArgs.hpp"
#include "System/EventHandler.hpp"

namespace Microsoft::Xna::Framework
{
    class Game;
}

namespace Microsoft::Xna::Framework::Audio
{
    class SoundEffect;
    class SoundEffectInstance;
}

namespace Microsoft::Xna::Framework::Graphics
{
    class GraphicsDevice;
    class SpriteBatch;
    class SpriteFont;
    class Texture2D;
}

namespace Microsoft::Xna::Framework::Media
{
    class Song;
}

namespace SoundAndMusicSample
{
    /** @brief Creates, lays out, loads, and renders the sample's user interface. */
    class UIHelper
    {
    public:
        using EventHandler = System::EventHandler<System::EventArgs>::HandlerType;

        /** @brief Constructs an empty UI helper. */
        UIHelper();

        /** @brief Destructor. */
        ~UIHelper();

        /**
         * @brief Creates the buttons and adds them to the game's component collection.
         * @param game Game that owns the components.
         * @param handleVolumeSong Receives the song-volume slider handle.
         * @param handleVolumeSound Receives the sound-volume slider handle.
         * @param handlePitchSound Receives the sound-pitch slider handle.
         * @param handlePanSound Receives the sound-pan slider handle.
         */
        void CreateUIComponents(
            Microsoft::Xna::Framework::Game& game,
            Button*& handleVolumeSong,
            Button*& handleVolumeSound,
            Button*& handlePitchSound,
            Button*& handlePanSound);

        /**
         * @brief Lays out the controls and attaches their event handlers.
         * @param buttonPlayFireForgetTouchDown Fire-and-forget play handler.
         * @param buttonPlayStoredSoundEffectTouchDown Stored sound play handler.
         * @param buttonPauseStoredSoundEffectTouchDown Stored sound pause handler.
         * @param buttonStopStoredSoundEffectTouchDown Stored sound stop handler.
         * @param sliderHandlePositionChanged Shared slider position handler.
         * @param buttonPlaySongTouchDown Song play handler.
         * @param buttonPauseSongTouchDown Song pause handler.
         * @param buttonStopSongTouchDown Song stop handler.
         */
        void InitializeUIComponents(
            EventHandler buttonPlayFireForgetTouchDown,
            EventHandler buttonPlayStoredSoundEffectTouchDown,
            EventHandler buttonPauseStoredSoundEffectTouchDown,
            EventHandler buttonStopStoredSoundEffectTouchDown,
            EventHandler sliderHandlePositionChanged,
            EventHandler buttonPlaySongTouchDown,
            EventHandler buttonPauseSongTouchDown,
            EventHandler buttonStopSongTouchDown);

        /**
         * @brief Draws the static background, slider strips, and labels.
         * @param graphicsDevice Graphics device whose viewport is filled by the background.
         */
        void RenderUI(Microsoft::Xna::Framework::Graphics::GraphicsDevice& graphicsDevice);

        /**
         * @brief Loads UI and audio assets and creates the looped sound instance.
         * @param game Game whose content manager and graphics device are used.
         * @param laserSoundEffect Receives the fire-and-forget sound.
         * @param loopedSoundEffect Receives the stored looping sound.
         * @param soundEffectInstance Receives the looping sound instance.
         * @param song Receives the background song.
         */
        void LoadAssets(
            Microsoft::Xna::Framework::Game& game,
            std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>& laserSoundEffect,
            std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>& loopedSoundEffect,
            std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance>& soundEffectInstance,
            std::unique_ptr<Microsoft::Xna::Framework::Media::Song>& song);

    private:
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteFont> gameFont_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> background_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> sliderStrip_;

        std::unique_ptr<Button> buttonPlayFireForget_;
        std::unique_ptr<Button> buttonPlayStoredSoundEffect_;
        std::unique_ptr<Button> buttonPlaySong_;
        std::unique_ptr<Button> buttonPauseStoredSoundEffect_;
        std::unique_ptr<Button> buttonPauseSong_;
        std::unique_ptr<Button> buttonStopStoredSoundEffect_;
        std::unique_ptr<Button> buttonStopSong_;
        std::unique_ptr<Button> handleVolumeSong_;
        std::unique_ptr<Button> handleVolumeSound_;
        std::unique_ptr<Button> handlePitchSound_;
        std::unique_ptr<Button> handlePanSound_;
    };
}
