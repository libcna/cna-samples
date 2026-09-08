// YachtService.cpp -- C++ port of YachtServices/YachtService.cs.

#include "YachtService.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>

#include "Microsoft/Phone/Notification/PushNotificationSender.hpp"

namespace YachtServices {

using Microsoft::Phone::Notification::MessageSendPriority;
using Microsoft::Phone::Notification::RawPushNotificationMessage;
using Microsoft::Phone::Notification::ToastPushNotificationMessage;

namespace {

void SendRawMessage(const System::Uri& clientURI,
                    const std::vector<SharpRuntime::bytecs>& rawData)
{
    RawPushNotificationMessage message(MessageSendPriority::High);
    message.RawData.assign(rawData.begin(), rawData.end());
    (void)message.SendAsync(clientURI);
}

void SendToastMessage(const System::Uri& clientURI, const std::string& toastTitle)
{
    ToastPushNotificationMessage toastMessage(MessageSendPriority::High);
    toastMessage.Title = toastTitle;
    (void)toastMessage.SendAsync(clientURI);
}

} // namespace

YachtService::~YachtService()
{
    const std::lock_guard<std::recursive_mutex> lock(subscribersSync_);
    for (auto& [id, game] : gameStates_) {
        game->TimerToDelete.reset();
        for (auto& player : game->Players) {
            player.Timer.reset();
        }
    }
}

GameState* YachtService::FindGame(const System::Guid& gameID)
{
    const auto found = gameStates_.find(gameID);
    return found == gameStates_.end() ? nullptr : found->second.get();
}

Subscription* YachtService::FindSubscriber(int sessionID)
{
    const auto found = subscribers_.find(sessionID);
    return found == subscribers_.end() ? nullptr : found->second.get();
}

std::vector<SharpRuntime::bytecs> YachtService::NewScoreCard()
{
    return std::vector<SharpRuntime::bytecs>(12, ServiceConstants::NullScore);
}

int YachtService::Register(const System::Uri& clientURI, const std::string& name, int playerID)
{
    const std::lock_guard<std::recursive_mutex> lock(subscribersSync_);

    int playerIDByserver = -1;

    if (auto* existing = FindSubscriber(playerID); existing != nullptr && existing->Name == name) {
        existing->ChannelUri = clientURI;
        playerIDByserver = playerID;
    } else {
        auto subscription =
            std::make_shared<Subscription>(PlayerIDCounter_++, clientURI, name);
        subscribers_[subscription->getSessionIDProperty()] = subscription;
        playerIDByserver = subscription->getSessionIDProperty();
    }

    return playerIDByserver;
}

void YachtService::Unregister(int sessionID)
{
    const std::lock_guard<std::recursive_mutex> lock(subscribersSync_);

    if (FindSubscriber(sessionID) != nullptr) {
        (void)LeaveGame(sessionID);
        subscribers_.erase(sessionID);
    }
}

bool YachtService::JoinGame(const System::Guid& gameID, int sessionID)
{
    const std::lock_guard<std::recursive_mutex> lock(subscribersSync_);

    auto* game = FindGame(gameID);
    auto* subscriber = FindSubscriber(sessionID);
    if (game == nullptr || subscriber == nullptr) {
        return false;
    }

    if (subscriber->GameID == gameID) {
        return true;
    }

    for (auto& player : game->Players) {
        // A seat held by the computer is a seat a person can take.
        if (player.AIPlayer != nullptr) {
            subscriber->GameID = gameID;
            player.Name = subscriber->Name;
            player.AIPlayer.reset();
            player.PlayerID = sessionID;

            NotifyUpdate(gameID, sessionID,
                         GetBytes(MessageContentType::GameState, *game, game->SequenceNumber++));
            return true;
        }
    }

    return false;
}

bool YachtService::LeaveGame(int sessionID)
{
    const std::lock_guard<std::recursive_mutex> lock(subscribersSync_);

    auto* subscriber = FindSubscriber(sessionID);
    if (subscriber == nullptr) {
        return false;
    }

    auto* game = FindGame(subscriber->GameID);
    if (game == nullptr) {
        subscribers_.erase(sessionID);
        return false;
    }

    const System::Guid gameID = game->GameID;
    subscriber->GameID = System::Guid();

    for (std::size_t i = 0; i < game->Players.size(); i++) {
        if (game->Players[i].PlayerID != sessionID) {
            continue;
        }

        game->Players[i].Name = ServiceConstants::AIMessageString + std::to_string(i);
        game->Players[i].PlayerID = -1;
        game->Players[i].AIPlayer = std::make_shared<Yacht::AIPlayerBehavior>();

        if (!IsHumanExists(*game)) {
            gameStates_.erase(gameID);
        } else {
            NotifyUpdate(gameID, sessionID,
                         GetBytes(MessageContentType::GameState, *game, game->SequenceNumber++));
            HandlePlayerStep(gameID, static_cast<int>(i));
            return true;
        }
        break;
    }

    return false;
}

void YachtService::GameStep(const System::Guid& gameID, int sessionID, int scoreIndex,
                            SharpRuntime::bytecs scoreValue, int playerIndex, int step)
{
    const std::lock_guard<std::recursive_mutex> lock(subscribersSync_);

    auto* game = FindGame(gameID);
    if (game == nullptr) {
        return;
    }

    if (auto* subscriber = FindSubscriber(sessionID);
        subscriber != nullptr && subscriber->GameID == game->GameID) {
        MakeStep(gameID, YachtStep(scoreIndex, scoreValue, playerIndex, step));
    }

    // The first real move means the game is alive, so the deletion timer is dropped.
    if (auto* stillThere = FindGame(gameID); stillThere != nullptr) {
        stillThere->TimerToDelete.reset();
    }
}

std::vector<SharpRuntime::bytecs> YachtService::GetGameState(const System::Guid& gameID,
                                                             int sessionID)
{
    const std::lock_guard<std::recursive_mutex> lock(subscribersSync_);

    auto* subscriber = FindSubscriber(sessionID);
    auto* game = FindGame(gameID);

    if (subscriber == nullptr || game == nullptr || subscriber->GameID != gameID) {
        return {};
    }

    const auto& player = game->Players[static_cast<std::size_t>(game->CurrentPlayer)];
    if (player.PlayerID == sessionID && game->IsStarted) {
        ResetTimeout(gameID, sessionID);
    }

    return GetBytes(MessageContentType::GameState, *game, game->SequenceNumber++);
}

std::vector<SharpRuntime::bytecs> YachtService::GetAvailableGames(int sessionID)
{
    const std::lock_guard<std::recursive_mutex> lock(subscribersSync_);

    AvailableGames availableGames;

    if (FindSubscriber(sessionID) != nullptr) {
        for (const auto& [id, game] : gameStates_) {
            GameInformation information;
            information.GameID = id;
            information.Name = game->Name;
            availableGames.Games.push_back(information);
        }
    }

    return GetBytes(MessageContentType::AvailableGames, availableGames, -1);
}

std::vector<SharpRuntime::bytecs> YachtService::NewGame(int sessionID, const std::string& name)
{
    const std::lock_guard<std::recursive_mutex> lock(subscribersSync_);

    auto* subscriber = FindSubscriber(sessionID);
    if (subscriber == nullptr || !CheckUniqueGameName(name)) {
        return {};
    }

    const System::Guid gameID = System::Guid::NewGuid();

    auto game = std::make_shared<GameState>();
    game->Name = name;
    game->GameID = gameID;
    game->GameType = GameTypes::Online;
    game->SequenceNumber = 0;
    gameStates_[gameID] = game;

    subscriber->GameID = gameID;

    // Creates the human player (the player who created the game)
    PlayerInformation human;
    human.Name = subscriber->Name;
    human.ScoreCard = NewScoreCard();
    human.PlayerID = sessionID;
    game->Players.push_back(std::move(human));

    // Creates additional AI players to fill the game.
    for (int i = 1; i < 4; i++) {
        PlayerInformation computer;
        computer.Name = ServiceConstants::AIMessageString + std::to_string(i);
        computer.ScoreCard = NewScoreCard();
        computer.AIPlayer = std::make_shared<Yacht::AIPlayerBehavior>();
        game->Players.push_back(std::move(computer));
    }

    // A game nobody joins is cleared away after two minutes.
    game->TimerToDelete = std::make_shared<System::Threading::Timer>(
        [this, gameID](void*) { DeleteGame(gameID); }, nullptr, 120000, -1);

    const auto bytes = gameID.ToByteArray();
    return std::vector<SharpRuntime::bytecs>(bytes.begin(), bytes.end());
}

void YachtService::ResetTimeout(const System::Guid& gameID, int sessionID)
{
    const std::lock_guard<std::recursive_mutex> lock(subscribersSync_);

    auto* game = FindGame(gameID);
    if (game == nullptr) {
        return;
    }

    auto& player = game->Players[static_cast<std::size_t>(game->CurrentPlayer)];

    if (FindSubscriber(sessionID) != nullptr && player.PlayerID == sessionID) {
        if (!game->IsStarted) {
            game->IsStarted = true;
        }

        if (player.Timer != nullptr) {
            player.ToastMessageSent = false;
            player.Timer->Change(25000, -1);
        } else {
            ArmTurnTimer(gameID, game->CurrentPlayer, 25000);
        }
    }
}

std::vector<SharpRuntime::bytecs> YachtService::GetScoreCard(int sessionID)
{
    const std::lock_guard<std::recursive_mutex> lock(subscribersSync_);

    auto* subscriber = FindSubscriber(sessionID);
    if (subscriber == nullptr) {
        return {};
    }

    auto* game = FindGame(subscriber->GameID);
    if (game == nullptr) {
        return {};
    }

    for (const auto& player : game->Players) {
        if (player.PlayerID == sessionID) {
            return player.ScoreCard;
        }
    }

    return {};
}

void YachtService::DeleteGame(const System::Guid& gameID)
{
    const std::lock_guard<std::recursive_mutex> lock(subscribersSync_);

    auto* game = FindGame(gameID);
    if (game == nullptr) {
        return;
    }

    SimpleType banned;
    banned.Name = ServiceConstants::BannedMessageString;
    const auto bytes = GetBytes(MessageContentType::SimpleType, banned, -1);

    for (const auto& [id, client] : subscribers_) {
        if (client->GameID == gameID) {
            SendRawMessage(client->ChannelUri, bytes);
        }
    }

    gameStates_.erase(gameID);
}

bool YachtService::CheckUniqueGameName(const std::string& name) const
{
    for (const auto& [id, game] : gameStates_) {
        if (game->Name == name) {
            return false;
        }
    }
    return true;
}

bool YachtService::IsHumanExists(const GameState& game)
{
    for (const auto& player : game.Players) {
        if (player.AIPlayer == nullptr) {
            return true;
        }
    }
    return false;
}

void YachtService::NotifyUpdate(const System::Guid& gameID, std::optional<int> sessionID,
                                const std::vector<SharpRuntime::bytecs>& raw)
{
    const std::lock_guard<std::recursive_mutex> lock(subscribersSync_);

    for (const auto& [id, subscriber] : subscribers_) {
        // Everyone in the game except whoever caused the update -- they already know.
        if (subscriber->GameID == gameID &&
            (!sessionID.has_value() || subscriber->getSessionIDProperty() != *sessionID)) {
            SendRawMessage(subscriber->ChannelUri, raw);
        }
    }
}

void YachtService::CheckIfGameEnded(const System::Guid& gameID)
{
    auto* game = FindGame(gameID);
    if (game == nullptr) {
        return;
    }

    if (game->StepsMade == 12 * static_cast<int>(game->Players.size())) {
        const PlayerInformation& winner = GetWinner(*game);

        EndGameInformation endGame;
        endGame.PlayerID = winner.PlayerID;
        endGame.ScoreCard = winner.ScoreCard;

        NotifyUpdate(gameID, std::nullopt,
                     GetBytes(MessageContentType::EndGameInformation, endGame, -1));
        gameStates_.erase(gameID);
    }
}

void YachtService::MakeStep(const System::Guid& gameID, const YachtStep& step)
{
    auto* game = FindGame(gameID);
    if (game == nullptr) {
        return;
    }

    auto& player = game->Players[static_cast<std::size_t>(step.PlayerIndex)];

    if (!IsStepValid(*game, step.PlayerIndex, step.StepNumber)) {
        return;
    }

    if (player.Timer != nullptr) {
        player.Timer->Change(-1, -1);
        if (player.AIPlayer == nullptr) {
            player.TimeOutsCounter = 0;
        }
    }

    MakeMove(gameID, step);

    NotifyUpdate(gameID, std::nullopt,
                 GetBytes(MessageContentType::GameState, *game, game->SequenceNumber++));

    // The player before this one may have had a turn played for them; if it was their first
    // such turn, nudge them that the game has moved on.
    const std::size_t previousIndex =
        step.PlayerIndex == 0 ? 3 : static_cast<std::size_t>(step.PlayerIndex - 1);
    if (previousIndex < game->Players.size()) {
        const PlayerInformation& previousPlayer = game->Players[previousIndex];
        if (previousPlayer.TimeOutsCounter == 1) {
            if (auto* subscriber = FindSubscriber(previousPlayer.PlayerID);
                subscriber != nullptr) {
                SendToastMessage(subscriber->ChannelUri,
                                 "Yacht - Network player made his step");
            }
        }
    }

    CheckIfGameEnded(gameID);

    if (auto* stillThere = FindGame(gameID); stillThere != nullptr) {
        HandlePlayerStep(gameID, stillThere->CurrentPlayer);
    }
}

void YachtService::ArmTurnTimer(const System::Guid& gameID, int playerIndex,
                                int dueMilliseconds)
{
    auto* game = FindGame(gameID);
    if (game == nullptr) {
        return;
    }

    auto& player = game->Players[static_cast<std::size_t>(playerIndex)];
    player.Timer = std::make_shared<System::Threading::Timer>(
        [this, gameID, playerIndex](void*) { PlayAI(gameID, playerIndex); }, nullptr,
        dueMilliseconds, -1);
}

void YachtService::HandlePlayerStep(const System::Guid& gameID, int playerIndex)
{
    auto* game = FindGame(gameID);
    if (game == nullptr) {
        return;
    }

    auto& player = game->Players[static_cast<std::size_t>(playerIndex)];

    if (game->CurrentPlayer != playerIndex ||
        game->StepsMade >= 12 * static_cast<int>(game->Players.size())) {
        return;
    }

    if (player.AIPlayer != nullptr) {
        auto* behavior = static_cast<Yacht::AIPlayerBehavior*>(player.AIPlayer.get());
        YachtStep step = behavior->Play(player.ScoreCard);
        step.PlayerIndex = playerIndex;
        step.StepNumber = game->StepsMade;
        MakeStep(gameID, step);
        return;
    }

    // A person's turn is given twenty-five seconds; when it runs out the computer takes it.
    if (player.Timer != nullptr) {
        player.Timer->Change(25000, -1);
    } else {
        ArmTurnTimer(gameID, playerIndex, 25000);
    }
}

void YachtService::PlayAI(const System::Guid& gameID, int playerIndex)
{
    const std::lock_guard<std::recursive_mutex> lock(subscribersSync_);

    auto* game = FindGame(gameID);
    if (game == nullptr) {
        return;
    }

    auto& player = game->Players[static_cast<std::size_t>(playerIndex)];

    if (game->StepsMade >= 12 * static_cast<int>(game->Players.size())) {
        return;
    }

    player.TimeOutsCounter++;

    // The seat is played by the computer for this one turn, and handed straight back -- the
    // player is still theirs unless they miss three in a row.
    auto behavior = std::make_shared<Yacht::AIPlayerBehavior>();
    player.AIPlayer = behavior;
    YachtStep step = behavior->Play(player.ScoreCard);
    step.PlayerIndex = playerIndex;
    step.StepNumber = game->StepsMade;
    MakeStep(gameID, step);

    if (auto* stillThere = FindGame(gameID); stillThere != nullptr) {
        auto& stillPlayer = stillThere->Players[static_cast<std::size_t>(playerIndex)];
        stillPlayer.AIPlayer.reset();
        CheckForInactivePlayer(gameID, playerIndex, stillPlayer);
    }
}

PlayerInformation& YachtService::GetWinner(GameState& game)
{
    std::size_t winnerPlayerIndex = 0;
    for (std::size_t i = 1; i < game.Players.size(); i++) {
        if (game.Players[i].TotalScore > game.Players[winnerPlayerIndex].TotalScore) {
            winnerPlayerIndex = i;
        }
    }
    return game.Players[winnerPlayerIndex];
}

void YachtService::CheckForInactivePlayer(const System::Guid& gameID, int playerIndex,
                                          PlayerInformation& playerInformation)
{
    if (playerInformation.TimeOutsCounter != 3) {
        return;
    }

    SimpleType banned;
    banned.Name = ServiceConstants::BannedMessageString;
    const auto bytes = GetBytes(MessageContentType::SimpleType, banned, -1);

    if (auto* subscriber = FindSubscriber(playerInformation.PlayerID); subscriber != nullptr) {
        SendRawMessage(subscriber->ChannelUri, bytes);
    }

    playerInformation.Name = ServiceConstants::AIMessageString + std::to_string(playerIndex);
    playerInformation.TimeOutsCounter = 0;
    playerInformation.AIPlayer = std::make_shared<Yacht::AIPlayerBehavior>();

    subscribers_.erase(playerInformation.PlayerID);

    auto* game = FindGame(gameID);
    if (game == nullptr || !IsHumanExists(*game)) {
        gameStates_.erase(gameID);
    } else {
        NotifyUpdate(gameID, playerInformation.PlayerID,
                     GetBytes(MessageContentType::GameState, *game, game->SequenceNumber++));
    }

    playerInformation.PlayerID = -1;
}

void YachtService::MakeMove(const System::Guid& gameID, const YachtStep& move)
{
    auto* game = FindGame(gameID);
    if (game == nullptr) {
        return;
    }

    auto& player = game->Players[static_cast<std::size_t>(move.PlayerIndex)];
    player.ScoreCard[static_cast<std::size_t>(move.ScoreLine)] = move.Score;
    player.TotalScore += move.Score;
    game->CurrentPlayer = (game->CurrentPlayer + 1) % static_cast<int>(game->Players.size());
    game->StepsMade++;
}

bool YachtService::IsStepValid(const GameState& game, int playerIndex, int step)
{
    return game.CurrentPlayer == playerIndex &&
           game.StepsMade < 12 * static_cast<int>(game.Players.size()) &&
           game.StepsMade == step;
}

std::vector<SharpRuntime::bytecs> YachtService::GetBytes(MessageContentType contentType,
                                                         const IXmlSerializable& body,
                                                         int sequenceNumber)
{
    Message message;
    message.ContentType = contentType;
    message.SequenceNumber = sequenceNumber;

    System::Xml::XmlWriterSettings settings;
    settings.OmitXmlDeclaration = true;
    std::unique_ptr<System::Xml::XmlWriter> writer(
        System::Xml::XmlWriter::CreateToString(settings));

    // The outer Message element is XmlSerializer's root wrapper in the original, and
    // Message::WriteXml writes its own inside it. Both are on the wire and the client's reader
    // steps past both.
    writer->WriteStartElement("Message");
    writer->WriteStartElement("Message");
    writer->WriteAttributeString("ContentType", YachtServices::ToString(contentType));
    writer->WriteAttributeString("SequenceNumber", std::to_string(sequenceNumber));
    writer->WriteStartElement("Body");
    body.WriteXml(*writer);
    writer->WriteEndElement();
    writer->WriteEndElement();
    writer->WriteFullEndElement();

    const std::string document =
        "\xEF\xBB\xBF<?xml version=\"1.0\" encoding=\"utf-8\"?>" + writer->ToString();
    return std::vector<SharpRuntime::bytecs>(document.begin(), document.end());
}

} // namespace YachtServices
