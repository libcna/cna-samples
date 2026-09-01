// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/PacketReader.hpp"
#include "Microsoft/Xna/Framework/Net/PacketWriter.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

namespace Microsoft::Xna::Framework::GamerServices
{
    class GamerServicesComponent;
}

namespace Microsoft::Xna::Framework::Net
{
    class GamerJoinedEventArgs;
    class LocalNetworkGamer;
    class NetworkSessionEndedEventArgs;
    enum class NetworkSessionEndReason;
}

namespace PeerToPeer
{
    class Tank;

    /**
     * @brief Demonstrates an XNA System Link session with peer-owned simulation.
     */
    class PeerToPeerGame final : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Creates the peer-to-peer networking sample. */
        PeerToPeerGame();

        /** @brief Releases the game-owned session, components, and tank instances. */
        ~PeerToPeerGame() override;

        /**
         * @brief Returns the fully qualified managed type name.
         *
         * @return The managed type name used by SharpRuntime type identity.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Creates the sprite batch and loads the original sprite font. */
        void LoadContent() override;

        /**
         * @brief Updates input, menu state, and the active network session.
         *
         * @param gameTime Timing information for the current update.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Draws either the create/join menu or the synchronized tanks.
         *
         * @param gameTime Timing information for the current frame.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        static constexpr int screenWidth = 1067;
        static constexpr int screenHeight = 600;
        static constexpr int maxGamers = 16;
        static constexpr int maxLocalGamers = 4;

        std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager> graphics;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font;

        Microsoft::Xna::Framework::Input::KeyboardState currentKeyboardState;
        Microsoft::Xna::Framework::Input::GamePadState currentGamePadState;

        std::unique_ptr<Microsoft::Xna::Framework::Net::NetworkSession> networkSession;
        bool sessionEndedDuringUpdate = false;

        Microsoft::Xna::Framework::Net::PacketWriter packetWriter;
        Microsoft::Xna::Framework::Net::PacketReader packetReader;

        std::string errorMessage;
        std::vector<std::unique_ptr<Tank>> tanks;
        std::unique_ptr<Microsoft::Xna::Framework::GamerServices::GamerServicesComponent>
            gamerServices;

        void UpdateMenuScreen();
        void CreateSession();
        void JoinSession();
        void HookSessionEvents();
        void GamerJoinedEventHandler(
            System::Object* sender,
            const Microsoft::Xna::Framework::Net::GamerJoinedEventArgs& e);
        void SessionEndedEventHandler(
            System::Object* sender,
            const Microsoft::Xna::Framework::Net::NetworkSessionEndedEventArgs& e);
        void UpdateNetworkSession();
        void UpdateLocalGamer(Microsoft::Xna::Framework::Net::LocalNetworkGamer& gamer);
        void ReadIncomingPackets(Microsoft::Xna::Framework::Net::LocalNetworkGamer& gamer);
        void DrawMenuScreen();
        void DrawNetworkSession();
        void DrawMessage(const std::string& message);
        void HandleInput();
        [[nodiscard]] bool IsPressed(
            Microsoft::Xna::Framework::Input::Keys key,
            Microsoft::Xna::Framework::Input::Buttons button) const;
        void ReadTankInputs(
            Tank& tank,
            Microsoft::Xna::Framework::PlayerIndex playerIndex);
        [[nodiscard]] static std::string ToString(
            Microsoft::Xna::Framework::Net::NetworkSessionEndReason reason);
        [[nodiscard]] static std::string ReplaceAll(
            std::string value,
            const std::string& from,
            const std::string& to);
    };
}
