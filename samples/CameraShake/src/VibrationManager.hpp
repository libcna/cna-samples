// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// VibrationManager.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameComponent.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "System/Collections/Generic/Dictionary.hpp"

namespace CameraShake
{
    using Microsoft::Xna::Framework::Game;
    using Microsoft::Xna::Framework::GameComponent;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::PlayerIndex;
    using System::Collections::Generic::Dictionary;

    /**
     * @brief Handles management of vibration of GamePads and Windows Phone.
     */
    class VibrationManager : public GameComponent
    {
    public:
        /**
         * @brief Initializes a new VibrationManager.
         * @param game The hosting game.
         * @throws System::InvalidOperationException if one already exists.
         */
        explicit VibrationManager(Game& game);

        /** @brief Releases the manager. */
        CNAEXT ~VibrationManager() override;

        /**
         * @brief Creates a vibration for a given player.
         *
         * On Windows and Xbox the vibration strength uses a linear fall off to reduce the
         * strength over time. On Windows Phone we don't have that control so the phone
         * vibrates at the same strength for the duration of the vibration.
         *
         * @param player The player whose GamePad should vibrate. On Windows Phone this
         *        should be PlayerIndex.One.
         * @param left The initial strength of the left motor, in the range 0-1.
         * @param right The initial strength of the right motor, in the range 0-1.
         * @param duration The length of time (in seconds) to vibrate the GamePad or phone.
         */
        static void Vibrate(PlayerIndex player, float left, float right, float duration);

        /** @brief Pauses all vibrations. */
        static void Pause();

        /** @brief Resumes all paused vibrations. */
        static void Resume();

        /** @brief Cancels vibrations for all players. */
        static void CancelAll();

        /**
         * @brief Advances every active vibration and applies its linear decay.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

    private:
        /**
         * A private helper that maintains information about the vibration state for a
         * single player.
         */
        class VibrationSettings
        {
        public:
            /** The strength for the left motor of a GamePad. */
            float Left = 0.0f;
            /** The strength for the right motor of a GamePad. */
            float Right = 0.0f;
            /** The duration of the vibration. */
            float Duration = 0.0f;
            /** A timer tracking how long the vibration has been active. */
            float Timer = 0.0f;
        };

        /** Updates the vibration for a given player. */
        static void UpdateVibration(PlayerIndex player, VibrationSettings& settings);

        // We track one instance of the manager for static use
        static VibrationManager* instance;

        // We associate each player with a single vibration, represented by a
        // VibrationSettings object. C#'s VibrationSettings is a class, so the dictionary
        // holds references and every write through the indexer mutates the shared object;
        // shared_ptr reproduces that, where a by-value TValue would not -- Dictionary's
        // .NET-style indexer hands back a proxy, not a mutable reference.
        Dictionary<PlayerIndex, std::shared_ptr<VibrationSettings>> vibrations;

        // We track our paused state separately from the Enabled property to avoid
        // accidentally calling Pause() without calling Resume()
        bool isPaused = false;
    };
}
