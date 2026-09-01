// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// NetworkPredictionGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Net/PacketReader.hpp"
#include "Microsoft/Xna/Framework/Net/PacketWriter.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Microsoft::Xna::Framework::GamerServices
{
    class GamerServicesComponent;
}

namespace Microsoft::Xna::Framework::Net
{
    class GamerJoinedEventArgs;
    class LocalNetworkGamer;
    class NetworkGamer;
    class NetworkSession;
    class NetworkSessionEndedEventArgs;
    enum class NetworkSessionEndReason;
}

namespace NetworkPrediction
{
    class Tank;

    /**
     * @brief Demonstrates prediction and smoothing under simulated network conditions.
     */
    class NetworkPredictionGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Creates the graphics and gamer-services components. */
        NetworkPredictionGame();

        /** @brief Releases the owned network session and sample objects. */
        ~NetworkPredictionGame() override;

        /**
         * @brief Returns the fully qualified logical type name.
         *
         * @return The sample game type name.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Creates the sprite batch and loads the sample font. */
        void LoadContent() override;

        /**
         * @brief Updates input, menu state, networking and tank simulation.
         *
         * @param gameTime Provides the current timing snapshot.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Draws either the session menu or the active network session.
         *
         * @param gameTime Provides the current timing snapshot.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        static constexpr int screenWidth = 1067;
        static constexpr int screenHeight = 600;
        static constexpr int maxGamers = 16;
        static constexpr int maxLocalGamers = 4;

        enum class NetworkQuality
        {
            Typical,
            Poor,
            Perfect,
        };

        std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager> graphics;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font;

        Microsoft::Xna::Framework::Input::KeyboardState currentKeyboardState;
        Microsoft::Xna::Framework::Input::GamePadState currentGamePadState;
        Microsoft::Xna::Framework::Input::KeyboardState previousKeyboardState;
        Microsoft::Xna::Framework::Input::GamePadState previousGamePadState;

        std::unique_ptr<Microsoft::Xna::Framework::Net::NetworkSession> networkSession;
        bool sessionEndedDuringUpdate = false;
        Microsoft::Xna::Framework::Net::PacketWriter packetWriter;
        Microsoft::Xna::Framework::Net::PacketReader packetReader;
        std::string errorMessage;

        NetworkQuality networkQuality = NetworkQuality::Typical;
        int framesBetweenPackets = 6;
        int framesSinceLastSend = 0;
        bool enablePrediction = true;
        bool enableSmoothing = true;

        std::vector<std::unique_ptr<Tank>> tanks;
        std::unique_ptr<Microsoft::Xna::Framework::GamerServices::GamerServicesComponent> gamerServices;

        void UpdateMenuScreen();
        void CreateSession();
        void JoinSession();
        void HookSessionEvents();
        void GamerJoinedEventHandler(
            System::Object* sender,
            const Microsoft::Xna::Framework::Net::GamerJoinedEventArgs& eventArgs);
        void SessionEndedEventHandler(
            System::Object* sender,
            const Microsoft::Xna::Framework::Net::NetworkSessionEndedEventArgs& eventArgs);
        void UpdateNetworkSession(Microsoft::Xna::Framework::GameTime& gameTime);
        void UpdateLocalGamer(
            Microsoft::Xna::Framework::Net::LocalNetworkGamer* gamer,
            const Microsoft::Xna::Framework::GameTime& gameTime,
            bool sendPacketThisFrame);
        void ReadIncomingPackets(
            Microsoft::Xna::Framework::Net::LocalNetworkGamer* gamer,
            const Microsoft::Xna::Framework::GameTime& gameTime);
        void UpdateOptions();
        void DrawMenuScreen();
        void DrawNetworkSession();
        void DrawOptions();
        void DrawMessage(const std::string& message);
        void HandleInput();
        [[nodiscard]] bool IsPressed(
            Microsoft::Xna::Framework::Input::Keys key,
            Microsoft::Xna::Framework::Input::Buttons button) const;
        static void ReadTankInputs(
            Microsoft::Xna::Framework::PlayerIndex playerIndex,
            Microsoft::Xna::Framework::Vector2& tankInput,
            Microsoft::Xna::Framework::Vector2& turretInput);
        [[nodiscard]] static std::string ToString(
            Microsoft::Xna::Framework::Net::NetworkSessionEndReason reason);
        [[nodiscard]] static std::string ReplaceAll(
            std::string value,
            const std::string& from,
            const std::string& to);
    };
}
