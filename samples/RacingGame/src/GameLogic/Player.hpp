// SPDX-License-Identifier: MS-PL

#pragma once

#include <string>
#include <vector>

#include "GameLogic/ChaseCamera.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"

namespace RacingGame::GameLogic
{
    /** @brief Player-owned sound events emitted by race outcome logic. */
    enum class PlayerSound
    {
        /** @brief Victory fanfare. */
        Victory,
        /** @brief Race-loss sound. */
        CarLose,
    };

    /** @brief Supplies UI, ranking and outcome-audio services used by Player. */
    class PlayerEnvironment : public CarPhysicsEnvironment
    {
    public:
        /** @brief Gets the current backbuffer width. */
        [[nodiscard]] virtual int GetDisplayWidth() const = 0;
        /** @brief Gets the current backbuffer height. */
        [[nodiscard]] virtual int GetDisplayHeight() const = 0;
        /** @brief Scales an original 768-high Y coordinate to the current display. */
        [[nodiscard]] virtual int YToRes(int value) const = 0;
        /** @brief Gets the player's rank for the completed race time. */
        [[nodiscard]] virtual int GetRankFromCurrentTime(
            int level, int timeMilliseconds) const = 0;
        /** @brief Writes one centered player result line. */
        virtual void WriteTextCentered(
            int x, int y, const std::string& text,
            Microsoft::Xna::Framework::Color color, float scale) = 0;
        /** @brief Plays a race-outcome sound. */
        virtual void PlayPlayerSound(PlayerSound sound) = 0;
        /** @brief Stops the looping engine gear sound. */
        virtual void StopGearSound() = 0;
    };

    /** @brief Adds race outcome, lap and fall handling to the chase-camera car. */
    class Player : public ChaseCamera
    {
    public:
        /** @brief Number of laps required to win a race. */
        static constexpr int LapCount = 3;
        /** @brief Time airborne before the race is lost. */
        static constexpr float InAirTimeoutMilliseconds = 3000.0f;

        /**
         * @brief Creates the local player at an initial car position.
         * @param setEnvironment Game-owned player services.
         * @param setCarPosition Initial car position.
         */
        Player(PlayerEnvironment& setEnvironment,
               Microsoft::Xna::Framework::Vector3 setCarPosition);

        /**
         * @brief Records one completed lap time in seconds for the result display.
         * @param setLapTime Completed lap time in seconds.
         */
        void AddLapTime(float setLapTime);
        /** @brief Resets inherited state and clears recorded lap times. */
        void Reset() override;
        /**
         * @brief Advances player outcome logic and then the car/camera controller.
         * @param input Logical controls captured for this frame.
         */
        void Update(const CarControlState& input) override;

    private:
        PlayerEnvironment& playerEnvironment;
        std::vector<float> lapTimes;
        float inAirTimeMilliseconds = 0.0f;
    };
}
