// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>
#include <future>
#include <memory>
#include <string>
#include <vector>

#include "GameLogic/Input.hpp"
#include "GameLogic/Player.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "System/Random.hpp"

namespace RacingGame::Rendering
{
    class CarRenderer;
    class StaticTrackScene;
}

namespace RacingGame::GameLogic
{
    class Replay;
}

namespace RacingGame
{
    /** @brief Optional deterministic settings used by the executable qualification probe. */
    struct RacingRunConfiguration
    {
        /** @brief ContentManager root; defaults to the deployed Content directory. */
        std::string contentRoot = "Content";
        /** @brief Draw count after which the game exits; zero runs normally. */
        int frameLimit = 0;
        /** @brief Environment frame duration override; zero uses GameTime. */
        float elapsedMillisecondsOverride = 0.0f;
        /** @brief Optional PPM backbuffer capture written on the final frame. */
        std::string capturePath;
        /** @brief Storage application identity; defaults to the product identity. */
        std::string storageAppName = "RacingGame";
    };

    /** @brief Runs the Racing Game Kit race scene on CNA. */
    class RacingGameManager final : public Microsoft::Xna::Framework::Game,
                                    private GameLogic::PlayerEnvironment
    {
    public:
        /** @brief Creates the normal desktop game with real device input. */
        RacingGameManager();

        /**
         * @brief Creates a game with an injected control source and run configuration.
         * @param controlSource Logical input provider owned by the game.
         * @param configuration Content and deterministic run settings.
         */
        RacingGameManager(
            std::unique_ptr<GameLogic::ControlSource> controlSource,
            RacingRunConfiguration configuration);
        /** @brief Destroys game-owned scene resources after their types are complete. */
        ~RacingGameManager() override;

        /** @brief Returns the original fully qualified game type name. */
        [[nodiscard]] const std::string& GetTypeName() const override;

        /** @brief Gets the number of completed update calls. */
        [[nodiscard]] int getUpdateCountProperty() const;
        /** @brief Gets the number of completed draw calls. */
        [[nodiscard]] int getDrawCountProperty() const;
        /** @brief Gets the car position after the latest update. */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getCarPositionProperty() const;
        /** @brief Gets the number of car parts submitted on the latest draw. */
        [[nodiscard]] int getLastCarPartCountProperty() const;
        /** @brief Gets the number of replay-ghost parts submitted on the latest draw. */
        [[nodiscard]] int getLastGhostPartCountProperty() const;
        /** @brief Gets straight-line displacement from the loaded start position. */
        [[nodiscard]] float getDistanceFromStartProperty() const;
        /** @brief Gets the generated/loaded best replay matrix count. */
        [[nodiscard]] int getBestReplayMatrixCountProperty() const;
        /** @brief Gets the current lap recording matrix count. */
        [[nodiscard]] int getNewReplayMatrixCountProperty() const;
        /** @brief Gets the best replay matrix at the current race time. */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix
        getGhostCarMatrixProperty() const;

    protected:
        void LoadContent() override;
        void UnloadContent() override;
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        struct TextRecord
        {
            int x = 0;
            int y = 0;
            std::string text;
            Microsoft::Xna::Framework::Color color;
            float scale = 1.0f;
        };

        RacingRunConfiguration configuration;
        std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager>
            graphics;
        std::unique_ptr<GameLogic::ControlSource> controlSource;
        std::unique_ptr<Rendering::StaticTrackScene> trackScene;
        std::unique_ptr<Rendering::CarRenderer> carRenderer;
        std::unique_ptr<GameLogic::Player> player;
        std::unique_ptr<GameLogic::Replay> bestReplay;
        std::unique_ptr<GameLogic::Replay> newReplay;
        std::future<void> replaySave;
        Microsoft::Xna::Framework::Matrix view =
            Microsoft::Xna::Framework::Matrix::getIdentityProperty();
        Microsoft::Xna::Framework::Vector3 initialCarPosition =
            Microsoft::Xna::Framework::Vector3::Zero;
        float elapsedMilliseconds = 0.001f;
        float totalMilliseconds = 0.0f;
        System::Random random;
        std::vector<TextRecord> textRecords;
        std::vector<GameLogic::TimeFadeupMode> timeFadeups;
        std::array<std::array<int, 10>, 3> highscoreTimes{};
        int updateCount = 0;
        int drawCount = 0;
        int lastCarPartCount = 0;
        int lastGhostPartCount = 0;
        int brakeTrackCount = 0;
        int brakeSoundCount = 0;
        int checkpointSoundCount = 0;
        int crashSoundCount = 0;
        int playerSoundCount = 0;
        int startLightState = 3;
        int submittedHighscoreLevel = -1;
        int submittedHighscoreMilliseconds = 0;
        Microsoft::Xna::Framework::Vector3 requestedCameraPosition =
            Microsoft::Xna::Framework::Vector3::Zero;
        Microsoft::Xna::Framework::Vector3 interpolatedCameraPosition =
            Microsoft::Xna::Framework::Vector3::Zero;
        bool gearSoundStopped = false;
        bool exitAfterDraw = false;

        void WriteCapture();

        [[nodiscard]] int GetSelectedTrackNumber() const override;
        [[nodiscard]] bool IsInMenu() const override;
        [[nodiscard]] float GetElapsedMilliseconds() const override;
        void StartLandscapeLap() override;
        void ReplaceStartLightObject(int state) override;
        void SubmitHighscore(int level, int milliseconds) override;
        [[nodiscard]] float GetMoveFactorPerSecond() const override;
        [[nodiscard]] float GetTotalTimeMilliseconds() const override;
        [[nodiscard]] float GetControllerSensitivity() const override;
        [[nodiscard]] bool IsFreeCamera() const override;
        [[nodiscard]] bool IsInGame() const override;
        [[nodiscard]] Microsoft::Xna::Framework::Matrix
        GetViewMatrix() const override;
        void SetViewMatrix(Microsoft::Xna::Framework::Matrix matrix) override;
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        GetRandomVector3(float minimum, float maximum) override;
        void UpdateCarTrackPosition(
            Microsoft::Xna::Framework::Vector3 carPosition,
            int& segment, float& segmentPercent) override;
        [[nodiscard]] Microsoft::Xna::Framework::Matrix
        GetTrackPositionMatrix(int segment, float segmentPercent,
                               float& roadWidth,
                               float& nextRoadWidth) override;
        [[nodiscard]] int GetCheckpointTimeCount() const override;
        [[nodiscard]] int GetCheckpointSegmentCount() const override;
        [[nodiscard]] int GetCheckpointSegment(int index) const override;
        [[nodiscard]] int CompareCheckpointTime(int index) override;
        void AddCheckpointTime(float seconds) override;
        void AddTimeFadeupEffect(
            int milliseconds, GameLogic::TimeFadeupMode mode) override;
        void PlayCheckpointSound(
            GameLogic::CheckpointSoundType type) override;
        void AddBrakeTrack(const GameLogic::CarPhysics& car) override;
        void PlayBrakeSound(GameLogic::BrakeSoundType type) override;
        void PlayCrashSound(bool totalCrash) override;
        void WobbelCamera(float factor) override;
        void SetCameraPosition(
            Microsoft::Xna::Framework::Vector3 position) override;
        void InterpolateCameraPosition(
            Microsoft::Xna::Framework::Vector3 position) override;
        [[nodiscard]] int GetReplayMatrixCount() const override;
        void AddReplayCarMatrix(
            Microsoft::Xna::Framework::Matrix matrix) override;
        [[nodiscard]] int GetDisplayWidth() const override;
        [[nodiscard]] int GetDisplayHeight() const override;
        [[nodiscard]] int YToRes(int value) const override;
        [[nodiscard]] int GetRankFromCurrentTime(
            int level, int timeMilliseconds) const override;
        void WriteTextCentered(
            int x, int y, const std::string& text,
            Microsoft::Xna::Framework::Color color, float scale) override;
        void PlayPlayerSound(GameLogic::PlayerSound sound) override;
        void StopGearSound() override;
    };
}
