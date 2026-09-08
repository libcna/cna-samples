#pragma once

// BasicAIPlayer.hpp -- C++ port of YachtServices/BasicAIPlayer.cs.

#include <optional>
#include <vector>

#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/Random.hpp"

#include "DataModel.hpp"
#include "ServiceConstants.hpp"

namespace Yacht {

/**
 * @brief The server's stand-in for a player who is not there.
 *
 * @note **It does not roll five dice, and most of its scores are always zero -- both are the
 * original's, and both are reproduced rather than corrected.** It rolls between one and four
 * dice, each between one and five, then scores the line that pays best. And `Sum` only adds a
 * die when it was given a value to match, so every call that passes none -- Choice, four of a
 * kind, full house -- returns zero however the dice fall. Fixing either would make a server
 * that plays a different game from the one the sample ships, which is what the client was
 * written against. See diff.md.
 */
class AIPlayerBehavior {
public:
    /**
     * @brief Chooses this player's move.
     *
     * @param scoreCard The twelve lines, with unscored ones marked.
     * @return The step to record: which line, and what it scores.
     */
    [[nodiscard]] YachtServices::YachtStep Play(
        const std::vector<SharpRuntime::bytecs>& scoreCard)
    {
        std::vector<int> dice(static_cast<std::size_t>(random_.Next(1, 5)));
        for (std::size_t i = 0; i < dice.size(); i++) {
            dice[i] = random_.Next(1, 6);
        }

        SharpRuntime::bytecs highestScore = 0;
        int scoreCardIndex = -1;

        for (int i = 0; i < static_cast<int>(YachtCombination::Yacht); i++) {
            if (scoreCard[static_cast<std::size_t>(i)] ==
                YachtServices::ServiceConstants::NullScore) {
                const SharpRuntime::bytecs currentScore =
                    CalculateDiceScore(dice, static_cast<YachtCombination>(i + 1));
                if (currentScore > highestScore) {
                    highestScore = currentScore;
                    scoreCardIndex = i;
                }
            }
        }

        if (scoreCardIndex == -1) {
            for (std::size_t i = 0; i < scoreCard.size(); i++) {
                if (scoreCard[i] == YachtServices::ServiceConstants::NullScore) {
                    scoreCardIndex = static_cast<int>(i);
                    break;
                }
            }
        }

        return YachtServices::YachtStep(scoreCardIndex, highestScore, 0, 0);
    }

private:
    /** @brief Possible score types on the score card. */
    enum class YachtCombination {
        Yacht = 12,
        LargeStraight = 11,
        SmallStraight = 10,
        FourOfAKind = 9,
        FullHouse = 8,
        Choise = 7,
        Sixes = 6,
        Fives = 5,
        Fours = 4,
        Threes = 3,
        Twos = 2,
        Ones = 1
    };

    [[nodiscard]] SharpRuntime::bytecs CalculateDiceScore(const std::vector<int>& dice,
                                                          YachtCombination stepType) const
    {
        const int first = dice.front();
        const int last = dice.back();

        switch (stepType) {
        case YachtCombination::Yacht:
            return Times(dice, first) == 5 ? 50 : 0;
        case YachtCombination::LargeStraight:
            return FollowingDice(dice) && last == 6 ? 30 : 0;
        case YachtCombination::SmallStraight:
            return FollowingDice(dice) && last == 5 ? 30 : 0;
        case YachtCombination::FourOfAKind:
            if (Times(dice, first) >= 4 || Times(dice, last) >= 4) {
                return Sum(dice, std::nullopt);
            }
            return 0;
        case YachtCombination::FullHouse:
            if ((Times(dice, first) == 3 && Times(dice, last) == 2) ||
                (Times(dice, first) == 2 && Times(dice, last) == 3)) {
                return Sum(dice, std::nullopt);
            }
            return 0;
        case YachtCombination::Choise:
            return Sum(dice, std::nullopt);
        case YachtCombination::Sixes:
        case YachtCombination::Fives:
        case YachtCombination::Fours:
        case YachtCombination::Threes:
        case YachtCombination::Twos:
        case YachtCombination::Ones:
            return Sum(dice, static_cast<int>(stepType));
        }

        return 0;
    }

    // The condition is the original's, and it is why a sum with nothing to match returns zero:
    // `value.HasValue &&` guards the whole test, so a null value adds nothing at all.
    [[nodiscard]] static SharpRuntime::bytecs Sum(const std::vector<int>& dice,
                                                  std::optional<int> value)
    {
        int sum = 0;
        for (const int die : dice) {
            if (value.has_value() && die == *value) {
                sum += die;
            }
        }
        return static_cast<SharpRuntime::bytecs>(sum);
    }

    [[nodiscard]] static int Times(const std::vector<int>& dice, int value)
    {
        int count = 0;
        for (const int die : dice) {
            if (die == value) {
                count++;
            }
        }
        return count;
    }

    [[nodiscard]] static bool FollowingDice(const std::vector<int>& dice)
    {
        int count = 0;
        for (std::size_t i = 0; i + 1 < dice.size(); i++) {
            if (dice[i] + 1 == dice[i + 1]) {
                count++;
            }
        }
        return count == static_cast<int>(dice.size()) - 1;
    }

    static inline System::Random random_;
};

} // namespace Yacht
