// SPDX-License-Identifier: MS-PL

#include <cstdio>
#include <exception>
#include <filesystem>
#include <memory>
#include <string>

#include "GameLogic/Input.hpp"
#include "GameLogic/MobileControls.hpp"
#include "Helpers/RandomHelper.hpp"
#include "RacingGameManager.hpp"

namespace
{
    using Microsoft::Xna::Framework::Rectangle;
    using RacingGame::GameLogic::ControlFrame;
    using RacingGame::GameLogic::MobileControlLayout;
    using RacingGame::GameLogic::MobileControlPreferences;

    class MobileDrivingSource final
        : public RacingGame::GameLogic::ControlSource
    {
    public:
        ControlFrame Capture(bool, bool, const int width,
                             const int height) override
        {
            ControlFrame result;
            const Rectangle safeArea(32, 20, width - 64, height - 40);
            result.mobile.layout = MobileControlLayout::Create(
                width, height, safeArea, preferences);
            result.mobile.overlayVisible = true;
            result.mobile.opacity = preferences.opacity;
            result.mobile.steering = 0.35f;
            result.mobile.throttle = 1.0f;
            result.mobile.handbrakePressed = frame >= 210 && frame < 225;
            result.mobile.hasSteeringPosition = true;
            result.mobile.steeringPosition =
                Microsoft::Xna::Framework::Vector2(
                    static_cast<float>(
                        result.mobile.layout.steering.X +
                        result.mobile.layout.steering.Width * 3 / 4),
                    static_cast<float>(
                        result.mobile.layout.steering.Y +
                        result.mobile.layout.steering.Height / 2));
            result.car.mobileSteering = result.mobile.steering;
            result.car.mobileThrottle = result.mobile.throttle;
            result.car.mobileHandbrakePressed =
                result.mobile.handbrakePressed;
            ++frame;
            return result;
        }

    private:
        MobileControlPreferences preferences;
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
                     "usage: RacingGameMobileOverlayProbe CONTENT_ROOT CAPTURE.ppm\n");
        return 2;
    }

    try
    {
        RacingGame::Helpers::RandomHelper::globalRandomGenerator =
            System::Random(152);
        RacingGame::RacingRunConfiguration configuration;
        configuration.contentRoot = argv[1];
        configuration.frameLimit = 240;
        configuration.elapsedMillisecondsOverride = 1000.0f / 60.0f;
        configuration.capturePath = argv[2];
        configuration.storageAppName = "RacingGameMobileOverlayProbeV1";
        configuration.skipScreens = true;
        configuration.honorDisplaySettings = false;
        RacingGame::RacingGameManager game(
            std::make_unique<MobileDrivingSource>(),
            std::move(configuration));
        game.Run();

        bool passed = true;
        passed = Check(game.getUpdateCountProperty() == 240,
                       "240 mobile-control updates completed") && passed;
        passed = Check(game.getDistanceFromStartProperty() > 0.1f,
                       "analog mobile throttle drove the authentic car physics") && passed;
        passed = Check(game.getLastMobileControlSpriteCountProperty() >= 31,
                       "touch overlay submitted every control, border and steering marker") && passed;
        const auto& controls = game.getControlsProperty().mobile;
        passed = Check(
            controls.layout.safeArea == Rectangle(32, 20, 1216, 680),
            "touch overlay retained its inset safe area") && passed;
        const std::filesystem::path capture(argv[2]);
        passed = Check(
            std::filesystem::exists(capture) &&
                std::filesystem::file_size(capture) > 1280U * 720U * 3U,
            "mobile-overlay GPU capture is complete") && passed;
        game.Dispose();
        std::printf("=== Racing Mobile Overlay: %s ===\n",
                    passed ? "PASS" : "FAIL");
        return passed ? 0 : 1;
    }
    catch (const std::exception& exception)
    {
        std::fprintf(stderr, "[FAIL] mobile overlay: %s\n", exception.what());
        return 1;
    }
}
