// SPDX-License-Identifier: MS-PL

#include <cstdio>
#include <exception>
#include <filesystem>
#include <memory>

#include "GameLogic/Input.hpp"
#include "GameScreens/IGameScreen.hpp"
#include "Helpers/RandomHelper.hpp"
#include "RacingGameManager.hpp"

namespace
{
    class ScreenFlowControls final
        : public RacingGame::GameLogic::ControlSource
    {
    public:
        RacingGame::GameLogic::ControlFrame Capture(
            bool, bool, int, int) override
        {
            RacingGame::GameLogic::ControlFrame result;
            if (frame == 5 || frame == 6 || frame == 8 || frame == 10)
                result.acceptJustPressed = true;
            if (frame == 7)
            {
                result.rightJustPressed = true;
                result.downJustPressed = true;
            }
            if (frame == 9) result.rightJustPressed = true;
            ++frame;
            return result;
        }

    private:
        int frame = 0;
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
            "usage: RacingGameScreenFlowProbe CONTENT_ROOT CAPTURE.ppm\n");
        return 2;
    }
    try
    {
        RacingGame::GameLogic::ControlFrame pending;
        RacingGame::GameLogic::ControlFrame pressed;
        pressed.acceptJustPressed = true;
        pressed.leftJustPressed = true;
        pressed.mouseLeftJustPressed = true;
        pressed.typedText = "C";
        pending.AccumulateTransients(pressed);
        RacingGame::GameLogic::ControlFrame released;
        released.ClearTransients();
        released.AccumulateTransients(pending);
        bool passed = Check(
            released.acceptJustPressed && released.leftJustPressed &&
                released.mouseLeftJustPressed && released.typedText == "C",
            "one-shot controls survive update catch-up until Draw");
        pending.ClearTransients();
        passed = Check(
            !pending.acceptJustPressed && !pending.leftJustPressed &&
                !pending.mouseLeftJustPressed && pending.typedText.empty(),
            "rendered one-shot controls clear before the next frame") && passed;

        RacingGame::Helpers::RandomHelper::globalRandomGenerator =
            System::Random(152);
        RacingGame::RacingRunConfiguration configuration;
        configuration.contentRoot = argv[1];
        configuration.frameLimit = 16;
        configuration.elapsedMillisecondsOverride = 1000.0f / 60.0f;
        configuration.capturePath = argv[2];
        configuration.storageAppName = "RacingGameScreenFlowProbeV1";
        configuration.honorDisplaySettings = false;
        configuration.loadingReadyDelayMilliseconds = 0.0f;
        RacingGame::RacingGameManager game(
            std::make_unique<ScreenFlowControls>(), std::move(configuration));
        game.Run();

        passed = Check(game.getCurrentScreenKindProperty() ==
                           RacingGame::GameScreens::ScreenKind::Game,
                       "loading, splash, menu, car and track screens reached a race") && passed;
        passed = Check(game.getScreenCountProperty() == 4,
                       "the original LIFO screen stack retained parent screens") && passed;
        passed = Check(game.getLoadingStatusCountProperty() == 4,
                       "the loading screen visited all authentic progress statuses") && passed;
        passed = Check(game.getCurrentCarNumberProperty() == 2,
                       "car selection changed the authentic texture variant") && passed;
        passed = Check(game.getCurrentCarColorProperty() == 1,
                       "car selection changed the authentic hue") && passed;
        passed = Check(game.getLastSelectionPlatePartCountProperty() > 0,
                       "authentic CarSelectionPlate XNB geometry was drawn") && passed;
        passed = Check(
            game.getLastSelectionShadowCasterCountProperty() >= 36 &&
                game.getLastSelectionShadowReceiverCountProperty() >
                    game.getLastSelectionShadowCasterCountProperty(),
            "three cars cast and their plates received authentic selection shadows") && passed;
        passed = Check(game.getSelectedTrackNumberProperty() == 2,
                       "track selection chose Expert without a hardcoded level") && passed;
        passed = Check(game.getBestReplayMatrixCountProperty() == 1135,
                       "the selected Expert level loaded its authentic replay") && passed;
        passed = Check(game.getLastCarPartCountProperty() == 12,
                       "the selected car completed a real GL race draw") && passed;
        passed = Check(game.getSoundInitializedProperty(),
                       "the authentic XGS, XWB and XSB products initialized") && passed;
        passed = Check(game.getSoundPlayRequestCountProperty() >= 9,
                       "screen, menu, selection and race cues played through XACT") && passed;
        passed = Check(game.getGearCueStartCountProperty() >= 1,
                       "the authentic Gear1 cue started for the race") && passed;
        const std::filesystem::path capture(argv[2]);
        passed = Check(std::filesystem::exists(capture) &&
                           std::filesystem::file_size(capture) >
                               1280U * 720U * 3U,
                       "the launch-to-race flow produced a complete capture") && passed;
        game.Dispose();
        std::printf("=== Racing Screen Flow: %s ===\n",
                    passed ? "PASS" : "FAIL");
        return passed ? 0 : 1;
    }
    catch (const std::exception& exception)
    {
        std::fprintf(stderr, "[FAIL] screen flow: %s\n", exception.what());
        return 1;
    }
}
