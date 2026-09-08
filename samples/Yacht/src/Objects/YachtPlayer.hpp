#pragma once

// YachtPlayer.hpp -- C++ port of Yacht/Objects/YachtPlayer.cs.

#include <string>

#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"

namespace Yacht {

using Microsoft::Xna::Framework::Graphics::SpriteBatch;

class DiceHandler;
class GameStateHandler;

/**
 * @brief One participant in a game, whoever is driving them.
 *
 * The three kinds differ only in where their moves come from: a human's from the touch screen,
 * an AI's from a small state machine, a network player's from the server. The game handles all
 * three through this base, which is why a turn does not care who is taking it.
 */
class YachtPlayer {
public:
    /**
     * @brief Creates a player.
     *
     * @param name        The player's name.
     * @param diceHandler The dice this player rolls, or null for a player whose dice are not
     *                    local -- a network player rolls on the server.
     */
    YachtPlayer(std::string name, DiceHandler* diceHandler)
        : name_(std::move(name)), diceHandler_(diceHandler)
    {
    }

    /** @brief Destroys the player. */
    virtual ~YachtPlayer() = default;

    YachtPlayer(const YachtPlayer&) = delete;
    YachtPlayer& operator=(const YachtPlayer&) = delete;

    /**
     * @brief The player's name.
     *
     * @return The name.
     */
    [[nodiscard]] const std::string& getNameProperty() const { return name_; }

    /**
     * @brief Sets the player's name.
     *
     * @param value The name.
     */
    void setNameProperty(std::string value) { name_ = std::move(value); }

    /**
     * @brief The dice this player rolls.
     *
     * @return The handler, or null for a player whose dice are not local.
     */
    [[nodiscard]] DiceHandler* getDiceHandlerProperty() const { return diceHandler_; }

    /**
     * @brief The game this player is taking part in.
     *
     * @return The handler.
     */
    [[nodiscard]] GameStateHandler* getGameStateHandlerProperty() const
    {
        return gameStateHandler_;
    }

    /**
     * @brief Sets the game this player is taking part in.
     *
     * @param value The handler.
     */
    void setGameStateHandlerProperty(GameStateHandler* value) { gameStateHandler_ = value; }

    /** @brief Takes whatever action this kind of player takes on its turn. */
    virtual void PerformPlayerLogic() {}

    /**
     * @brief Draws whatever this kind of player shows during its turn.
     *
     * @param spriteBatch The batch to draw into; must already be begun.
     */
    virtual void Draw(SpriteBatch& spriteBatch) { (void)spriteBatch; }

protected:
    /** @brief The player's name. */
    std::string name_;

    /** @brief The dice this player rolls, or null. */
    DiceHandler* diceHandler_ = nullptr;

    /** @brief The game this player is taking part in. */
    GameStateHandler* gameStateHandler_ = nullptr;
};

} // namespace Yacht
