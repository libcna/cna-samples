#pragma once

// InputState.hpp -- C++ port of Yacht/ScreenManager/InputState.cs.

#include <array>
#include <optional>
#include <vector>

#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

namespace GameStateManagement {

using Microsoft::Xna::Framework::PlayerIndex;
using Microsoft::Xna::Framework::Input::Buttons;
using Microsoft::Xna::Framework::Input::GamePad;
using Microsoft::Xna::Framework::Input::GamePadState;
using Microsoft::Xna::Framework::Input::Keyboard;
using Microsoft::Xna::Framework::Input::KeyboardState;
using Microsoft::Xna::Framework::Input::Keys;
using Microsoft::Xna::Framework::Input::Touch::GestureSample;
using Microsoft::Xna::Framework::Input::Touch::TouchCollection;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

/**
 * @brief Helper for reading input from keyboard, gamepad, and touch input.
 *
 * This class tracks both the current and previous state of the input devices, and implements
 * query methods for high level input actions such as "move up through the menu" or "pause the
 * game".
 *
 * @note The gamepad and keyboard halves are read on every platform, phone included, and the
 * original does the same -- what varies is only whether anything is connected to answer them.
 * The touch half is the phone's, and this is the phone product.
 */
class InputState {
public:
    /** @brief How many players' devices are tracked. */
    static constexpr int MaxInputs = 4;

    /** @brief This frame's keyboard state, per player. */
    std::array<KeyboardState, MaxInputs> CurrentKeyboardStates{};

    /** @brief This frame's gamepad state, per player. */
    std::array<GamePadState, MaxInputs> CurrentGamePadStates{};

    /** @brief Last frame's keyboard state, per player. */
    std::array<KeyboardState, MaxInputs> LastKeyboardStates{};

    /** @brief Last frame's gamepad state, per player. */
    std::array<GamePadState, MaxInputs> LastGamePadStates{};

    /**
     * @brief Whether a gamepad has ever been connected for a player.
     *
     * Once true it stays true, so a controller unplugged mid-game does not make the game
     * forget it was being played with one.
     */
    std::array<bool, MaxInputs> GamePadWasConnected{};

    /** @brief This frame's touch points. */
    TouchCollection TouchState;

    /** @brief The gestures recognised this frame. */
    std::vector<GestureSample> Gestures;

    /** @brief Constructs a new input state. */
    InputState() = default;

    /** @brief Reads the latest state of the keyboard, gamepad and touch panel. */
    void Update()
    {
        for (int i = 0; i < MaxInputs; i++) {
            const auto index = static_cast<std::size_t>(i);
            LastKeyboardStates[index] = CurrentKeyboardStates[index];
            LastGamePadStates[index] = CurrentGamePadStates[index];

            CurrentKeyboardStates[index] = Keyboard::GetState(static_cast<PlayerIndex>(i));
            CurrentGamePadStates[index] = GamePad::GetState(static_cast<PlayerIndex>(i));

            // Keep track of whether a gamepad has ever been connected, so we can detect if it
            // is unplugged.
            if (CurrentGamePadStates[index].getIsConnectedProperty()) {
                GamePadWasConnected[index] = true;
            }
        }

        TouchState = TouchPanel::GetState();

        Gestures.clear();
        while (TouchPanel::getIsGestureAvailableProperty()) {
            Gestures.push_back(TouchPanel::ReadGesture());
        }
    }

    /**
     * @brief Helper for checking if a key was newly pressed during this update.
     *
     * @param key               The key to test.
     * @param controllingPlayer Which player to read, or nothing to accept any player.
     * @param playerIndex       Receives the player who pressed it.
     * @return True when the key went down this frame.
     */
    bool IsNewKeyPress(Keys key, const std::optional<PlayerIndex>& controllingPlayer,
                       PlayerIndex& playerIndex) const
    {
        if (controllingPlayer.has_value()) {
            // Read input from the specified player.
            playerIndex = *controllingPlayer;
            const auto i = static_cast<std::size_t>(playerIndex);
            return CurrentKeyboardStates[i].IsKeyDown(key) && LastKeyboardStates[i].IsKeyUp(key);
        }

        // Accept input from any player.
        return IsNewKeyPress(key, PlayerIndex::One, playerIndex) ||
               IsNewKeyPress(key, PlayerIndex::Two, playerIndex) ||
               IsNewKeyPress(key, PlayerIndex::Three, playerIndex) ||
               IsNewKeyPress(key, PlayerIndex::Four, playerIndex);
    }

    /**
     * @brief Helper for checking if a button was newly pressed during this update.
     *
     * @param button            The button to test.
     * @param controllingPlayer Which player to read, or nothing to accept any player.
     * @param playerIndex       Receives the player who pressed it.
     * @return True when the button went down this frame.
     */
    bool IsNewButtonPress(Buttons button, const std::optional<PlayerIndex>& controllingPlayer,
                          PlayerIndex& playerIndex) const
    {
        if (controllingPlayer.has_value()) {
            // Read input from the specified player.
            playerIndex = *controllingPlayer;
            const auto i = static_cast<std::size_t>(playerIndex);
            return CurrentGamePadStates[i].IsButtonDown(button) &&
                   LastGamePadStates[i].IsButtonUp(button);
        }

        // Accept input from any player.
        return IsNewButtonPress(button, PlayerIndex::One, playerIndex) ||
               IsNewButtonPress(button, PlayerIndex::Two, playerIndex) ||
               IsNewButtonPress(button, PlayerIndex::Three, playerIndex) ||
               IsNewButtonPress(button, PlayerIndex::Four, playerIndex);
    }

    /**
     * @brief Checks for a "menu select" input action.
     *
     * @param controllingPlayer Which player to read, or nothing to accept any player.
     * @param playerIndex       Receives the player who selected.
     * @return True on a select this frame.
     */
    bool IsMenuSelect(const std::optional<PlayerIndex>& controllingPlayer,
                      PlayerIndex& playerIndex) const
    {
        return IsNewKeyPress(Keys::Space, controllingPlayer, playerIndex) ||
               IsNewKeyPress(Keys::Enter, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::A, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::Start, controllingPlayer, playerIndex);
    }

    /**
     * @brief Checks for a "menu cancel" input action.
     *
     * @param controllingPlayer Which player to read, or nothing to accept any player.
     * @param playerIndex       Receives the player who cancelled.
     * @return True on a cancel this frame.
     */
    bool IsMenuCancel(const std::optional<PlayerIndex>& controllingPlayer,
                      PlayerIndex& playerIndex) const
    {
        return IsNewKeyPress(Keys::Escape, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::B, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::Back, controllingPlayer, playerIndex);
    }

    /**
     * @brief Checks for a "menu up" input action.
     *
     * @param controllingPlayer Which player to read, or nothing to accept any player.
     * @return True on an up this frame.
     */
    bool IsMenuUp(const std::optional<PlayerIndex>& controllingPlayer) const
    {
        PlayerIndex playerIndex = PlayerIndex::One;
        return IsNewKeyPress(Keys::Up, controllingPlayer, playerIndex) ||
               IsNewKeyPress(Keys::Left, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::DPadLeft, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::LeftThumbstickLeft, controllingPlayer, playerIndex);
    }

    /**
     * @brief Checks for a "menu down" input action.
     *
     * @param controllingPlayer Which player to read, or nothing to accept any player.
     * @return True on a down this frame.
     */
    bool IsMenuDown(const std::optional<PlayerIndex>& controllingPlayer) const
    {
        PlayerIndex playerIndex = PlayerIndex::One;
        return IsNewKeyPress(Keys::Down, controllingPlayer, playerIndex) ||
               IsNewKeyPress(Keys::Right, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::DPadRight, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::LeftThumbstickRight, controllingPlayer, playerIndex);
    }

    /**
     * @brief Checks for a "pause the game" input action.
     *
     * @param controllingPlayer Which player to read, or nothing to accept any player.
     * @return True on a pause this frame.
     */
    bool IsPauseGame(const std::optional<PlayerIndex>& controllingPlayer) const
    {
        PlayerIndex playerIndex = PlayerIndex::One;
        return IsNewKeyPress(Keys::Escape, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::Back, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::Start, controllingPlayer, playerIndex);
    }
};

} // namespace GameStateManagement

namespace Yacht {

using GameStateManagement::InputState;

} // namespace Yacht
