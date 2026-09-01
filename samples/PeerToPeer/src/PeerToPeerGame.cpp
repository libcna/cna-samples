// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PeerToPeerGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "PeerToPeerGame.hpp"

#include <any>
#include <cstddef>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameComponentCollection.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Gamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesComponent.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamerCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Net/AvailableNetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/AvailableNetworkSessionCollection.hpp"
#include "Microsoft/Xna/Framework/Net/GamerJoinedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/LocalNetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionEndedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionEndReason.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionProperties.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionType.hpp"
#include "Microsoft/Xna/Framework/Net/SendDataOptions.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/Exception.hpp"
#include "Tank.hpp"

namespace PeerToPeer
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::GamerServices;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;
    using namespace Microsoft::Xna::Framework::Net;

    namespace
    {
        class AvailableSessionsDisposer final
        {
        public:
            explicit AvailableSessionsDisposer(AvailableNetworkSessionCollection& sessions)
                : sessions_(sessions)
            {
            }

            ~AvailableSessionsDisposer()
            {
                sessions_.Dispose();
            }

        private:
            AvailableNetworkSessionCollection& sessions_;
        };
    }

    PeerToPeerGame::PeerToPeerGame()
    {
        graphics = std::make_unique<GraphicsDeviceManager>(this);
        graphics->setPreferredBackBufferWidthProperty(screenWidth);
        graphics->setPreferredBackBufferHeightProperty(screenHeight);

        getContentProperty().setRootDirectoryProperty("Content");

        gamerServices = std::make_unique<GamerServicesComponent>(*this);
        getComponentsProperty().Add(gamerServices.get());
    }

    PeerToPeerGame::~PeerToPeerGame() = default;

    const std::string& PeerToPeerGame::GetTypeName() const
    {
        static const std::string name = "PeerToPeer.PeerToPeerGame";
        return name;
    }

    void PeerToPeerGame::LoadContent()
    {
        spriteBatch.emplace(getGraphicsDeviceProperty());
        font.emplace(getContentProperty().Load<SpriteFont>("Font"));
    }

    void PeerToPeerGame::Update(GameTime& gameTime)
    {
        HandleInput();

        if (!networkSession)
        {
            UpdateMenuScreen();
        }
        else
        {
            UpdateNetworkSession();
        }

        Game::Update(gameTime);
    }

    void PeerToPeerGame::UpdateMenuScreen()
    {
        if (getIsActiveProperty())
        {
            if (Gamer::getSignedInGamersProperty()->getCountProperty() == 0)
            {
                Guide::ShowSignIn(maxLocalGamers, false);
            }
            else if (IsPressed(Keys::A, Buttons::A))
            {
                CreateSession();
            }
            else if (IsPressed(Keys::B, Buttons::B))
            {
                JoinSession();
            }
        }
    }

    void PeerToPeerGame::CreateSession()
    {
        DrawMessage("Creating session...");

        try
        {
            networkSession.reset(NetworkSession::Create(
                NetworkSessionType::SystemLink,
                maxLocalGamers,
                maxGamers));
            HookSessionEvents();
        }
        catch (const System::Exception& e)
        {
            errorMessage = e.getMessageProperty();
        }
    }

    void PeerToPeerGame::JoinSession()
    {
        DrawMessage("Joining session...");

        try
        {
            AvailableNetworkSessionCollection availableSessions = NetworkSession::Find(
                NetworkSessionType::SystemLink,
                maxLocalGamers,
                NetworkSessionProperties());
            const AvailableSessionsDisposer disposer(availableSessions);

            if (availableSessions.getCountProperty() == 0)
            {
                errorMessage = "No network sessions found.";
                return;
            }

            const AvailableNetworkSession& availableSession = availableSessions.getItem(0);
            networkSession.reset(NetworkSession::Join(&availableSession));
            HookSessionEvents();
        }
        catch (const System::Exception& e)
        {
            errorMessage = e.getMessageProperty();
        }
    }

    void PeerToPeerGame::HookSessionEvents()
    {
        networkSession->GamerJoined +=
            [this](System::Object* sender, const GamerJoinedEventArgs& e)
            {
                GamerJoinedEventHandler(sender, e);
            };
        networkSession->SessionEnded +=
            [this](System::Object* sender, const NetworkSessionEndedEventArgs& e)
            {
                SessionEndedEventHandler(sender, e);
            };
    }

    void PeerToPeerGame::GamerJoinedEventHandler(
        System::Object* /*sender*/,
        const GamerJoinedEventArgs& e)
    {
        NetworkGamer* gamer = e.getGamerProperty();
        int gamerIndex = 0;

        for (NetworkGamer* existingGamer : networkSession->getAllGamersProperty())
        {
            if (existingGamer == gamer)
            {
                break;
            }

            ++gamerIndex;
        }

        tanks.push_back(std::make_unique<Tank>(
            gamerIndex,
            getContentProperty(),
            screenWidth,
            screenHeight));
        gamer->setTagProperty(std::any(tanks.back().get()));
    }

    void PeerToPeerGame::SessionEndedEventHandler(
        System::Object* /*sender*/,
        const NetworkSessionEndedEventArgs& e)
    {
        errorMessage = ToString(e.getEndReasonProperty());

        networkSession->Dispose();

        // C# can clear the managed reference from inside the callback. C++ must keep the
        // session object alive until its currently executing Update method has returned.
        sessionEndedDuringUpdate = true;
    }

    void PeerToPeerGame::UpdateNetworkSession()
    {
        for (LocalNetworkGamer* gamer : networkSession->getLocalGamersProperty())
        {
            UpdateLocalGamer(*gamer);
        }

        networkSession->Update();

        if (sessionEndedDuringUpdate)
        {
            networkSession.reset();
            tanks.clear();
            sessionEndedDuringUpdate = false;
            return;
        }

        for (LocalNetworkGamer* gamer : networkSession->getLocalGamersProperty())
        {
            ReadIncomingPackets(*gamer);
        }
    }

    void PeerToPeerGame::UpdateLocalGamer(LocalNetworkGamer& gamer)
    {
        Tank* localTank = std::any_cast<Tank*>(gamer.getTagProperty());

        ReadTankInputs(
            *localTank,
            gamer.getSignedInGamerProperty()->getPlayerIndexProperty());
        localTank->Update();

        packetWriter.Write(localTank->Position);
        packetWriter.Write(localTank->TankRotation);
        packetWriter.Write(localTank->TurretRotation);

        gamer.SendData(packetWriter, SendDataOptions::InOrder);
    }

    void PeerToPeerGame::ReadIncomingPackets(LocalNetworkGamer& gamer)
    {
        while (gamer.getIsDataAvailableProperty())
        {
            NetworkGamer* sender = nullptr;
            gamer.ReceiveData(packetReader, sender);

            if (sender->getIsLocalProperty())
            {
                continue;
            }

            Tank* remoteTank = std::any_cast<Tank*>(sender->getTagProperty());
            remoteTank->Position = packetReader.ReadVector2();
            remoteTank->TankRotation = packetReader.ReadSingle();
            remoteTank->TurretRotation = packetReader.ReadSingle();
        }
    }

    void PeerToPeerGame::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

        if (!networkSession)
        {
            DrawMenuScreen();
        }
        else
        {
            DrawNetworkSession();
        }

        Game::Draw(gameTime);
    }

    void PeerToPeerGame::DrawMenuScreen()
    {
        std::string message;

        if (!errorMessage.empty())
        {
            message += "Error:\n" + ReplaceAll(errorMessage, ". ", ".\n") + "\n\n";
        }

        message += "A = create session\nB = join session";

        spriteBatch->Begin();
        spriteBatch->DrawString(*font, message, Vector2(161.0f, 161.0f), Color::Black);
        spriteBatch->DrawString(*font, message, Vector2(160.0f, 160.0f), Color::White);
        spriteBatch->End();
    }

    void PeerToPeerGame::DrawNetworkSession()
    {
        spriteBatch->Begin();

        for (NetworkGamer* gamer : networkSession->getAllGamersProperty())
        {
            Tank* tank = std::any_cast<Tank*>(gamer->getTagProperty());
            tank->Draw(*spriteBatch);

            std::string label = gamer->getGamertagProperty();
            Color labelColor = Color::Black;
            const Vector2 labelOffset(100.0f, 150.0f);

            if (gamer->getIsHostProperty())
            {
                label += " (host)";
            }

            if (gamer->getIsTalkingProperty())
            {
                labelColor = Color::Yellow;
            }

            spriteBatch->DrawString(
                *font,
                label,
                tank->Position,
                labelColor,
                0.0f,
                labelOffset,
                0.6f,
                SpriteEffects::None,
                0.0f);
        }

        spriteBatch->End();
    }

    void PeerToPeerGame::DrawMessage(const std::string& message)
    {
        if (!BeginDraw())
        {
            return;
        }

        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

        spriteBatch->Begin();
        spriteBatch->DrawString(*font, message, Vector2(161.0f, 161.0f), Color::Black);
        spriteBatch->DrawString(*font, message, Vector2(160.0f, 160.0f), Color::White);
        spriteBatch->End();

        EndDraw();
    }

    void PeerToPeerGame::HandleInput()
    {
        currentKeyboardState = Keyboard::GetState();
        currentGamePadState = GamePad::GetState(PlayerIndex::One);

        if (getIsActiveProperty() && IsPressed(Keys::Escape, Buttons::Back))
        {
            Exit();
        }
    }

    bool PeerToPeerGame::IsPressed(Keys key, Buttons button) const
    {
        return currentKeyboardState.IsKeyDown(key) ||
               currentGamePadState.IsButtonDown(button);
    }

    void PeerToPeerGame::ReadTankInputs(Tank& tank, PlayerIndex playerIndex)
    {
        const GamePadState gamePad = GamePad::GetState(playerIndex);
        Vector2 tankInput = gamePad.getThumbSticksProperty().getLeftProperty();
        Vector2 turretInput = gamePad.getThumbSticksProperty().getRightProperty();

        const KeyboardState keyboard = Keyboard::GetState(playerIndex);

        if (keyboard.IsKeyDown(Keys::Left))
        {
            tankInput.X = -1.0f;
        }
        else if (keyboard.IsKeyDown(Keys::Right))
        {
            tankInput.X = 1.0f;
        }

        if (keyboard.IsKeyDown(Keys::Up))
        {
            tankInput.Y = 1.0f;
        }
        else if (keyboard.IsKeyDown(Keys::Down))
        {
            tankInput.Y = -1.0f;
        }

        if (keyboard.IsKeyDown(Keys::A))
        {
            turretInput.X = -1.0f;
        }
        else if (keyboard.IsKeyDown(Keys::D))
        {
            turretInput.X = 1.0f;
        }

        if (keyboard.IsKeyDown(Keys::W))
        {
            turretInput.Y = 1.0f;
        }
        else if (keyboard.IsKeyDown(Keys::S))
        {
            turretInput.Y = -1.0f;
        }

        if (tankInput.Length() > 1.0f)
        {
            tankInput.Normalize();
        }

        if (turretInput.Length() > 1.0f)
        {
            turretInput.Normalize();
        }

        tank.TankInput = tankInput;
        tank.TurretInput = turretInput;
    }

    std::string PeerToPeerGame::ToString(NetworkSessionEndReason reason)
    {
        switch (reason)
        {
            case NetworkSessionEndReason::ClientSignedOut:
                return "ClientSignedOut";
            case NetworkSessionEndReason::HostEndedSession:
                return "HostEndedSession";
            case NetworkSessionEndReason::RemovedByHost:
                return "RemovedByHost";
            case NetworkSessionEndReason::Disconnected:
                return "Disconnected";
            default:
                return "Unknown";
        }
    }

    std::string PeerToPeerGame::ReplaceAll(
        std::string value,
        const std::string& from,
        const std::string& to)
    {
        std::size_t offset = 0;

        while ((offset = value.find(from, offset)) != std::string::npos)
        {
            value.replace(offset, from.size(), to);
            offset += to.size();
        }

        return value;
    }
}
