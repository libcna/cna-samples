// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// LobbyScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Networking/LobbyScreen.hpp"

#include <cmath>
#include <optional>
#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Net/LocalNetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionState.hpp"
#include "Networking/NetworkSessionComponent.hpp"
#include "Resources.hpp"
#include "ScreenManager/InputState.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/GameplayScreen.hpp"
#include "Screens/LoadingScreen.hpp"
#include "Screens/MessageBoxScreen.hpp"

namespace NetworkStateManagement
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Net;

    LobbyScreen::LobbyScreen(std::shared_ptr<NetworkSession> networkSession)
        : networkSession(std::move(networkSession))
    {
        setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(0.5));
        setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(0.5));
    }

    void LobbyScreen::LoadContent()
    {
        auto& content = getScreenManagerProperty().getGameProperty().getContentProperty();

        isReadyTexture.emplace(content.Load<Texture2D>("chat_ready"));
        hasVoiceTexture.emplace(content.Load<Texture2D>("chat_able"));
        isTalkingTexture.emplace(content.Load<Texture2D>("chat_talking"));
        voiceMutedTexture.emplace(content.Load<Texture2D>("chat_mute"));
    }

    void LobbyScreen::Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen)
    {
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);

        if (!getIsExitingProperty())
        {
            if (networkSession->getSessionStateProperty() == NetworkSessionState::Playing)
            {
                // Check if we should leave the lobby and begin gameplay.
                // We pass null as the controlling player, because the networked
                // gameplay screen accepts input from any local players who
                // are in the session, not just a single controlling player.
                std::vector<std::shared_ptr<GameScreen>> screens;
                screens.push_back(std::make_shared<GameplayScreen>(networkSession));
                LoadingScreen::Load(getScreenManagerProperty(), true, std::nullopt, std::move(screens));
            }
            else if (networkSession->getIsHostProperty() && networkSession->getIsEveryoneReadyProperty())
            {
                // The host checks whether everyone has marked themselves
                // as ready, and starts the game in response.
                networkSession->StartGame();
            }
        }
    }

    void LobbyScreen::HandleInput(InputState& input)
    {
        for (LocalNetworkGamer* gamer : networkSession->getLocalGamersProperty())
        {
            const PlayerIndex playerIndex = gamer->getSignedInGamerProperty()->getPlayerIndexProperty();

            PlayerIndex unwantedOutput;

            if (input.IsMenuSelect(playerIndex, unwantedOutput))
            {
                HandleMenuSelect(*gamer);
            }
            else if (input.IsMenuCancel(playerIndex, unwantedOutput))
            {
                HandleMenuCancel(*gamer);
            }
        }
    }

    // Handle MenuSelect inputs by marking ourselves as ready.
    void LobbyScreen::HandleMenuSelect(LocalNetworkGamer& gamer)
    {
        if (!gamer.getIsReadyProperty())
        {
            gamer.setIsReadyProperty(true);
        }
        else if (gamer.getIsHostProperty())
        {
            // The host has an option to force starting the game, even if not
            // everyone has marked themselves ready. If they press select twice
            // in a row, the first time marks the host ready, then the second
            // time we ask if they want to force start.
            auto messageBox = std::make_shared<MessageBoxScreen>(Resources::ConfirmForceStartGame);

            messageBox->Accepted += [this](System::Object* sender, const PlayerIndexEventArgs& e)
            {
                ConfirmStartGameMessageBoxAccepted(sender, e);
            };

            getScreenManagerProperty().AddScreen(std::move(messageBox), gamer.getSignedInGamerProperty()->getPlayerIndexProperty());
        }
    }

    // Event handler for when the host selects ok on the "are you sure you want to start even
    // though not everyone is ready" message box.
    void LobbyScreen::ConfirmStartGameMessageBoxAccepted(System::Object*, const PlayerIndexEventArgs&)
    {
        if (networkSession->getSessionStateProperty() == NetworkSessionState::Lobby)
        {
            networkSession->StartGame();
        }
    }

    // Handle MenuCancel inputs by clearing our ready status, or if it is already clear, prompting
    // if the user wants to leave the session.
    void LobbyScreen::HandleMenuCancel(LocalNetworkGamer& gamer)
    {
        if (gamer.getIsReadyProperty())
        {
            gamer.setIsReadyProperty(false);
        }
        else
        {
            const PlayerIndex playerIndex = gamer.getSignedInGamerProperty()->getPlayerIndexProperty();

            NetworkSessionComponent::LeaveSession(getScreenManagerProperty(), playerIndex);
        }
    }

    void LobbyScreen::Draw(const GameTime&)
    {
        auto& manager = getScreenManagerProperty();
        auto& spriteBatch = manager.getSpriteBatchProperty();
        auto& font = manager.getFontProperty();

        Vector2 position(100.0f, 150.0f);

        // Make the lobby slide into place during transitions.
        const float transitionOffset = static_cast<float>(std::pow(getTransitionPositionProperty(), 2));

        if (getScreenStateProperty() == ScreenState::TransitionOn)
            position.X -= transitionOffset * 256.0f;
        else
            position.X += transitionOffset * 512.0f;

        spriteBatch.Begin();

        // Draw all the gamers in the session.
        int gamerCount = 0;

        for (NetworkGamer* gamer : networkSession->getAllGamersProperty())
        {
            DrawGamer(*gamer, position);

            // Advance to the next screen position, wrapping into two
            // columns if there are more than 8 gamers in the session.
            if (++gamerCount == 8)
            {
                position.X += 433.0f;
                position.Y = 150.0f;
            }
            else
                position.Y += static_cast<float>(font.getLineSpacingProperty());
        }

        // Draw the screen title.
        const std::string& title = Resources::Lobby;

        Vector2 titlePosition(533.0f, 80.0f);
        const Vector2 titleOrigin = font.MeasureString(title) / 2.0f;
        const Color titleColor = Color(192, 192, 192) * getTransitionAlphaProperty();
        constexpr float titleScale = 1.25f;

        titlePosition.Y -= transitionOffset * 100.0f;

        spriteBatch.DrawString(font, title, titlePosition, titleColor, 0.0f, titleOrigin, titleScale, SpriteEffects::None, 0.0f);

        spriteBatch.End();
    }

    // Helper draws the gamertag and status icons for a single NetworkGamer.
    void LobbyScreen::DrawGamer(NetworkGamer& gamer, Vector2 position)
    {
        auto& manager = getScreenManagerProperty();
        auto& spriteBatch = manager.getSpriteBatchProperty();
        auto& font = manager.getFontProperty();

        const Vector2 iconWidth(34.0f, 0.0f);
        const Vector2 iconOffset(0.0f, 12.0f);

        Vector2 iconPosition = position + iconOffset;

        // Draw the "is ready" icon.
        if (gamer.getIsReadyProperty())
        {
            spriteBatch.Draw(*isReadyTexture, iconPosition, Color::Lime * getTransitionAlphaProperty());
        }

        iconPosition += iconWidth;

        // Draw the "is muted", "is talking", or "has voice" icon.
        if (gamer.getIsMutedByLocalUserProperty())
        {
            spriteBatch.Draw(*voiceMutedTexture, iconPosition, Color::Red * getTransitionAlphaProperty());
        }
        else if (gamer.getIsTalkingProperty())
        {
            spriteBatch.Draw(*isTalkingTexture, iconPosition, Color::Yellow * getTransitionAlphaProperty());
        }
        else if (gamer.getHasVoiceProperty())
        {
            spriteBatch.Draw(*hasVoiceTexture, iconPosition, Color::White * getTransitionAlphaProperty());
        }

        // Draw the gamertag, normally in white, but yellow for local players.
        std::string text = gamer.getGamertagProperty();

        if (gamer.getIsHostProperty())
            text += Resources::HostSuffix;

        const Color color = gamer.getIsLocalProperty() ? Color::Yellow : Color::White;

        spriteBatch.DrawString(font, text, position + iconWidth * 2.0f, color * getTransitionAlphaProperty());
    }

    const std::string& LobbyScreen::GetTypeName() const
    {
        static const std::string name = "NetworkStateManagement.LobbyScreen";
        return name;
    }
}
