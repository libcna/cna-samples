// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// VibrationManager.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "VibrationManager.hpp"

#include <memory>

#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "System/InvalidOperationException.hpp"
#include "System/Math.hpp"

namespace CameraShake
{
    using Microsoft::Xna::Framework::Input::GamePad;

    namespace
    {
        // C#'s `for (PlayerIndex p = One; p <= Four; p++)` over the enum itself.
        constexpr PlayerIndex kPlayers[] = {PlayerIndex::One, PlayerIndex::Two,
                                            PlayerIndex::Three, PlayerIndex::Four};
    }

    VibrationManager* VibrationManager::instance = nullptr;

    VibrationManager::VibrationManager(Game& game)
        : GameComponent(game)
    {
        // We only allow one manager per game
        if (instance != nullptr)
            throw System::InvalidOperationException("Cannot create multiple VibrationManagers");

        // Store the manager instance
        instance = this;

        // Add a setting for each player
        for (PlayerIndex player : kPlayers)
            vibrations.Add(player, std::make_shared<VibrationSettings>());
    }

    VibrationManager::~VibrationManager()
    {
        // C# leaves the static reference standing for the process's lifetime; C++ objects
        // really are destroyed, so the slot is released to keep the "only one" rule honest.
        if (instance == this) instance = nullptr;
    }

    void VibrationManager::Vibrate(PlayerIndex player, float left, float right, float duration)
    {
        // We don't allow starting new vibrations when the system is paused.
        if (instance->isPaused)
            return;

        // Get our settings. Dictionary's .NET-style indexer hands back a proxy, so the
        // shared_ptr is bound before it is followed.
        const std::shared_ptr<VibrationSettings> slot = instance->vibrations[player];
        VibrationSettings& settings = *slot;

        // Initialize the settings object
        settings.Left = left;
        settings.Right = right;
        settings.Duration = duration;
        settings.Timer = 0.0f;

        // Set our initial vibration for the GamePad.
        GamePad::SetVibration(player, left, right);

#if defined(WINDOWS_PHONE)
        // If we're setting the vibration for player one, we also start the VibrateController
        if (player == PlayerIndex::One)
        {
            Microsoft::Devices::VibrateController::getDefaultProperty().Start(
                System::TimeSpan::FromSeconds(duration));
        }
#endif
    }

    void VibrationManager::Pause()
    {
        // Pause our instance
        instance->isPaused = true;

        // Stop the vibration on all GamePads
        for (PlayerIndex player : kPlayers)
            GamePad::SetVibration(player, 0, 0);

#if defined(WINDOWS_PHONE)
        // Stop the VibrateController for the phone
        Microsoft::Devices::VibrateController::getDefaultProperty().Stop();
#endif
    }

    void VibrationManager::Resume()
    {
        // Unpause the instance
        instance->isPaused = false;

        for (PlayerIndex player : kPlayers)
        {
            // We call UpdateVibration for all players which will resume our vibrations
            // from the correct point in our progress.
            const std::shared_ptr<VibrationSettings> slot = instance->vibrations[player];
            VibrationSettings& settings = *slot;
            UpdateVibration(player, settings);

#if defined(WINDOWS_PHONE)
            // If we're looking at player one and the vibration is not complete, we also
            // start the VibrateController with the remaining time needed to complete
            // the original duration.
            if (player == PlayerIndex::One && settings.Timer < settings.Duration)
            {
                Microsoft::Devices::VibrateController::getDefaultProperty().Start(
                    System::TimeSpan::FromSeconds(settings.Duration - settings.Timer));
            }
#endif
        }
    }

    void VibrationManager::CancelAll()
    {
        // Iterate all vibrations, setting their durations to 0 and turning off the vibration
        // of the GamePads.
        for (PlayerIndex player : kPlayers)
        {
            const std::shared_ptr<VibrationSettings> slot = instance->vibrations[player];
            VibrationSettings& settings = *slot;
            settings.Duration = 0.0f;
            GamePad::SetVibration(player, 0, 0);
        }

#if defined(WINDOWS_PHONE)
        // On the phone we also need to stop the VibrateController.
        Microsoft::Devices::VibrateController::getDefaultProperty().Stop();
#endif
    }

    void VibrationManager::Update(GameTime& gameTime)
    {
        // Do nothing if we're currently paused
        if (instance->isPaused)
            return;

        // Get the elapsed frame time.
        const float time =
            (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        // Update all of the vibrations
        for (PlayerIndex player : kPlayers)
        {
            const std::shared_ptr<VibrationSettings> slot = vibrations[player];
            VibrationSettings& settings = *slot;

            // If the vibration is not complete
            if (settings.Timer < settings.Duration)
            {
                // Update our timer, clamping at the duration
                settings.Timer = (float)System::Math::Min(settings.Timer + time,
                                                          settings.Duration);

                // Update the vibration
                UpdateVibration(player, settings);
            }
        }
    }

    void VibrationManager::UpdateVibration(PlayerIndex player, VibrationSettings& settings)
    {
        // If the vibration is at its duration, stop the vibration of the GamePad.
        if (settings.Timer >= settings.Duration)
        {
            GamePad::SetVibration(player, 0, 0);
        }
        else
        {
            // Compute our progress in a [0, 1] range
            const float progress = settings.Timer / settings.Duration;

            // Calculate our left and right motor strengths while applying a linear decay
            // over time.
            const float left = settings.Left * (1.0f - progress);
            const float right = settings.Right * (1.0f - progress);

            // Set the vibration of the GamePad.
            GamePad::SetVibration(player, left, right);
        }
    }
}
