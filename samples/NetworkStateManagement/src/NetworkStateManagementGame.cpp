// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "NetworkStateManagementGame.hpp"

#include <optional>

#include "MessageDisplayComponent.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/GameComponentCollection.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesComponent.hpp"
#include "Microsoft/Xna/Framework/GamerServices/InviteAcceptedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Networking/NetworkSessionComponent.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/BackgroundScreen.hpp"
#include "Screens/MainMenuScreen.hpp"

namespace NetworkStateManagement
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::GamerServices;
    using Microsoft::Xna::Framework::Graphics::Texture2D;
    using Microsoft::Xna::Framework::Net::NetworkSession;

    const std::array<std::string, 6> NetworkStateManagementGame::preloadAssets = {
        "gradient", "cat", "chat_ready", "chat_able", "chat_talking", "chat_mute",
    };

    NetworkStateManagementGame::NetworkStateManagementGame()
    {
        getContentProperty().setRootDirectoryProperty("Content");

        graphics = std::make_unique<GraphicsDeviceManager>(this);

        graphics->setPreferredBackBufferWidthProperty(1067);
        graphics->setPreferredBackBufferHeightProperty(600);

        // Create components.
        screenManager = std::make_unique<ScreenManager>(*this);
        messageDisplay = std::make_unique<MessageDisplayComponent>(*this);
        gamerServices = std::make_unique<GamerServicesComponent>(*this);

        getComponentsProperty().Add(screenManager.get());
        getComponentsProperty().Add(messageDisplay.get());
        getComponentsProperty().Add(gamerServices.get());

        // Activate the first screens.
        screenManager->AddScreen(std::make_shared<BackgroundScreen>(), std::nullopt);
        screenManager->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);

        // Listen for invite notification events.
        inviteAcceptedSubscription = NetworkSession::InviteAccepted.Add(
            [this](System::Object*, const InviteAcceptedEventArgs& e) { NetworkSessionComponent::InviteAccepted(*screenManager, e); });

        // To test the trial mode behavior while developing your game,
        // uncomment this line:

        // Guide.SimulateTrialMode = true;
    }

    NetworkStateManagementGame::~NetworkStateManagementGame()
    {
        // The event is static; C# keeps the game alive through the delegate, C++ must detach it.
        NetworkSession::InviteAccepted.Remove(inviteAcceptedSubscription);
        NetworkSessionComponent::ReleaseRetired();
    }

    void NetworkStateManagementGame::LoadContent()
    {
        // ContentManager's statically typed C++ Load route requires the concrete runtime asset
        // type; retaining the textures has the same preload/cache effect as C# Load<object>.
        for (const std::string& asset : preloadAssets)
        {
            preloaded.push_back(getContentProperty().Load<Texture2D>(asset));
        }
    }

    void NetworkStateManagementGame::Update(GameTime& gameTime)
    {
        // A session component that left its session last frame can now go: the component loop
        // below holds no pointer to it any more (the garbage collector's job in C#).
        NetworkSessionComponent::ReleaseRetired();

        Game::Update(gameTime);
    }

    void NetworkStateManagementGame::Draw(const GameTime& gameTime)
    {
        graphics->getGraphicsDeviceProperty()->Clear(Color::Black);

        // The real drawing happens inside the screen manager component.
        Game::Draw(gameTime);
    }

    const std::string& NetworkStateManagementGame::GetTypeName() const
    {
        static const std::string name = "NetworkStateManagement.NetworkStateManagementGame";
        return name;
    }
}
