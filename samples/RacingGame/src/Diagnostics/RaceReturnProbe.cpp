// SPDX-License-Identifier: MS-PL

#include <cstdio>
#include <exception>
#include <filesystem>
#include <memory>
#include <string>

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

#include "GameLogic/Input.hpp"
#include "GameScreens/IGameScreen.hpp"
#include "Helpers/RandomHelper.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageDevice.hpp"
#include "Platform/PersistentStorage.hpp"
#include "Properties/GameSettings.hpp"
#include "RacingGameManager.hpp"

#if defined(__EMSCRIPTEN__)
EM_JS(int, IsRacingPersistenceReload, (), {
    return new URLSearchParams(globalThis.location?.search ?? "")
        .get("reload") === "1" ? 1 : 0;
});
#endif

namespace
{
    class RaceReturnControls final
        : public RacingGame::GameLogic::ControlSource
    {
    public:
        void Attach(RacingGame::RacingGameManager& value)
        {
            game = &value;
        }

        RacingGame::GameLogic::ControlFrame Capture(
            const bool inGame, bool, int, int) override
        {
            RacingGame::GameLogic::ControlFrame result;
            if (!enteredRace && game && game->getContentLoadedProperty())
            {
                const auto screen = game->getCurrentScreenKindProperty();
                if (screen != lastAcceptedScreen &&
                    screen != RacingGame::GameScreens::ScreenKind::Loading)
                {
                    result.acceptJustPressed = true;
                    lastAcceptedScreen = screen;
                }
            }

            if (inGame)
            {
                if (!enteredRace && game)
                    enteredReplayMatrixCount =
                        game->getBestReplayMatrixCountProperty();
                enteredRace = true;
                result.car.keyboardUpPressed = true;
                if (game)
                {
                    const auto track = game->getCurrentTrackMatrixProperty();
                    const auto carPosition = game->getCarPositionProperty();
                    const auto target =
                        Microsoft::Xna::Framework::Vector3::Normalize(
                            track.getForwardProperty() +
                            (track.getTranslationProperty() - carPosition) *
                                0.08f);
                    const float turn =
                        Microsoft::Xna::Framework::Vector3::Dot(
                            Microsoft::Xna::Framework::Vector3::Cross(
                                game->getCarDirectionProperty(), target),
                            track.getUpProperty());
                    result.car.keyboardLeftPressed = turn > 0.01f;
                    result.car.keyboardRightPressed = turn < -0.01f;
                    result.acceptJustPressed =
                        game->getRaceGameOverProperty();
                }
            }
            else if (enteredRace && game)
            {
                const auto screen = game->getCurrentScreenKindProperty();
                if ((screen == RacingGame::GameScreens::ScreenKind::TrackSelection ||
                     screen == RacingGame::GameScreens::ScreenKind::CarSelection) &&
                    screen != lastCanceledScreen)
                {
                    result.cancelJustPressed = true;
                    lastCanceledScreen = screen;
                }
            }
            return result;
        }

        [[nodiscard]] int getEnteredReplayMatrixCountProperty() const
        {
            return enteredReplayMatrixCount;
        }

    private:
        int enteredReplayMatrixCount = 0;
        bool enteredRace = false;
        RacingGame::GameScreens::ScreenKind lastAcceptedScreen =
            RacingGame::GameScreens::ScreenKind::Loading;
        RacingGame::GameScreens::ScreenKind lastCanceledScreen =
            RacingGame::GameScreens::ScreenKind::Game;
        RacingGame::RacingGameManager* game = nullptr;
    };

    bool Check(const bool condition, const char* label)
    {
        std::printf("[%s] %s\n", condition ? "PASS" : "FAIL", label);
        return condition;
    }

    std::string SlowHighscores()
    {
        std::string result;
        for (int level = 0; level < 3; ++level)
        {
            for (int rank = 0; rank < 10; ++rank)
            {
                if (!result.empty()) result += ',';
                result += "Web Seed " + std::to_string(rank + 1) + ':' +
                    std::to_string(900000 + level * 100000 + rank * 10000);
            }
        }
        return result;
    }
}

int main(int argc, char** argv)
{
#if defined(__EMSCRIPTEN__)
    const bool browserDefaults = argc == 1;
#else
    constexpr bool browserDefaults = false;
#endif
    if (argc != 3 && !browserDefaults)
    {
        std::fprintf(stderr,
            "usage: RacingGameRaceReturnProbe CONTENT_ROOT CAPTURE.ppm\n");
        return 2;
    }
    try
    {
        RacingGame::Platform::PersistentStorage::Prepare();
        constexpr const char* StorageApp = "RacingGameRaceReturnProbeV1";
#if defined(__EMSCRIPTEN__)
        const bool persistenceReload = IsRacingPersistenceReload() != 0;
#else
        constexpr bool persistenceReload = false;
#endif
        const std::string contentRoot = browserDefaults ? "/Content" : argv[1];
        const std::string capturePath = browserDefaults
            ? "/tmp/racing-race-return.ppm" : argv[2];
        using Microsoft::Xna::Framework::Storage::StorageDevice;
        using Microsoft::Xna::Framework::PlayerIndex;
        StorageDevice::SetAppNameEXT(StorageApp);
        auto selection = StorageDevice::BeginShowSelector(nullptr, nullptr);
        auto device = StorageDevice::EndShowSelector(selection.get());
        if (!persistenceReload)
        {
            device->DeleteContainer("RacingGame");
            RacingGame::Properties::GameSettings settings;
            settings.setHighscoresProperty(SlowHighscores());
            settings.setResolutionWidthProperty(800);
            settings.setResolutionHeightProperty(480);
            settings.setFullscreenProperty(false);
            settings.setPostScreenEffectsProperty(false);
            settings.setShadowMappingProperty(false);
            settings.setHighDetailProperty(false);
            settings.Save();
        }
        auto replaySelection = StorageDevice::BeginShowSelector(
            PlayerIndex::One, nullptr, nullptr);
        auto replayDevice = StorageDevice::EndShowSelector(
            replaySelection.get());
        auto initialOpen = replayDevice->BeginOpenContainer(
            "RacingGame", nullptr, nullptr);
        auto initialContainer = replayDevice->EndOpenContainer(initialOpen.get());
        const bool hadPersistedReplay = initialContainer->FileExists(
            "TrackAdvanced.Replay");
        initialContainer->Dispose();

        RacingGame::Helpers::RandomHelper::globalRandomGenerator =
            System::Random(8152);
        RacingGame::RacingRunConfiguration configuration;
        configuration.contentRoot = contentRoot;
        configuration.frameLimit = persistenceReload ? 600 : 12000;
        configuration.elapsedMillisecondsOverride = 100.0f;
        configuration.capturePath = capturePath;
        configuration.storageAppName = StorageApp;
        configuration.honorDisplaySettings = true;
        configuration.loadingReadyDelayMilliseconds = 0.0f;
        configuration.exitAfterCompletedRaceReturn = !persistenceReload;
        configuration.exitAfterGameFrames = persistenceReload ? 60 : 0;
        auto controls = std::make_unique<RaceReturnControls>();
        RaceReturnControls* controlsPointer = controls.get();
        RacingGame::RacingGameManager game(
            std::move(controls), std::move(configuration));
        controlsPointer->Attach(game);
        const int initialAdvancedTop =
            game.getHighscoreTimesProperty(1)[0];
        game.Run();

        std::printf("[INFO] segment=%d lap=%d speed=%.6f position=(%.3f,%.3f,%.3f)\n",
                    game.getCarTrackSegmentProperty(),
                    game.getCurrentLapProperty(), game.getCarSpeedProperty(),
                    game.getCarPositionProperty().X,
                    game.getCarPositionProperty().Y,
                    game.getCarPositionProperty().Z);

        using RacingGame::GameScreens::ScreenKind;
        const int submittedMilliseconds =
            game.getSubmittedHighscoreMillisecondsProperty();
        const int enteredReplayMatrices =
            controlsPointer->getEnteredReplayMatrixCountProperty();
        bool passed = true;
        passed = Check(game.getScreenVisitCountProperty(ScreenKind::Game) >
                           (persistenceReload ? 0 : 80),
                       "the real screen stack entered and drove a race") && passed;
        if (!persistenceReload)
        {
            passed = Check(game.getRaceGameOverProperty(),
                           "real car physics reached the original Game Over state") && passed;
            passed = Check(game.getSubmittedHighscoreLevelProperty() == 1 &&
                               game.getSubmittedHighscoreMillisecondsProperty() > 0,
                           "the completed race submitted its real advanced-track time") && passed;
            passed = Check(game.getHighscoreTimesProperty(1)[0] <=
                               game.getSubmittedHighscoreMillisecondsProperty(),
                           "the completed race entered the persisted highscore table") && passed;
            passed = Check(game.getPlayerSoundCountProperty() >= 1,
                           "the player emitted an authentic race-outcome cue") && passed;
            passed = Check(game.getGearSoundStoppedProperty(),
                           "accepting Game Over stopped the looping gear cue") && passed;
            passed = Check(
                game.getCurrentScreenKindProperty() == ScreenKind::MainMenu &&
                    game.getScreenCountProperty() <= 1,
                "race return unwound Track and Car back to the main menu") && passed;
            if (game.getMaximumTrophyCountProperty() > 0)
                passed = Check(game.getMaximumTrophyCountProperty() == 1,
                               "victory drew one authentic rank trophy") && passed;
        }
        else
        {
            passed = Check(hadPersistedReplay && enteredReplayMatrices > 0,
                           "the restarted race loaded the prior replay") && passed;
            passed = Check(initialAdvancedTop < 1000000,
                           "the restarted race loaded the prior highscore") && passed;
        }
        const std::filesystem::path capture(capturePath);
        const std::uintmax_t minimumCaptureBytes =
            static_cast<std::uintmax_t>(game.getDisplayWidthProperty()) *
            static_cast<std::uintmax_t>(game.getDisplayHeightProperty()) * 3U;
        passed = Check(std::filesystem::exists(capture) &&
                           std::filesystem::file_size(capture) >
                               minimumCaptureBytes,
                       persistenceReload
                           ? "the restarted race produced a complete capture"
                           : "the returned main menu produced a complete capture") && passed;
        game.Dispose();
        auto savedOpen = device->BeginOpenContainer(
            "RacingGame", nullptr, nullptr);
        auto savedContainer = device->EndOpenContainer(savedOpen.get());
        passed = Check(savedContainer->FileExists(
                           "RacingGameSettings.xml"),
                       persistenceReload
                           ? "the restarted process retained settings/highscores"
                           : "the completed race persisted its settings/highscores") && passed;
        auto savedReplayOpen = replayDevice->BeginOpenContainer(
            "RacingGame", nullptr, nullptr);
        auto savedReplayContainer = replayDevice->EndOpenContainer(
            savedReplayOpen.get());
        passed = Check(savedReplayContainer->FileExists("TrackAdvanced.Replay"),
                       persistenceReload
                           ? "the restarted process retained the advanced replay"
                           : "the completed race persisted its advanced replay") && passed;
        savedReplayContainer->Dispose();
        savedContainer->Dispose();
        std::printf(
            "[INFO] persistenceMode=%s initialAdvancedTop=%d "
            "enteredReplayMatrices=%d submitted=%d\n",
            persistenceReload ? "reload" : "write", initialAdvancedTop,
            enteredReplayMatrices, submittedMilliseconds);
        std::printf("=== Racing Race Return: %s ===\n",
                    passed ? "PASS" : "FAIL");
        return passed ? 0 : 1;
    }
    catch (const std::exception& exception)
    {
        std::fprintf(stderr, "[FAIL] race return: %s\n", exception.what());
        return 1;
    }
}
