// SPDX-License-Identifier: MS-PL

#include <cstdio>
#include <exception>
#include <filesystem>
#include <memory>
#include <string>

#include "GameLogic/Input.hpp"
#include "Helpers/RandomHelper.hpp"
#include "RacingGameManager.hpp"

namespace
{
    class AcceleratingControlSource final
        : public RacingGame::GameLogic::ControlSource
    {
    public:
        RacingGame::GameLogic::ControlFrame Capture(
            bool, bool, int, int) override
        {
            RacingGame::GameLogic::ControlFrame result;
            if (frame < 360 || frame >= 390)
                result.car.keyboardUpPressed = true;
            else
                result.car.keySpace = true;
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
                     "usage: RacingGameDrivableSceneProbe CONTENT_ROOT CAPTURE.ppm\n");
        return 2;
    }

    try
    {
        RacingGame::Helpers::RandomHelper::globalRandomGenerator =
            System::Random(152);
        RacingGame::RacingRunConfiguration configuration;
        configuration.contentRoot = argv[1];
        configuration.frameLimit = 420;
        configuration.elapsedMillisecondsOverride = 1000.0f / 60.0f;
        configuration.capturePath = argv[2];
        configuration.storageAppName = "RacingGameDrivableSceneProbeV1";
        RacingGame::RacingGameManager game(
            std::make_unique<AcceleratingControlSource>(),
            std::move(configuration));
        game.Run();

        bool passed = true;
        passed = Check(game.getUpdateCountProperty() == 420,
                       "420 externally driven game updates completed") && passed;
        passed = Check(game.getDrawCountProperty() >= 400,
                       "the drivable scene completed repeated draw/present cycles") && passed;
        passed = Check(game.getDistanceFromStartProperty() > 1.0f,
                       "desktop acceleration moved the car after the start countdown") && passed;
        passed = Check(game.getLastCarPartCountProperty() == 12,
                       "all 12 authentic car mesh parts were submitted") && passed;
        passed = Check(game.getLastGhostPartCountProperty() == 12,
                       "all 12 authentic replay-ghost parts were submitted") && passed;
        passed = Check(game.getLandscapeModelCountProperty() == 53,
                       "all 53 original landscape model XNBs were loaded") && passed;
        passed = Check(game.getLandscapeObjectCountProperty() == 1252,
                       "seed 152 reproduces the complete generated world") && passed;
        passed = Check(game.getLastLandscapeModelPartCountProperty() > 0,
                       "visible authentic landscape model parts were submitted") && passed;
        passed = Check(game.getLastCityPlaneSubmissionCountProperty() == 1,
                       "the original textured city-ground plane was submitted") && passed;
        passed = Check(game.getBrakeTrackVertexCountProperty() >= 6,
                       "major braking generated original tire-mark geometry") && passed;
        passed = Check(game.getLastBrakeTrackPrimitiveCountProperty() >= 2,
                       "the authentic tire-mark material rendered the geometry") && passed;
        passed = Check(game.getLastShadowCasterSubmissionCountProperty() > 0,
                       "authentic track, objects and car cast into the shadow map") && passed;
        passed = Check(game.getLastShadowReceiverSubmissionCountProperty() > 0,
                       "authentic terrain, track, objects and car received shadows") && passed;
        const int shadowMapPixels =
            game.getShadowMapNonWhitePixelCountProperty();
        const int shadowReceiverPixels =
            game.getShadowReceiverNonWhitePixelCountProperty();
        passed = Check(shadowMapPixels > 0,
                       "authentic caster geometry changed the 16-bit shadow map") && passed;
        passed = Check(shadowReceiverPixels > 0,
                       "authentic receiver shader changed the 16-bit scene map") && passed;
        passed = Check(game.getBestReplayMatrixCountProperty() == 385,
                       "the beginner track generated its complete best replay") && passed;
        passed = Check(game.getNewReplayMatrixCountProperty() >= 9,
                       "the current lap recorded matrices at 0.2-second intervals") && passed;
        passed = Check(
            game.getGhostCarMatrixProperty().getTranslationProperty() !=
                Microsoft::Xna::Framework::Vector3::Zero,
            "best-replay interpolation produced a live ghost transform") && passed;
        const std::filesystem::path capture(argv[2]);
        passed = Check(
            std::filesystem::exists(capture) &&
                std::filesystem::file_size(capture) > 1280U * 720U * 3U,
            "final GPU backbuffer capture is complete") && passed;

        std::printf(
            "[INFO] updates=%d draws=%d distance=%.6f carParts=%d ghostParts=%d "
            "landscapeModels=%d landscapeObjects=%d landscapeParts=%d "
            "cityPlanes=%d brakeVertices=%d brakePrimitives=%d "
            "shadowCasters=%d shadowReceivers=%d shadowPixels=%d receiverPixels=%d "
            "bestReplay=%d newReplay=%d\n",
            game.getUpdateCountProperty(), game.getDrawCountProperty(),
            game.getDistanceFromStartProperty(),
            game.getLastCarPartCountProperty(),
            game.getLastGhostPartCountProperty(),
            game.getLandscapeModelCountProperty(),
            game.getLandscapeObjectCountProperty(),
            game.getLastLandscapeModelPartCountProperty(),
            game.getLastCityPlaneSubmissionCountProperty(),
            game.getBrakeTrackVertexCountProperty(),
            game.getLastBrakeTrackPrimitiveCountProperty(),
            game.getLastShadowCasterSubmissionCountProperty(),
            game.getLastShadowReceiverSubmissionCountProperty(),
            shadowMapPixels, shadowReceiverPixels,
            game.getBestReplayMatrixCountProperty(),
            game.getNewReplayMatrixCountProperty());
        game.Dispose();
        std::printf("=== Racing Drivable Scene: %s ===\n",
                    passed ? "PASS" : "FAIL");
        return passed ? 0 : 1;
    }
    catch (const std::exception& exception)
    {
        std::fprintf(stderr, "[FAIL] drivable scene: %s\n", exception.what());
        return 1;
    }
}
