// SPDX-License-Identifier: MS-PL

#include "UIHelper.hpp"

#include <utility>

#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffectInstance.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Media/Song.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace SoundAndMusicSample
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Game;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Audio::SoundEffect;
    using Microsoft::Xna::Framework::Audio::SoundEffectInstance;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteFont;
    using Microsoft::Xna::Framework::Graphics::Texture2D;
    using Microsoft::Xna::Framework::Media::Song;

    UIHelper::UIHelper() = default;
    UIHelper::~UIHelper() = default;

    void UIHelper::CreateUIComponents(
        Game& game,
        Button*& handleVolumeSong,
        Button*& handleVolumeSound,
        Button*& handlePitchSound,
        Button*& handlePanSound)
    {
        auto addButton = [&game](std::unique_ptr<Button>& button, const std::string& assetName)
        {
            button = std::make_unique<Button>(assetName, game);
            game.getComponentsProperty().Add(button.get());
        };

        addButton(buttonPlayFireForget_, R"(Images\playButton)");
        addButton(buttonPlayStoredSoundEffect_, R"(Images\playButton)");
        addButton(buttonPlaySong_, R"(Images\playButton)");
        addButton(buttonPauseStoredSoundEffect_, R"(Images\pauseButton)");
        addButton(buttonPauseSong_, R"(Images\pauseButton)");
        addButton(buttonStopStoredSoundEffect_, R"(Images\stopButton)");
        addButton(buttonStopSong_, R"(Images\stopButton)");
        addButton(handleVolumeSong_, R"(Images\sliderHandle)");
        addButton(handleVolumeSound_, R"(Images\sliderHandle)");
        addButton(handlePitchSound_, R"(Images\sliderHandle)");
        addButton(handlePanSound_, R"(Images\sliderHandle)");

        handleVolumeSong = handleVolumeSong_.get();
        handleVolumeSound = handleVolumeSound_.get();
        handlePitchSound = handlePitchSound_.get();
        handlePanSound = handlePanSound_.get();
    }

    void UIHelper::InitializeUIComponents(
        EventHandler buttonPlayFireForgetTouchDown,
        EventHandler buttonPlayStoredSoundEffectTouchDown,
        EventHandler buttonPauseStoredSoundEffectTouchDown,
        EventHandler buttonStopStoredSoundEffectTouchDown,
        EventHandler sliderHandlePositionChanged,
        EventHandler buttonPlaySongTouchDown,
        EventHandler buttonPauseSongTouchDown,
        EventHandler buttonStopSongTouchDown)
    {
        buttonPlayFireForget_->setPositionOriginProperty(buttonPlayFireForget_->getTextureCenterProperty());
        buttonPlayFireForget_->setPositionOfOriginProperty(Vector2(239.0f, 112.0f));
        buttonPlayFireForget_->TouchDown += std::move(buttonPlayFireForgetTouchDown);

        buttonPlayStoredSoundEffect_->setPositionOriginProperty(
            buttonPlayStoredSoundEffect_->getTextureCenterProperty());
        buttonPlayStoredSoundEffect_->setPositionOfOriginProperty(Vector2(152.0f, 286.0f));
        buttonPlayStoredSoundEffect_->TouchDown += std::move(buttonPlayStoredSoundEffectTouchDown);

        buttonPauseStoredSoundEffect_->setPositionOriginProperty(
            buttonPauseStoredSoundEffect_->getTextureCenterProperty());
        buttonPauseStoredSoundEffect_->setPositionOfOriginProperty(Vector2(240.0f, 286.0f));
        buttonPauseStoredSoundEffect_->TouchDown += std::move(buttonPauseStoredSoundEffectTouchDown);

        buttonStopStoredSoundEffect_->setPositionOriginProperty(
            buttonStopStoredSoundEffect_->getTextureCenterProperty());
        buttonStopStoredSoundEffect_->setPositionOfOriginProperty(Vector2(327.0f, 286.0f));
        buttonStopStoredSoundEffect_->TouchDown += std::move(buttonStopStoredSoundEffectTouchDown);

        handlePitchSound_->setPositionOriginProperty(handlePitchSound_->getTextureCenterProperty());
        handlePitchSound_->setPositionOfOriginProperty(Vector2(280.0f, 434.0f));
        handlePitchSound_->setDragRestrictionsProperty(Rectangle(120, 434, 300, 0));
        handlePitchSound_->PositionChanged += sliderHandlePositionChanged;
        handlePitchSound_->setAllowDragProperty(true);

        handleVolumeSound_->setPositionOriginProperty(handleVolumeSound_->getTextureCenterProperty());
        handleVolumeSound_->setPositionOfOriginProperty(Vector2(360.0f, 505.0f));
        handleVolumeSound_->setDragRestrictionsProperty(Rectangle(120, 505, 300, 0));
        handleVolumeSound_->PositionChanged += sliderHandlePositionChanged;
        handleVolumeSound_->setAllowDragProperty(true);

        handlePanSound_->setPositionOriginProperty(handlePanSound_->getTextureCenterProperty());
        handlePanSound_->setPositionOfOriginProperty(Vector2(280.0f, 364.0f));
        handlePanSound_->setDragRestrictionsProperty(Rectangle(120, 364, 300, 0));
        handlePanSound_->PositionChanged += sliderHandlePositionChanged;
        handlePanSound_->setAllowDragProperty(true);

        buttonPlaySong_->setPositionOriginProperty(buttonPlaySong_->getTextureCenterProperty());
        buttonPlaySong_->setPositionOfOriginProperty(Vector2(112.0f, 660.0f));
        buttonPlaySong_->TouchDown += std::move(buttonPlaySongTouchDown);

        buttonPauseSong_->setPositionOriginProperty(buttonPauseSong_->getTextureCenterProperty());
        buttonPauseSong_->setPositionOfOriginProperty(Vector2(240.0f, 660.0f));
        buttonPauseSong_->TouchDown += std::move(buttonPauseSongTouchDown);

        buttonStopSong_->setPositionOriginProperty(buttonStopSong_->getTextureCenterProperty());
        buttonStopSong_->setPositionOfOriginProperty(Vector2(367.0f, 660.0f));
        buttonStopSong_->TouchDown += std::move(buttonStopSongTouchDown);

        handleVolumeSong_->setPositionOriginProperty(handleVolumeSong_->getTextureCenterProperty());
        handleVolumeSong_->setPositionOfOriginProperty(Vector2(300.0f, 743.0f));
        handleVolumeSong_->setDragRestrictionsProperty(Rectangle(120, 743, 300, 0));
        handleVolumeSong_->PositionChanged += std::move(sliderHandlePositionChanged);
        handleVolumeSong_->setAllowDragProperty(true);
    }

    void UIHelper::RenderUI(GraphicsDevice& graphicsDevice)
    {
        spriteBatch_->Begin();

        spriteBatch_->Draw(
            *background_, graphicsDevice.getViewportProperty().getBoundsProperty(), Color::White);

        spriteBatch_->Draw(*sliderStrip_, Vector2(96.0f, 364.0f), Color::White);
        spriteBatch_->Draw(*sliderStrip_, Vector2(96.0f, 434.0f), Color::White);
        spriteBatch_->Draw(*sliderStrip_, Vector2(96.0f, 504.0f), Color::White);
        spriteBatch_->Draw(*sliderStrip_, Vector2(96.0f, 742.0f), Color::White);

        const Color blue(0, 168, 255);
        spriteBatch_->DrawString(
            *gameFont_, "Fire and Forget SoundEffect", Vector2(94.0f, 6.0f), blue);
        spriteBatch_->DrawString(
            *gameFont_, "Stored SoundEffectInstance", Vector2(96.0f, 189.0f), blue);
        spriteBatch_->DrawString(*gameFont_, "Song", Vector2(213.0f, 565.0f), blue);

        spriteBatch_->DrawString(*gameFont_, "Pan", Vector2(50.0f, 346.0f), Color::White);
        spriteBatch_->DrawString(*gameFont_, "Pitch", Vector2(37.0f, 416.0f), Color::White);
        spriteBatch_->DrawString(*gameFont_, "Volume", Vector2(8.0f, 486.0f), Color::White);
        spriteBatch_->DrawString(*gameFont_, "Volume", Vector2(8.0f, 725.0f), Color::White);

        spriteBatch_->End();
    }

    void UIHelper::LoadAssets(
        Game& game,
        std::unique_ptr<SoundEffect>& laserSoundEffect,
        std::unique_ptr<SoundEffect>& loopedSoundEffect,
        std::unique_ptr<SoundEffectInstance>& soundEffectInstance,
        std::unique_ptr<Song>& song)
    {
        spriteBatch_ = std::make_unique<SpriteBatch>(game.getGraphicsDeviceProperty());

        auto& content = game.getContentProperty();
        gameFont_ = std::make_unique<SpriteFont>(content.Load<SpriteFont>(R"(Fonts\GameFont)"));
        background_ = std::make_unique<Texture2D>(content.Load<Texture2D>(R"(Images\bg)"));
        sliderStrip_ = std::make_unique<Texture2D>(content.Load<Texture2D>(R"(Images\sliderStrip)"));

        song = std::make_unique<Song>(content.Load<Song>(R"(Sounds\Music)"));
        laserSoundEffect = std::make_unique<SoundEffect>(
            content.Load<SoundEffect>(R"(Sounds\Laser)"));
        loopedSoundEffect = std::make_unique<SoundEffect>(
            content.Load<SoundEffect>(R"(Sounds\EngineLoop)"));
        soundEffectInstance = std::make_unique<SoundEffectInstance>(
            loopedSoundEffect->CreateInstance());
        soundEffectInstance->setIsLoopedProperty(true);
    }
}
