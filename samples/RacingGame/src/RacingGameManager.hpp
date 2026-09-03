// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>
#include <future>
#include <memory>
#include <string>
#include <vector>

#include "GameLogic/Input.hpp"
#include "GameLogic/Player.hpp"
#include "GameScreens/IGameScreen.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"

namespace RacingGame::Rendering
{
    class CarRenderer;
    class ShadowMapRenderer;
    class StaticTrackScene;
}

namespace RacingGame::GameLogic
{
    class Replay;
    class ScreenshotCapturer;
}

namespace RacingGame::Shaders
{
    class PostScreenGlow;
    class PostScreenMenu;
}

namespace RacingGame::Sounds
{
    enum class SoundCue;
    class Sound;
}

namespace RacingGame::Properties
{
    class GameSettings;
}

namespace RacingGame::Platform
{
    class ContentDelivery;
}

namespace RacingGame
{
    namespace Graphics
    {
        class LensFlare;
        class UIRenderer;
    }

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
        /** @brief Starts directly in a race for deterministic scene qualification. */
        bool skipScreens = false;
        /** @brief Applies persisted desktop resolution/fullscreen settings. */
        bool honorDisplaySettings = true;
        /** @brief Minimum time the completed loading status remains visible. */
        float loadingReadyDelayMilliseconds = 1000.0f;
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
        /** @brief Gets the original whole-frame FPS measurement. */
        [[nodiscard]] int getFpsProperty() const;
        /** @brief Gets the original slowly interpolated FPS measurement. */
        [[nodiscard]] float getFpsInterpolatedProperty() const;
        /** @brief Gets the number of frames counted by the original FPS tracker. */
        [[nodiscard]] int getTotalFrameCountProperty() const;
        /** @brief Gets whether the original debug FPS overlay is enabled. */
        [[nodiscard]] bool getShowFpsProperty() const;
        /** @brief Gets the latest screenshot number allocated by PrintScreen. */
        [[nodiscard]] int getScreenshotNumberProperty() const;
        /** @brief Gets the number of graphics-device resets handled by the game. */
        [[nodiscard]] int getDeviceResetCountProperty() const;
        /** @brief Gets effect passes executed by the latest menu post-process. */
        [[nodiscard]] int getLastMenuPostScreenPassCountProperty() const;
        /** @brief Gets the car position after the latest update. */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getCarPositionProperty() const;
        /** @brief Gets the car's current forward direction for drive qualification. */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getCarDirectionProperty() const;
        /** @brief Gets the interpolated track frame beneath the active car. */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix
        getCurrentTrackMatrixProperty();
        /** @brief Gets the active car's track segment for drive qualification. */
        [[nodiscard]] int getCarTrackSegmentProperty() const;
        /** @brief Gets the active car's scalar speed for drive qualification. */
        [[nodiscard]] float getCarSpeedProperty() const;
        /** @brief Gets the active player's completed-lap index. */
        [[nodiscard]] int getCurrentLapProperty() const;
        /** @brief Gets the number of car parts submitted on the latest draw. */
        [[nodiscard]] int getLastCarPartCountProperty() const;
        /** @brief Gets the number of replay-ghost parts submitted on the latest draw. */
        [[nodiscard]] int getLastGhostPartCountProperty() const;
        /** @brief Gets the number of authentic landscape models loaded by the race scene. */
        [[nodiscard]] int getLandscapeModelCountProperty() const;
        /** @brief Gets the number of resolved landscape object instances. */
        [[nodiscard]] int getLandscapeObjectCountProperty() const;
        /** @brief Gets the number of visible landscape model parts submitted most recently. */
        [[nodiscard]] int getLastLandscapeModelPartCountProperty() const;
        /** @brief Gets whether the original city-ground plane was submitted most recently. */
        [[nodiscard]] int getLastCityPlaneSubmissionCountProperty() const;
        /** @brief Gets the number of retained tire-mark vertices. */
        [[nodiscard]] int getBrakeTrackVertexCountProperty() const;
        /** @brief Gets the number of tire-mark triangles submitted most recently. */
        [[nodiscard]] int getLastBrakeTrackPrimitiveCountProperty() const;
        /** @brief Gets the number of original lens-flare sprites submitted most recently. */
        [[nodiscard]] int getLastLensFlareSubmissionCountProperty() const;
        /** @brief Gets effect passes executed by the latest post-screen glow. */
        [[nodiscard]] int getLastPostScreenPassCountProperty() const;
        /** @brief Gets authentic HUD atlas sprites submitted most recently. */
        [[nodiscard]] int getLastUiAtlasSpriteCountProperty() const;
        /** @brief Gets authentic bitmap-font glyphs submitted most recently. */
        [[nodiscard]] int getLastUiGlyphCountProperty() const;
        /** @brief Gets sprites submitted by the latest mobile-control overlay. */
        [[nodiscard]] int getLastMobileControlSpriteCountProperty() const;
        /** @brief Gets the authentic in-game texture width. */
        [[nodiscard]] int getIngameUiTextureWidthProperty() const;
        /** @brief Gets the authentic in-game texture height. */
        [[nodiscard]] int getIngameUiTextureHeightProperty() const;
        /** @brief Gets the authentic in-game XNB surface-format identifier. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::SurfaceFormat
        getIngameUiTextureFormatProperty() const;
        /** @brief Gets caster submissions from the latest authentic shadow map. */
        [[nodiscard]] int getLastShadowCasterSubmissionCountProperty() const;
        /** @brief Gets receiver submissions from the latest authentic shadow pass. */
        [[nodiscard]] int getLastShadowReceiverSubmissionCountProperty() const;
        /** @brief Counts non-white pixels in the latest 16-bit shadow depth map. */
        [[nodiscard]] int getShadowMapNonWhitePixelCountProperty() const;
        /** @brief Counts non-white pixels in the latest 16-bit receiver map. */
        [[nodiscard]] int getShadowReceiverNonWhitePixelCountProperty() const;
        /** @brief Gets straight-line displacement from the loaded start position. */
        [[nodiscard]] float getDistanceFromStartProperty() const;
        /** @brief Gets the generated/loaded best replay matrix count. */
        [[nodiscard]] int getBestReplayMatrixCountProperty() const;
        /** @brief Gets the current lap recording matrix count. */
        [[nodiscard]] int getNewReplayMatrixCountProperty() const;
        /** @brief Gets the best replay matrix at the current race time. */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix
        getGhostCarMatrixProperty() const;
        /** @brief Gets the logical inputs captured for the current frame. */
        [[nodiscard]] const GameLogic::ControlFrame&
        getControlsProperty() const;
        /** @brief Gets the original UI renderer used by the screen stack. */
        [[nodiscard]] Graphics::UIRenderer& getUIProperty() const;
        /** @brief Gets elapsed seconds used by original menu interpolation. */
        [[nodiscard]] float getMoveFactorPerSecondProperty() const;
        /** @brief Gets total elapsed game time in seconds. */
        [[nodiscard]] float getTotalTimeSecondsProperty() const;
        /** @brief Gets the currently selected track number. */
        [[nodiscard]] int getSelectedTrackNumberProperty() const;
        /** @brief Sets the track selected by the track-selection screen. */
        void setSelectedTrackNumberProperty(int value);
        /** @brief Gets the currently selected car texture number. */
        [[nodiscard]] int getCurrentCarNumberProperty() const;
        /** @brief Sets the currently selected car texture number. */
        void setCurrentCarNumberProperty(int value);
        /** @brief Gets the currently selected car hue index. */
        [[nodiscard]] int getCurrentCarColorProperty() const;
        /** @brief Sets the currently selected car hue index. */
        void setCurrentCarColorProperty(int value);
        /** @brief Gets the selected original car hue. */
        [[nodiscard]] Microsoft::Xna::Framework::Color
        getCarColorProperty() const;
        /** @brief Gets the number of selectable original car hues. */
        [[nodiscard]] int getCarColorCountProperty() const;
        /** @brief Gets one original car hue without changing the selection. */
        [[nodiscard]] Microsoft::Xna::Framework::Color
        getCarColorProperty(int index) const;
        /** @brief Plays one cue from the authentic Racing XACT sound bank. */
        void PlaySound(Sounds::SoundCue cue);
        /** @brief Starts the original looping gear sound. */
        void StartGearSound();
        /** @brief Plays the original menu music cue. */
        void PlayMenuMusic();
        /** @brief Plays the original in-race music cue. */
        void PlayGameMusic();
        /** @brief Stops the original looping gear sound. */
        void StopGearSoundNow();
        /** @brief Applies current effects and music volumes. */
        void SetSoundVolumes(float soundVolume, float musicVolume);
        /** @brief Gets whether authentic XACT products initialized. */
        [[nodiscard]] bool getSoundInitializedProperty() const;
        /** @brief Gets successful named XACT cue play requests. */
        [[nodiscard]] int getSoundPlayRequestCountProperty() const;
        /** @brief Gets started XACT gear and transition cues. */
        [[nodiscard]] int getGearCueStartCountProperty() const;
        /** @brief Gets selection-plate mesh parts submitted in the latest carousel frame. */
        [[nodiscard]] int getLastSelectionPlatePartCountProperty() const;
        /** @brief Gets authentic car caster submissions from the last selection frame. */
        [[nodiscard]] int getLastSelectionShadowCasterCountProperty() const;
        /** @brief Gets car plus plate receiver submissions from the last selection frame. */
        [[nodiscard]] int getLastSelectionShadowReceiverCountProperty() const;
        /** @brief Pushes one screen on the original last-in-first-out stack. */
        void AddGameScreen(std::unique_ptr<GameScreens::IGameScreen> screen);
        /** @brief Starts the menu post-process and draws its 3D background. */
        void BeginMenuFrame(bool renderOverlay, bool renderWorld = true,
                            bool usePostProcess = true);
        /** @brief Flushes menu UI and executes the authentic menu post-process. */
        void EndMenuFrame();
        /** @brief Flushes car-selection UI, draws its cars, and executes post-process. */
        void EndCarSelectionFrame(float rotation);
        /** @brief Generates the original three-car selection shadow maps. */
        void PrepareCarSelectionShadows(float rotation);
        /** @brief Draws the selected car carousel behind its menu UI. */
        void DrawCarSelectionWorld(float rotation);
        /** @brief Recreates level-owned resources for the selected track. */
        void LoadSelectedLevel();
        /** @brief Updates the active race from the current logical input. */
        void UpdateRace();
        /** @brief Draws the complete active race scene. */
        void DrawRace();
        /** @brief Gets the current screen identity. */
        [[nodiscard]] GameScreens::ScreenKind getCurrentScreenKindProperty() const;
        /** @brief Gets the number of screens in the active stack. */
        [[nodiscard]] int getScreenCountProperty() const;
        /** @brief Gets authentic loading statuses reached by the staged loader. */
        [[nodiscard]] int getLoadingStatusCountProperty() const;
        /** @brief Gets the greatest line count submitted by a UI screen. */
        [[nodiscard]] int getMaximumUiLinePrimitiveCountProperty() const;
        /** @brief Gets whether the latest UI final pass drew the menu cursor. */
        [[nodiscard]] int getLastMouseCursorCountProperty() const;
        /** @brief Gets whether a completed race drew an authentic trophy. */
        [[nodiscard]] int getMaximumTrophyCountProperty() const;
        /** @brief Gets update frames observed for one screen kind. */
        [[nodiscard]] int getScreenVisitCountProperty(
            GameScreens::ScreenKind kind) const;
        /** @brief Gets whether the active player has completed or lost the race. */
        [[nodiscard]] bool getRaceGameOverProperty() const;
        /** @brief Gets whether the race outcome stopped the looping gear cue. */
        [[nodiscard]] bool getGearSoundStoppedProperty() const;
        /** @brief Gets race-outcome sound requests emitted by the player. */
        [[nodiscard]] int getPlayerSoundCountProperty() const;
        /** @brief Gets the last level submitted to the highscore table. */
        [[nodiscard]] int getSubmittedHighscoreLevelProperty() const;
        /** @brief Gets the last time submitted to the highscore table. */
        [[nodiscard]] int getSubmittedHighscoreMillisecondsProperty() const;
        /** @brief Gets the ten lap times for one track. */
        [[nodiscard]] const std::array<int, 10>&
        getHighscoreTimesProperty(int level) const;
        /** @brief Gets one persisted highscore player name. */
        [[nodiscard]] const std::string& getHighscoreNameProperty(
            int level, int rank) const;
        /** @brief Gets the persistent game settings instance. */
        [[nodiscard]] Properties::GameSettings& getSettingsProperty() const;
        /** @brief Gets the active backbuffer width used by the options screen. */
        [[nodiscard]] int getDisplayWidthProperty() const;
        /** @brief Gets the active backbuffer height used by the options screen. */
        [[nodiscard]] int getDisplayHeightProperty() const;
        /** @brief Gets whether the active device is currently fullscreen. */
        [[nodiscard]] bool getFullscreenProperty() const;
        /** @brief Advances one owner-thread content-loading stage. */
        void AdvanceLoading();
        /** @brief Gets the current authentic loading status text. */
        [[nodiscard]] const std::string& getLoadingStatusProperty() const;
        /** @brief Gets whether all deferred game resources are ready. */
        [[nodiscard]] bool getContentLoadedProperty() const;

    protected:
        void Initialize() override;
        void LoadContent() override;
        void UnloadContent() override;
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        enum class LoadingStage
        {
            Unstarted,
            Models,
            Landscape,
            Textures,
            Ready,
            Complete,
        };

        RacingRunConfiguration configuration;
        std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager>
            graphics;
        std::unique_ptr<GameLogic::ControlSource> controlSource;
        std::unique_ptr<Platform::ContentDelivery> contentDelivery;
        std::unique_ptr<GameLogic::ScreenshotCapturer> screenshotCapturer;
        std::unique_ptr<Rendering::StaticTrackScene> trackScene;
        std::unique_ptr<Rendering::CarRenderer> carRenderer;
        std::unique_ptr<Rendering::ShadowMapRenderer> shadowRenderer;
        std::unique_ptr<Graphics::LensFlare> lensFlare;
        std::unique_ptr<Graphics::UIRenderer> uiRenderer;
        std::unique_ptr<Shaders::PostScreenGlow> postScreenGlow;
        std::unique_ptr<Shaders::PostScreenMenu> postScreenMenu;
        std::unique_ptr<Sounds::Sound> sound;
        std::unique_ptr<Properties::GameSettings> settings;
        std::unique_ptr<GameLogic::Player> player;
        std::unique_ptr<GameLogic::Replay> bestReplay;
        std::unique_ptr<GameLogic::Replay> newReplay;
        std::future<void> replaySave;
        std::future<void> settingsSave;
        std::vector<std::unique_ptr<GameScreens::IGameScreen>> gameScreens;
        std::array<int, 9> screenVisitCounts{};
        GameLogic::ControlFrame currentControls;
        GameLogic::ControlFrame pendingControlTransients;
        Microsoft::Xna::Framework::Matrix view =
            Microsoft::Xna::Framework::Matrix::getIdentityProperty();
        Microsoft::Xna::Framework::Vector3 initialCarPosition =
            Microsoft::Xna::Framework::Vector3::Zero;
        Microsoft::Xna::Framework::Vector3 menuCarForward =
            Microsoft::Xna::Framework::Vector3::Zero;
        Microsoft::Xna::Framework::Vector3 menuCarUp =
            Microsoft::Xna::Framework::Vector3::Zero;
        float elapsedMilliseconds = 0.001f;
        float totalMilliseconds = 0.0f;
        float startTimeThisSecond = 0.0f;
        float fpsInterpolated = 100.0f;
        float menuCarTimeSeconds = 0.0f;
        int selectedTrackNumber = 0;
        int currentCarNumber = 0;
        int currentCarColor = 0;
        int menuPreviewCarNumber = 0;
        Microsoft::Xna::Framework::Color menuPreviewCarColor;
        std::array<std::array<int, 10>, 3> highscoreTimes{};
        std::array<std::array<std::string, 10>, 3> highscoreNames{};
        int updateCount = 0;
        int drawCount = 0;
        int frameCountThisSecond = 0;
        int totalFrameCount = 0;
        int fpsLastSecond = 60;
        int deviceResetCount = 0;
#ifdef NDEBUG
        int renderLoopErrorCount = 0;
#endif
        int lastCarPartCount = 0;
        int lastGhostPartCount = 0;
        int lastSelectionPlatePartCount = 0;
        int lastSelectionShadowCasterCount = 0;
        int lastSelectionShadowReceiverCount = 0;
        int brakeTrackCount = 0;
        int brakeSoundCount = 0;
        int checkpointSoundCount = 0;
        bool disableLensFlareInTunnel = false;
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
        bool exitSoundPlayed = false;
        LoadingStage loadingStage = LoadingStage::Unstarted;
        std::string loadingStatus;
        float loadingReadyMilliseconds = 0.0f;
        int loadingStatusMask = 0;
        int maximumUiLinePrimitiveCount = 0;
        int maximumTrophyCount = 0;

        static const std::array<Microsoft::Xna::Framework::Color, 11>
            CarColors;

        friend class Sounds::Sound;

        void WriteCapture();
        void InitializeHighscores();
        void WriteHighscoresToSettings();
        void ApplyDisplaySettings();
        void HandleDeviceReset();
        void LoadCarResources();
        void LoadLandscapeResources();
        void LoadTextureResources();
        void InitializePlayerForCurrentTrack();

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
