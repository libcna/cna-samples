// SPDX-License-Identifier: MS-PL

#include "GameLogic/Player.hpp"

#include <iomanip>
#include <sstream>

namespace RacingGame::GameLogic
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;

    namespace
    {
        std::string TwoDigits(const int value)
        {
            std::ostringstream stream;
            stream << std::setw(2) << std::setfill('0') << value;
            return stream.str();
        }

        std::string LapTimeText(const std::size_t lapIndex, const float seconds)
        {
            const int wholeSeconds = static_cast<int>(seconds);
            const int hundredths =
                static_cast<int>(seconds * 100.0f) % 100;
            return "Lap " + std::to_string(lapIndex + 1) + " Time: " +
                TwoDigits(wholeSeconds / 60) + ":" +
                TwoDigits(wholeSeconds % 60) + "." +
                TwoDigits(hundredths);
        }
    }

    Player::Player(
        PlayerEnvironment& setEnvironment, const Vector3 setCarPosition)
        : ChaseCamera(setEnvironment, setCarPosition),
          playerEnvironment(setEnvironment)
    {
    }

    void Player::AddLapTime(const float setLapTime)
    {
        lapTimes.push_back(setLapTime);
    }

    void Player::Reset()
    {
        ChaseCamera::Reset();
        lapTimes.clear();
    }

    void Player::Update(const CarControlState& input)
    {
        if (playerEnvironment.IsInGame() &&
            getZoomInTimeProperty() <= 0.0f)
        {
            if (isGameOver)
            {
                cameraPos = getCarPositionProperty() +
                    Vector3(0.0f, -5.0f, 20.0f) +
                    Vector3::TransformNormal(
                        Vector3(30.0f, 0.0f, 0.0f),
                        Matrix::CreateRotationZ(
                            playerEnvironment.GetTotalTimeMilliseconds() /
                            2593.0f));
                playerEnvironment.SetViewMatrix(Matrix::CreateLookAt(
                    cameraPos, getCarPositionProperty(),
                    getCarUpVectorProperty()));
                const int rank = playerEnvironment.GetRankFromCurrentTime(
                    levelNum,
                    static_cast<int>(getBestTimeMillisecondsProperty()));
                currentGameTimeMilliseconds =
                    getBestTimeMillisecondsProperty();

                playerEnvironment.WriteTextCentered(
                    playerEnvironment.GetDisplayWidth() / 2,
                    playerEnvironment.GetDisplayHeight() / 7,
                    victory ? "Victory! You won." : "Game Over! You lost.",
                    victory ? Color::LightGreen : Color::Red, 1.25f);
                for (std::size_t index = 0; index < lapTimes.size(); ++index)
                {
                    playerEnvironment.WriteTextCentered(
                        playerEnvironment.GetDisplayWidth() / 2,
                        playerEnvironment.GetDisplayHeight() / 7 +
                            playerEnvironment.YToRes(35) *
                                (1 + static_cast<int>(index)),
                        LapTimeText(index, lapTimes[index]),
                        Color::White, 1.25f);
                }
                playerEnvironment.WriteTextCentered(
                    playerEnvironment.GetDisplayWidth() / 2,
                    playerEnvironment.GetDisplayHeight() / 7 +
                        playerEnvironment.YToRes(35) *
                            (1 + static_cast<int>(lapTimes.size())),
                    "Rank: " + std::to_string(1 + rank),
                    Color::White, 1.25f);
                return;
            }

            if (!isCarOnGround)
            {
                inAirTimeMilliseconds +=
                    playerEnvironment.GetElapsedMilliseconds();
            }
            else
            {
                inAirTimeMilliseconds = 0.0f;
            }

            const float trackDistance = Vector3::Distance(
                getCarPositionProperty(), groundPlanePos);
            if (trackDistance > 20.0f ||
                inAirTimeMilliseconds > InAirTimeoutMilliseconds)
            {
                ClearVariablesForGameOver();
                isGameOver = true;
                victory = false;
                playerEnvironment.PlayPlayerSound(PlayerSound::CarLose);
                playerEnvironment.StopGearSound();
            }

            if (getCurrentLapProperty() >= LapCount)
            {
                ClearVariablesForGameOver();
                --lap;
                isGameOver = true;
                victory = true;
                playerEnvironment.PlayPlayerSound(PlayerSound::Victory);
                playerEnvironment.StopGearSound();
            }
        }

        ChaseCamera::Update(input);
    }
}
