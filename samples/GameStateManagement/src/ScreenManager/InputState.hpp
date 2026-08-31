// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <optional>
#include <vector>

#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"

namespace GameStateManagement
{
    /** @brief Tracks current and previous keyboard, gamepad, touch and gesture input. */
    class InputState
    {
    public:
        /** @brief Maximum number of XNA player input slots. */
        static constexpr int MaxInputs = 4;

        std::array<Microsoft::Xna::Framework::Input::KeyboardState, MaxInputs> CurrentKeyboardStates;
        std::array<Microsoft::Xna::Framework::Input::GamePadState, MaxInputs> CurrentGamePadStates;
        std::array<Microsoft::Xna::Framework::Input::KeyboardState, MaxInputs> LastKeyboardStates;
        std::array<Microsoft::Xna::Framework::Input::GamePadState, MaxInputs> LastGamePadStates;
        std::array<bool, MaxInputs> GamePadWasConnected{};
        Microsoft::Xna::Framework::Input::Touch::TouchCollection TouchState;
        std::vector<Microsoft::Xna::Framework::Input::Touch::GestureSample> Gestures;

        /** @brief Constructs an empty input snapshot. */
        InputState() = default;
        /** @brief Reads all supported input devices. */
        void Update();
        /** @brief Tests a new key press. @param key Key. @param controllingPlayer Player filter. @param playerIndex Triggering player. @return true if newly pressed. */
        bool IsNewKeyPress(Microsoft::Xna::Framework::Input::Keys key,
                           std::optional<Microsoft::Xna::Framework::PlayerIndex> controllingPlayer,
                           Microsoft::Xna::Framework::PlayerIndex& playerIndex);
        /** @brief Tests a new button press. @param button Button. @param controllingPlayer Player filter. @param playerIndex Triggering player. @return true if newly pressed. */
        bool IsNewButtonPress(Microsoft::Xna::Framework::Input::Buttons button,
                              std::optional<Microsoft::Xna::Framework::PlayerIndex> controllingPlayer,
                              Microsoft::Xna::Framework::PlayerIndex& playerIndex);
        /** @brief Tests menu select. @param controllingPlayer Player filter. @param playerIndex Triggering player. @return true if selected. */
        bool IsMenuSelect(std::optional<Microsoft::Xna::Framework::PlayerIndex> controllingPlayer,
                          Microsoft::Xna::Framework::PlayerIndex& playerIndex);
        /** @brief Tests menu cancel. @param controllingPlayer Player filter. @param playerIndex Triggering player. @return true if cancelled. */
        bool IsMenuCancel(std::optional<Microsoft::Xna::Framework::PlayerIndex> controllingPlayer,
                          Microsoft::Xna::Framework::PlayerIndex& playerIndex);
        /** @brief Tests menu up. @param controllingPlayer Player filter. @return true if pressed. */
        bool IsMenuUp(std::optional<Microsoft::Xna::Framework::PlayerIndex> controllingPlayer);
        /** @brief Tests menu down. @param controllingPlayer Player filter. @return true if pressed. */
        bool IsMenuDown(std::optional<Microsoft::Xna::Framework::PlayerIndex> controllingPlayer);
        /** @brief Tests pause. @param controllingPlayer Player filter. @return true if pressed. */
        bool IsPauseGame(std::optional<Microsoft::Xna::Framework::PlayerIndex> controllingPlayer);
    };
}
