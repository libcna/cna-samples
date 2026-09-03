// SPDX-License-Identifier: MS-PL

#include <cstdio>
#include <cmath>
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
    class MenuControls final : public RacingGame::GameLogic::ControlSource
    {
    public:
        RacingGame::GameLogic::ControlFrame Capture(
            bool, bool, int, int) override
        {
            RacingGame::GameLogic::ControlFrame result;
            switch (frame - 4)
            {
            case 1: result.acceptJustPressed = true; break;
            case 2: result.rightJustPressed = true; break;
            case 3: result.acceptJustPressed = true; break;
            case 4: result.rightJustPressed = true; break;
            case 5: result.cancelJustPressed = true; break;
            case 6: result.rightJustPressed = true; break;
            case 7: result.acceptJustPressed = true; break;
            case 8:
                result.typedText = " CNA";
                result.rightJustPressed = true;
                break;
            case 9: result.downJustPressed = true; break;
            case 10: result.leftJustPressed = true; break;
            case 11: result.downJustPressed = true; break;
            case 12: result.rightJustPressed = true; break;
            case 13: result.cancelJustPressed = true; break;
            case 14: result.rightJustPressed = true; break;
            case 15: result.acceptJustPressed = true; break;
            case 16: result.cancelJustPressed = true; break;
            default: break;
            }
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
            "usage: RacingGameMenuScreensProbe CONTENT_ROOT CAPTURE.ppm\n");
        return 2;
    }
    try
    {
        constexpr const char* StorageApp = "RacingGameMenuScreensProbeV1";
        using Microsoft::Xna::Framework::Storage::StorageDevice;
        StorageDevice::SetAppNameEXT(StorageApp);
        auto selection = StorageDevice::BeginShowSelector(nullptr, nullptr);
        auto device = StorageDevice::EndShowSelector(selection.get());
        device->DeleteContainer("RacingGame");

        RacingGame::Helpers::RandomHelper::globalRandomGenerator =
            System::Random(8152);
        RacingGame::RacingRunConfiguration configuration;
        configuration.contentRoot = argv[1];
        configuration.frameLimit = 23;
        configuration.elapsedMillisecondsOverride = 1000.0f / 60.0f;
        configuration.capturePath = argv[2];
        configuration.storageAppName = StorageApp;
        configuration.honorDisplaySettings = false;
        configuration.loadingReadyDelayMilliseconds = 0.0f;
        RacingGame::RacingGameManager game(
            std::make_unique<MenuControls>(), std::move(configuration));
        game.Run();

        using RacingGame::GameScreens::ScreenKind;
        bool passed = true;
        passed = Check(game.getCurrentScreenKindProperty() ==
                           ScreenKind::MainMenu &&
                           game.getScreenCountProperty() == 1,
                       "highscores, options and help returned to main menu") && passed;
        passed = Check(
            game.getScreenVisitCountProperty(ScreenKind::Highscores) >= 2,
            "the authentic highscore screen was active") && passed;
        passed = Check(game.getMaximumUiLinePrimitiveCountProperty() == 2,
                       "highscores drew both authentic LineRendering separators") && passed;
        passed = Check(
            game.getScreenVisitCountProperty(ScreenKind::Options) >= 6,
            "the authentic options screen processed every setting group") && passed;
        passed = Check(
            game.getScreenVisitCountProperty(ScreenKind::Help) >= 1,
            "the authentic desktop help screen was active") && passed;
        passed = Check(game.getLastMouseCursorCountProperty() == 1,
                       "the authentic mouse cursor was drawn after menu effects") && passed;
        auto& settings = game.getSettingsProperty();
        passed = Check(settings.getPlayerNameProperty() == "Player CNA",
                       "options persisted original keyboard name editing") && passed;
        passed = Check(std::abs(settings.getSoundVolumeProperty() - 0.9f) <
                               0.0001f &&
                           std::abs(settings.getMusicVolumeProperty() - 0.5f) <
                               0.0001f &&
                           std::abs(settings.getControllerSensitivityProperty() -
                                    0.6f) < 0.0001f,
                       "options persisted sound, music and sensitivity controls") && passed;
        RacingGame::Properties::GameSettings reloaded;
        reloaded.Load();
        passed = Check(reloaded.getPlayerNameProperty() == "Player CNA" &&
                           reloaded.getMusicVolumeProperty() == 0.5f,
                       "options changes survived a fresh XML/storage load") && passed;
        const std::filesystem::path capture(argv[2]);
        passed = Check(std::filesystem::exists(capture) &&
                           std::filesystem::file_size(capture) >
                               1280U * 720U * 3U,
                       "the complete menu tour produced a GL capture") && passed;
        game.Dispose();
        std::printf("=== Racing Menu Screens: %s ===\n",
                    passed ? "PASS" : "FAIL");
        return passed ? 0 : 1;
    }
    catch (const std::exception& exception)
    {
        std::fprintf(stderr, "[FAIL] menu screens: %s\n", exception.what());
        return 1;
    }
}
