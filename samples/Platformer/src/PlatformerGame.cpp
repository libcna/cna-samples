// SPDX-License-Identifier: MS-PL

#include "PlatformerGame.hpp"

#include "TouchCollectionExtensions.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/TitleContainer.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "Microsoft/Xna/Framework/Media/MediaPlayer.hpp"

namespace Platformer
{
    PlatformerGame::PlatformerGame()
        : graphics_(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

#if defined(WINDOWS_PHONE)
        graphics_.setIsFullScreenProperty(true);
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
#endif

        Accelerometer::Initialize();
    }

    const std::string& PlatformerGame::GetTypeName() const
    {
        static const std::string name = "Platformer.PlatformerGame";
        return name;
    }

    void PlatformerGame::LoadContent()
    {
        using namespace Microsoft::Xna::Framework::Graphics;
        using namespace Microsoft::Xna::Framework::Media;

        spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        hudFont_.emplace(getContentProperty().Load<SpriteFont>("Fonts/Hud"));
        winOverlay_.emplace(getContentProperty().Load<Texture2D>("Overlays/you_win"));
        loseOverlay_.emplace(getContentProperty().Load<Texture2D>("Overlays/you_lose"));
        diedOverlay_.emplace(getContentProperty().Load<Texture2D>("Overlays/you_died"));

        try
        {
            MediaPlayer::setIsRepeatingProperty(true);
            music_.emplace(getContentProperty().Load<Song>("Sounds/Music"));
            MediaPlayer::Play(&*music_);
        }
        catch (...)
        {
        }

        LoadNextLevel();
    }

    void PlatformerGame::Update(Microsoft::Xna::Framework::GameTime& gameTime)
    {
        HandleInput();
        level_->Update(gameTime, keyboardState_, gamePadState_, touchState_, accelerometerState_,
                       getWindowProperty().getCurrentOrientationProperty());
        Game::Update(gameTime);
    }

    void PlatformerGame::HandleInput()
    {
        using namespace Microsoft::Xna::Framework::Input;
        using Microsoft::Xna::Framework::PlayerIndex;

        keyboardState_ = Keyboard::GetState();
        gamePadState_ = GamePad::GetState(PlayerIndex::One);
        touchState_ = Touch::TouchPanel::GetState();
        accelerometerState_ = Accelerometer::GetState();

        if (gamePadState_.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
            Exit();

        const bool continuePressed = keyboardState_.IsKeyDown(Keys::Space) ||
                                     gamePadState_.IsButtonDown(Buttons::A) ||
                                     TouchCollectionExtensions::AnyTouch(touchState_);

        if (!wasContinuePressed_ && continuePressed)
        {
            if (!level_->getPlayerProperty()->getIsAliveProperty())
                level_->StartNewLife();
            else if (level_->getTimeRemainingProperty() == System::TimeSpan::Zero)
            {
                if (level_->getReachedExitProperty())
                    LoadNextLevel();
                else
                    ReloadCurrentLevel();
            }
        }

        wasContinuePressed_ = continuePressed;
    }

    void PlatformerGame::LoadNextLevel()
    {
        levelIndex_ = (levelIndex_ + 1) % numberOfLevels;

        if (level_ != nullptr)
            level_->Dispose();
        level_.reset();

        const std::string levelPath =
            "Content/Levels/" + std::to_string(levelIndex_) + ".txt";
        auto fileStream = Microsoft::Xna::Framework::TitleContainer::OpenStream(levelPath);
        level_ = std::make_unique<Level>(&getServicesProperty(), *fileStream, levelIndex_);
    }

    void PlatformerGame::ReloadCurrentLevel()
    {
        --levelIndex_;
        LoadNextLevel();
    }

    void PlatformerGame::Draw(const Microsoft::Xna::Framework::GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Microsoft::Xna::Framework::Color::CornflowerBlue);

        spriteBatch_->Begin();
        level_->Draw(gameTime, *spriteBatch_);
        DrawHud();
        spriteBatch_->End();

        Game::Draw(gameTime);
    }

    void PlatformerGame::DrawHud()
    {
        using namespace Microsoft::Xna::Framework;
        using namespace Microsoft::Xna::Framework::Graphics;

        const Rectangle titleSafeArea =
            getGraphicsDeviceProperty().getViewportProperty().getTitleSafeAreaProperty();
        const Vector2 hudLocation(static_cast<float>(titleSafeArea.X),
                                  static_cast<float>(titleSafeArea.Y));
        const Vector2 center(titleSafeArea.X + titleSafeArea.Width / 2.0f,
                             titleSafeArea.Y + titleSafeArea.Height / 2.0f);

        const System::TimeSpan timeRemaining = level_->getTimeRemainingProperty();
        const auto pad2 = [](const int value)
        {
            return (value < 10 ? std::string("0") : std::string()) + std::to_string(value);
        };
        const std::string timeString =
            "TIME: " + pad2(timeRemaining.getMinutesProperty()) + ":" +
            pad2(timeRemaining.getSecondsProperty());

        const Color timeColor =
            timeRemaining > WarningTime || level_->getReachedExitProperty() ||
                    static_cast<int>(timeRemaining.getTotalSecondsProperty()) % 2 == 0
                ? Color::Yellow
                : Color::Red;
        DrawShadowedString(*hudFont_, timeString, hudLocation, timeColor);

        const float timeHeight = hudFont_->MeasureString(timeString).Y;
        DrawShadowedString(*hudFont_,
                           "SCORE: " + std::to_string(level_->getScoreProperty()),
                           hudLocation + Vector2(0.0f, timeHeight * 1.2f), Color::Yellow);

        const Texture2D* status = nullptr;
        if (timeRemaining == System::TimeSpan::Zero)
            status = level_->getReachedExitProperty() ? &*winOverlay_ : &*loseOverlay_;
        else if (!level_->getPlayerProperty()->getIsAliveProperty())
            status = &*diedOverlay_;

        if (status != nullptr)
        {
            const Vector2 statusSize(static_cast<float>(status->getWidthProperty()),
                                     static_cast<float>(status->getHeightProperty()));
            spriteBatch_->Draw(*status, center - statusSize / 2.0f, Color::White);
        }
    }

    void PlatformerGame::DrawShadowedString(
        const Microsoft::Xna::Framework::Graphics::SpriteFont& font,
        const std::string& value, const Microsoft::Xna::Framework::Vector2 position,
        const Microsoft::Xna::Framework::Color color)
    {
        spriteBatch_->DrawString(font, value,
                                 position + Microsoft::Xna::Framework::Vector2(1.0f, 1.0f),
                                 Microsoft::Xna::Framework::Color::Black);
        spriteBatch_->DrawString(font, value, position, color);
    }
}
