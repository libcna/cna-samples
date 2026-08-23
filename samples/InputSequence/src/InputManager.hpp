// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>
#include <cstddef>
#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "System/TimeSpan.hpp"

#include "Direction.hpp"
#include "Move.hpp"

namespace InputSequenceSample
{
    /** @brief Tracks recent keyboard and gamepad input for one player. */
    class InputManager
    {
        std::size_t bufferCapacity;

    public:
        /** @brief Player whose input is tracked. */
        Microsoft::Xna::Framework::PlayerIndex PlayerIndex;
        /** @brief Latest gamepad state. */
        Microsoft::Xna::Framework::Input::GamePadState GamePadState;
        /** @brief Latest keyboard state. */
        Microsoft::Xna::Framework::Input::KeyboardState KeyboardState;
        /** @brief Game time at which the latest unmerged input was received. */
        System::TimeSpan LastInputTime;
        /** @brief Current sequence of pressed buttons. */
        std::vector<Microsoft::Xna::Framework::Input::Buttons> Buffer;

        /** @brief Time after which all buffered input expires. */
        const System::TimeSpan BufferTimeOut = System::TimeSpan::FromMilliseconds(500);
        /** @brief Window in which nearly simultaneous button presses are merged. */
        const System::TimeSpan MergeInputTime = System::TimeSpan::FromMilliseconds(100);

        /** @brief Mapping of supported non-direction gamepad buttons to keyboard keys. */
        inline static constexpr std::array<
            std::pair<Microsoft::Xna::Framework::Input::Buttons,
                      Microsoft::Xna::Framework::Input::Keys>,
            4>
            NonDirectionButtons = {{
                {Microsoft::Xna::Framework::Input::Buttons::A,
                 Microsoft::Xna::Framework::Input::Keys::A},
                {Microsoft::Xna::Framework::Input::Buttons::B,
                 Microsoft::Xna::Framework::Input::Keys::B},
                {Microsoft::Xna::Framework::Input::Buttons::X,
                 Microsoft::Xna::Framework::Input::Keys::X},
                {Microsoft::Xna::Framework::Input::Buttons::Y,
                 Microsoft::Xna::Framework::Input::Keys::Y},
            }};

        /**
         * @brief Creates an input manager for one player.
         *
         * @param playerIndex Player to read.
         * @param bufferSize Maximum number of input steps retained.
         */
        InputManager(
            Microsoft::Xna::Framework::PlayerIndex playerIndex,
            int bufferSize)
            : bufferCapacity(static_cast<std::size_t>(bufferSize))
            , PlayerIndex(playerIndex)
        {
            Buffer.reserve(bufferCapacity);
        }

        /**
         * @brief Reads current input and updates the history buffer.
         *
         * @param gameTime Current timing snapshot.
         */
        void Update(const Microsoft::Xna::Framework::GameTime& gameTime)
        {
            using namespace Microsoft::Xna::Framework::Input;
            const Microsoft::Xna::Framework::Input::GamePadState lastGamePadState =
                GamePadState;
            const Microsoft::Xna::Framework::Input::KeyboardState lastKeyboardState =
                KeyboardState;
            GamePadState = GamePad::GetState(this->PlayerIndex);
            if (this->PlayerIndex == Microsoft::Xna::Framework::PlayerIndex::One)
            {
                KeyboardState = Keyboard::GetState(this->PlayerIndex);
            }

            const System::TimeSpan time = gameTime.getTotalGameTimeProperty();
            const System::TimeSpan timeSinceLast = time - LastInputTime;
            if (timeSinceLast > BufferTimeOut)
            {
                Buffer.clear();
            }

            Buttons buttons = Direction::None;
            for (const auto& [button, key] : NonDirectionButtons)
            {
                if ((lastGamePadState.IsButtonUp(button) &&
                     GamePadState.IsButtonDown(button)) ||
                    (lastKeyboardState.IsKeyUp(key) && KeyboardState.IsKeyDown(key)))
                {
                    buttons |= button;
                }
            }

            bool mergeInput = !Buffer.empty() && timeSinceLast < MergeInputTime;

            const Buttons direction = Direction::FromInput(GamePadState, KeyboardState);
            if (Direction::FromInput(lastGamePadState, lastKeyboardState) != direction)
            {
                buttons |= direction;
                mergeInput = false;
            }

            if (buttons != Direction::None)
            {
                if (mergeInput)
                {
                    Buffer.back() |= buttons;
                }
                else
                {
                    if (Buffer.size() == bufferCapacity)
                    {
                        Buffer.erase(Buffer.begin());
                    }
                    Buffer.push_back(buttons);
                    LastInputTime = time;
                }
            }
        }

        /**
         * @brief Tests whether the end of the input history matches a move.
         *
         * @param move Move to test.
         * @return True when the move matches.
         */
        bool Matches(const Move& move)
        {
            if (Buffer.size() < move.Sequence.size())
            {
                return false;
            }

            const std::size_t bufferSize = Buffer.size();
            const std::size_t sequenceSize = move.Sequence.size();
            for (std::size_t i = 1; i <= sequenceSize; ++i)
            {
                if (Buffer[bufferSize - i] != move.Sequence[sequenceSize - i])
                {
                    return false;
                }
            }

            if (!move.IsSubMove)
            {
                Buffer.clear();
            }

            return true;
        }
    };
}
