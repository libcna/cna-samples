// SPDX-License-Identifier: MS-PL

#pragma once

#include <memory>
#include <unordered_set>

#include "GameLogic/Input.hpp"
#include "GameLogic/MobileControls.hpp"

namespace Microsoft::Devices::Sensors
{
    class Accelerometer;
}

namespace RacingGame::GameLogic
{
    /** @brief Combines the unchanged desktop devices with Android touch and optional tilt. */
    class MobileInput final : public ControlSource
    {
    public:
        /** @brief Creates a provider with the default touch-control preferences. */
        MobileInput();

        /**
         * @brief Creates a provider with explicit mobile preferences.
         * @param preferences Touch layout and optional tilt behavior.
         */
        explicit MobileInput(MobileControlPreferences preferences);

        /** @brief Stops and releases an optional accelerometer session. */
        ~MobileInput() override;

        /**
         * @brief Captures desktop/gamepad input and merges real touch and tilt actions.
         * @param inGame True while the race screen is active.
         * @param appActive True while the application is active.
         * @param displayWidth Current backbuffer width.
         * @param displayHeight Current backbuffer height.
         * @return One merged logical control snapshot.
         */
        [[nodiscard]] ControlFrame Capture(
            bool inGame, bool appActive, int displayWidth,
            int displayHeight) override;

        /**
         * @brief Updates the safe rectangle used for touch layout.
         * @param safeArea Safe interactive rectangle in backbuffer coordinates.
         */
        void SetSafeArea(
            Microsoft::Xna::Framework::Rectangle safeArea) override;

        /**
         * @brief Replaces the live mobile preferences.
         * @param preferences New layout and tilt preferences.
         */
        void setPreferencesProperty(
            const MobileControlPreferences& preferences);

        /**
         * @brief Gets the current mobile preferences.
         * @return Current layout and tilt preferences.
         */
        [[nodiscard]] const MobileControlPreferences&
        getPreferencesProperty() const;

    private:
        Input desktopInput;
        MobileControlPreferences preferences;
        MobileControlMapper mapper;
        TiltSteeringFilter tiltFilter;
        Microsoft::Xna::Framework::Rectangle safeArea;
        std::unique_ptr<Microsoft::Devices::Sensors::Accelerometer> accelerometer;
        std::unordered_set<int> ignoredUntilReleaseTouchIds;
        bool tiltStartAttempted = false;
        bool touchWasActive = false;
        bool wasInGame = false;

        void EnsureTiltSensor();
        void StopTiltSensor();
    };
}
