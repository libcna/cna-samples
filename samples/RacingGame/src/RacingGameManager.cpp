// SPDX-License-Identifier: MS-PL

#include "RacingGameManager.hpp"

#include <algorithm>
#include <bit>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <future>
#include <sstream>
#include <stdexcept>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsAdapter.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsProfile.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageDevice.hpp"
#include "GameLogic/Replay.hpp"
#include "GameScreens/GameScreen.hpp"
#include "GameScreens/LoadingScreen.hpp"
#include "GameScreens/MainMenu.hpp"
#include "GameScreens/SplashScreen.hpp"
#include "Graphics/LensFlare.hpp"
#include "Graphics/UIRenderer.hpp"
#include "Helpers/RandomHelper.hpp"
#include "Properties/GameSettings.hpp"
#include "Rendering/CarRenderer.hpp"
#include "Rendering/ShadowMapRenderer.hpp"
#include "Rendering/StaticTrackScene.hpp"
#include "Shaders/PostScreenGlow.hpp"
#include "Shaders/PostScreenMenu.hpp"
#include "Sounds/Sound.hpp"
#include "Tracks/Track.hpp"

namespace RacingGame
{
    using GameLogic::BrakeSoundType;
    using GameLogic::CarPhysics;
    using GameLogic::ChaseCamera;
    using GameLogic::CheckpointSoundType;
    using GameLogic::ControlFrame;
    using GameLogic::PlayerSound;
    using GameLogic::Replay;
    using GameLogic::TimeFadeupMode;
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::PresentationMode;
    using Microsoft::Xna::Framework::Storage::StorageDevice;
    using Sounds::SoundCue;
    using namespace Microsoft::Xna::Framework::Graphics;

    const std::array<Color, 11> RacingGameManager::CarColors{
        Color(255, 255, 255), Color(255, 255, 0), Color(0, 0, 255),
        Color(128, 0, 128), Color(255, 0, 0), Color(0, 128, 0),
        Color(0, 128, 128), Color(128, 128, 128), Color(210, 105, 30),
        Color(255, 165, 0), Color(46, 139, 87)};

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
        StorageDevice::SetAppNameEXT(configuration.storageAppName);
        settings = std::make_unique<Properties::GameSettings>();
        settings->Load();
        graphics->setGraphicsProfileProperty(GraphicsProfile::HiDef);
        int backBufferWidth = 1280;
        int backBufferHeight = 720;
        if (configuration.honorDisplaySettings)
        {
            backBufferWidth = settings->getResolutionWidthProperty();
            backBufferHeight = settings->getResolutionHeightProperty();
            if (backBufferWidth <= 0 || backBufferHeight <= 0)
            {
                const auto mode = GraphicsAdapter::getDefaultAdapterProperty()
                    .getCurrentDisplayModeProperty();
                backBufferWidth = mode.getWidthProperty();
                backBufferHeight = mode.getHeightProperty();
            }
            graphics->setIsFullScreenProperty(
                settings->getFullscreenProperty());
        }
        graphics->setPreferredBackBufferWidthProperty(backBufferWidth);
        graphics->setPreferredBackBufferHeightProperty(backBufferHeight);
        graphics->setPreferredPresentationModeProperty(
            PresentationMode::NativeBackBuffer);
        graphics->setSynchronizeWithVerticalRetraceProperty(false);
        getContentProperty().setRootDirectoryProperty(
            configuration.contentRoot);
        getWindowProperty().setTitleProperty("Racing Game");
        setIsFixedTimeStepProperty(false);
        setIsMouseVisibleProperty(false);
        InitializeHighscores();
        menuPreviewCarNumber = Helpers::RandomHelper::GetRandomInt(3);
        menuPreviewCarColor = Helpers::RandomHelper::getRandomColorProperty();
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

    Vector3 RacingGameManager::getCarDirectionProperty() const
    {
        return player ? player->getCarDirectionProperty() : Vector3::Zero;
    }

    Matrix RacingGameManager::getCurrentTrackMatrixProperty()
    {
        if (!player || !trackScene)
            return Matrix::getIdentityProperty();
        float roadWidth = 0.0f;
        float nextRoadWidth = 0.0f;
        return trackScene->getTrackProperty().GetTrackPositionMatrix(
            player->getTrackSegmentNumberProperty(),
            player->getTrackSegmentPercentProperty(), roadWidth,
            nextRoadWidth);
    }

    int RacingGameManager::getCarTrackSegmentProperty() const
    {
        return player ? player->getTrackSegmentNumberProperty() : -1;
    }

    float RacingGameManager::getCarSpeedProperty() const
    {
        return player ? player->getSpeedProperty() : 0.0f;
    }

    int RacingGameManager::getCurrentLapProperty() const
    {
        return player ? player->getCurrentLapProperty() : 0;
    }

    int RacingGameManager::getLastCarPartCountProperty() const
    {
        return lastCarPartCount;
    }

    int RacingGameManager::getLastGhostPartCountProperty() const
    {
        return lastGhostPartCount;
    }

    int RacingGameManager::getLandscapeModelCountProperty() const
    {
        return trackScene ? trackScene->getLandscapeModelCountProperty() : 0;
    }

    int RacingGameManager::getLandscapeObjectCountProperty() const
    {
        return trackScene ? trackScene->getLandscapeObjectCountProperty() : 0;
    }

    int RacingGameManager::getLastLandscapeModelPartCountProperty() const
    {
        return trackScene
            ? trackScene->getLastLandscapeModelPartCountProperty() : 0;
    }

    int RacingGameManager::getLastCityPlaneSubmissionCountProperty() const
    {
        return trackScene
            ? trackScene->getLastCityPlaneSubmissionCountProperty() : 0;
    }

    int RacingGameManager::getBrakeTrackVertexCountProperty() const
    {
        return trackScene ? trackScene->getBrakeTrackVertexCountProperty() : 0;
    }

    int RacingGameManager::getLastBrakeTrackPrimitiveCountProperty() const
    {
        return trackScene
            ? trackScene->getLastBrakeTrackPrimitiveCountProperty() : 0;
    }

    int RacingGameManager::getLastLensFlareSubmissionCountProperty() const
    {
        return lensFlare ? lensFlare->getLastSubmissionCountProperty() : 0;
    }

    int RacingGameManager::getLastPostScreenPassCountProperty() const
    {
        return postScreenGlow
            ? postScreenGlow->getLastPassCountProperty() : 0;
    }

    int RacingGameManager::getLastUiAtlasSpriteCountProperty() const
    {
        return uiRenderer
            ? uiRenderer->getLastAtlasSpriteCountProperty() : 0;
    }

    int RacingGameManager::getLastUiGlyphCountProperty() const
    {
        return uiRenderer ? uiRenderer->getLastGlyphCountProperty() : 0;
    }

    int RacingGameManager::getIngameUiTextureWidthProperty() const
    {
        return uiRenderer
            ? uiRenderer->getIngameTextureWidthProperty() : 0;
    }

    int RacingGameManager::getIngameUiTextureHeightProperty() const
    {
        return uiRenderer
            ? uiRenderer->getIngameTextureHeightProperty() : 0;
    }

    SurfaceFormat RacingGameManager::getIngameUiTextureFormatProperty() const
    {
        return uiRenderer
            ? uiRenderer->getIngameTextureFormatProperty()
            : SurfaceFormat::Color;
    }

    int RacingGameManager::getLastShadowCasterSubmissionCountProperty() const
    {
        return shadowRenderer
            ? shadowRenderer->getLastCasterSubmissionCountProperty() : 0;
    }

    int RacingGameManager::getLastShadowReceiverSubmissionCountProperty() const
    {
        return shadowRenderer
            ? shadowRenderer->getLastReceiverSubmissionCountProperty() : 0;
    }

    int RacingGameManager::getShadowMapNonWhitePixelCountProperty() const
    {
        return shadowRenderer
            ? shadowRenderer->getShadowMapNonWhitePixelCountProperty() : 0;
    }

    int RacingGameManager::getShadowReceiverNonWhitePixelCountProperty() const
    {
        return shadowRenderer
            ? shadowRenderer->getReceiverMapNonWhitePixelCountProperty() : 0;
    }

    float RacingGameManager::getDistanceFromStartProperty() const
    {
        return player ? Vector3::Distance(
            initialCarPosition, player->getCarPositionProperty()) : 0.0f;
    }

    int RacingGameManager::getBestReplayMatrixCountProperty() const
    {
        return bestReplay
            ? bestReplay->getNumberOfTrackMatricesProperty() : 0;
    }

    int RacingGameManager::getNewReplayMatrixCountProperty() const
    {
        return newReplay
            ? newReplay->getNumberOfTrackMatricesProperty() : 0;
    }

    Matrix RacingGameManager::getGhostCarMatrixProperty() const
    {
        if (!bestReplay || !player)
            return Matrix::getIdentityProperty();
        return bestReplay->GetCarMatrixAtTime(
            player->getGameTimeMillisecondsProperty() / 1000.0f);
    }

    const ControlFrame& RacingGameManager::getControlsProperty() const
    {
        return currentControls;
    }

    Graphics::UIRenderer& RacingGameManager::getUIProperty() const
    {
        if (!uiRenderer)
            throw std::runtime_error("Racing UI has not been loaded");
        return *uiRenderer;
    }

    float RacingGameManager::getMoveFactorPerSecondProperty() const
    {
        return elapsedMilliseconds / 1000.0f;
    }

    float RacingGameManager::getTotalTimeSecondsProperty() const
    {
        return totalMilliseconds / 1000.0f;
    }

    int RacingGameManager::getSelectedTrackNumberProperty() const
    {
        return selectedTrackNumber;
    }

    void RacingGameManager::setSelectedTrackNumberProperty(const int value)
    {
        if (value < 0 || value > 2)
            throw std::out_of_range("Racing track number must be between 0 and 2");
        selectedTrackNumber = value;
    }

    int RacingGameManager::getCurrentCarNumberProperty() const
    {
        return currentCarNumber;
    }

    void RacingGameManager::setCurrentCarNumberProperty(const int value)
    {
        if (value < 0 || value > 2)
            throw std::out_of_range("Racing car number must be between 0 and 2");
        currentCarNumber = value;
    }

    int RacingGameManager::getCurrentCarColorProperty() const
    {
        return currentCarColor;
    }

    void RacingGameManager::setCurrentCarColorProperty(const int value)
    {
        if (value < 0 || static_cast<std::size_t>(value) >= CarColors.size())
            throw std::out_of_range("Racing car color is outside the palette");
        currentCarColor = value;
    }

    Color RacingGameManager::getCarColorProperty() const
    {
        return CarColors[static_cast<std::size_t>(currentCarColor)];
    }

    int RacingGameManager::getCarColorCountProperty() const
    {
        return static_cast<int>(CarColors.size());
    }

    Color RacingGameManager::getCarColorProperty(const int index) const
    {
        return CarColors.at(static_cast<std::size_t>(index));
    }

    void RacingGameManager::PlaySound(const SoundCue cue)
    {
        if (sound) sound->Play(cue);
    }

    void RacingGameManager::StartGearSound()
    {
        if (sound) sound->StartGearSound();
    }

    void RacingGameManager::PlayMenuMusic()
    {
        PlaySound(SoundCue::MenuMusic);
    }

    void RacingGameManager::PlayGameMusic()
    {
        PlaySound(SoundCue::GameMusic);
    }

    void RacingGameManager::StopGearSoundNow()
    {
        if (sound) sound->StopGearSound();
    }

    void RacingGameManager::SetSoundVolumes(
        const float soundVolume, const float musicVolume)
    {
        if (sound) sound->SetVolumes(soundVolume, musicVolume);
    }

    bool RacingGameManager::getSoundInitializedProperty() const
    {
        return sound && sound->getIsInitializedProperty();
    }

    int RacingGameManager::getSoundPlayRequestCountProperty() const
    {
        return sound ? sound->getPlayRequestCountProperty() : 0;
    }

    int RacingGameManager::getGearCueStartCountProperty() const
    {
        return sound ? sound->getGearCueStartCountProperty() : 0;
    }

    int RacingGameManager::getLastSelectionPlatePartCountProperty() const
    {
        return lastSelectionPlatePartCount;
    }

    int RacingGameManager::getLastSelectionShadowCasterCountProperty() const
    {
        return lastSelectionShadowCasterCount;
    }

    int RacingGameManager::getLastSelectionShadowReceiverCountProperty() const
    {
        return lastSelectionShadowReceiverCount;
    }

    void RacingGameManager::AddGameScreen(
        std::unique_ptr<GameScreens::IGameScreen> screen)
    {
        if (!screen)
            throw std::invalid_argument("Racing game screen cannot be null");
        PlaySound(SoundCue::ScreenClick);
        gameScreens.push_back(std::move(screen));
    }

    void RacingGameManager::LoadContent()
    {
        sound = std::make_unique<Sounds::Sound>(
            *this, (std::filesystem::path(configuration.contentRoot) /
                    "Audio").string(), settings->getSoundVolumeProperty(),
            settings->getMusicVolumeProperty());
        uiRenderer = std::make_unique<Graphics::UIRenderer>(
            getGraphicsDeviceProperty(), getContentProperty());

        if (configuration.skipScreens)
        {
            selectedTrackNumber = 0;
            LoadCarResources();
            LoadLandscapeResources();
            LoadTextureResources();
            InitializePlayerForCurrentTrack();
            loadingStage = LoadingStage::Complete;
            AddGameScreen(std::make_unique<GameScreens::GameScreen>(
                *this, false));
        }
        else
        {
            selectedTrackNumber = 1;
            AddGameScreen(std::make_unique<GameScreens::MainMenu>(*this));
            AddGameScreen(std::make_unique<GameScreens::SplashScreen>(*this));
            AddGameScreen(std::make_unique<GameScreens::LoadingScreen>(*this));
        }
    }

    void RacingGameManager::LoadCarResources()
    {
        carRenderer = std::make_unique<Rendering::CarRenderer>(
            getGraphicsDeviceProperty(), getContentProperty());
    }

    void RacingGameManager::LoadLandscapeResources()
    {
        trackScene = std::make_unique<Rendering::StaticTrackScene>(
            getGraphicsDeviceProperty(), getContentProperty(),
            "TrackBeginner", settings->getHighDetailProperty());
    }

    void RacingGameManager::LoadTextureResources()
    {
        shadowRenderer = std::make_unique<Rendering::ShadowMapRenderer>(
            getGraphicsDeviceProperty(), getContentProperty(),
            settings->getHighDetailProperty());
        lensFlare = std::make_unique<Graphics::LensFlare>(
            getGraphicsDeviceProperty(), getContentProperty(),
            Graphics::LensFlare::DefaultSunPos);
        postScreenGlow = std::make_unique<Shaders::PostScreenGlow>(
            getGraphicsDeviceProperty(), getContentProperty());
        postScreenMenu = std::make_unique<Shaders::PostScreenMenu>(
            getGraphicsDeviceProperty(), getContentProperty());
    }

    void RacingGameManager::InitializePlayerForCurrentTrack()
    {
        const Tracks::Track& track = trackScene->getTrackProperty();
        initialCarPosition = track.getStartPositionProperty();
        const int level = GetSelectedTrackNumber();
        const float topLapTime = static_cast<float>(
            highscoreTimes[0][0]) / 1000.0f;
        bestReplay = std::make_unique<Replay>(
            0, false, track, topLapTime, configuration.contentRoot);
        newReplay = std::make_unique<Replay>(
            level, true, track, topLapTime, configuration.contentRoot);
        player = std::make_unique<GameLogic::Player>(
            static_cast<GameLogic::PlayerEnvironment&>(*this),
            initialCarPosition);
        player->SetCarPosition(
            track.getStartPositionProperty(),
            track.getStartDirectionProperty(),
            track.getStartUpVectorProperty());
        player->Reset();
    }

    void RacingGameManager::AdvanceLoading()
    {
        // XNA loaded these resources on a worker thread. CNA graphics resources
        // are owner-thread-affine, so the same observable stages are advanced
        // cooperatively on the game thread.
        switch (loadingStage)
        {
        case LoadingStage::Unstarted:
            loadingStatus = "Models...";
            loadingStatusMask |= 1;
            loadingStage = LoadingStage::Models;
            break;
        case LoadingStage::Models:
            LoadCarResources();
            loadingStatus = "Landscape...";
            loadingStatusMask |= 2;
            loadingStage = LoadingStage::Landscape;
            break;
        case LoadingStage::Landscape:
            LoadLandscapeResources();
            loadingStatus = "Textures...";
            loadingStatusMask |= 4;
            loadingStage = LoadingStage::Textures;
            break;
        case LoadingStage::Textures:
            LoadTextureResources();
            InitializePlayerForCurrentTrack();
            loadingStatus = "All systems go!";
            loadingStatusMask |= 8;
            loadingReadyMilliseconds = 0.0f;
            loadingStage = LoadingStage::Ready;
            break;
        case LoadingStage::Ready:
            loadingReadyMilliseconds += elapsedMilliseconds;
            if (loadingReadyMilliseconds >=
                configuration.loadingReadyDelayMilliseconds)
                loadingStage = LoadingStage::Complete;
            break;
        case LoadingStage::Complete:
            break;
        }
    }

    const std::string& RacingGameManager::getLoadingStatusProperty() const
    {
        return loadingStatus;
    }

    bool RacingGameManager::getContentLoadedProperty() const
    {
        return loadingStage == LoadingStage::Complete;
    }

    void RacingGameManager::UnloadContent()
    {
        gameScreens.clear();
        player.reset();
        if (replaySave.valid())
            replaySave.wait();
        if (settingsSave.valid())
            settingsSave.wait();
        newReplay.reset();
        bestReplay.reset();
        uiRenderer.reset();
        postScreenMenu.reset();
        postScreenGlow.reset();
        lensFlare.reset();
        shadowRenderer.reset();
        carRenderer.reset();
        trackScene.reset();
        sound.reset();
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

        if (sound) sound->Update();

        currentControls = controlSource->Capture(
            IsInGame(), getIsActiveProperty(), GetDisplayWidth(),
            GetDisplayHeight());
        if (!gameScreens.empty())
        {
            if (player && gameScreens.back()->getKindProperty() !=
                              GameScreens::ScreenKind::Loading)
                UpdateRace();
            ++screenVisitCounts[static_cast<std::size_t>(
                gameScreens.back()->getKindProperty())];
            gameScreens.back()->Update(gameTime);
        }
        if (configuration.frameLimit > 0 &&
            updateCount >= configuration.frameLimit)
            exitAfterDraw = true;
    }

    void RacingGameManager::Draw(const GameTime& gameTime)
    {
        ++drawCount;
        if (gameScreens.empty())
        {
            if (!exitSoundPlayed)
            {
                PlayCrashSound(true);
                if (sound) sound->StopMusic();
                exitSoundPlayed = true;
            }
            Exit();
            Game::Draw(gameTime);
            return;
        }
        const GameScreens::ScreenKind renderedKind =
            gameScreens.back()->getKindProperty();
        if (gameScreens.back()->Render())
        {
            PlaySound(SoundCue::ScreenBack);
            gameScreens.pop_back();
            if (configuration.honorDisplaySettings &&
                renderedKind == GameScreens::ScreenKind::Options)
                ApplyDisplaySettings();
        }

        if (exitAfterDraw)
        {
            if (!configuration.capturePath.empty()) WriteCapture();
            Exit();
        }
        Game::Draw(gameTime);
    }

    void RacingGameManager::UpdateRace()
    {
        player->Update(currentControls.car);
        if (updateCount % 10 == 0)
        {
            disableLensFlareInTunnel = trackScene->getTrackProperty().IsTunnel(
                player->getTrackSegmentNumberProperty());
        }
    }

    void RacingGameManager::DrawRace()
    {
        GraphicsDevice& device = getGraphicsDeviceProperty();
        device.Clear(Color(71, 112, 156, 255));
        const float aspect =
            device.getViewportProperty().getAspectRatioProperty();
        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::Pi / 2.0f, aspect, 0.5f, 1750.0f);
        if (settings->getShadowMappingProperty())
        {
            shadowRenderer->Prepare(
                *trackScene, *carRenderer,
                player->getCarRenderMatrixProperty(),
                player->getCarPositionProperty(),
                player->getCarDirectionProperty(), view, projection,
                totalMilliseconds / 1000.0f);
        }
        if (settings->getPostScreenEffectsProperty()) postScreenGlow->Start();
        trackScene->Draw(view, projection, totalMilliseconds / 1000.0f);
        lastCarPartCount = carRenderer->Draw(
            currentCarNumber, player->getCarWheelPosProperty(),
            player->getCarRenderMatrixProperty(), view, projection,
            getCarColorProperty());
        lastGhostPartCount = 0;
        if (player->getGameTimeMillisecondsProperty() > 0.0f)
        {
            const Matrix replayMatrix =
                Matrix::CreateRotationX(MathHelper::Pi / 2.0f) *
                Matrix::CreateRotationZ(MathHelper::Pi) *
                getGhostCarMatrixProperty();
            lastGhostPartCount = carRenderer->DrawGhost(
                player->getCarWheelPosProperty(), replayMatrix,
                view, projection);
        }
        if (settings->getShadowMappingProperty())
            shadowRenderer->ShowShadows();
        postScreenGlow->Show(player->getSpeedProperty());
        if (sound)
            sound->UpdateGearSound(
                player->getSpeedProperty(),
                player->getAccelerationProperty());
        lensFlare->Render(Color::White, view, projection,
                          !disableLensFlareInTunnel);
        const float speed = player->getSpeedProperty();
        uiRenderer->RenderGameUI(
            static_cast<int>(player->getGameTimeMillisecondsProperty()),
            static_cast<int>(player->getBestTimeMillisecondsProperty()),
            player->getCurrentLapProperty() + 1,
            speed * CarPhysics::MeterPerSecToMph,
            1 + static_cast<int>(5.0f * speed /
                                 CarPhysics::MaxPossibleSpeed),
            0.5f * speed / CarPhysics::MaxPossibleSpeed +
                0.5f * player->getAccelerationProperty(),
            std::array<const char*, 3>{"Beginner", "Advanced", "Expert"}[
                static_cast<std::size_t>(selectedTrackNumber)],
            highscoreTimes[static_cast<std::size_t>(
                GetSelectedTrackNumber())],
            player->getGameOverProperty(), elapsedMilliseconds);
        if (player->getVictoryProperty())
        {
            uiRenderer->RenderTrophy(GetRankFromCurrentTime(
                player->getLevelNumProperty(),
                static_cast<int>(player->getBestTimeMillisecondsProperty())));
        }
        uiRenderer->RenderTextsAndMouseCursor(
            currentControls, false, elapsedMilliseconds);
        maximumTrophyCount = std::max(
            maximumTrophyCount,
            uiRenderer->getLastTrophyCountProperty());

    }

    void RacingGameManager::BeginMenuFrame(
        const bool renderOverlay, const bool renderWorld,
        const bool usePostProcess)
    {
        GraphicsDevice& device = getGraphicsDeviceProperty();
        device.Clear(Color(71, 112, 156, 255));
        if (usePostProcess && settings->getPostScreenEffectsProperty())
            postScreenMenu->Start();

        if (renderWorld)
        {
            const Matrix replay = bestReplay->GetCarMatrixAtTime(menuCarTimeSeconds);
            menuCarTimeSeconds += elapsedMilliseconds / 1000.0f;
            if (menuCarTimeSeconds > bestReplay->getLapTimeProperty())
                menuCarTimeSeconds -= bestReplay->getLapTimeProperty();
            const Vector3 position = replay.getTranslationProperty();
            const Vector3 forward = replay.getForwardProperty();
            const Vector3 up = replay.getUpProperty();
            if (menuCarForward == Vector3::Zero) menuCarForward = forward;
            if (menuCarUp == Vector3::Zero) menuCarUp = up;
            menuCarForward = menuCarForward * 0.95f + forward * 0.05f;
            menuCarUp = menuCarUp * 0.95f + up * 0.05f;
            player->SetCarPosition(position, forward, up);
            player->SetCameraPosition(
                position + menuCarForward * 13.0f - menuCarUp * 1.3f);
            player->Update(currentControls.car);
            const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
                MathHelper::Pi / 2.0f,
                device.getViewportProperty().getAspectRatioProperty(),
                0.5f, 1750.0f);
            if (!usePostProcess && settings->getShadowMappingProperty())
            {
                shadowRenderer->Prepare(
                    *trackScene, *carRenderer,
                    player->getCarRenderMatrixProperty(),
                    player->getCarPositionProperty(),
                    player->getCarDirectionProperty(), view, projection,
                    getTotalTimeSecondsProperty());
            }
            trackScene->Draw(
                view, projection, getTotalTimeSecondsProperty());
            (void)carRenderer->Draw(
                menuPreviewCarNumber, 0.0f,
                player->getCarRenderMatrixProperty(), view, projection,
                menuPreviewCarColor);
            if (!usePostProcess && settings->getShadowMappingProperty())
                shadowRenderer->ShowShadows();
            lensFlare->Render(Color::White, view, projection,
                              !disableLensFlareInTunnel);
        }

        uiRenderer->BeginScreen();
        if (renderOverlay)
            uiRenderer->RenderMenuOverlay(getTotalTimeSecondsProperty());
    }

    void RacingGameManager::EndMenuFrame()
    {
        uiRenderer->EndScreen();
        if (postScreenMenu)
            postScreenMenu->Show(getTotalTimeSecondsProperty());
        const bool showCursor = !gameScreens.empty() &&
            gameScreens.back()->getKindProperty() !=
                GameScreens::ScreenKind::Splash &&
            gameScreens.back()->getKindProperty() !=
                GameScreens::ScreenKind::Loading;
        uiRenderer->RenderTextsAndMouseCursor(
            currentControls, showCursor, elapsedMilliseconds);
        maximumUiLinePrimitiveCount = std::max(
            maximumUiLinePrimitiveCount,
            uiRenderer->getLastLinePrimitiveCountProperty());
    }

    void RacingGameManager::EndCarSelectionFrame(const float rotation)
    {
        uiRenderer->EndScreen();
        DrawCarSelectionWorld(rotation);
        postScreenMenu->Show(getTotalTimeSecondsProperty());
        uiRenderer->RenderTextsAndMouseCursor(
            currentControls, true, elapsedMilliseconds);
    }

    void RacingGameManager::PrepareCarSelectionShadows(const float rotation)
    {
        if (!settings->getShadowMappingProperty()) return;
        GraphicsDevice& device = getGraphicsDeviceProperty();
        const Matrix selectionView = Matrix::CreateLookAt(
            Vector3(0.0f, 10.45f, 2.75f), Vector3(0.0f, 0.0f, -1.0f),
            Vector3(0.0f, 0.0f, 1.0f));
        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::Pi / 2.0f,
            device.getViewportProperty().getAspectRatioProperty(),
            0.5f, 1750.0f);
        const float perCarRotation = MathHelper::TwoPi / 3.0f;
        std::array<Matrix, 3> matrices;
        for (int car = 0; car < 3; ++car)
        {
            matrices[static_cast<std::size_t>(car)] =
                Matrix::CreateRotationZ(getTotalTimeSecondsProperty() / 3.9f) *
                Matrix::CreateTranslation(Vector3(0.0f, 5.0f, 0.0f)) *
                Matrix::CreateRotationZ(-rotation + car * perCarRotation) *
                Matrix::CreateTranslation(Vector3(1.5f, 0.0f, 1.0f));
        }
        shadowRenderer->PrepareCarSelection(
            *carRenderer, matrices, selectionView, projection);
        lastSelectionShadowCasterCount =
            shadowRenderer->getLastCasterSubmissionCountProperty();
        lastSelectionShadowReceiverCount =
            shadowRenderer->getLastReceiverSubmissionCountProperty();
    }

    void RacingGameManager::DrawCarSelectionWorld(const float rotation)
    {
        GraphicsDevice& device = getGraphicsDeviceProperty();
        const Matrix selectionView = Matrix::CreateLookAt(
            Vector3(0.0f, 10.45f, 2.75f), Vector3(0.0f, 0.0f, -1.0f),
            Vector3(0.0f, 0.0f, 1.0f));
        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::Pi / 2.0f,
            device.getViewportProperty().getAspectRatioProperty(),
            0.5f, 1750.0f);
        const float perCarRotation = MathHelper::TwoPi / 3.0f;
        lastSelectionPlatePartCount = 0;
        for (int car = 0; car < 3; ++car)
        {
            const Matrix world =
                Matrix::CreateRotationZ(getTotalTimeSecondsProperty() / 3.9f) *
                Matrix::CreateTranslation(Vector3(0.0f, 5.0f, 0.0f)) *
                Matrix::CreateRotationZ(-rotation + car * perCarRotation) *
                Matrix::CreateTranslation(Vector3(1.5f, 0.0f, 1.0f));
            lastSelectionPlatePartCount += carRenderer->DrawSelectionPlate(
                world, selectionView, projection);
            (void)carRenderer->Draw(
                car, 0.0f, world, selectionView, projection,
                getCarColorProperty());
        }
        if (settings->getShadowMappingProperty())
            shadowRenderer->ShowShadows();
    }

    void RacingGameManager::LoadSelectedLevel()
    {
        static constexpr std::array<const char*, 3> TrackAssets{
            "TrackBeginner", "TrackAdvanced", "TrackExpert"};
        trackScene = std::make_unique<Rendering::StaticTrackScene>(
            getGraphicsDeviceProperty(), getContentProperty(),
            TrackAssets[static_cast<std::size_t>(selectedTrackNumber)],
            settings->getHighDetailProperty());
        const Tracks::Track& track = trackScene->getTrackProperty();
        initialCarPosition = track.getStartPositionProperty();
        const float topLapTime = static_cast<float>(
            highscoreTimes[static_cast<std::size_t>(selectedTrackNumber)][0]) /
            1000.0f;
        bestReplay = std::make_unique<Replay>(
            selectedTrackNumber, false, track, topLapTime,
            configuration.contentRoot);
        newReplay = std::make_unique<Replay>(
            selectedTrackNumber, true, track, topLapTime,
            configuration.contentRoot);
        player = std::make_unique<GameLogic::Player>(
            static_cast<GameLogic::PlayerEnvironment&>(*this),
            initialCarPosition);
        player->SetCarPosition(
            track.getStartPositionProperty(), track.getStartDirectionProperty(),
            track.getStartUpVectorProperty());
        player->Reset();
        menuCarTimeSeconds = 0.0f;
    }

    GameScreens::ScreenKind
    RacingGameManager::getCurrentScreenKindProperty() const
    {
        if (gameScreens.empty()) return GameScreens::ScreenKind::MainMenu;
        return gameScreens.back()->getKindProperty();
    }

    int RacingGameManager::getScreenCountProperty() const
    {
        return static_cast<int>(gameScreens.size());
    }

    int RacingGameManager::getLoadingStatusCountProperty() const
    {
        return std::popcount(static_cast<unsigned int>(loadingStatusMask));
    }

    int RacingGameManager::getMaximumUiLinePrimitiveCountProperty() const
    {
        return maximumUiLinePrimitiveCount;
    }

    int RacingGameManager::getLastMouseCursorCountProperty() const
    {
        return uiRenderer
            ? uiRenderer->getLastMouseCursorCountProperty() : 0;
    }

    int RacingGameManager::getMaximumTrophyCountProperty() const
    {
        return maximumTrophyCount;
    }

    int RacingGameManager::getScreenVisitCountProperty(
        const GameScreens::ScreenKind kind) const
    {
        return screenVisitCounts.at(static_cast<std::size_t>(kind));
    }

    bool RacingGameManager::getRaceGameOverProperty() const
    {
        return player && player->getGameOverProperty();
    }

    bool RacingGameManager::getGearSoundStoppedProperty() const
    {
        return gearSoundStopped;
    }

    int RacingGameManager::getPlayerSoundCountProperty() const
    {
        return playerSoundCount;
    }

    int RacingGameManager::getSubmittedHighscoreLevelProperty() const
    {
        return submittedHighscoreLevel;
    }

    int RacingGameManager::getSubmittedHighscoreMillisecondsProperty() const
    {
        return submittedHighscoreMilliseconds;
    }

    const std::array<int, 10>& RacingGameManager::getHighscoreTimesProperty(
        const int level) const
    {
        return highscoreTimes.at(static_cast<std::size_t>(level));
    }

    const std::string& RacingGameManager::getHighscoreNameProperty(
        const int level, const int rank) const
    {
        return highscoreNames.at(static_cast<std::size_t>(level))
            .at(static_cast<std::size_t>(rank));
    }

    Properties::GameSettings& RacingGameManager::getSettingsProperty() const
    {
        if (!settings)
            throw std::runtime_error("Racing settings are not initialized");
        return *settings;
    }

    int RacingGameManager::getDisplayWidthProperty() const
    {
        return GetDisplayWidth();
    }

    int RacingGameManager::getDisplayHeightProperty() const
    {
        return GetDisplayHeight();
    }

    bool RacingGameManager::getFullscreenProperty() const
    {
        return graphics->getIsFullScreenProperty();
    }

    void RacingGameManager::InitializeHighscores()
    {
        const auto fillDefaults = [this]
        {
            for (std::size_t level = 0; level < highscoreTimes.size(); ++level)
            {
                for (std::size_t rank = 0;
                     rank < highscoreTimes[level].size(); ++rank)
                {
                    highscoreNames[level][rank] =
                        "Player " + std::to_string(rank + 1);
                    highscoreTimes[level][rank] =
                        (75000 + static_cast<int>(rank) * 5000) *
                        (static_cast<int>(level) + 1);
                }
            }
        };
        const std::string serialized = settings->getHighscoresProperty();
        if (serialized.empty())
        {
            fillDefaults();
            WriteHighscoresToSettings();
            return;
        }

        std::istringstream entries(serialized);
        std::string entry;
        int index = 0;
        while (index < 30 && std::getline(entries, entry, ','))
        {
            const std::size_t separator = entry.find(':');
            if (separator == std::string::npos)
                throw std::invalid_argument("missing highscore separator");
            const int level = index / 10;
            const int rank = index % 10;
            highscoreNames[static_cast<std::size_t>(level)]
                          [static_cast<std::size_t>(rank)] =
                entry.substr(0, separator);
            highscoreTimes[static_cast<std::size_t>(level)]
                          [static_cast<std::size_t>(rank)] =
                std::stoi(entry.substr(separator + 1));
            ++index;
        }
    }

    void RacingGameManager::WriteHighscoresToSettings()
    {
        std::string serialized;
        for (std::size_t level = 0; level < highscoreTimes.size(); ++level)
        {
            for (std::size_t rank = 0;
                 rank < highscoreTimes[level].size(); ++rank)
            {
                if (!serialized.empty()) serialized += ',';
                serialized += highscoreNames[level][rank] + ':' +
                    std::to_string(highscoreTimes[level][rank]);
            }
        }
        settings->setHighscoresProperty(serialized);
        if (settingsSave.valid()) settingsSave.wait();
        settingsSave = std::async(
            std::launch::async, [this] { settings->Save(); });
    }

    void RacingGameManager::ApplyDisplaySettings()
    {
        if (trackScene)
            trackScene->setHighDetailProperty(
                settings->getHighDetailProperty());
        int width = settings->getResolutionWidthProperty();
        int height = settings->getResolutionHeightProperty();
        if (width <= 0 || height <= 0)
        {
            const auto mode = GraphicsAdapter::getDefaultAdapterProperty()
                .getCurrentDisplayModeProperty();
            width = mode.getWidthProperty();
            height = mode.getHeightProperty();
        }
        if (width == GetDisplayWidth() && height == GetDisplayHeight() &&
            settings->getFullscreenProperty() ==
                graphics->getIsFullScreenProperty())
            return;
        graphics->setPreferredBackBufferWidthProperty(width);
        graphics->setPreferredBackBufferHeightProperty(height);
        graphics->setIsFullScreenProperty(settings->getFullscreenProperty());
        graphics->ApplyChanges();

        shadowRenderer = std::make_unique<Rendering::ShadowMapRenderer>(
            getGraphicsDeviceProperty(), getContentProperty(),
            settings->getHighDetailProperty());
        postScreenGlow = std::make_unique<Shaders::PostScreenGlow>(
            getGraphicsDeviceProperty(), getContentProperty());
        postScreenMenu = std::make_unique<Shaders::PostScreenMenu>(
            getGraphicsDeviceProperty(), getContentProperty());
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

    int RacingGameManager::GetSelectedTrackNumber() const
    {
        return selectedTrackNumber;
    }
    bool RacingGameManager::IsInMenu() const
    {
        return !gameScreens.empty() &&
            gameScreens.back()->getKindProperty() !=
                GameScreens::ScreenKind::Game;
    }
    float RacingGameManager::GetElapsedMilliseconds() const
    {
        return elapsedMilliseconds;
    }
    void RacingGameManager::StartLandscapeLap()
    {
        const float lapTime =
            player->getGameTimeMillisecondsProperty() / 1000.0f;
        const int level = player->getLevelNumProperty();
        SubmitHighscore(level,
                        static_cast<int>(
                            player->getGameTimeMillisecondsProperty()));
        player->AddLapTime(lapTime);

        if (lapTime < bestReplay->getLapTimeProperty())
        {
            newReplay->getCheckpointTimesProperty().push_back(lapTime);
            newReplay->setLapTimeProperty(lapTime);
            auto replayToSave =
                std::make_shared<Replay>(*newReplay);
            if (replaySave.valid())
                replaySave.wait();
            replaySave = std::async(
                std::launch::async,
                [replayToSave] { replayToSave->Save(); });
            bestReplay = std::make_unique<Replay>(*newReplay);
        }

        const Tracks::Track& track = trackScene->getTrackProperty();
        const float topLapTime = static_cast<float>(
            highscoreTimes[static_cast<std::size_t>(level)][0]) / 1000.0f;
        newReplay = std::make_unique<Replay>(
            level, true, track, topLapTime, configuration.contentRoot);
    }
    void RacingGameManager::ReplaceStartLightObject(const int state)
    {
        startLightState = state;
        if (trackScene) trackScene->ReplaceStartLightObject(state);
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
        auto& names = highscoreNames[static_cast<std::size_t>(level)];
        for (std::size_t rank = 0; rank < scores.size(); ++rank)
        {
            if (milliseconds <= scores[rank])
            {
                std::move_backward(
                    scores.begin() + static_cast<std::ptrdiff_t>(rank),
                    scores.end() - 1, scores.end());
                std::move_backward(
                    names.begin() + static_cast<std::ptrdiff_t>(rank),
                    names.end() - 1, names.end());
                scores[rank] = milliseconds;
                names[rank] = settings->getPlayerNameProperty();
                WriteHighscoresToSettings();
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
        return settings->getControllerSensitivityProperty();
    }
    bool RacingGameManager::IsFreeCamera() const
    {
        return player && player->getFreeCameraProperty();
    }
    bool RacingGameManager::IsInGame() const
    {
        return !gameScreens.empty() &&
            gameScreens.back()->getKindProperty() ==
                GameScreens::ScreenKind::Game;
    }
    Matrix RacingGameManager::GetViewMatrix() const { return view; }
    void RacingGameManager::SetViewMatrix(const Matrix matrix) { view = matrix; }
    Vector3 RacingGameManager::GetRandomVector3(
        const float minimum, const float maximum)
    {
        return Helpers::RandomHelper::GetRandomVector3(minimum, maximum);
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
        return newReplay
            ? static_cast<int>(
                newReplay->getCheckpointTimesProperty().size()) : 0;
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
        if (!bestReplay || index < 0 ||
            static_cast<std::size_t>(index) >=
                bestReplay->getCheckpointTimesProperty().size())
            return 0;
        return static_cast<int>(
            player->getGameTimeMillisecondsProperty() -
            bestReplay->getCheckpointTimesProperty()[
                static_cast<std::size_t>(index)] * 1000.0f);
    }
    void RacingGameManager::AddCheckpointTime(const float seconds)
    {
        newReplay->getCheckpointTimesProperty().push_back(seconds);
    }
    void RacingGameManager::AddTimeFadeupEffect(
        const int milliseconds, const TimeFadeupMode mode)
    {
        if (uiRenderer)
            uiRenderer->AddTimeFadeupEffect(milliseconds, mode);
    }
    void RacingGameManager::PlayCheckpointSound(const CheckpointSoundType type)
    {
        ++checkpointSoundCount;
        if (sound) sound->PlayCheckpointSound(type);
    }
    void RacingGameManager::AddBrakeTrack(const CarPhysics& car)
    {
        ++brakeTrackCount;
        if (trackScene) trackScene->AddBrakeTrack(car);
    }
    void RacingGameManager::PlayBrakeSound(const BrakeSoundType type)
    {
        ++brakeSoundCount;
        if (sound) sound->PlayBrakeSound(type);
    }
    void RacingGameManager::PlayCrashSound(const bool totalCrash)
    {
        ++crashSoundCount;
        if (sound) sound->PlayCrashSound(totalCrash);
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
        return newReplay
            ? newReplay->getNumberOfTrackMatricesProperty() : 0;
    }
    void RacingGameManager::AddReplayCarMatrix(const Matrix matrix)
    {
        newReplay->AddCarMatrix(matrix);
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
        if (uiRenderer)
            uiRenderer->WriteTextCentered(x, y, text, color, scale);
    }
    void RacingGameManager::PlayPlayerSound(const PlayerSound playerSound)
    {
        ++playerSoundCount;
        if (sound) sound->PlayPlayerSound(playerSound);
    }
    void RacingGameManager::StopGearSound()
    {
        gearSoundStopped = true;
        StopGearSoundNow();
    }
}
