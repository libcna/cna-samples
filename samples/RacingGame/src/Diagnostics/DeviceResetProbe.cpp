// SPDX-License-Identifier: MS-PL

#include <algorithm>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include "GameLogic/Input.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageDevice.hpp"
#include "Properties/GameSettings.hpp"
#include "RacingGameManager.hpp"

namespace
{
    class ResetControls final : public RacingGame::GameLogic::ControlSource
    {
    public:
        RacingGame::GameLogic::ControlFrame Capture(
            bool, bool, const int width, const int height) override
        {
            RacingGame::GameLogic::ControlFrame result;
            switch (frame - 4)
            {
            case 1: result.acceptJustPressed = true; break;
            case 2: result.rightJustPressed = true; break;
            case 3: result.rightJustPressed = true; break;
            case 4: result.acceptJustPressed = true; break;
            case 5:
                SetOptionClick(result, width, height, 388.0f, 253.0f);
                break;
            case 6: result.cancelJustPressed = true; break;
            default: break;
            }
            ++frame;
            return result;
        }

    private:
        int frame = 0;

        static void SetOptionClick(
            RacingGame::GameLogic::ControlFrame& result,
            const int width, const int height,
            const float sourceX, const float sourceY)
        {
            const float scale = std::min(
                width / 1024.0f, height / 768.0f);
            const float horizontalOffset =
                (width - 1024.0f * scale) / 2.0f;
            result.mousePosition = Microsoft::Xna::Framework::Point(
                static_cast<int>(horizontalOffset + sourceX * scale),
                static_cast<int>(sourceY * scale));
            result.mouseLeftJustPressed = true;
        }
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
            "usage: RacingGameDeviceResetProbe CONTENT_ROOT CAPTURE.ppm\n");
        return 2;
    }
    try
    {
        constexpr const char* StorageApp =
            "RacingGameDeviceResetQualificationV1";
        using Microsoft::Xna::Framework::Storage::StorageDevice;
        StorageDevice::SetAppNameEXT(StorageApp);
        auto selection = StorageDevice::BeginShowSelector(nullptr, nullptr);
        auto device = StorageDevice::EndShowSelector(selection.get());
        device->DeleteContainer("RacingGame");
        RacingGame::Properties::GameSettings initialSettings;
        initialSettings.setResolutionWidthProperty(1280);
        initialSettings.setResolutionHeightProperty(720);
        initialSettings.setFullscreenProperty(false);
        initialSettings.Save();

        RacingGame::RacingRunConfiguration configuration;
        configuration.contentRoot = argv[1];
        configuration.frameLimit = 18;
        configuration.elapsedMillisecondsOverride = 1000.0f / 60.0f;
        configuration.capturePath = argv[2];
        configuration.storageAppName = StorageApp;
        configuration.honorDisplaySettings = true;
        configuration.loadingReadyDelayMilliseconds = 0.0f;
        RacingGame::RacingGameManager game(
            std::make_unique<ResetControls>(), std::move(configuration));
        game.Run();

        bool passed = true;
        passed = Check(game.getDeviceResetCountProperty() == 1,
                       "options resolution change raised one real device reset") && passed;
        passed = Check(game.getDisplayWidthProperty() == 640 &&
                           game.getDisplayHeightProperty() == 480,
                       "device reset applied the original 640x480 option") && passed;
        passed = Check(game.getLastMenuPostScreenPassCountProperty() == 4,
                       "recreated menu render targets completed all four passes") && passed;
        const std::filesystem::path capture(argv[2]);
        passed = Check(std::filesystem::exists(capture) &&
                           std::filesystem::file_size(capture) >
                               640U * 480U * 3U,
                       "post-reset backbuffer produced a complete capture") && passed;
        if (std::filesystem::exists(capture))
        {
            std::ifstream input(capture, std::ios::binary);
            std::string magic;
            int width = 0;
            int height = 0;
            int maximum = 0;
            input >> magic >> width >> height >> maximum;
            passed = Check(magic == "P6" && width == 640 && height == 480 &&
                               maximum == 255,
                           "post-reset capture reports the applied dimensions") && passed;
        }
        game.Dispose();
        std::printf("=== Racing Device Reset: %s ===\n",
                    passed ? "PASS" : "FAIL");
        return passed ? 0 : 1;
    }
    catch (const std::exception& exception)
    {
        std::fprintf(stderr, "[FAIL] device reset: %s\n", exception.what());
        return 1;
    }
}
