// SPDX-License-Identifier: MS-PL

#include "SoundAndMusicSampleGame.hpp"

#include "Button.hpp"
#include "Microsoft/Devices/DeviceType.hpp"
#include "Microsoft/Devices/Environment.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffectInstance.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundState.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Media/MediaPlayer.hpp"
#include "Microsoft/Xna/Framework/Media/MediaState.hpp"
#include "Microsoft/Xna/Framework/Media/Song.hpp"
#include "System/TimeSpan.hpp"
#include "UIHelper.hpp"

namespace SoundAndMusicSample
{
    using Microsoft::Devices::DeviceType;
    using Microsoft::Devices::Environment;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Audio::SoundState;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Touch::TouchCollection;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;
    using Microsoft::Xna::Framework::Media::MediaPlayer;
    using Microsoft::Xna::Framework::Media::MediaState;

    SoundAndMusicSampleGame::SoundAndMusicSampleGame()
        : touchLocation(),
          graphics_(std::make_unique<GraphicsDeviceManager>(this)),
          handleVolumeSong_(nullptr),
          handleVolumeSound_(nullptr),
          handlePitchSound_(nullptr),
          handlePanSound_(nullptr),
          laserSoundEffect_(),
          loopedSoundEffect_(),
          soundEffectInstance_(),
          song_(),
          uiHelper_(std::make_unique<UIHelper>())
    {
        getContentProperty().setRootDirectoryProperty("Content");
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));

        graphics_->setIsFullScreenProperty(true);
        graphics_->setPreferredBackBufferWidthProperty(480);
        graphics_->setPreferredBackBufferHeightProperty(800);

        // CNAEXT — owner-approved pointer support for this otherwise touch-only Phone sample.
        // Mouse input enters the unchanged TouchPanel path; see ../diff.md.
        CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true);

        uiHelper_->CreateUIComponents(
            *this,
            handleVolumeSong_,
            handleVolumeSound_,
            handlePitchSound_,
            handlePanSound_);
    }

    SoundAndMusicSampleGame::~SoundAndMusicSampleGame() = default;

    void SoundAndMusicSampleGame::Initialize()
    {
        Game::Initialize();

        uiHelper_->InitializeUIComponents(
            [this](System::Object* sender, const System::EventArgs& e)
            {
                ButtonPlayFireForgetTouchDown(sender, e);
            },
            [this](System::Object* sender, const System::EventArgs& e)
            {
                ButtonPlayStoredSoundEffectTouchDown(sender, e);
            },
            [this](System::Object* sender, const System::EventArgs& e)
            {
                ButtonPauseStoredSoundEffectTouchDown(sender, e);
            },
            [this](System::Object* sender, const System::EventArgs& e)
            {
                ButtonStopStoredSoundEffectTouchDown(sender, e);
            },
            [this](System::Object* sender, const System::EventArgs& e)
            {
                SliderHandlePositionChanged(sender, e);
            },
            [this](System::Object* sender, const System::EventArgs& e)
            {
                ButtonPlaySongTouchDown(sender, e);
            },
            [this](System::Object* sender, const System::EventArgs& e)
            {
                ButtonPauseSongTouchDown(sender, e);
            },
            [this](System::Object* sender, const System::EventArgs& e)
            {
                ButtonStopSongTouchDown(sender, e);
            });
    }

    void SoundAndMusicSampleGame::LoadContent()
    {
        uiHelper_->LoadAssets(
            *this,
            laserSoundEffect_,
            loopedSoundEffect_,
            soundEffectInstance_,
            song_);
        Game::LoadContent();
    }

    void SoundAndMusicSampleGame::Update(GameTime& gameTime)
    {
        if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBackProperty() ==
            ButtonState::Pressed)
        {
            Exit();
        }

        const TouchCollection touches = TouchPanel::GetState();
        if (touches.getCountProperty() == 1)
        {
            touchLocation = touches[0];
        }
        else
        {
            touchLocation = std::nullopt;
        }

        Game::Update(gameTime);
    }

    void SoundAndMusicSampleGame::Draw(const GameTime& gameTime)
    {
        uiHelper_->RenderUI(getGraphicsDeviceProperty());
        Game::Draw(gameTime);
    }

    const std::string& SoundAndMusicSampleGame::GetTypeName() const
    {
        static const std::string typeName = "SoundAndMusicSample.SoundAndMusicSampleGame";
        return typeName;
    }

    void SoundAndMusicSampleGame::SliderHandlePositionChanged(
        System::Object* sender,
        const System::EventArgs& e)
    {
        (void)e;
        auto* handle = dynamic_cast<Button*>(sender);
        float scaledValue =
            (handle->getPositionOfOriginProperty().X -
             static_cast<float>(handle->getDragRestrictionsProperty().getLeftProperty())) /
            static_cast<float>(handle->getDragRestrictionsProperty().Width);

        if (handle == handlePanSound_)
        {
            scaledValue = (scaledValue - 0.5f) * 2.0f;
            soundEffectInstance_->setPanProperty(scaledValue);
        }
        else if (handle == handlePitchSound_)
        {
            scaledValue = (scaledValue - 0.5f) * 2.0f;
            soundEffectInstance_->setPitchProperty(scaledValue);
        }
        else if (handle == handleVolumeSound_)
        {
            soundEffectInstance_->setVolumeProperty(scaledValue);
        }
        else if (handle == handleVolumeSong_)
        {
            if (Environment::getDeviceTypeProperty() == DeviceType::Device)
            {
                MediaPlayer::setVolumeProperty(scaledValue);
            }
            else
            {
                MediaPlayer::setVolumeProperty(MathHelper::Clamp(scaledValue, 0.000001f, 1.0f));
            }
        }
    }

    void SoundAndMusicSampleGame::ButtonPlayFireForgetTouchDown(
        System::Object* sender,
        const System::EventArgs& e)
    {
        (void)sender;
        (void)e;
        laserSoundEffect_->Play();
    }

    void SoundAndMusicSampleGame::ButtonStopStoredSoundEffectTouchDown(
        System::Object* sender,
        const System::EventArgs& e)
    {
        (void)sender;
        (void)e;
        if (soundEffectInstance_->getStateProperty() != SoundState::Stopped)
        {
            soundEffectInstance_->Stop();
        }
    }

    void SoundAndMusicSampleGame::ButtonPauseStoredSoundEffectTouchDown(
        System::Object* sender,
        const System::EventArgs& e)
    {
        (void)sender;
        (void)e;
        if (soundEffectInstance_->getStateProperty() == SoundState::Playing)
        {
            soundEffectInstance_->Pause();
        }
    }

    void SoundAndMusicSampleGame::ButtonPlayStoredSoundEffectTouchDown(
        System::Object* sender,
        const System::EventArgs& e)
    {
        (void)sender;
        (void)e;
        if (soundEffectInstance_->getStateProperty() == SoundState::Paused)
        {
            soundEffectInstance_->Resume();
        }
        else if (soundEffectInstance_->getStateProperty() == SoundState::Stopped)
        {
            soundEffectInstance_->Play();
        }
    }

    void SoundAndMusicSampleGame::ButtonStopSongTouchDown(
        System::Object* sender,
        const System::EventArgs& e)
    {
        (void)sender;
        (void)e;
        if (MediaPlayer::getStateProperty() != MediaState::Stopped)
        {
            MediaPlayer::Stop();
        }
    }

    void SoundAndMusicSampleGame::ButtonPauseSongTouchDown(
        System::Object* sender,
        const System::EventArgs& e)
    {
        (void)sender;
        (void)e;
        if (MediaPlayer::getStateProperty() == MediaState::Playing)
        {
            MediaPlayer::Pause();
        }
    }

    void SoundAndMusicSampleGame::ButtonPlaySongTouchDown(
        System::Object* sender,
        const System::EventArgs& e)
    {
        (void)sender;
        (void)e;
        if (MediaPlayer::getStateProperty() == MediaState::Paused)
        {
            MediaPlayer::Resume();
        }
        else if (MediaPlayer::getStateProperty() == MediaState::Stopped)
        {
            MediaPlayer::setIsRepeatingProperty(true);
            MediaPlayer::Play(song_.get());
        }
    }
}
