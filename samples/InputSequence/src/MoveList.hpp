// SPDX-License-Identifier: MS-PL

#pragma once

#include <algorithm>
#include <vector>

#include "InputManager.hpp"
#include "Move.hpp"

namespace InputSequenceSample
{
    /** @brief Stores moves in longest-first order for efficient matching. */
    class MoveList
    {
        std::vector<Move*> moves;

    public:
        /**
         * @brief Creates a longest-first shallow copy of the master move list.
         *
         * @param sourceMoves Master list whose move objects remain owned by the game.
         */
        explicit MoveList(std::vector<Move>& sourceMoves)
        {
            moves.reserve(sourceMoves.size());
            for (Move& move : sourceMoves)
            {
                moves.push_back(&move);
            }
            std::stable_sort(
                moves.begin(),
                moves.end(),
                [](const Move* left, const Move* right)
                {
                    return left->Sequence.size() > right->Sequence.size();
                });
        }

        /**
         * @brief Finds the longest move matching the current input history.
         *
         * @param input Input history to inspect.
         * @return Matching move, or null when no move matches.
         */
        [[nodiscard]] Move* DetectMove(InputManager& input) const
        {
            for (Move* move : moves)
            {
                if (input.Matches(*move))
                {
                    return move;
                }
            }
            return nullptr;
        }

        /**
         * @brief Gets the number of steps in the longest move.
         *
         * @return Longest move length.
         */
        [[nodiscard]] int LongestMoveLength() const
        {
            return static_cast<int>(moves[0]->Sequence.size());
        }
    };
}
