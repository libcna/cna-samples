// SPDX-License-Identifier: MS-PL
#pragma once

#include <any>
#include <regex>
#include <string>
#include <vector>

#include "System/IAsyncResult.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameComponent.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Gamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesDispatcher.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamerCollection.hpp"
#include "Microsoft/Xna/Framework/Net/AvailableNetworkSessionCollection.hpp"
#include "Microsoft/Xna/Framework/Net/LocalNetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionEndedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionProperties.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionType.hpp"
#include "Microsoft/Xna/Framework/Net/PacketReader.hpp"
#include "Microsoft/Xna/Framework/Net/PacketWriter.hpp"
#include "Microsoft/Xna/Framework/Net/SendDataOptions.hpp"

#include "DebugCommandUI.hpp"
#include "IDebugCommandHost.hpp"

namespace PerformanceUtility::GameDebugTools
{
    using Microsoft::Xna::Framework::Game;
    using Microsoft::Xna::Framework::GameComponent;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GamerServices::Gamer;
    using Microsoft::Xna::Framework::GamerServices::GamerServicesDispatcher;
    using Microsoft::Xna::Framework::Net::AvailableNetworkSessionCollection;
    using Microsoft::Xna::Framework::Net::LocalNetworkGamer;
    using Microsoft::Xna::Framework::Net::NetworkGamer;
    using Microsoft::Xna::Framework::Net::NetworkSession;
    using Microsoft::Xna::Framework::Net::NetworkSessionEndedEventArgs;
    using Microsoft::Xna::Framework::Net::NetworkSessionProperties;
    using Microsoft::Xna::Framework::Net::NetworkSessionType;
    using Microsoft::Xna::Framework::Net::PacketReader;
    using Microsoft::Xna::Framework::Net::PacketWriter;
    using Microsoft::Xna::Framework::Net::SendDataOptions;

    /** @brief SystemLink component that forwards debug commands between a Windows client and host. */
    class RemoteDebugCommand : public GameComponent,
                               public IDebugCommandExecutioner,
                               public IDebugEchoListner
    {
    public:
        explicit RemoteDebugCommand(Game& game)
            : GameComponent(game)
        {
            commandHost = game.getServicesProperty().GetService<IDebugCommandHost>();
            if (!IsHost)
            {
                commandHost->RegisterCommand(
                    "remote", "Start remote command",
                    [this](IDebugCommandHost& host, const std::string& command,
                           const std::vector<std::string>& arguments)
                    {
                        ExecuteRemoteCommand(host, command, arguments);
                    });
            }
        }

        ~RemoteDebugCommand() override
        {
            if (ownsNetworkSession && networkSession != nullptr)
            {
                networkSession->Dispose();
                delete networkSession;
            }
        }

        [[nodiscard]] NetworkSession* getNetworkSessionProperty() const { return networkSession; }
        void setNetworkSessionProperty(NetworkSession* value) { networkSession = value; }
        [[nodiscard]] bool getOwnsNetworkSessionProperty() const { return ownsNetworkSession; }

        void Initialize() override
        {
            if (IsHost)
            {
                commandHost->RegisterEchoListner(this);
                if (networkSession == nullptr)
                {
                    GamerServicesDispatcher::setWindowHandleProperty(
                        getGameProperty().getWindowProperty().getHandleProperty());
                    GamerServicesDispatcher::Initialize(getGameProperty().getServicesProperty());
                    networkSession = NetworkSession::Create(NetworkSessionType::SystemLink, 1, 2);
                    ownsNetworkSession = true;
                }
            }

            GameComponent::Initialize();
        }

        [[nodiscard]] bool ProcessRecievedPacket(const std::string& packetString)
        {
            std::smatch match;
            if (!std::regex_search(packetString, match, packetRe))
                return false;

            const std::string packetHeader = match[1].str();
            const std::string text = match[2].str();
            if (packetHeader == ExecutePacketHeader)
                commandHost->ExecuteCommand(text);
            else if (packetHeader == EchoPacketHeader)
                commandHost->Echo(text);
            else if (packetHeader == ErrorPacketHeader)
                commandHost->EchoError(text);
            else if (packetHeader == WarningPacketHeader)
                commandHost->EchoWarning(text);
            else if (packetHeader == StartPacketHeader)
            {
                ConnectedToRemote();
                commandHost->Echo(text);
            }
            else if (packetHeader == QuitPacketHeader)
            {
                commandHost->Echo(text);
                DisconnectedFromRemote();
            }
            else
                return false;

            return true;
        }

        void Update(GameTime& gameTime) override
        {
            switch (phase)
            {
            case ConnectionPahse::EnsureSignedIn:
                GamerServicesDispatcher::Update();
                break;

            case ConnectionPahse::FindSessions:
                GamerServicesDispatcher::Update();
                if (asyncResult->getIsCompletedProperty())
                {
                    AvailableNetworkSessionCollection sessions = NetworkSession::EndFind(asyncResult);
                    if (sessions.getCountProperty() > 0)
                    {
                        asyncResult = NetworkSession::BeginJoin(
                            &sessions.getItem(0), System::AsyncCallback{}, std::any{});
                        commandHost->EchoError("Connecting to the host...");
                        phase = ConnectionPahse::Joining;
                    }
                    else
                    {
                        commandHost->EchoError("Couldn't find a session.");
                        phase = ConnectionPahse::None;
                    }
                }
                break;

            case ConnectionPahse::Joining:
                GamerServicesDispatcher::Update();
                if (asyncResult->getIsCompletedProperty())
                {
                    networkSession = NetworkSession::EndJoin(asyncResult);
                    networkSession->SessionEnded +=
                        [this](System::Object* sender, const NetworkSessionEndedEventArgs& eventArgs)
                        {
                            NetworkSession_SessionEnded(sender, eventArgs);
                        };
                    ownsNetworkSession = true;
                    commandHost->EchoError("Connected to the host.");
                    phase = ConnectionPahse::None;
                    asyncResult = nullptr;
                    ConnectedToRemote();
                }
                break;

            case ConnectionPahse::None:
                break;
            }

            if (ownsNetworkSession)
            {
                GamerServicesDispatcher::Update();
                networkSession->Update();
                if (networkSession != nullptr)
                {
                    for (LocalNetworkGamer* gamer : networkSession->getLocalGamersProperty())
                    {
                        while (gamer->getIsDataAvailableProperty())
                        {
                            NetworkGamer* sender = nullptr;
                            gamer->ReceiveData(packetReader, sender);
                            if (!sender->getIsLocalProperty())
                                (void)ProcessRecievedPacket(packetReader.ReadString());
                        }
                    }
                }
            }

            GameComponent::Update(gameTime);
        }

        void ExecuteCommand(const std::string& command) override
        {
            SendPacket(ExecutePacketHeader, command);
        }

        void Echo(DebugCommandMessage messageType, const std::string& text) override
        {
            switch (messageType)
            {
            case DebugCommandMessage::Standard:
                SendPacket(EchoPacketHeader, text);
                break;
            case DebugCommandMessage::Warning:
                SendPacket(WarningPacketHeader, text);
                break;
            case DebugCommandMessage::Error:
                SendPacket(ErrorPacketHeader, text);
                break;
            }
        }

    private:
        static constexpr const char* StartPacketHeader = "RmtStart";
        static constexpr const char* ExecutePacketHeader = "RmtCmd";
        static constexpr const char* EchoPacketHeader = "RmtEcho";
        static constexpr const char* ErrorPacketHeader = "RmtErr";
        static constexpr const char* WarningPacketHeader = "RmtWrn";
        static constexpr const char* QuitPacketHeader = "RmtQuit";

#if defined(WINDOWS)
        static constexpr bool IsHost = false;
#else
        static constexpr bool IsHost = true;
#endif

        enum class ConnectionPahse
        {
            None,
            EnsureSignedIn,
            FindSessions,
            Joining,
        };

        void SendPacket(const std::string& header, const std::string& text)
        {
            if (networkSession != nullptr)
            {
                packetWriter.Write("$" + header + "$:" + text);
                networkSession->getLocalGamersProperty()[0]->SendData(
                    packetWriter, SendDataOptions::ReliableInOrder);
            }
        }

        void ConnectedToRemote()
        {
            auto* commandUI = dynamic_cast<DebugCommandUI*>(commandHost);
            if (IsHost)
            {
                if (commandUI != nullptr)
                    commandUI->setPromptProperty("[Host]>");
            }
            else
            {
                if (commandUI != nullptr)
                    commandUI->setPromptProperty("[Client]>");
                commandHost->PushExecutioner(this);
                SendPacket(StartPacketHeader, "Remote Debug Command Started!!");
            }

            commandHost->RegisterCommand(
                "quit", "Quit from remote command",
                [this](IDebugCommandHost& host, const std::string& command,
                       const std::vector<std::string>& arguments)
                {
                    ExecuteQuitCommand(host, command, arguments);
                });
        }

        void DisconnectedFromRemote()
        {
            if (auto* commandUI = dynamic_cast<DebugCommandUI*>(commandHost))
                commandUI->setPromptProperty(DebugCommandUI::DefaultPrompt);

            commandHost->UnregisterCommand("quit");
            if (!IsHost)
            {
                commandHost->PopExecutioner();
                if (ownsNetworkSession)
                {
                    networkSession->Dispose();
                    delete networkSession;
                    networkSession = nullptr;
                    ownsNetworkSession = false;
                }
            }
        }

        void ExecuteRemoteCommand(IDebugCommandHost& host, const std::string&,
                                  const std::vector<std::string>&)
        {
            if (networkSession == nullptr)
            {
                try
                {
                    GamerServicesDispatcher::setWindowHandleProperty(
                        getGameProperty().getWindowProperty().getHandleProperty());
                    GamerServicesDispatcher::Initialize(getGameProperty().getServicesProperty());
                }
                catch (...)
                {
                }

                if (Gamer::getSignedInGamersProperty()->getCountProperty() > 0)
                {
                    commandHost->Echo("Finding available sessions...");
                    asyncResult = NetworkSession::BeginFind(
                        NetworkSessionType::SystemLink, 1, NetworkSessionProperties{},
                        System::AsyncCallback{}, std::any{});
                    phase = ConnectionPahse::FindSessions;
                }
                else
                {
                    host.Echo("Please signed in.");
                    phase = ConnectionPahse::EnsureSignedIn;
                }
            }
            else
            {
                ConnectedToRemote();
            }
        }

        void ExecuteQuitCommand(IDebugCommandHost&, const std::string&,
                                const std::vector<std::string>&)
        {
            SendPacket(QuitPacketHeader, "End Remote Debug Command.");
            DisconnectedFromRemote();
        }

        void NetworkSession_SessionEnded(System::Object*, const NetworkSessionEndedEventArgs&)
        {
            DisconnectedFromRemote();
            commandHost->EchoWarning("Disconnected from the Host.");
        }

        IDebugCommandHost* commandHost = nullptr;
        NetworkSession* networkSession = nullptr;
        bool ownsNetworkSession = false;
        const std::regex packetRe{R"(\$([^$]+)\$:(.+))"};
        PacketReader packetReader;
        PacketWriter packetWriter;
        System::IAsyncResult* asyncResult = nullptr;
        ConnectionPahse phase = ConnectionPahse::None;
    };
}
