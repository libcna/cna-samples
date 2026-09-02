// SPDX-License-Identifier: MS-PL

#include "GameLogic/BasePlayer.hpp"

namespace RacingGame::GameLogic
{
    BasePlayer::BasePlayer(BasePlayerEnvironment& setEnvironment)
        : environment(setEnvironment)
    {
    }

    int BasePlayer::getCurrentLapProperty() const
    {
        return lap;
    }

    float BasePlayer::getBestTimeMillisecondsProperty() const
    {
        return bestLapTimeMilliseconds;
    }

    float BasePlayer::getGameTimeMillisecondsProperty() const
    {
        return currentGameTimeMilliseconds - zoomInTime;
    }

    float BasePlayer::getZoomInTimeProperty() const
    {
        return zoomInTime;
    }

    void BasePlayer::setZoomInTimeProperty(const float value)
    {
        zoomInTime = value;
    }

    bool BasePlayer::getVictoryProperty() const
    {
        return victory;
    }

    int BasePlayer::getLevelNumProperty() const
    {
        return levelNum;
    }

    bool BasePlayer::getGameOverProperty() const
    {
        return isGameOver;
    }

    bool BasePlayer::getWonGameProperty() const
    {
        return victory;
    }

    bool BasePlayer::getCanControlCarProperty() const
    {
        return zoomInTime <= 0.0f && !isGameOver;
    }

    void BasePlayer::StartNewLap()
    {
        ++lap;
        environment.StartLandscapeLap();
        if (bestLapTimeMilliseconds == 0.0f ||
            currentGameTimeMilliseconds < bestLapTimeMilliseconds)
        {
            bestLapTimeMilliseconds = currentGameTimeMilliseconds;
        }
        currentGameTimeMilliseconds = zoomInTime;
    }

    void BasePlayer::SetGameOverAndUploadHighscore()
    {
        isGameOver = true;
        if (!alreadyUploadedHighscore)
        {
            alreadyUploadedHighscore = true;
            environment.SubmitHighscore(
                levelNum, static_cast<int>(currentGameTimeMilliseconds));
        }
    }

    void BasePlayer::Reset()
    {
        levelNum = environment.GetSelectedTrackNumber();
        isGameOver = false;
        alreadyUploadedHighscore = false;
        currentGameTimeMilliseconds = 0.0f;
        bestLapTimeMilliseconds = 0.0f;
        lap = 0;
        victory = false;
        zoomInTime = StartGameZoomTimeMilliseconds;
        firstFrame = true;
    }

    void BasePlayer::ClearVariablesForGameOver()
    {
    }

    void BasePlayer::Update()
    {
        if (firstFrame)
        {
            firstFrame = false;
            return;
        }

        if (!environment.IsInMenu() && zoomInTime > 0.0f)
        {
            const float lastZoomInTime = zoomInTime;
            zoomInTime -= environment.GetElapsedMilliseconds();
            if (zoomInTime < 2000.0f &&
                static_cast<int>((lastZoomInTime + 1000.0f) / 1000.0f) !=
                static_cast<int>((zoomInTime + 1000.0f) / 1000.0f))
            {
                environment.ReplaceStartLightObject(
                    2 - static_cast<int>((zoomInTime + 1000.0f) / 1000.0f));
            }
        }

        if (!getCanControlCarProperty())
            return;
        currentGameTimeMilliseconds += environment.GetElapsedMilliseconds();
    }
}
