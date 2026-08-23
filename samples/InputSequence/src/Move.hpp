// SPDX-License-Identifier: MS-PL

#pragma once

#include <string>
#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Buttons.hpp"

namespace InputSequenceSample
{
    /** @brief Describes a sequence of button presses that activates a move. */
    class Move
    {
    public:
        /** @brief Display name of the move. */
        std::string Name;
        /** @brief Ordered button sequence required to activate the move. */
        std::vector<Microsoft::Xna::Framework::Input::Buttons> Sequence;
        /** @brief Whether matching input may be reused as part of a longer move. */
        bool IsSubMove = false;

        /**
         * @brief Creates a move with its display name and input sequence.
         *
         * @param name Display name.
         * @param sequence Required input sequence.
         */
        Move(
            std::string name,
            std::vector<Microsoft::Xna::Framework::Input::Buttons> sequence)
            : Name(std::move(name))
            , Sequence(std::move(sequence))
        {
        }
    };
}
