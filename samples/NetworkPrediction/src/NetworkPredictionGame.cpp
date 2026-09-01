// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// NetworkPredictionGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "NetworkPredictionGame.hpp"

#include <any>
#include <cstddef>
#include <utility>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameComponentCollection.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Gamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesComponent.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamerCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Net/AvailableNetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/AvailableNetworkSessionCollection.hpp"
#include "Microsoft/Xna/Framework/Net/GamerJoinedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/LocalNetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionEndReason.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionEndedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionProperties.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionType.hpp"
#include "Microsoft/Xna/Framework/Net/SendDataOptions.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/Exception.hpp"
#include "System/Int32.hpp"
#include "System/TimeSpan.hpp"
#include "Tank.hpp"

namespace NetworkPrediction
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::GamerServices;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;
    using namespace Microsoft::Xna::Framework::Net;

    NetworkPredictionGame::NetworkPredictionGame()
    {
        graphics = std::make_unique<GraphicsDeviceManager>(this);
        graphics->setPreferredBackBufferWidthProperty(screenWidth);
        graphics->setPreferredBackBufferHeightProperty(screenHeight);

        getContentProperty().setRootDirectoryProperty("Content");

        gamerServices = std::make_unique<GamerServicesComponent>(*this);
        getComponentsProperty().Add(gamerServices.get());
    }

    NetworkPredictionGame::~NetworkPredictionGame() = default;

    const std::string& NetworkPredictionGame::GetTypeName() const
    {
        static const std::string name = "NetworkPrediction.NetworkPredictionGame";
        return name;
    }

    void NetworkPredictionGame::LoadContent()
    {
        spriteBatch.emplace(getGraphicsDeviceProperty());
        font.emplace(getContentProperty().Load<SpriteFont>("Font"));
    }

    void NetworkPredictionGame::Update(GameTime& gameTime)
    {
        HandleInput();

        if (networkSession == nullptr)
        {
            UpdateMenuScreen();
        }
        else
        {
            UpdateNetworkSession(gameTime);
        }

        Game::Update(gameTime);
    }

    void NetworkPredictionGame::UpdateMenuScreen()
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

    void NetworkPredictionGame::CreateSession()
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
        catch (const System::Exception& exception)
        {
            errorMessage = exception.getMessageProperty();
        }
    }

    void NetworkPredictionGame::JoinSession()
    {
        DrawMessage("Joining session...");

        try
        {
            AvailableNetworkSessionCollection availableSessions = NetworkSession::Find(
                NetworkSessionType::SystemLink,
                maxLocalGamers,
                NetworkSessionProperties());

            if (availableSessions.getCountProperty() == 0)
            {
                errorMessage = "No network sessions found.";
                availableSessions.Dispose();
                return;
            }

            const AvailableNetworkSession& availableSession = availableSessions.getItem(0);
            networkSession.reset(NetworkSession::Join(&availableSession));
            HookSessionEvents();
            availableSessions.Dispose();
        }
        catch (const System::Exception& exception)
        {
            errorMessage = exception.getMessageProperty();
        }
    }

    void NetworkPredictionGame::HookSessionEvents()
    {
        networkSession->GamerJoined += [this](System::Object* sender, const GamerJoinedEventArgs& eventArgs)
        {
            GamerJoinedEventHandler(sender, eventArgs);
        };
        networkSession->SessionEnded +=
            [this](System::Object* sender, const NetworkSessionEndedEventArgs& eventArgs)
        {
            SessionEndedEventHandler(sender, eventArgs);
        };
    }

    void NetworkPredictionGame::GamerJoinedEventHandler(
        System::Object* /*sender*/,
        const GamerJoinedEventArgs& eventArgs)
    {
        NetworkGamer* gamer = eventArgs.getGamerProperty();
        const int gamerIndex = networkSession->getAllGamersProperty().IndexOf(gamer);

        tanks.push_back(std::make_unique<Tank>(
            gamerIndex,
            getContentProperty(),
            screenWidth,
            screenHeight));
        gamer->setTagProperty(std::any(tanks.back().get()));
    }

    void NetworkPredictionGame::SessionEndedEventHandler(
        System::Object* /*sender*/,
        const NetworkSessionEndedEventArgs& eventArgs)
    {
        errorMessage = ToString(eventArgs.getEndReasonProperty());

        networkSession->Dispose();
        sessionEndedDuringUpdate = true;
    }

    void NetworkPredictionGame::UpdateNetworkSession(GameTime& gameTime)
    {
        bool sendPacketThisFrame = false;

        ++framesSinceLastSend;

        if (framesSinceLastSend >= framesBetweenPackets)
        {
            sendPacketThisFrame = true;
            framesSinceLastSend = 0;
        }

        for (LocalNetworkGamer* gamer : networkSession->getLocalGamersProperty())
        {
            UpdateLocalGamer(gamer, gameTime, sendPacketThisFrame);
        }

        try
        {
            networkSession->Update();
        }
        catch (const System::Exception& exception)
        {
            errorMessage = exception.getMessageProperty();
            networkSession->Dispose();
            networkSession.reset();
            tanks.clear();
        }

        if (networkSession == nullptr)
        {
            return;
        }

        if (sessionEndedDuringUpdate)
        {
            networkSession.reset();
            tanks.clear();
            sessionEndedDuringUpdate = false;
            return;
        }

        for (LocalNetworkGamer* gamer : networkSession->getLocalGamersProperty())
        {
            ReadIncomingPackets(gamer, gameTime);
        }

        for (NetworkGamer* gamer : networkSession->getRemoteGamersProperty())
        {
            Tank* tank = std::any_cast<Tank*>(gamer->getTagProperty());
            tank->UpdateRemote(framesBetweenPackets, enablePrediction);
        }

        UpdateOptions();
    }

    void NetworkPredictionGame::UpdateLocalGamer(
        LocalNetworkGamer* gamer,
        const GameTime& gameTime,
        bool sendPacketThisFrame)
    {
        Tank* tank = std::any_cast<Tank*>(gamer->getTagProperty());

        const PlayerIndex playerIndex = gamer->getSignedInGamerProperty()->getPlayerIndexProperty();

        Vector2 tankInput;
        Vector2 turretInput;
        ReadTankInputs(playerIndex, tankInput, turretInput);

        tank->UpdateLocal(tankInput, turretInput);

        if (sendPacketThisFrame)
        {
            tank->WriteNetworkPacket(packetWriter, gameTime);
            gamer->SendData(packetWriter, SendDataOptions::InOrder);
        }
    }

    void NetworkPredictionGame::ReadIncomingPackets(
        LocalNetworkGamer* gamer,
        const GameTime& gameTime)
    {
        while (gamer->getIsDataAvailableProperty())
        {
            NetworkGamer* sender = nullptr;
            gamer->ReceiveData(packetReader, sender);

            if (sender->getIsLocalProperty())
            {
                continue;
            }

            Tank* tank = std::any_cast<Tank*>(sender->getTagProperty());

            TimeSpan latency = networkSession->getSimulatedLatencyProperty()
                + TimeSpan::FromTicks(sender->getRoundtripTimeProperty().getTicksProperty() / 2);

            tank->ReadNetworkPacket(
                packetReader,
                gameTime,
                latency,
                enablePrediction,
                enableSmoothing);
        }
    }

    void NetworkPredictionGame::UpdateOptions()
    {
        if (networkSession->getIsHostProperty())
        {
            if (IsPressed(Keys::A, Buttons::A))
            {
                networkQuality = static_cast<NetworkQuality>(static_cast<int>(networkQuality) + 1);

                if (networkQuality > NetworkQuality::Perfect)
                {
                    networkQuality = NetworkQuality::Typical;
                }
            }

            if (IsPressed(Keys::B, Buttons::B))
            {
                if (framesBetweenPackets == 6)
                {
                    framesBetweenPackets = 3;
                }
                else if (framesBetweenPackets == 3)
                {
                    framesBetweenPackets = 1;
                }
                else
                {
                    framesBetweenPackets = 6;
                }
            }

            if (IsPressed(Keys::X, Buttons::X))
            {
                enablePrediction = !enablePrediction;
            }

            if (IsPressed(Keys::Y, Buttons::Y))
            {
                enableSmoothing = !enableSmoothing;
            }

            NetworkSessionProperties& properties = networkSession->getSessionPropertiesProperty();
            properties[0] = static_cast<int>(networkQuality);
            properties[1] = framesBetweenPackets;
            properties[2] = enablePrediction ? 1 : 0;
            properties[3] = enableSmoothing ? 1 : 0;
        }
        else
        {
            const NetworkSession& session = *networkSession;
            const NetworkSessionProperties& properties = session.getSessionPropertiesProperty();
            networkQuality = static_cast<NetworkQuality>(properties.getItem(0).value());
            framesBetweenPackets = properties.getItem(1).value();
            enablePrediction = properties.getItem(2) != 0;
            enableSmoothing = properties.getItem(3) != 0;
        }

        switch (networkQuality)
        {
            case NetworkQuality::Typical:
                networkSession->setSimulatedLatencyProperty(TimeSpan::FromMilliseconds(100));
                networkSession->setSimulatedPacketLossProperty(0.1f);
                break;

            case NetworkQuality::Poor:
                networkSession->setSimulatedLatencyProperty(TimeSpan::FromMilliseconds(200));
                networkSession->setSimulatedPacketLossProperty(0.2f);
                break;

            case NetworkQuality::Perfect:
                networkSession->setSimulatedLatencyProperty(TimeSpan::Zero);
                networkSession->setSimulatedPacketLossProperty(0.0f);
                break;
        }
    }

    void NetworkPredictionGame::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

        if (networkSession == nullptr)
        {
            DrawMenuScreen();
        }
        else
        {
            DrawNetworkSession();
        }

        Game::Draw(gameTime);
    }

    void NetworkPredictionGame::DrawMenuScreen()
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

    void NetworkPredictionGame::DrawNetworkSession()
    {
        spriteBatch->Begin();

        DrawOptions();

        for (NetworkGamer* gamer : networkSession->getAllGamersProperty())
        {
            Tank* tank = std::any_cast<Tank*>(gamer->getTagProperty());
            tank->Draw(*spriteBatch);

            spriteBatch->DrawString(
                *font,
                gamer->getGamertagProperty(),
                tank->getPositionProperty(),
                Color::Black,
                0.0f,
                Vector2(100.0f, 150.0f),
                0.6f,
                SpriteEffects::None,
                0.0f);
        }

        spriteBatch->End();
    }

    void NetworkPredictionGame::DrawOptions()
    {
        std::string quality = "Network simulation = "
            + System::Int32::ToString(static_cast<int>(
                networkSession->getSimulatedLatencyProperty().getTotalMillisecondsProperty()))
            + " ms, "
            + System::Int32::ToString(static_cast<int>(
                networkSession->getSimulatedPacketLossProperty() * 100.0f))
            + "% packet loss";

        std::string sendRate = "Packets per second = "
            + System::Int32::ToString(60 / framesBetweenPackets);
        std::string prediction = std::string("Prediction = ") + (enablePrediction ? "on" : "off");
        std::string smoothing = std::string("Smoothing = ") + (enableSmoothing ? "on" : "off");

        if (networkSession->getIsHostProperty())
        {
            quality += " (A to change)";
            sendRate += " (B to change)";
            prediction += " (X to toggle)";
            smoothing += " (Y to toggle)";
        }

        const std::string message = quality + "\n" + sendRate + "\n" + prediction + "\n" + smoothing;

        spriteBatch->DrawString(*font, message, Vector2(161.0f, 321.0f), Color::Black);
        spriteBatch->DrawString(*font, message, Vector2(160.0f, 320.0f), Color::White);
    }

    void NetworkPredictionGame::DrawMessage(const std::string& message)
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

    void NetworkPredictionGame::HandleInput()
    {
        previousKeyboardState = currentKeyboardState;
        previousGamePadState = currentGamePadState;

        currentKeyboardState = Keyboard::GetState();
        currentGamePadState = GamePad::GetState(PlayerIndex::One);

        if (getIsActiveProperty() && IsPressed(Keys::Escape, Buttons::Back))
        {
            Exit();
        }
    }

    bool NetworkPredictionGame::IsPressed(Keys key, Buttons button) const
    {
        return (currentKeyboardState.IsKeyDown(key) && previousKeyboardState.IsKeyUp(key))
            || (currentGamePadState.IsButtonDown(button) && previousGamePadState.IsButtonUp(button));
    }

    void NetworkPredictionGame::ReadTankInputs(
        PlayerIndex playerIndex,
        Vector2& tankInput,
        Vector2& turretInput)
    {
        const GamePadState gamePad = GamePad::GetState(playerIndex);

        tankInput = gamePad.getThumbSticksProperty().getLeftProperty();
        turretInput = gamePad.getThumbSticksProperty().getRightProperty();

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

        if (keyboard.IsKeyDown(Keys::K))
        {
            turretInput.X = -1.0f;
        }
        else if (keyboard.IsKeyDown(Keys::OemSemicolon))
        {
            turretInput.X = 1.0f;
        }

        if (keyboard.IsKeyDown(Keys::O))
        {
            turretInput.Y = 1.0f;
        }
        else if (keyboard.IsKeyDown(Keys::L))
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
    }

    std::string NetworkPredictionGame::ToString(NetworkSessionEndReason reason)
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

    std::string NetworkPredictionGame::ReplaceAll(
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
