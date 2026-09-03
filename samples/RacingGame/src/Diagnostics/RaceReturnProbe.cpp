// SPDX-License-Identifier: MS-PL

#include <cstdio>
#include <exception>
#include <filesystem>
#include <memory>

#include "GameLogic/Input.hpp"
#include "GameScreens/IGameScreen.hpp"
#include "Helpers/RandomHelper.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageDevice.hpp"
#include "Properties/GameSettings.hpp"
#include "RacingGameManager.hpp"

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
            if (!enteredRace)
            {
                if (frame == 5 || frame == 6 || frame == 8 || frame == 10)
                    result.acceptJustPressed = true;
                if (inGame)
                {
                    enteredRace = true;
                    raceFrame = 0;
                }
            }

            if (inGame)
            {
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
                ++raceFrame;
            }
            else if (enteredRace && returnCancels < 2)
            {
                result.cancelJustPressed = true;
                ++returnCancels;
            }
            else if (enteredRace && frame % 300 == 0)
            {
                result.leftJustPressed = true;
            }
            ++frame;
            return result;
        }

    private:
        int frame = 0;
        int raceFrame = 0;
        int returnCancels = 0;
        bool enteredRace = false;
        RacingGame::RacingGameManager* game = nullptr;
    };

    bool Check(const bool condition, const char* label)
    {
        std::printf("[%s] %s\n", condition ? "PASS" : "FAIL", label);
        return condition;
    }
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::fprintf(stderr,
            "usage: RacingGameRaceReturnProbe CONTENT_ROOT CAPTURE.ppm\n");
        return 2;
    }
    try
    {
        constexpr const char* StorageApp = "RacingGameRaceReturnProbeV1";
        using Microsoft::Xna::Framework::Storage::StorageDevice;
        StorageDevice::SetAppNameEXT(StorageApp);
        auto selection = StorageDevice::BeginShowSelector(nullptr, nullptr);
        auto device = StorageDevice::EndShowSelector(selection.get());
        device->DeleteContainer("RacingGame");

        RacingGame::Properties::GameSettings settings;
        settings.setPostScreenEffectsProperty(false);
        settings.setShadowMappingProperty(false);
        settings.setHighDetailProperty(false);
        settings.Save();

        RacingGame::Helpers::RandomHelper::globalRandomGenerator =
            System::Random(8152);
        RacingGame::RacingRunConfiguration configuration;
        configuration.contentRoot = argv[1];
        configuration.frameLimit = 7200;
        configuration.elapsedMillisecondsOverride = 100.0f;
        configuration.capturePath = argv[2];
        configuration.storageAppName = StorageApp;
        configuration.honorDisplaySettings = false;
        configuration.loadingReadyDelayMilliseconds = 0.0f;
        auto controls = std::make_unique<RaceReturnControls>();
        RaceReturnControls* controlsPointer = controls.get();
        RacingGame::RacingGameManager game(
            std::move(controls), std::move(configuration));
        controlsPointer->Attach(game);
        game.Run();

        std::printf("[INFO] segment=%d lap=%d speed=%.6f position=(%.3f,%.3f,%.3f)\n",
                    game.getCarTrackSegmentProperty(),
                    game.getCurrentLapProperty(), game.getCarSpeedProperty(),
                    game.getCarPositionProperty().X,
                    game.getCarPositionProperty().Y,
                    game.getCarPositionProperty().Z);

        using RacingGame::GameScreens::ScreenKind;
        bool passed = true;
        passed = Check(game.getScreenVisitCountProperty(ScreenKind::Game) > 80,
                       "the real screen stack entered and drove a race") && passed;
        passed = Check(game.getRaceGameOverProperty(),
                       "real car physics reached the original Game Over state") && passed;
        passed = Check(game.getSubmittedHighscoreLevelProperty() == 1 &&
                           game.getSubmittedHighscoreMillisecondsProperty() > 0,
                       "the completed race submitted its real advanced-track time") && passed;
        passed = Check(game.getPlayerSoundCountProperty() >= 1,
                       "the player emitted an authentic race-outcome cue") && passed;
        passed = Check(game.getGearSoundStoppedProperty(),
                       "accepting Game Over stopped the looping gear cue") && passed;
        passed = Check(game.getCurrentScreenKindProperty() == ScreenKind::MainMenu &&
                           game.getScreenCountProperty() == 1,
                       "race return unwound Track and Car back to the main menu") && passed;
        if (game.getMaximumTrophyCountProperty() > 0)
            passed = Check(game.getMaximumTrophyCountProperty() == 1,
                           "victory drew one authentic rank trophy") && passed;
        const std::filesystem::path capture(argv[2]);
        passed = Check(std::filesystem::exists(capture) &&
                           std::filesystem::file_size(capture) >
                               1280U * 720U * 3U,
                       "the returned main menu produced a complete capture") && passed;
        game.Dispose();
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
