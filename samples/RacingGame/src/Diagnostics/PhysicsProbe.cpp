// SPDX-License-Identifier: MS-PL

#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "GameLogic/CarPhysics.hpp"
#include "GameLogic/Physics/SpringPhysicsObject.hpp"
#include "Helpers/Vector3Helper.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace
{
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::Buttons;
    using Microsoft::Xna::Framework::Input::GamePadState;
    using Microsoft::Xna::Framework::Input::KeyboardState;
    using Microsoft::Xna::Framework::Input::Keys;
    using Microsoft::Xna::Framework::Input::MouseState;
    using RacingGame::GameLogic::BrakeSoundType;
    using RacingGame::GameLogic::CarControlState;
    using RacingGame::GameLogic::CarPhysics;
    using RacingGame::GameLogic::CarPhysicsEnvironment;
    using RacingGame::GameLogic::TimeFadeupMode;
    using RacingGame::GameLogic::Physics::SpringPhysicsObject;
    using RacingGame::Helpers::Vector3Helper;

    constexpr std::uint64_t OffsetBasis = UINT64_C(14695981039346656037);

    std::uint64_t HashByte(std::uint64_t hash, const std::uint8_t value)
    {
        return (hash ^ value) * UINT64_C(1099511628211);
    }

    std::uint64_t HashSingle(std::uint64_t hash, const float value)
    {
        const std::uint32_t bits = std::bit_cast<std::uint32_t>(value);
        hash = HashByte(hash, static_cast<std::uint8_t>(bits));
        hash = HashByte(hash, static_cast<std::uint8_t>(bits >> 8));
        hash = HashByte(hash, static_cast<std::uint8_t>(bits >> 16));
        return HashByte(hash, static_cast<std::uint8_t>(bits >> 24));
    }

    std::uint64_t HashSpringState(
        std::uint64_t hash, const SpringPhysicsObject& spring)
    {
        hash = HashSingle(hash, spring.pos);
        hash = HashSingle(hash, spring.velocity);
        return HashSingle(hash, spring.force);
    }

    std::uint64_t HashInt32(std::uint64_t hash, const int value)
    {
        const std::uint32_t bits = static_cast<std::uint32_t>(value);
        hash = HashByte(hash, static_cast<std::uint8_t>(bits));
        hash = HashByte(hash, static_cast<std::uint8_t>(bits >> 8));
        hash = HashByte(hash, static_cast<std::uint8_t>(bits >> 16));
        return HashByte(hash, static_cast<std::uint8_t>(bits >> 24));
    }

    std::uint64_t HashVector3(std::uint64_t hash, const Vector3 value)
    {
        hash = HashSingle(hash, value.X);
        hash = HashSingle(hash, value.Y);
        return HashSingle(hash, value.Z);
    }

    std::uint64_t HashMatrix(std::uint64_t hash, const Matrix& value)
    {
        hash = HashSingle(hash, value.M11);
        hash = HashSingle(hash, value.M12);
        hash = HashSingle(hash, value.M13);
        hash = HashSingle(hash, value.M14);
        hash = HashSingle(hash, value.M21);
        hash = HashSingle(hash, value.M22);
        hash = HashSingle(hash, value.M23);
        hash = HashSingle(hash, value.M24);
        hash = HashSingle(hash, value.M31);
        hash = HashSingle(hash, value.M32);
        hash = HashSingle(hash, value.M33);
        hash = HashSingle(hash, value.M34);
        hash = HashSingle(hash, value.M41);
        hash = HashSingle(hash, value.M42);
        hash = HashSingle(hash, value.M43);
        return HashSingle(hash, value.M44);
    }

    class FlatCarEnvironment final : public CarPhysicsEnvironment
    {
    public:
        [[nodiscard]] int GetSelectedTrackNumber() const override { return 0; }
        [[nodiscard]] bool IsInMenu() const override { return false; }
        [[nodiscard]] float GetElapsedMilliseconds() const override
        {
            return elapsedMilliseconds;
        }
        void StartLandscapeLap() override { ++startedLaps; }
        void ReplaceStartLightObject(const int state) override
        {
            startLightHash = startLightHash * 31 + state;
        }
        void SubmitHighscore(int, int) override { ++highscoreSubmissions; }
        [[nodiscard]] float GetMoveFactorPerSecond() const override
        {
            return moveFactor;
        }
        [[nodiscard]] float GetTotalTimeMilliseconds() const override
        {
            return totalMilliseconds;
        }
        [[nodiscard]] float GetControllerSensitivity() const override
        {
            return 0.5f;
        }
        [[nodiscard]] bool IsFreeCamera() const override { return false; }
        [[nodiscard]] bool IsInGame() const override { return true; }

        void UpdateCarTrackPosition(
            const Vector3 carPosition, int& segment,
            float& segmentPercent) override
        {
            lastCarPosition = carPosition;
            segment = 0;
            segmentPercent = 0.0f;
        }

        [[nodiscard]] Matrix GetTrackPositionMatrix(
            int, float, float& roadWidth,
            float& nextRoadWidth) override
        {
            roadWidth = trackRoadWidth;
            nextRoadWidth = trackRoadWidth;
            Matrix matrix = Matrix::getIdentityProperty();
            matrix.setRightProperty(Vector3::UnitX);
            matrix.setUpProperty(Vector3::UnitZ);
            matrix.setForwardProperty(Vector3::UnitY);
            matrix.setTranslationProperty(Vector3(
                lastCarPosition.X, lastCarPosition.Y, 0.0f));
            return matrix;
        }

        [[nodiscard]] int GetCheckpointTimeCount() const override
        {
            return static_cast<int>(checkpointTimes.size());
        }
        [[nodiscard]] int GetCheckpointSegmentCount() const override
        {
            return 0;
        }
        [[nodiscard]] int GetCheckpointSegment(int) const override
        {
            return 0;
        }
        [[nodiscard]] int CompareCheckpointTime(int) override
        {
            return 0;
        }
        void AddCheckpointTime(const float seconds) override
        {
            checkpointTimes.push_back(seconds);
        }
        void AddTimeFadeupEffect(int, TimeFadeupMode) override {}
        void AddBrakeTrack(const CarPhysics&) override { ++brakeTracks; }
        void PlayBrakeSound(BrakeSoundType) override { ++brakeSounds; }
        void PlayCrashSound(bool) override { ++crashSounds; }
        void WobbelCamera(float) override {}
        void SetCameraPosition(const Vector3 position) override
        {
            cameraPosition = position;
        }
        void InterpolateCameraPosition(const Vector3 position) override
        {
            cameraPosition = position;
        }
        [[nodiscard]] int GetReplayMatrixCount() const override
        {
            return static_cast<int>(replayMatrices.size());
        }
        void AddReplayCarMatrix(const Matrix matrix) override
        {
            replayMatrices.push_back(matrix);
        }

        float moveFactor = 0.0f;
        float elapsedMilliseconds = 0.0f;
        float totalMilliseconds = 0.0f;
        Vector3 cameraPosition;
        int brakeTracks = 0;
        int brakeSounds = 0;
        int crashSounds = 0;
        int startedLaps = 0;
        int startLightHash = 0;
        int highscoreSubmissions = 0;
        float trackRoadWidth = 100.0f;

    private:
        Vector3 lastCarPosition;
        std::vector<float> checkpointTimes;
        std::vector<Matrix> replayMatrices;
    };

    class BasePlayerProbe final : public RacingGame::GameLogic::BasePlayer
    {
    public:
        explicit BasePlayerProbe(FlatCarEnvironment& environment)
            : BasePlayer(environment)
        {
        }

        [[nodiscard]] float GetZoom() const
        {
            return getZoomInTimeProperty();
        }

        void SetZoom(const float value)
        {
            setZoomInTimeProperty(value);
        }

        void CompleteLap()
        {
            StartNewLap();
        }
    };

    std::uint64_t ProbeBasePlayer()
    {
        FlatCarEnvironment environment;
        BasePlayerProbe player(environment);
        player.Reset();
        std::uint64_t hash = OffsetBasis;
        hash = HashInt32(hash, player.getCurrentLapProperty());
        hash = HashSingle(hash, player.getBestTimeMillisecondsProperty());
        hash = HashSingle(hash, player.getGameTimeMillisecondsProperty());
        hash = HashSingle(hash, player.GetZoom());
        hash = HashInt32(hash, player.getGameOverProperty() ? 1 : 0);

        environment.elapsedMilliseconds = 100.0f;
        player.Update();
        for (int index = 0; index < 4; ++index)
        {
            environment.elapsedMilliseconds = 1600.0f;
            player.Update();
        }
        player.SetZoom(0.0f);
        environment.elapsedMilliseconds = 33.0f;
        player.Update();
        player.CompleteLap();
        player.SetGameOverAndUploadHighscore();
        player.SetGameOverAndUploadHighscore();

        hash = HashInt32(hash, player.getCurrentLapProperty());
        hash = HashSingle(hash, player.getBestTimeMillisecondsProperty());
        hash = HashSingle(hash, player.getGameTimeMillisecondsProperty());
        hash = HashSingle(hash, player.GetZoom());
        hash = HashInt32(hash, player.getGameOverProperty() ? 1 : 0);
        hash = HashInt32(hash, environment.startedLaps);
        hash = HashInt32(hash, environment.startLightHash);
        hash = HashInt32(hash, environment.highscoreSubmissions);
        return hash;
    }

    std::uint64_t ProbeControlMapping()
    {
        const KeyboardState keyboard{
            Keys::Left, Keys::A, Keys::E, Keys::PageUp,
            Keys::W, Keys::O, Keys::Space};
        const MouseState mouse(
            17, 23, 240, ButtonState::Pressed, ButtonState::Pressed,
            ButtonState::Released, ButtonState::Released,
            ButtonState::Released);
        const GamePadState gamePad(
            Vector2(0.35f, -0.2f), Vector2::Zero,
            0.75f, 0.65f,
            {Buttons::DPadRight, Buttons::A, Buttons::X});
        const CarControlState input = CarControlState::FromXnaInput(
            keyboard, mouse, 2.25f, -120, gamePad);
        std::uint64_t hash = OffsetBasis;
        const auto addBool = [&hash](const bool value)
        {
            hash = HashInt32(hash, value ? 1 : 0);
        };
        addBool(input.keyboardLeftPressed);
        addBool(input.keyboardRightPressed);
        addBool(input.keyA);
        addBool(input.keyD);
        addBool(input.keyE);
        hash = HashSingle(hash, input.mouseXMovement);
        addBool(input.gamePadConnected);
        hash = HashSingle(hash, input.gamePadLeftStickX);
        addBool(input.gamePadDPadLeft);
        addBool(input.gamePadDPadRight);
        addBool(input.keyPageUp);
        addBool(input.keyPageDown);
        addBool(input.gamePadX);
        addBool(input.gamePadY);
        hash = HashInt32(hash, input.mouseWheelDelta);
        addBool(input.keyboardUpPressed);
        addBool(input.keyboardDownPressed);
        addBool(input.keyW);
        addBool(input.keyS);
        addBool(input.keyO);
        addBool(input.mouseLeftButtonPressed);
        addBool(input.mouseRightButtonPressed);
        addBool(input.mouseMiddleButtonPressed);
        addBool(input.gamePadA);
        addBool(input.gamePadB);
        hash = HashSingle(hash, input.gamePadLeftTrigger);
        hash = HashSingle(hash, input.gamePadRightTrigger);
        addBool(input.gamePadDPadUp);
        addBool(input.gamePadDPadDown);
        addBool(input.keySpace);
        return hash;
    }

    CarControlState InputForFrame(const int frame)
    {
        CarControlState input;
        if (frame >= 330 && frame < 420)
        {
            input.keyboardUpPressed = true;
            input.keyW = true;
        }
        else if (frame >= 420 && frame < 460)
        {
            input.keyboardLeftPressed = true;
            input.keyA = true;
            input.keyW = true;
        }
        else if (frame >= 460 && frame < 490)
        {
            input.mouseXMovement = (frame & 1) == 0 ? 2.5f : -1.25f;
            input.mouseLeftButtonPressed = true;
        }
        else if (frame >= 490 && frame < 520)
        {
            input.keySpace = true;
        }
        else if (frame >= 520 && frame < 550)
        {
            input.keyboardDownPressed = true;
            input.keyS = true;
        }
        else if (frame >= 550)
        {
            input.gamePadConnected = true;
            input.gamePadLeftStickX = 0.35f;
            input.gamePadRightTrigger = 0.65f;
            input.gamePadDPadRight = true;
        }
        return input;
    }

    std::uint64_t ProbeCarPhysics(std::ostream& output)
    {
        CarPhysics::SetCarVariablesForCarType(
            CarPhysics::DefaultMaxSpeed * 1.05f,
            CarPhysics::DefaultCarMass * 1.015f,
            CarPhysics::DefaultMaxAccelerationPerSec * 0.85f);
        FlatCarEnvironment environment;
        CarPhysics car(environment, Vector3::Zero);
        car.Reset();
        std::uint64_t hash = OffsetBasis;
        for (int frame = 0; frame < 600; ++frame)
        {
            environment.moveFactor = frame % 113 == 0 ? 0.0005f :
                frame % 127 == 0 ? 0.6f :
                frame % 17 == 0 ? 0.033f : 0.016f;
            environment.elapsedMilliseconds =
                environment.moveFactor * 1000.0f;
            environment.totalMilliseconds += environment.elapsedMilliseconds;
            car.Update(InputForFrame(frame));

            hash = HashVector3(hash, car.getCarPositionProperty());
            hash = HashVector3(hash, car.getCarDirectionProperty());
            hash = HashVector3(hash, car.getCarUpVectorProperty());
            hash = HashSingle(hash, car.getSpeedProperty());
            const float acceleration = car.getAccelerationProperty();
            hash = HashSingle(hash, acceleration);
            hash = HashSingle(hash, car.getCarWheelPosProperty());
            hash = HashMatrix(hash, car.getCarRenderMatrixProperty());
            hash = HashSingle(hash, car.getGameTimeMillisecondsProperty());
            hash = HashInt32(hash, static_cast<int>(
                environment.GetReplayMatrixCount()));
            hash = HashInt32(hash, environment.brakeTracks);
            hash = HashInt32(hash, environment.brakeSounds);
            hash = HashInt32(hash, environment.crashSounds);
            output << "CARFRAME" << std::setw(3) << std::setfill('0')
                   << frame << " hash=" << std::hex << std::setw(16)
                   << hash << std::dec << '\n';
        }
        return hash;
    }

    std::uint64_t ProbeCarCollisions(std::ostream& output)
    {
        CarPhysics::SetCarVariablesForCarType(
            CarPhysics::DefaultMaxSpeed * 1.05f,
            CarPhysics::DefaultCarMass * 1.015f,
            CarPhysics::DefaultMaxAccelerationPerSec * 0.85f);
        FlatCarEnvironment environment;
        environment.trackRoadWidth = 3.0f;
        CarPhysics car(environment, Vector3::Zero);
        car.Reset();
        std::uint64_t hash = OffsetBasis;
        for (int frame = 0; frame < 60; ++frame)
        {
            environment.moveFactor = 0.016f;
            environment.elapsedMilliseconds = frame < 2 ? 3000.0f : 16.0f;
            environment.totalMilliseconds += environment.elapsedMilliseconds;
            CarControlState input;
            if (frame >= 2)
            {
                input.keyboardUpPressed = true;
                input.keyW = true;
                input.keyboardLeftPressed = (frame & 1) == 0;
                input.keyA = (frame & 1) == 0;
            }
            car.Update(input);
            hash = HashVector3(hash, car.getCarPositionProperty());
            hash = HashVector3(hash, car.getCarDirectionProperty());
            hash = HashSingle(hash, car.getSpeedProperty());
            hash = HashSingle(hash, car.getCarWheelPosProperty());
            hash = HashMatrix(hash, car.getCarRenderMatrixProperty());
            hash = HashInt32(hash, environment.brakeTracks);
            hash = HashInt32(hash, environment.brakeSounds);
            hash = HashInt32(hash, environment.crashSounds);
            output << "COLLISIONFRAME" << std::setw(3)
                   << std::setfill('0') << frame << " hash=" << std::hex
                   << std::setw(16) << hash << std::dec << '\n';
        }
        return hash;
    }

    std::uint64_t ProbeVectors()
    {
        std::uint64_t hash = OffsetBasis;
        hash = HashSingle(hash, Vector3Helper::GetAngleBetweenVectors(
            Vector3::UnitX, Vector3::UnitY));
        hash = HashSingle(hash, Vector3Helper::GetAngleBetweenVectors(
            Vector3(0.6f, 0.8f, 0.0f), Vector3::UnitX));
        hash = HashSingle(hash, Vector3Helper::DistanceToLine(
            Vector3(2.0f, 3.0f, 4.0f), Vector3(-1.0f, 0.5f, 2.0f),
            Vector3(5.0f, 4.5f, -2.0f)));
        return HashSingle(hash, Vector3Helper::SignedDistanceToPlane(
            Vector3(2.0f, -1.0f, 5.0f), Vector3(0.0f, 0.0f, 2.0f),
            Vector3::UnitZ));
    }

    std::uint64_t ProbeDefaultSpring()
    {
        SpringPhysicsObject spring;
        spring.ChangePos(1.25f);
        std::uint64_t hash = HashSpringState(OffsetBasis, spring);
        for (const float step : std::array{0.016f, 0.010f, 0.033f, 0.025f,
                                           0.016f, 0.008f})
        {
            spring.Simulate(step);
            hash = HashSpringState(hash, spring);
        }
        return hash;
    }

    std::uint64_t ProbeCarSpring()
    {
        SpringPhysicsObject spring(0.75f, 0.65f, 2.25f, -0.4f);
        spring.force = 0.125f;
        std::uint64_t hash = HashSpringState(OffsetBasis, spring);
        for (const float step : std::array{0.020f, 0.020f, 0.015f, 0.040f})
        {
            spring.Simulate(step);
            hash = HashSpringState(hash, spring);
        }
        spring.ChangePos(-0.35f);
        return HashSpringState(hash, spring);
    }
}

int main(int argc, char** argv)
{
    std::ofstream file;
    std::ostream* output = &std::cout;
    if (argc == 2)
    {
        file.open(argv[1], std::ios::trunc);
        output = &file;
    }
    else if (argc != 1)
    {
        std::fprintf(stderr, "usage: RacingGamePhysicsProbe [report path]\n");
        return 2;
    }

    *output << "FORMAT racing-cna-physics-oracle-v1\n"
            << "VECTOR hash=" << std::hex << std::setw(16)
            << std::setfill('0') << ProbeVectors() << '\n'
            << "SPRING defaultHash=" << std::setw(16) << ProbeDefaultSpring()
            << " carHash=" << std::setw(16) << ProbeCarSpring() << std::dec
            << '\n'
            << "BASE stateHash=" << std::hex << std::setw(16)
            << ProbeBasePlayer() << std::dec << '\n'
            << "CONTROL stateHash=" << std::hex << std::setw(16)
            << ProbeControlMapping() << std::dec << '\n';
    const std::uint64_t carHash = ProbeCarPhysics(*output);
    *output << "CAR stateHash=" << std::hex << std::setw(16)
            << carHash << std::dec << '\n';
    const std::uint64_t collisionHash = ProbeCarCollisions(*output);
    *output << "COLLISION stateHash=" << std::hex << std::setw(16)
            << collisionHash << std::dec << '\n'
            << "RESULT PASS\n";
    return 0;
}
