#pragma once

// IYachtService.hpp -- C++ port of YachtServices/IYachtService.cs.

#include <string>
#include <vector>

#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/Guid.hpp"
#include "System/Uri.hpp"

namespace YachtServices {

/**
 * @brief The ten operations the game server offers.
 *
 * The server is authoritative: a client asks it to do things and is told what happened, and
 * every reply that carries more than a number comes back as an XML-serialized Message in a byte
 * array. That is why so many of these return `byte[]` rather than a typed result -- the wire
 * format is the data model's own, not the contract's.
 *
 * @note The original decorates this interface with `[ServiceContract]` and each method with
 * `[OperationContract]`, and the client talks to it through a generated proxy that adds an
 * asynchronous method and a completion event per operation. This interface is the synchronous
 * contract itself; the asynchronous shape belongs to the proxy, as it does in the original.
 */
class IYachtService {
public:
    /** @brief Destroys the service. */
    virtual ~IYachtService() = default;

    /**
     * @brief Registers a client with the server.
     *
     * @param clientURI Where the server should send this client's notifications.
     * @param name      The player's name.
     * @param playerID  A previous session's player ID to reclaim, or -1 for a new player.
     * @return The player's session ID.
     */
    virtual int Register(const System::Uri& clientURI, const std::string& name, int playerID) = 0;

    /**
     * @brief Drops a client's registration.
     *
     * @param sessionID The session to drop.
     */
    virtual void Unregister(int sessionID) = 0;

    /**
     * @brief Joins a game that is waiting for players.
     *
     * @param gameID    The game to join.
     * @param sessionID The joining player's session.
     * @return True when the player was admitted.
     */
    virtual bool JoinGame(const System::Guid& gameID, int sessionID) = 0;

    /**
     * @brief Leaves the game a player is in.
     *
     * @param sessionID The leaving player's session.
     * @return True when the player was in a game to leave.
     */
    virtual bool LeaveGame(int sessionID) = 0;

    /**
     * @brief Reports one player scoring one line.
     *
     * @param gameID    The game the step belongs to.
     * @param sessionID The stepping player's session.
     * @param scoreLine Which line was scored.
     * @param score     The score placed on it.
     * @param player    The index of the player who stepped.
     * @param step      The step's sequence number within the game.
     */
    virtual void GameStep(const System::Guid& gameID, int sessionID, int scoreLine,
                          SharpRuntime::bytecs score, int player, int step) = 0;

    /**
     * @brief Asks for the current state of a game.
     *
     * @param gameID    The game to ask about.
     * @param sessionID The asking player's session.
     * @return A serialized Message carrying a GameState.
     */
    virtual std::vector<SharpRuntime::bytecs> GetGameState(const System::Guid& gameID,
                                                           int sessionID) = 0;

    /**
     * @brief Asks which games are waiting for players.
     *
     * @param sessionID The asking player's session.
     * @return A serialized Message carrying an AvailableGames.
     */
    virtual std::vector<SharpRuntime::bytecs> GetAvailableGames(int sessionID) = 0;

    /**
     * @brief Creates a game and joins it.
     *
     * @param sessionID The creating player's session.
     * @param name      The game's name.
     * @return A serialized Message carrying the new GameState.
     */
    virtual std::vector<SharpRuntime::bytecs> NewGame(int sessionID, const std::string& name) = 0;

    /**
     * @brief Tells the server the player is still there.
     *
     * A player who says nothing for long enough is dropped from their game, so the client sends
     * this whenever the player does something the server would not otherwise hear about.
     *
     * @param gameID    The game the player is in.
     * @param sessionID The player's session.
     */
    virtual void ResetTimeout(const System::Guid& gameID, int sessionID) = 0;

    /**
     * @brief Asks for the player's own score card.
     *
     * @param sessionID The asking player's session.
     * @return A serialized Message carrying the card.
     */
    virtual std::vector<SharpRuntime::bytecs> GetScoreCard(int sessionID) = 0;
};

} // namespace YachtServices
