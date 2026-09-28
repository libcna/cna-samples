// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// GameplayScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Screens/GameplayScreen.hpp"

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Graphics/ClearOptions.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Net/LocalNetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionState.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Networking/LobbyScreen.hpp"
#include "ScreenManager/InputState.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/BackgroundScreen.hpp"
#include "Screens/LoadingScreen.hpp"
#include "Screens/PauseMenuScreen.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/Threading/Thread.hpp"

namespace NetworkStateManagement
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using Microsoft::Xna::Framework::Input::Keys;
    using Microsoft::Xna::Framework::Net::LocalNetworkGamer;
    using Microsoft::Xna::Framework::Net::NetworkSession;
    using Microsoft::Xna::Framework::Net::NetworkSessionState;

    GameplayScreen::GameplayScreen(std::shared_ptr<NetworkSession> networkSession)
        : networkSession_(std::move(networkSession))
    {
        setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(1.5));
        setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(0.5));
    }

    // The logic for deciding whether the game is paused depends on whether this is a networked or
    // single player game. If we are in a network session, we should go on updating the game even
    // when the user tabs away from us or brings up the pause menu, because even though the local
    // player is not responding to input, other remote players may not be paused. In single player
    // modes, however, we want everything to pause if the game loses focus.
    bool GameplayScreen::IsActive() const
    {
        if (networkSession_ == nullptr)
        {
            // Pause behavior for single player games.
            return getIsActiveProperty();
        }
        // Pause behavior for networked games.
        return !getIsExitingProperty();
    }

    void GameplayScreen::LoadContent()
    {
        auto& game = getScreenManagerProperty().getGameProperty();
        if (!content_)
            content_ = std::make_unique<Content::ContentManager>(&game.getServicesProperty(), "Content");

        gameFont_.emplace(content_->Load<SpriteFont>("gamefont"));

        // A real game would probably have more content than this sample, so
        // it would take longer to load. We simulate that by delaying for a
        // while, giving you a chance to admire the beautiful loading screen.
        System::Threading::Thread::Sleep(1000);

        // once the load has finished, we use ResetElapsedTime to tell the game's
        // timing mechanism that we have just finished a very long frame, and that
        // it should not try to catch up.
        game.ResetElapsedTime();
    }

    void GameplayScreen::UnloadContent() { content_->Unload(); }

    void GameplayScreen::Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen)
    {
        GameScreen::Update(gameTime, otherScreenHasFocus, false);

        // Gradually fade in or out depending on whether we are covered by the pause screen.
        pauseAlpha_ = coveredByOtherScreen ? std::min(pauseAlpha_ + 1.0f / 32.0f, 1.0f)
                                           : std::max(pauseAlpha_ - 1.0f / 32.0f, 0.0f);

        if (IsActive())
        {
            // Apply some random jitter to make the enemy move around.
            constexpr float randomization = 10.0f;

            enemyPosition_.X += static_cast<float>(random_.NextDouble() - 0.5) * randomization;
            enemyPosition_.Y += static_cast<float>(random_.NextDouble() - 0.5) * randomization;

            // Apply a stabilizing force to stop the enemy moving off the screen.
            const Vector2 targetPosition(200.0f, 200.0f);

            enemyPosition_ = Vector2::Lerp(enemyPosition_, targetPosition, 0.05f);
        }

        // If we are in a network game, check if we should return to the lobby.
        if ((networkSession_ != nullptr) && !getIsExitingProperty())
        {
            if (networkSession_->getSessionStateProperty() == NetworkSessionState::Lobby)
            {
                std::vector<std::shared_ptr<GameScreen>> screens;
                screens.push_back(std::make_shared<BackgroundScreen>());
                screens.push_back(std::make_shared<LobbyScreen>(networkSession_));
                LoadingScreen::Load(getScreenManagerProperty(), true, std::nullopt, std::move(screens));
            }
        }
    }

    void GameplayScreen::HandleInput(InputState& input)
    {
        if (getControllingPlayerProperty().has_value())
        {
            // In single player games, handle input for the controlling player.
            HandlePlayerInput(input, getControllingPlayerProperty().value());
        }
        else if (networkSession_ != nullptr)
        {
            // In network game modes, handle input for all the
            // local players who are participating in the session.
            for (LocalNetworkGamer* gamer : networkSession_->getLocalGamersProperty())
            {
                if (!HandlePlayerInput(input, gamer->getSignedInGamerProperty()->getPlayerIndexProperty()))
                    break;
            }
        }
    }

    // Returns true if we should continue to handle input for subsequent players,
    // or false if this player has paused the game.
    bool GameplayScreen::HandlePlayerInput(InputState& input, PlayerIndex playerIndex)
    {
        // Look up inputs for the specified player profile.
        const auto& keyboardState = input.CurrentKeyboardStates[static_cast<int>(playerIndex)];
        const auto& gamePadState = input.CurrentGamePadStates[static_cast<int>(playerIndex)];

        // The game pauses either if the user presses the pause button, or if
        // they unplug the active gamepad. This requires us to keep track of
        // whether a gamepad was ever plugged in, because we don't want to pause
        // on PC if they are playing with a keyboard and have no gamepad at all!
        const bool gamePadDisconnected = !gamePadState.getIsConnectedProperty() &&
                                         input.GamePadWasConnected[static_cast<int>(playerIndex)];

        if (input.IsPauseGame(playerIndex) || gamePadDisconnected)
        {
            getScreenManagerProperty().AddScreen(std::make_shared<PauseMenuScreen>(networkSession_), playerIndex);
            return false;
        }

        // Otherwise move the player position.
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

        return true;
    }

    void GameplayScreen::Draw(const GameTime&)
    {
        auto& manager = getScreenManagerProperty();

        // This game has a blue background. Why? Because!
        manager.getGraphicsDeviceProperty().Clear(ClearOptions::Target, Color::CornflowerBlue, 0.0f, 0);

        // Our player and enemy are both actually just text strings.
        auto& spriteBatch = manager.getSpriteBatchProperty();
        spriteBatch.Begin();

        spriteBatch.DrawString(*gameFont_, "// TODO", playerPosition_, Color::Green);
        spriteBatch.DrawString(*gameFont_, "Insert Gameplay Here", enemyPosition_, Color::DarkRed);

        if (networkSession_ != nullptr)
        {
            const std::string message = "Players: " + std::to_string(networkSession_->getAllGamersProperty().getCountProperty());
            const Vector2 messagePosition(100.0f, 480.0f);
            spriteBatch.DrawString(*gameFont_, message, messagePosition, Color::White);
        }

        spriteBatch.End();

        // If the game is transitioning on or off, fade it out to black.
        if (getTransitionPositionProperty() > 0.0f || pauseAlpha_ > 0.0f)
        {
            const float alpha = MathHelper::Lerp(1.0f - getTransitionAlphaProperty(), 1.0f, pauseAlpha_ / 2.0f);
            manager.FadeBackBufferToBlack(alpha);
        }
    }

    const std::string& GameplayScreen::GetTypeName() const
    {
        static const std::string name = "NetworkStateManagement.GameplayScreen";
        return name;
    }
}
