// SPDX-License-Identifier: MS-PL

#include "RacingGameManager.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsProfile.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Rendering/CarRenderer.hpp"
#include "Rendering/StaticTrackScene.hpp"
#include "Tracks/Track.hpp"

namespace RacingGame
{
    using GameLogic::BrakeSoundType;
    using GameLogic::CarPhysics;
    using GameLogic::ChaseCamera;
    using GameLogic::ControlFrame;
    using GameLogic::PlayerSound;
    using GameLogic::TimeFadeupMode;
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::PresentationMode;
    using namespace Microsoft::Xna::Framework::Graphics;

    RacingGameManager::RacingGameManager()
        : RacingGameManager(std::make_unique<GameLogic::Input>(), {})
    {
    }

    RacingGameManager::RacingGameManager(
        std::unique_ptr<GameLogic::ControlSource> setControlSource,
        RacingRunConfiguration setConfiguration)
        : configuration(std::move(setConfiguration)),
          graphics(std::make_unique<GraphicsDeviceManager>(this)),
          controlSource(std::move(setControlSource))
    {
        if (!controlSource)
            throw std::invalid_argument("Racing control source cannot be null");
        graphics->setGraphicsProfileProperty(GraphicsProfile::HiDef);
        graphics->setPreferredBackBufferWidthProperty(1280);
        graphics->setPreferredBackBufferHeightProperty(720);
        graphics->setPreferredPresentationModeProperty(
            PresentationMode::NativeBackBuffer);
        graphics->setSynchronizeWithVerticalRetraceProperty(false);
        getContentProperty().setRootDirectoryProperty(
            configuration.contentRoot);
        getWindowProperty().setTitleProperty("Racing Game");
        setIsFixedTimeStepProperty(false);
        setIsMouseVisibleProperty(false);
        for (std::size_t level = 0; level < highscoreTimes.size(); ++level)
        {
            for (std::size_t rank = 0;
                 rank < highscoreTimes[level].size(); ++rank)
            {
                highscoreTimes[level][rank] =
                    (75000 + static_cast<int>(rank) * 5000) *
                    (static_cast<int>(level) + 1);
            }
        }
    }

    RacingGameManager::~RacingGameManager() = default;

    const std::string& RacingGameManager::GetTypeName() const
    {
        static const std::string name = "RacingGame.RacingGameManager";
        return name;
    }

    int RacingGameManager::getUpdateCountProperty() const
    {
        return updateCount;
    }

    int RacingGameManager::getDrawCountProperty() const
    {
        return drawCount;
    }

    Vector3 RacingGameManager::getCarPositionProperty() const
    {
        return player ? player->getCarPositionProperty() : Vector3::Zero;
    }

    int RacingGameManager::getLastCarPartCountProperty() const
    {
        return lastCarPartCount;
    }

    float RacingGameManager::getDistanceFromStartProperty() const
    {
        return player ? Vector3::Distance(
            initialCarPosition, player->getCarPositionProperty()) : 0.0f;
    }

    void RacingGameManager::LoadContent()
    {
        trackScene = std::make_unique<Rendering::StaticTrackScene>(
            getGraphicsDeviceProperty(), getContentProperty(),
            "TrackBeginner");
        carRenderer = std::make_unique<Rendering::CarRenderer>(
            getGraphicsDeviceProperty(), getContentProperty());
        const Tracks::Track& track = trackScene->getTrackProperty();
        initialCarPosition = track.getStartPositionProperty();
        player = std::make_unique<GameLogic::Player>(
            static_cast<GameLogic::PlayerEnvironment&>(*this),
            initialCarPosition);
        player->SetCarPosition(
            track.getStartPositionProperty(),
            track.getStartDirectionProperty(),
            track.getStartUpVectorProperty());
        player->Reset();
    }

    void RacingGameManager::UnloadContent()
    {
        player.reset();
        carRenderer.reset();
        trackScene.reset();
    }

    void RacingGameManager::Update(GameTime& gameTime)
    {
        Game::Update(gameTime);
        elapsedMilliseconds = configuration.elapsedMillisecondsOverride > 0.0f
            ? configuration.elapsedMillisecondsOverride
            : static_cast<float>(gameTime.getElapsedGameTimeProperty()
                                     .getTotalMillisecondsProperty());
        if (elapsedMilliseconds <= 0.0f)
            elapsedMilliseconds = 0.001f;
        totalMilliseconds += elapsedMilliseconds;
        ++updateCount;

        const ControlFrame controls = controlSource->Capture(
            true, getIsActiveProperty(), GetDisplayWidth(),
            GetDisplayHeight());
        player->Update(controls.car);
        if (controls.exitRequested ||
            (configuration.frameLimit > 0 &&
             updateCount >= configuration.frameLimit))
            exitAfterDraw = true;
    }

    void RacingGameManager::Draw(const GameTime& gameTime)
    {
        ++drawCount;
        GraphicsDevice& device = getGraphicsDeviceProperty();
        device.Clear(Color(71, 112, 156, 255));
        const float aspect =
            device.getViewportProperty().getAspectRatioProperty();
        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::Pi / 2.0f, aspect, 0.5f, 1750.0f);
        trackScene->Draw(view, projection);
        lastCarPartCount = carRenderer->Draw(
            player->getCarWheelPosProperty(),
            player->getCarRenderMatrixProperty(), view, projection,
            Color::White);

        if (exitAfterDraw)
        {
            if (!configuration.capturePath.empty())
                WriteCapture();
            Exit();
        }
        Game::Draw(gameTime);
    }

    void RacingGameManager::WriteCapture()
    {
        GraphicsDevice& device = getGraphicsDeviceProperty();
        const int width = GetDisplayWidth();
        const int height = GetDisplayHeight();
        std::vector<Color> pixels(
            static_cast<std::size_t>(width * height), Color::Transparent);
        device.GetBackBufferData(pixels.data(),
                                 static_cast<int>(pixels.size()));
        const std::filesystem::path path(configuration.capturePath);
        if (path.has_parent_path())
            std::filesystem::create_directories(path.parent_path());
        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        output << "P6\n" << width << ' ' << height << "\n255\n";
        for (const Color& pixel : pixels)
        {
            const char rgb[3]{
                static_cast<char>(pixel.getRProperty()),
                static_cast<char>(pixel.getGProperty()),
                static_cast<char>(pixel.getBProperty()),
            };
            output.write(rgb, sizeof(rgb));
        }
        if (!output)
            throw std::runtime_error("Racing backbuffer capture failed");
    }

    int RacingGameManager::GetSelectedTrackNumber() const { return 0; }
    bool RacingGameManager::IsInMenu() const { return false; }
    float RacingGameManager::GetElapsedMilliseconds() const
    {
        return elapsedMilliseconds;
    }
    void RacingGameManager::StartLandscapeLap()
    {
        SubmitHighscore(
            player->getLevelNumProperty(),
            static_cast<int>(player->getGameTimeMillisecondsProperty()));
        player->AddLapTime(
            player->getGameTimeMillisecondsProperty() / 1000.0f);
        checkpointTimes.clear();
        replayMatrices.clear();
    }
    void RacingGameManager::ReplaceStartLightObject(const int state)
    {
        startLightState = state;
    }
    void RacingGameManager::SubmitHighscore(
        const int level, const int milliseconds)
    {
        submittedHighscoreLevel = level;
        submittedHighscoreMilliseconds = milliseconds;
        if (level < 0 ||
            static_cast<std::size_t>(level) >= highscoreTimes.size())
            return;
        auto& scores = highscoreTimes[static_cast<std::size_t>(level)];
        for (std::size_t rank = 0; rank < scores.size(); ++rank)
        {
            if (milliseconds <= scores[rank])
            {
                std::move_backward(
                    scores.begin() + static_cast<std::ptrdiff_t>(rank),
                    scores.end() - 1, scores.end());
                scores[rank] = milliseconds;
                break;
            }
        }
    }
    float RacingGameManager::GetMoveFactorPerSecond() const
    {
        return elapsedMilliseconds / 1000.0f;
    }
    float RacingGameManager::GetTotalTimeMilliseconds() const
    {
        return totalMilliseconds;
    }
    float RacingGameManager::GetControllerSensitivity() const
    {
        return 0.0f;
    }
    bool RacingGameManager::IsFreeCamera() const
    {
        return player && player->getFreeCameraProperty();
    }
    bool RacingGameManager::IsInGame() const { return true; }
    Matrix RacingGameManager::GetViewMatrix() const { return view; }
    void RacingGameManager::SetViewMatrix(const Matrix matrix) { view = matrix; }
    Vector3 RacingGameManager::GetRandomVector3(
        const float minimum, const float maximum)
    {
        const auto next = [this, minimum, maximum]
        {
            return static_cast<float>(random.NextDouble()) *
                   (maximum - minimum) + minimum;
        };
        return {next(), next(), next()};
    }
    void RacingGameManager::UpdateCarTrackPosition(
        const Vector3 carPosition, int& segment, float& segmentPercent)
    {
        trackScene->getTrackProperty().UpdateCarTrackPosition(
            carPosition, segment, segmentPercent);
    }
    Matrix RacingGameManager::GetTrackPositionMatrix(
        const int segment, const float segmentPercent,
        float& roadWidth, float& nextRoadWidth)
    {
        return trackScene->getTrackProperty().GetTrackPositionMatrix(
            segment, segmentPercent, roadWidth, nextRoadWidth);
    }
    int RacingGameManager::GetCheckpointTimeCount() const
    {
        return static_cast<int>(checkpointTimes.size());
    }
    int RacingGameManager::GetCheckpointSegmentCount() const
    {
        return static_cast<int>(trackScene->getTrackProperty()
            .getCheckpointSegmentPositionsProperty().size());
    }
    int RacingGameManager::GetCheckpointSegment(const int index) const
    {
        return trackScene->getTrackProperty()
            .getCheckpointSegmentPositionsProperty()
            .at(static_cast<std::size_t>(index));
    }
    int RacingGameManager::CompareCheckpointTime(const int index)
    {
        if (index < 0 ||
            static_cast<std::size_t>(index) >= bestCheckpointTimes.size())
            return 0;
        return static_cast<int>(
            player->getGameTimeMillisecondsProperty() -
            bestCheckpointTimes[static_cast<std::size_t>(index)] * 1000.0f);
    }
    void RacingGameManager::AddCheckpointTime(const float seconds)
    {
        checkpointTimes.push_back(seconds);
    }
    void RacingGameManager::AddTimeFadeupEffect(
        int, const TimeFadeupMode mode)
    {
        timeFadeups.push_back(mode);
    }
    void RacingGameManager::AddBrakeTrack(const CarPhysics&)
    {
        ++brakeTrackCount;
    }
    void RacingGameManager::PlayBrakeSound(BrakeSoundType)
    {
        ++brakeSoundCount;
    }
    void RacingGameManager::PlayCrashSound(bool)
    {
        ++crashSoundCount;
    }
    void RacingGameManager::WobbelCamera(const float factor)
    {
        ChaseCamera::WobbelCamera(factor);
    }
    void RacingGameManager::SetCameraPosition(const Vector3 position)
    {
        requestedCameraPosition = position;
    }
    void RacingGameManager::InterpolateCameraPosition(const Vector3 position)
    {
        interpolatedCameraPosition = position;
    }
    int RacingGameManager::GetReplayMatrixCount() const
    {
        return static_cast<int>(replayMatrices.size());
    }
    void RacingGameManager::AddReplayCarMatrix(const Matrix matrix)
    {
        replayMatrices.push_back(matrix);
    }
    int RacingGameManager::GetDisplayWidth() const
    {
        return graphics->getGraphicsDeviceProperty()->getViewportProperty()
            .getWidthProperty();
    }
    int RacingGameManager::GetDisplayHeight() const
    {
        return graphics->getGraphicsDeviceProperty()->getViewportProperty()
            .getHeightProperty();
    }
    int RacingGameManager::YToRes(const int value) const
    {
        return static_cast<int>(std::nearbyint(
            value * GetDisplayHeight() / 640.0f));
    }
    int RacingGameManager::GetRankFromCurrentTime(
        const int level, const int timeMilliseconds) const
    {
        if (timeMilliseconds < 1000 || level < 0 ||
            static_cast<std::size_t>(level) >= highscoreTimes.size())
            return 10;
        const auto& scores = highscoreTimes[static_cast<std::size_t>(level)];
        for (std::size_t rank = 0; rank < scores.size(); ++rank)
        {
            if (timeMilliseconds <= scores[rank])
                return static_cast<int>(rank);
        }
        return 10;
    }
    void RacingGameManager::WriteTextCentered(
        const int x, const int y, const std::string& text,
        const Color color, const float scale)
    {
        textRecords.push_back({x, y, text, color, scale});
    }
    void RacingGameManager::PlayPlayerSound(PlayerSound)
    {
        ++playerSoundCount;
    }
    void RacingGameManager::StopGearSound()
    {
        gearSoundStopped = true;
    }
}
