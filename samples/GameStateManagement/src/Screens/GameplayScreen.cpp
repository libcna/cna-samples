// SPDX-License-Identifier: MS-PL

#include "Screens/GameplayScreen.hpp"

#include <algorithm>
#include <memory>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Graphics/ClearOptions.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "ScreenManager/InputState.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/PauseMenuScreen.hpp"
#include "System/Threading/Thread.hpp"

namespace GameStateManagement
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using Microsoft::Xna::Framework::Input::Keys;

    GameplayScreen::GameplayScreen()
    {
        setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(1.5));
        setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(0.5));
    }

    void GameplayScreen::LoadContent()
    {
        auto& game = getScreenManagerProperty().getGameProperty();
        if (!content_)
            content_ = std::make_unique<Content::ContentManager>(&game.getServicesProperty(), "Content");
        gameFont_.emplace(content_->Load<SpriteFont>("gamefont"));
        System::Threading::Thread::Sleep(1000);
        game.ResetElapsedTime();
    }

    void GameplayScreen::UnloadContent() { content_->Unload(); }

    void GameplayScreen::Update(GameTime& gameTime, bool otherScreenHasFocus,
                                bool coveredByOtherScreen)
    {
        GameScreen::Update(gameTime, otherScreenHasFocus, false);
        pauseAlpha_ = coveredByOtherScreen
            ? std::min(pauseAlpha_ + 1.0f / 32.0f, 1.0f)
            : std::max(pauseAlpha_ - 1.0f / 32.0f, 0.0f);
        if (getIsActiveProperty())
        {
            constexpr float randomization = 10.0f;
            enemyPosition_.X += static_cast<float>(random_.NextDouble() - 0.5) * randomization;
            enemyPosition_.Y += static_cast<float>(random_.NextDouble() - 0.5) * randomization;
            const Vector2 targetPosition(
                getScreenManagerProperty().getGraphicsDeviceProperty().getViewportProperty()
                    .getWidthProperty() / 2.0f -
                    gameFont_->MeasureString("Insert Gameplay Here").X / 2.0f,
                200.0f);
            enemyPosition_ = Vector2::Lerp(enemyPosition_, targetPosition, 0.05f);
        }
    }

    void GameplayScreen::HandleInput(InputState& input)
    {
        const int playerIndex = static_cast<int>(getControllingPlayerProperty().value());
        const auto& keyboardState = input.CurrentKeyboardStates[playerIndex];
        const auto& gamePadState = input.CurrentGamePadStates[playerIndex];
        const bool gamePadDisconnected = !gamePadState.getIsConnectedProperty() &&
                                         input.GamePadWasConnected[playerIndex];
        if (input.IsPauseGame(getControllingPlayerProperty()) || gamePadDisconnected)
        {
            getScreenManagerProperty().AddScreen(std::make_shared<PauseMenuScreen>(),
                                                 getControllingPlayerProperty());
            return;
        }

        Vector2 movement = Vector2::Zero;
        if (keyboardState.IsKeyDown(Keys::Left)) --movement.X;
        if (keyboardState.IsKeyDown(Keys::Right)) ++movement.X;
        if (keyboardState.IsKeyDown(Keys::Up)) --movement.Y;
        if (keyboardState.IsKeyDown(Keys::Down)) ++movement.Y;
        const Vector2 thumbstick = gamePadState.getThumbSticksProperty().getLeftProperty();
        movement.X += thumbstick.X;
        movement.Y -= thumbstick.Y;
        if (movement.Length() > 1.0f)
            movement.Normalize();
        playerPosition_ += movement * 2.0f;
    }

    void GameplayScreen::Draw(const GameTime&)
    {
        auto& manager = getScreenManagerProperty();
        manager.getGraphicsDeviceProperty().Clear(ClearOptions::Target,
            Color::CornflowerBlue, 0.0f, 0);
        auto& spriteBatch = manager.getSpriteBatchProperty();
        spriteBatch.Begin();
        spriteBatch.DrawString(*gameFont_, "// TODO", playerPosition_, Color::Green);
        spriteBatch.DrawString(*gameFont_, "Insert Gameplay Here", enemyPosition_, Color::DarkRed);
        spriteBatch.End();
        if (getTransitionPositionProperty() > 0.0f || pauseAlpha_ > 0.0f)
        {
            const float alpha = MathHelper::Lerp(1.0f - getTransitionAlphaProperty(),
                                                  1.0f, pauseAlpha_ / 2.0f);
            manager.FadeBackBufferToBlack(alpha);
        }
    }

    const std::string& GameplayScreen::GetTypeName() const
    {
        static const std::string name = "GameStateManagement.GameplayScreen";
        return name;
    }
}
