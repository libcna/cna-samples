// SPDX-License-Identifier: MS-PL

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <exception>
#include <memory>
#include <string>

#include "GameLogic/Input.hpp"
#include "GameScreens/IGameScreen.hpp"
#include "Helpers/RandomHelper.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "RacingGameManager.hpp"

namespace
{
    class AcceleratingDesktopControls final
        : public RacingGame::GameLogic::ControlSource
    {
    public:
        void Attach(RacingGame::RacingGameManager& value)
        {
            game = &value;
        }

        RacingGame::GameLogic::ControlFrame Capture(
            const bool inGame, const bool appActive,
            const int displayWidth, const int displayHeight) override
        {
            RacingGame::GameLogic::ControlFrame result = input.Capture(
                inGame, appActive, displayWidth, displayHeight);

            if (!inGame && game && game->getContentLoadedProperty())
            {
                const auto screen = game->getCurrentScreenKindProperty();
                if (screen == RacingGame::GameScreens::ScreenKind::TrackSelection &&
                    screen != lastAcceptedScreen)
                {
                    Microsoft::Xna::Framework::Input::Mouse::SetPosition(
                        100, displayHeight / 2);
                    movedPointerBeforeRace = true;
                }
                if (screen != RacingGame::GameScreens::ScreenKind::Loading)
                {
                    if (screen != lastObservedScreen)
                    {
                        lastObservedScreen = screen;
                        stableScreenFrames = 0;
                    }
                    if (++stableScreenFrames >= 30 &&
                        screen != lastAcceptedScreen)
                    {
                        result.acceptJustPressed = true;
                        lastAcceptedScreen = screen;
                    }
                }
            }

            if (!inGame) return result;

            enteredRace = true;
            const auto& car = result.car;
            maxAbsMouseX = std::max(maxAbsMouseX, std::abs(car.mouseXMovement));
            maxAbsGamePadX = std::max(
                maxAbsGamePadX, std::abs(car.gamePadLeftStickX));
            if (car.mouseXMovement < -0.01f) ++negativeMouseFrames;
            if (car.mouseXMovement > 0.01f) ++positiveMouseFrames;
            if (car.keyboardLeftPressed || car.keyA || car.gamePadDPadLeft)
                ++leftFrames;
            if (car.keyboardRightPressed || car.keyD || car.keyE ||
                car.gamePadDPadRight)
                ++rightFrames;

            result.car.keyboardUpPressed = true;
            return result;
        }

        [[nodiscard]] float MaxAbsMouseX() const { return maxAbsMouseX; }
        [[nodiscard]] float MaxAbsGamePadX() const { return maxAbsGamePadX; }
        [[nodiscard]] int NegativeMouseFrames() const
        {
            return negativeMouseFrames;
        }
        [[nodiscard]] int PositiveMouseFrames() const
        {
            return positiveMouseFrames;
        }
        [[nodiscard]] int LeftFrames() const { return leftFrames; }
        [[nodiscard]] int RightFrames() const { return rightFrames; }
        [[nodiscard]] bool EnteredRace() const { return enteredRace; }
        [[nodiscard]] bool MovedPointerBeforeRace() const
        {
            return movedPointerBeforeRace;
        }

    private:
        RacingGame::GameLogic::Input input;
        RacingGame::RacingGameManager* game = nullptr;
        RacingGame::GameScreens::ScreenKind lastObservedScreen =
            RacingGame::GameScreens::ScreenKind::Loading;
        RacingGame::GameScreens::ScreenKind lastAcceptedScreen =
            RacingGame::GameScreens::ScreenKind::Loading;
        float maxAbsMouseX = 0.0f;
        float maxAbsGamePadX = 0.0f;
        int negativeMouseFrames = 0;
        int positiveMouseFrames = 0;
        int leftFrames = 0;
        int rightFrames = 0;
        int stableScreenFrames = 0;
        bool enteredRace = false;
        bool movedPointerBeforeRace = false;
    };

    bool Check(const bool condition, const char* label)
    {
        std::printf("[%s] %s\n", condition ? "PASS" : "FAIL", label);
        return condition;
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::fprintf(stderr,
                     "usage: RacingGameDesktopDrivingInputProbe CONTENT_ROOT\n");
        return 2;
    }

    try
    {
        RacingGame::Helpers::RandomHelper::globalRandomGenerator =
            System::Random(152);
        auto controls = std::make_unique<AcceleratingDesktopControls>();
        AcceleratingDesktopControls* observed = controls.get();

        RacingGame::RacingRunConfiguration configuration;
        configuration.contentRoot = argv[1];
        configuration.frameLimit = 540;
        configuration.elapsedMillisecondsOverride = 1000.0f / 60.0f;
        configuration.storageAppName =
            "RacingGameDesktopDrivingInputProbeV1";
        configuration.honorDisplaySettings = false;
        configuration.loadingReadyDelayMilliseconds = 0.0f;

        RacingGame::RacingGameManager game(
            std::move(controls), std::move(configuration));
        observed->Attach(game);
        game.Run();

        const Microsoft::Xna::Framework::Vector3 direction =
            game.getCarDirectionProperty();
        bool passed = true;
        passed = Check(observed->EnteredRace() &&
                           observed->MovedPointerBeforeRace(),
                       "real screen flow entered the race after off-center input") &&
                 passed;
        passed = Check(game.getDistanceFromStartProperty() > 1.0f,
                       "desktop acceleration moved the car") && passed;
        passed = Check(observed->LeftFrames() == 0 &&
                           observed->RightFrames() == 0,
                       "neutral desktop input reported no digital steering") &&
                 passed;
        passed = Check(observed->MaxAbsGamePadX() < 0.01f,
                       "neutral desktop input reported no gamepad steering") &&
                 passed;
        passed = Check(observed->MaxAbsMouseX() < 0.01f,
                       "repeated in-game cursor centering stayed neutral") &&
                 passed;

        std::printf(
            "[INFO] distance=%.6f direction=(%.6f,%.6f,%.6f) "
            "mouseMax=%.6f mouseNegativeFrames=%d mousePositiveFrames=%d "
            "gamePadMax=%.6f leftFrames=%d rightFrames=%d\n",
            game.getDistanceFromStartProperty(), direction.X, direction.Y,
            direction.Z, observed->MaxAbsMouseX(),
            observed->NegativeMouseFrames(), observed->PositiveMouseFrames(),
            observed->MaxAbsGamePadX(), observed->LeftFrames(),
            observed->RightFrames());
        game.Dispose();
        std::printf("=== Racing Desktop Driving Input: %s ===\n",
                    passed ? "PASS" : "FAIL");
        return passed ? 0 : 1;
    }
    catch (const std::exception& exception)
    {
        std::fprintf(stderr, "[FAIL] desktop driving input: %s\n",
                     exception.what());
        return 1;
    }
}
