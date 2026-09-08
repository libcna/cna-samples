#pragma once

// YachtService.hpp -- C++ port of YachtServices/YachtService.cs.

#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Phone/Notification/PushNotificationSender.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/Guid.hpp"
#include "System/Threading/Timer.hpp"
#include "System/Uri.hpp"
#include "System/Xml/XmlWriter.hpp"

#include "BasicAIPlayer.hpp"
#include "DataModel.hpp"
#include "IYachtService.hpp"
#include "ServiceConstants.hpp"
#include "Subscription.hpp"

namespace YachtServices {

/**
 * @brief The authoritative game server.
 *
 * One instance serves everyone -- the original marks the class
 * `[ServiceBehavior(InstanceContextMode = Single)]` -- so every game and every subscriber lives
 * in this object, and the locks around the subscriber map are what make that safe.
 *
 * The server owns the rules: a client asks it to record a step, it decides whether the step is
 * legal, applies it, and pushes the new state to everyone else in the game. A player who lets
 * their turn run out has it played for them, and one who does that three times running is
 * dropped and replaced by the computer.
 */
class YachtService : public IYachtService {
public:
    /** @brief Creates the service. */
    YachtService() = default;

    /** @brief Stops every timer and destroys the service. */
    ~YachtService() override;

    /**
     * @brief Registers a client, or refreshes the address of one that is already known.
     *
     * @param clientURI Where to push this client's notifications.
     * @param name      The player's name.
     * @param playerID  A session to reclaim, or -1 for a new one.
     * @return The player's session ID, or -1 when the registration was refused.
     */
    int Register(const System::Uri& clientURI, const std::string& name, int playerID) override;

    /**
     * @brief Drops a client's registration, leaving whatever game it was in.
     *
     * @param sessionID The session to drop.
     */
    void Unregister(int sessionID) override;

    /**
     * @brief Puts a player into a game, in place of one of its computer opponents.
     *
     * @param gameID    The game to join.
     * @param sessionID The joining player's session.
     * @return True when a seat was found.
     */
    bool JoinGame(const System::Guid& gameID, int sessionID) override;

    /**
     * @brief Takes a player out of their game, leaving the computer in their seat.
     *
     * @param sessionID The leaving player's session.
     * @return True when the player was in a game and it survived their leaving.
     */
    bool LeaveGame(int sessionID) override;

    /**
     * @brief Records one player scoring one line.
     *
     * @param gameID     The game the step belongs to.
     * @param sessionID  The stepping player's session.
     * @param scoreIndex Which line was scored.
     * @param scoreValue The score placed on it.
     * @param playerIndex The index of the player who stepped.
     * @param step       The step's sequence number within the game.
     */
    void GameStep(const System::Guid& gameID, int sessionID, int scoreIndex,
                  SharpRuntime::bytecs scoreValue, int playerIndex, int step) override;

    /**
     * @brief Hands back the state of a game the asking player is in.
     *
     * @param gameID    The game to report.
     * @param sessionID The asking player's session.
     * @return A serialized Message carrying the state, or nothing when the player may not see it.
     */
    std::vector<SharpRuntime::bytecs> GetGameState(const System::Guid& gameID,
                                                   int sessionID) override;

    /**
     * @brief Hands back every game a player could join.
     *
     * @param sessionID The asking player's session.
     * @return A serialized Message carrying the list.
     */
    std::vector<SharpRuntime::bytecs> GetAvailableGames(int sessionID) override;

    /**
     * @brief Creates a game with three computer opponents and puts the caller in the first seat.
     *
     * @param sessionID The creating player's session.
     * @param name      The game's name, which must not already be in use.
     * @return The new game's identifier as raw bytes, or nothing when the name was taken.
     */
    std::vector<SharpRuntime::bytecs> NewGame(int sessionID, const std::string& name) override;

    /**
     * @brief Restarts the current player's turn clock, and starts the game on the first call.
     *
     * @param gameID    The game the player is in.
     * @param sessionID The player's session.
     */
    void ResetTimeout(const System::Guid& gameID, int sessionID) override;

    /**
     * @brief Hands back the asking player's own score card.
     *
     * @param sessionID The asking player's session.
     * @return The twelve scores, or nothing when the player is not in a game.
     */
    std::vector<SharpRuntime::bytecs> GetScoreCard(int sessionID) override;

private:
    void DeleteGame(const System::Guid& gameID);
    [[nodiscard]] bool CheckUniqueGameName(const std::string& name) const;
    [[nodiscard]] static bool IsHumanExists(const GameState& game);
    void NotifyUpdate(const System::Guid& gameID, std::optional<int> sessionID,
                      const std::vector<SharpRuntime::bytecs>& raw);
    void CheckIfGameEnded(const System::Guid& gameID);
    void MakeStep(const System::Guid& gameID, const YachtStep& step);
    void HandlePlayerStep(const System::Guid& gameID, int playerIndex);
    void PlayAI(const System::Guid& gameID, int playerIndex);
    [[nodiscard]] static PlayerInformation& GetWinner(GameState& game);
    void CheckForInactivePlayer(const System::Guid& gameID, int playerIndex,
                                PlayerInformation& playerInformation);
    void MakeMove(const System::Guid& gameID, const YachtStep& move);
    [[nodiscard]] static bool IsStepValid(const GameState& game, int playerIndex, int step);
    [[nodiscard]] static std::vector<SharpRuntime::bytecs> NewScoreCard();

    // Serializes one body into the Message envelope the client reads back.
    //
    // The document carries a Message element wrapping another: the original serializes the
    // Message through XmlSerializer, whose root wrapper is the outer one, while Message::WriteXml
    // writes its own inside it. The client's reader steps past both, so producing only one here
    // would break it -- and the same doubled element was captured off the original service. It
    // also begins with a UTF-8 byte order mark, because the original's writer emits one.
    [[nodiscard]] static std::vector<SharpRuntime::bytecs> GetBytes(
        MessageContentType contentType, const IXmlSerializable& body, int sequenceNumber);

    [[nodiscard]] GameState* FindGame(const System::Guid& gameID);
    [[nodiscard]] Subscription* FindSubscriber(int sessionID);
    void ArmTurnTimer(const System::Guid& gameID, int playerIndex, int dueMilliseconds);

    static inline int PlayerIDCounter_ = 100;

    std::map<int, std::shared_ptr<Subscription>> subscribers_;
    std::map<System::Guid, std::shared_ptr<GameState>> gameStates_;
    std::recursive_mutex subscribersSync_;
};

} // namespace YachtServices
