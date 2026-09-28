// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Microsoft::Xna::Framework::GamerServices
{
    class GamerServicesComponent;
}

namespace NetworkStateManagement
{
    class MessageDisplayComponent;
    class ScreenManager;

    /**
     * @brief Sample showing how to manage the different game states involved in implementing a
     * networked game, with menus for creating, searching, and joining sessions, a lobby screen, and
     * the game itself. This main game class is extremely simple: all the interesting stuff happens
     * in the ScreenManager component.
     */
    class NetworkStateManagementGame final : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief The main game constructor. */
        NetworkStateManagementGame();
        /** @brief Detaches the invitation handler and releases the components. */
        ~NetworkStateManagementGame() override;
        /** @brief Returns the managed type name. @return The type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads graphics content. */
        void LoadContent() override;
        /** @brief Releases retired session components, then updates the components. @param gameTime Timing. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief This is called when the game should draw itself. @param gameTime Timing. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager> graphics;
        std::unique_ptr<ScreenManager> screenManager;
        std::unique_ptr<MessageDisplayComponent> messageDisplay;
        std::unique_ptr<Microsoft::Xna::Framework::GamerServices::GamerServicesComponent> gamerServices;
        std::size_t inviteAcceptedSubscription = 0;

        // By preloading any assets used by UI rendering, we avoid framerate glitches
        // when they suddenly need to be loaded in the middle of a menu transition.
        static const std::array<std::string, 6> preloadAssets;
        std::vector<Microsoft::Xna::Framework::Graphics::Texture2D> preloaded;
    };
}
