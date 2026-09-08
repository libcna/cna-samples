#pragma once

// NetworkManager.hpp -- C++ port of Yacht/Misc/NetworkManager.cs.

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "Microsoft/Phone/Notification/HttpNotificationChannel.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/EventArgs.hpp"
#include "System/EventHandler.hpp"
#include "System/Guid.hpp"
#include "System/IDisposable.hpp"
#include "System/Object.hpp"
#include "System/Xml/Serialization/IXmlSerializable.hpp"
#include "System/Xml/XmlReader.hpp"
#include "System/Xml/XmlWriter.hpp"

#include "../YachtServices/DataModel.hpp"
#include "../YachtServices/ServiceConstants.hpp"
#include "../YachtServices/YachtServiceClient.hpp"
#include "YachtEventArgs.hpp"

namespace Yacht {

using Microsoft::Phone::Notification::HttpNotificationChannel;
using Microsoft::Phone::Notification::HttpNotificationEventArgs;
using Microsoft::Phone::Notification::NotificationChannelUriEventArgs;

/**
 * @brief The client's whole relationship with the game server.
 *
 * Everything the game knows about an online match goes through here: it holds the session, asks
 * the server to do things, and turns both the server's replies and its pushed notifications
 * into events the screens subscribe to. It is also serialized into the saved game, which is how
 * a session survives the application being moved to the background -- what is written is the
 * identity of the player and the game, not the connection, and reconnecting is what resuming
 * an online game means.
 *
 * @note **The proxy's completions and the channel's notifications are queued, and the game
 * drains them.** Windows Phone's generated proxy and its dispatcher raised both back on the UI
 * thread; here `DispatchPendingCompletions` does it, called once per frame from the game's own
 * thread. Nothing in this class runs on a worker thread except the calls themselves.
 */
class NetworkManager : public System::Object,
                       public System::Xml::Serialization::IXmlSerializable,
                       public System::IDisposable {
public:
    /**
     * @brief The single network manager.
     *
     * @return The instance, or null when one could not be created.
     */
    [[nodiscard]] static NetworkManager* getInstanceProperty()
    {
        static std::unique_ptr<NetworkManager> instance = [] {
            std::unique_ptr<NetworkManager> created;
            try {
                // Try to initialize new instance of NetworkManager
                created.reset(new NetworkManager("OneTimePatternChannel",
                                                 "YachtServices.YachtService"));
            } catch (...) {
                created.reset();
            }
            return created;
        }();
        return instance.get();
    }

    /** @brief The type's name. @return "NetworkManager". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "NetworkManager";
        return name;
    }

    /**
     * @brief Where the server is.
     *
     * @return The endpoint address, or an empty string before Connect.
     */
    [[nodiscard]] std::string getServerAddressProperty() const
    {
        return proxy_ != nullptr ? proxy_->getEndpointAddressProperty() : std::string();
    }

    /** @brief The player's session ID on the server. */
    int playerID = -1;

    /** @brief The game the player is in. */
    System::Guid gameID;

    /** @brief The player's name. */
    std::string name;

    /** @brief The name of the game the player is in. */
    std::string gameName;

    /** @brief Raised when registration with the server finishes. */
    System::EventHandler<BooleanEventArgs> Registered;
    /** @brief Raised when joining a game finishes. */
    System::EventHandler<BooleanEventArgs> JoinedGame;
    /** @brief Raised when leaving a game finishes. */
    System::EventHandler<BooleanEventArgs> LeftGame;
    /** @brief Raised when creating a game finishes. */
    System::EventHandler<BooleanEventArgs> NewGameCreated;
    /** @brief Raised when the player's score card arrives. */
    System::EventHandler<YachtScoreCardEventArgs> ScoreCardArrived;
    /** @brief Raised when a game state arrives, whether asked for or pushed. */
    System::EventHandler<YachtGameStateEventArgs> GameStateArrived;
    /** @brief Raised when the list of joinable games arrives. */
    System::EventHandler<YachtAvailableGamesEventArgs> AvailableGamesArrived;
    /** @brief Raised when the server bans this player from a game. */
    System::EventHandler<System::EventArgs> Banned;
    /** @brief Raised when the game the player asked about no longer exists. */
    System::EventHandler<System::EventArgs> GameUnavailable;
    /** @brief Raised when a game ends. */
    System::EventHandler<YachtGameOverEventArgs> GameOver;
    /** @brief Raised when contacting the server fails. */
    System::EventHandler<ExceptionEventArgs> ServiceError;

    /**
     * @brief Opens a proxy to the server and a channel for it to push to.
     *
     * @param playerName The player's name.
     */
    void Connect(const std::string& playerName)
    {
        name = playerName;

        // Create a proxy to the server
        proxy_ = std::make_unique<YachtServices::YachtServiceClient>();

        // Register to server communication events
        proxy_->RegisterCompleted +=
            [this](System::Object*, const YachtServices::RegisterCompletedEventArgs& e) {
                ProxyRegisterCompleted(e);
            };
        proxy_->JoinGameCompleted +=
            [this](System::Object*, const YachtServices::JoinGameCompletedEventArgs& e) {
                ProxyJoinGameCompleted(e);
            };
        proxy_->LeaveGameCompleted +=
            [this](System::Object*, const YachtServices::LeaveGameCompletedEventArgs& e) {
                ProxyLeaveGameCompleted(e);
            };
        proxy_->GetGameStateCompleted +=
            [this](System::Object*, const YachtServices::GetGameStateCompletedEventArgs& e) {
                ProxyGetGameStateCompleted(e);
            };
        proxy_->GetAvailableGamesCompleted +=
            [this](System::Object*, const YachtServices::GetAvailableGamesCompletedEventArgs& e) {
                ProxyGetAvailableGamesCompleted(e);
            };
        proxy_->NewGameCompleted +=
            [this](System::Object*, const YachtServices::NewGameCompletedEventArgs& e) {
                ProxyNewGameCompleted(e);
            };
        proxy_->GetScoreCardCompleted +=
            [this](System::Object*, const YachtServices::GetScoreCardCompletedEventArgs& e) {
                ProxyGetScoreCardCompleted(e);
            };
        proxy_->UnregisterCompleted +=
            [this](System::Object*, const YachtServices::AsyncCompletedEventArgs& e) {
                ProxyErrorChecked(e);
            };
        proxy_->ResetTimeoutCompleted +=
            [this](System::Object*, const YachtServices::AsyncCompletedEventArgs& e) {
                ProxyErrorChecked(e);
            };
        proxy_->GameStepCompleted +=
            [this](System::Object*, const YachtServices::AsyncCompletedEventArgs& e) {
                ProxyErrorChecked(e);
            };

        // Create the push notification channel
        InitializePushNotification(channelName_, serviceName_);
    }

    /** @brief Drops this client's registration with the server. */
    void Unregister()
    {
        gameID = System::Guid();
        name.clear();
        gameName.clear();

        if (proxy_ != nullptr) {
            proxy_->UnregisterAsync(playerID);
        }
    }

    /** @brief Leaves the game this player is in. */
    void LeaveGame()
    {
        gameID = System::Guid();
        gameName.clear();
        proxy_->LeaveGameAsync(playerID);
    }

    /**
     * @brief Reports a scoring step to the server.
     *
     * @param step What was scored.
     */
    void GameStep(const YachtServices::YachtStep& step)
    {
        proxy_->GameStepAsync(gameID, playerID, step.ScoreLine, step.Score, step.PlayerIndex,
                              step.StepNumber);
    }

    /**
     * @brief Joins a game.
     *
     * @param id The game to join.
     */
    void JoinGame(const System::Guid& id)
    {
        gameID = id;
        proxy_->JoinGameAsync(id, playerID);
    }

    /** @brief Asks the server for the current state of the player's game. */
    void GetGameState() { proxy_->GetGameStateAsync(gameID, playerID); }

    /**
     * @brief Asks the server to create a game.
     *
     * @param newGameName The game's name.
     */
    void NewGame(const std::string& newGameName)
    {
        gameName = newGameName;
        proxy_->NewGameAsync(playerID, newGameName);
    }

    /** @brief Asks the server which games are waiting for players. */
    void GetAvailableGames() { proxy_->GetAvailableGamesAsync(playerID); }

    /** @brief Tells the server the player is still there. */
    void ResetTimeout()
    {
        if (proxy_ != nullptr) {
            proxy_->ResetTimeoutAsync(gameID, playerID);
        }
    }

    /** @brief Asks the server for the player's own score card. */
    void GetScoreCard() { proxy_->GetScoreCardAsync(playerID); }

    /** @brief Closes the push channel, so the server can no longer reach this client. */
    void StopListeningToPushNotification()
    {
        if (channel_ != nullptr && channel_->getChannelUriProperty() != nullptr) {
            channel_->Close();
        }
    }

    /**
     * @brief Raises everything that has arrived since the last call.
     *
     * Called once per frame from the game's own thread. Both the proxy's completions and the
     * channel's notifications are drained here, and nowhere else.
     */
    void DispatchPendingCompletions()
    {
        if (proxy_ != nullptr) {
            proxy_->DispatchPendingCompletions();
        }
        if (channel_ != nullptr) {
            channel_->DispatchPendingNotificationsEXT();
        }
    }

    /**
     * @brief Reads the manager's persisted identity back.
     *
     * @param reader The xml reader from which to read.
     */
    void Deserialize(System::Xml::XmlReader& reader)
    {
        gameID = System::Guid(reader.GetAttribute("GameID"));
        try {
            playerID = std::stoi(reader.GetAttribute("PlayerID"));
        } catch (...) {
            // int.TryParse: an unparsable value leaves the field at zero rather than throwing.
            playerID = 0;
        }
        name = reader.GetAttribute("Name");
        gameName = reader.GetAttribute("GameName");

        // Read end element
        reader.Read();
    }

    /**
     * @brief Not implemented.
     *
     * @return Never returns.
     * @throws std::logic_error always, as the original throws NotImplementedException.
     */
    [[nodiscard]] System::Xml::Schema::XmlSchema* GetSchema() const override
    {
        throw std::logic_error("NetworkManager::GetSchema is not implemented.");
    }

    /**
     * @brief Reads the manager's persisted identity back.
     *
     * @param reader The xml reader from which to read.
     */
    void ReadXml(System::Xml::XmlReader& reader) override
    {
        reader.Read();
        Deserialize(reader);
    }

    /**
     * @brief Writes the manager's identity: who the player is and which game they are in.
     *
     * @param writer The xml writer to use when writing.
     */
    void WriteXml(System::Xml::XmlWriter& writer) const override
    {
        writer.WriteStartElement("NetworkManager");

        writer.WriteAttributeString("GameID", gameID.ToString());
        writer.WriteAttributeString("PlayerID", std::to_string(playerID));
        writer.WriteAttributeString("Name", name);
        writer.WriteAttributeString("GameName", gameName);

        writer.WriteEndElement();
    }

    /** @brief Performs necessary cleanup. */
    void Dispose() override
    {
        if (channel_ != nullptr) {
            // Dispose the push notification channel
            channel_->Close();
        }
    }

private:
    NetworkManager(std::string channelName, std::string serviceName)
        : channelName_(std::move(channelName)), serviceName_(std::move(serviceName))
    {
    }

    // Reads the message the server sent. The server's payload wraps the Message element in a
    // second one, so the reader steps past the declaration and then past that wrapper before
    // handing over -- which is why this is not simply Message::ReadXml.
    static std::shared_ptr<YachtServices::Message> ReadServerMessage(
        const std::vector<SharpRuntime::bytecs>& bytes)
    {
        std::string document(bytes.begin(), bytes.end());
        // The server writes a UTF-8 byte order mark; the reader is given the document without it.
        if (document.size() >= 3 && static_cast<unsigned char>(document[0]) == 0xEF &&
            static_cast<unsigned char>(document[1]) == 0xBB &&
            static_cast<unsigned char>(document[2]) == 0xBF) {
            document.erase(0, 3);
        }

        std::unique_ptr<System::Xml::XmlReader> reader(
            System::Xml::XmlReader::CreateFromString(document));

        // Read past xml declaration
        reader->Read();
        // Read the wrapper node created by xml serialization
        reader->Read();

        // Read the data into an object
        auto message = std::make_shared<YachtServices::Message>();
        message->ReadXml(*reader);
        return message;
    }

    void InitializePushNotification(const std::string& channelName, const std::string& serviceName)
    {
        try {
            // Look for an already existing channel
            channel_ = HttpNotificationChannel::Find(channelName);

            if (channel_ == nullptr) {
                // Create a new channel and open it
                ownedChannel_ = std::make_unique<HttpNotificationChannel>(channelName, serviceName);
                channel_ = ownedChannel_.get();
                SubscribeToChannel();
                channel_->Open();
            } else {
                // Register the client using the existing channel
                SubscribeToChannel();
                RegisterForPushNotifications();
            }
        } catch (...) {
            ExceptionEventArgs args;
            args.Error = std::current_exception();
            ServiceError.Raise(this, args);
        }
    }

    void SubscribeToChannel()
    {
        channel_->ChannelUriUpdated +=
            [this](System::Object* sender, const NotificationChannelUriEventArgs&) {
                ChannelUriUpdated(sender);
            };
        channel_->HttpNotificationReceived +=
            [this](System::Object*, const HttpNotificationEventArgs& e) {
                HttpNotificationReceived(e);
            };
    }

    void RegisterForPushNotifications()
    {
        proxy_->RegisterAsync(*channel_->getChannelUriProperty(), name, playerID);
    }

    void ChannelUriUpdated(System::Object*)
    {
        RegisterForPushNotifications();

        if (!channel_->getIsShellToastBoundProperty()) {
            channel_->BindToShellToast();
        }
    }

    void HttpNotificationReceived(const HttpNotificationEventArgs& e)
    {
        // Reads the stream into an object
        std::shared_ptr<YachtServices::Message> message;
        try {
            message = ReadServerMessage(e.getBodyProperty());
        } catch (...) {
            ExceptionEventArgs args;
            args.Error = std::current_exception();
            ServiceError.Raise(this, args);
            return;
        }

        if (message->SequenceNumber > lastMessageSequenceNumber_) {
            lastMessageSequenceNumber_ = message->SequenceNumber;
        } else if (message->SequenceNumber != -1) {
            // An out-of-order push is dropped: the server numbers them, and a state older than
            // one already applied would move the game backwards. -1 means "not numbered".
            return;
        }

        // Parse the message body according to its type
        switch (message->ContentType) {
        case YachtServices::MessageContentType::GameState:
            NewGameState(std::dynamic_pointer_cast<YachtServices::GameState>(message->Body));
            break;
        case YachtServices::MessageContentType::SimpleType:
            NewSimpleType(std::dynamic_pointer_cast<YachtServices::SimpleType>(message->Body));
            break;
        case YachtServices::MessageContentType::EndGameInformation:
            NewEndGameState(
                std::dynamic_pointer_cast<YachtServices::EndGameInformation>(message->Body));
            break;
        default:
            break;
        }
    }

    void NewSimpleType(const std::shared_ptr<YachtServices::SimpleType>& simpleType)
    {
        if (simpleType == nullptr) {
            return;
        }
        // Parse the message according to its name
        if (simpleType->Name == YachtServices::ServiceConstants::BannedMessageString) {
            NewBanned();
        }
    }

    void NewBanned()
    {
        System::EventArgs empty;
        Banned.Raise(this, empty);
    }

    void NewAvailableGames(const std::shared_ptr<YachtServices::AvailableGames>& availableGames)
    {
        YachtAvailableGamesEventArgs args;
        args.AvailableGames = availableGames;
        AvailableGamesArrived.Raise(this, args);
    }

    void NewGameState(const std::shared_ptr<YachtServices::GameState>& gameState)
    {
        YachtGameStateEventArgs args;
        args.GameState = gameState;
        GameStateArrived.Raise(this, args);
    }

    void NewEndGameState(const std::shared_ptr<YachtServices::EndGameInformation>& endGameState)
    {
        YachtGameOverEventArgs args;
        args.EndGameState = endGameState;
        GameOver.Raise(this, args);
    }

    bool RaiseServiceErrorIfFailed(const std::exception_ptr& error)
    {
        if (!error) {
            return false;
        }
        ExceptionEventArgs args;
        args.Error = error;
        ServiceError.Raise(this, args);
        return true;
    }

    void ProxyErrorChecked(const YachtServices::AsyncCompletedEventArgs& e)
    {
        (void)RaiseServiceErrorIfFailed(e.Error);
    }

    void ProxyRegisterCompleted(const YachtServices::RegisterCompletedEventArgs& e)
    {
        if (RaiseServiceErrorIfFailed(e.Error)) {
            return;
        }
        playerID = e.Result;
        // Raise a notification about the registration
        BooleanEventArgs args;
        args.Answer = e.Result != -1;
        Registered.Raise(this, args);
    }

    void ProxyJoinGameCompleted(const YachtServices::JoinGameCompletedEventArgs& e)
    {
        if (RaiseServiceErrorIfFailed(e.Error)) {
            return;
        }
        BooleanEventArgs args;
        args.Answer = e.Result;
        JoinedGame.Raise(this, args);
    }

    void ProxyLeaveGameCompleted(const YachtServices::LeaveGameCompletedEventArgs& e)
    {
        if (RaiseServiceErrorIfFailed(e.Error)) {
            return;
        }
        BooleanEventArgs args;
        args.Answer = e.Result;
        LeftGame.Raise(this, args);
    }

    void ProxyNewGameCompleted(const YachtServices::NewGameCompletedEventArgs& e)
    {
        if (RaiseServiceErrorIfFailed(e.Error)) {
            return;
        }
        if (e.Result.has_value() && e.Result->size() == 16) {
            // NewGame answers with Guid.ToByteArray(), not a serialized message.
            gameID = GuidFromDotNetBytes(*e.Result);
        }
        lastMessageSequenceNumber_ = -1;
        BooleanEventArgs args;
        args.Answer = e.Result.has_value();
        NewGameCreated.Raise(this, args);
    }

    void ProxyGetGameStateCompleted(const YachtServices::GetGameStateCompletedEventArgs& e)
    {
        if (RaiseServiceErrorIfFailed(e.Error)) {
            return;
        }
        if (e.Result.has_value()) {
            NewGameState(std::dynamic_pointer_cast<YachtServices::GameState>(
                ReadServerMessage(*e.Result)->Body));
        } else {
            System::EventArgs empty;
            GameUnavailable.Raise(this, empty);
        }
    }

    void ProxyGetAvailableGamesCompleted(
        const YachtServices::GetAvailableGamesCompletedEventArgs& e)
    {
        if (RaiseServiceErrorIfFailed(e.Error)) {
            return;
        }
        if (e.Result.has_value()) {
            NewAvailableGames(std::dynamic_pointer_cast<YachtServices::AvailableGames>(
                ReadServerMessage(*e.Result)->Body));
        }
    }

    void ProxyGetScoreCardCompleted(const YachtServices::GetScoreCardCompletedEventArgs& e)
    {
        if (RaiseServiceErrorIfFailed(e.Error)) {
            return;
        }
        YachtScoreCardEventArgs args;
        if (e.Result.has_value()) {
            args.ScoreCard = *e.Result;
        }
        ScoreCardArrived.Raise(this, args);
    }

    // .NET's Guid(byte[]) reads the first three fields little-endian and the last eight in
    // order, which is not the order the canonical text form prints them in.
    static System::Guid GuidFromDotNetBytes(const std::vector<SharpRuntime::bytecs>& bytes)
    {
        static const char* digits = "0123456789abcdef";
        const int order[16] = {3, 2, 1, 0, 5, 4, 7, 6, 8, 9, 10, 11, 12, 13, 14, 15};
        std::string text;
        text.reserve(36);
        for (int i = 0; i < 16; i++) {
            if (i == 4 || i == 6 || i == 8 || i == 10) {
                text.push_back('-');
            }
            const auto value = static_cast<unsigned char>(bytes[static_cast<std::size_t>(order[i])]);
            text.push_back(digits[value >> 4]);
            text.push_back(digits[value & 0x0F]);
        }
        return System::Guid(text);
    }

    std::unique_ptr<HttpNotificationChannel> ownedChannel_;
    HttpNotificationChannel* channel_ = nullptr;
    std::unique_ptr<YachtServices::YachtServiceClient> proxy_;

    std::string channelName_;
    std::string serviceName_;
    int lastMessageSequenceNumber_ = 0;
};

} // namespace Yacht
