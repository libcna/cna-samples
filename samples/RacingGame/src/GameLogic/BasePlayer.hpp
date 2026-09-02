// SPDX-License-Identifier: MS-PL

#pragma once

namespace RacingGame::GameLogic
{
    /** @brief Supplies game-owned state and side effects used by BasePlayer. */
    class BasePlayerEnvironment
    {
    public:
        /** @brief Destroys a game-owned BasePlayer environment interface. */
        virtual ~BasePlayerEnvironment() = default;

        /** @brief Gets the selected track number. */
        [[nodiscard]] virtual int GetSelectedTrackNumber() const = 0;
        /** @brief Gets whether the game currently displays a menu. */
        [[nodiscard]] virtual bool IsInMenu() const = 0;
        /** @brief Gets elapsed milliseconds for the current frame. */
        [[nodiscard]] virtual float GetElapsedMilliseconds() const = 0;
        /** @brief Starts a new landscape lap. */
        virtual void StartLandscapeLap() = 0;
        /** @brief Replaces the start-light object with the requested state. */
        virtual void ReplaceStartLightObject(int state) = 0;
        /** @brief Submits a completed time to the high-score service. */
        virtual void SubmitHighscore(int level, int milliseconds) = 0;
    };

    /** @brief Stores the Racing player's shared race state and timing. */
    class BasePlayer
    {
    public:
        /** @brief Total pre-race camera zoom duration in milliseconds. */
        static constexpr int StartGameZoomTimeMilliseconds = 5000;
        /** @brief Fully zoomed-in start-light duration in milliseconds. */
        static constexpr int StartGameZoomedInTime = 3000;

        /**
         * @brief Creates player state backed by the game-owned environment.
         * @param setEnvironment Environment supplying frame state and effects.
         */
        explicit BasePlayer(BasePlayerEnvironment& setEnvironment);
        /** @brief Destroys shared player state through its polymorphic base. */
        virtual ~BasePlayer() = default;

        /** @brief Gets the current lap. */
        [[nodiscard]] int getCurrentLapProperty() const;
        /** @brief Gets the best completed lap time in milliseconds. */
        [[nodiscard]] float getBestTimeMillisecondsProperty() const;
        /** @brief Gets current race time excluding the pre-race zoom. */
        [[nodiscard]] float getGameTimeMillisecondsProperty() const;
        /** @brief Gets whether the last race result was a victory. */
        [[nodiscard]] bool getVictoryProperty() const;
        /** @brief Gets the selected level number. */
        [[nodiscard]] int getLevelNumProperty() const;
        /** @brief Gets whether the current race is over. */
        [[nodiscard]] bool getGameOverProperty() const;
        /** @brief Gets whether the player won the current race. */
        [[nodiscard]] bool getWonGameProperty() const;
        /** @brief Gets whether ordinary car control is currently allowed. */
        [[nodiscard]] bool getCanControlCarProperty() const;

        /** @brief Marks the race over and submits its high score once. */
        void SetGameOverAndUploadHighscore();
        /** @brief Resets all shared player state for a new race. */
        virtual void Reset();
        /** @brief Clears derived state after game over. */
        virtual void ClearVariablesForGameOver();
        /** @brief Advances shared player timing and start-light state. */
        virtual void Update();

    protected:
        /** @brief Completes a lap and resets lap-relative timing. */
        void StartNewLap();
        /** @brief Gets the remaining pre-race zoom time. */
        [[nodiscard]] float getZoomInTimeProperty() const;
        /** @brief Sets the remaining pre-race zoom time. */
        void setZoomInTimeProperty(float value);

        BasePlayerEnvironment& environment;
        float currentGameTimeMilliseconds = 0.0f;
        int lap = 0;
        bool victory = false;
        int levelNum = 0;
        bool isGameOver = false;

    private:
        float bestLapTimeMilliseconds = 0.0f;
        float zoomInTime = StartGameZoomTimeMilliseconds;
        bool alreadyUploadedHighscore = false;
        bool firstFrame = true;
    };
}
