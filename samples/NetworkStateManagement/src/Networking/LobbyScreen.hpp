// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// LobbyScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "ScreenManager/GameScreen.hpp"
#include "Screens/PlayerIndexEventArgs.hpp"

namespace Microsoft::Xna::Framework::Net
{
    class LocalNetworkGamer;
    class NetworkGamer;
    class NetworkSession;
}

namespace NetworkStateManagement
{
    /**
     * @brief The lobby screen provides a place for gamers to congregate before starting the actual
     * gameplay. It displays a list of all the gamers in the session, and indicates which ones are
     * currently talking. Each gamer can press a button to mark themselves as ready: gameplay will
     * begin after everyone has done this.
     */
    class LobbyScreen final : public GameScreen
    {
    public:
        /**
         * @brief Constructs a new lobby screen.
         *
         * @param networkSession The session; shared with the session component.
         */
        explicit LobbyScreen(std::shared_ptr<Microsoft::Xna::Framework::Net::NetworkSession> networkSession);

        /** @brief Loads graphics content used by the lobby screen. */
        void LoadContent() override;
        /**
         * @brief Updates the lobby screen.
         *
         * @param gameTime Timing information.
         * @param otherScreenHasFocus Whether another screen has focus.
         * @param coveredByOtherScreen Whether another screen covers this one.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime, bool otherScreenHasFocus,
                    bool coveredByOtherScreen) override;
        /** @brief Lets the lobby screen handle user input. @param input Input state. */
        void HandleInput(InputState& input) override;
        /** @brief Draws the lobby screen. @param gameTime Timing information. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

        /** @brief Returns the managed type name. @return The type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        void HandleMenuSelect(Microsoft::Xna::Framework::Net::LocalNetworkGamer& gamer);
        void ConfirmStartGameMessageBoxAccepted(System::Object* sender, const PlayerIndexEventArgs& e);
        void HandleMenuCancel(Microsoft::Xna::Framework::Net::LocalNetworkGamer& gamer);
        void DrawGamer(Microsoft::Xna::Framework::Net::NetworkGamer& gamer, Microsoft::Xna::Framework::Vector2 position);

        std::shared_ptr<Microsoft::Xna::Framework::Net::NetworkSession> networkSession;

        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> isReadyTexture;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> hasVoiceTexture;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> isTalkingTexture;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> voiceMutedTexture;
    };
}
