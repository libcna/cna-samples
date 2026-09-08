#pragma once

// AIPlayer.hpp -- C++ port of Yacht/Objects/AIPlayer.cs.

#include <string>

#include "System/Random.hpp"

#include "../YachtTypes.hpp"
#include "DiceHandler.hpp"
#include "YachtPlayer.hpp"

namespace Yacht {

/** @brief Possible AI states. */
enum class AIState {
    /** @brief About to roll. */
    Roll,
    /** @brief Waiting for the dice to settle. */
    Rolling,
    /** @brief Deciding which dice to keep. */
    ChooseDice,
    /** @brief Deciding which line to score. */
    SelectScore,
    /** @brief Writing the score down and ending the turn. */
    WriteScore
};

/**
 * @brief A computer opponent.
 *
 * @note **It plays badly on purpose, and the sample says so.** The documentation calls the
 * implementation "extremly basic" and says it "operates relatively randomly", which is exactly
 * what this is: it keeps a die chosen at random, stops keeping them on a one-in-five roll, and
 * picks a scoring line at random until one is legal. Nothing here is a simplification of a
 * cleverer original -- there is no cleverer original, and improving it is listed among the
 * sample's own suggested exercises.
 */
class AIPlayer : public YachtPlayer {
public:
    /**
     * @brief Creates a computer player.
     *
     * @param name        The player's name.
     * @param diceHandler The dice this player rolls.
     */
    AIPlayer(std::string name, DiceHandler* diceHandler)
        : YachtPlayer(std::move(name), diceHandler)
    {
    }

    /**
     * @brief Where the player is in its turn.
     *
     * @return The state.
     */
    [[nodiscard]] AIState getStateProperty() const { return state_; }

    /**
     * @brief Sets where the player is in its turn.
     *
     * @param value The state.
     */
    void setStateProperty(AIState value) { state_ = value; }

    /** @brief Advances the player's turn by one step. */
    void PerformPlayerLogic() override;

private:
    // One generator shared by every computer player, as the original's static Random is.
    static inline System::Random random_;

    AIState state_ = AIState::Roll;
};

} // namespace Yacht
