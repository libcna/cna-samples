// SPDX-License-Identifier: MS-PL
#pragma once

#include <vector>

#include "SpacewarScreen.hpp"
#include "System/Random.hpp"

namespace Spacewar
{
    class Asteroid;

    class EvolvedScreen final : public SpacewarScreen
    {
    public:
        explicit EvolvedScreen(Microsoft::Xna::Framework::Game* game);
        GameState Update(System::TimeSpan time, System::TimeSpan elapsedTime) override;
        void Render() override;
        void OnCreateDevice() override;

    protected:
        void HandleCollisions(System::TimeSpan gameTime) override;

    private:
        static std::string Currency(int value);

        double sunScale_ = 1000.0;
        std::vector<Asteroid*> asteroids_;
        float levelTime_ = 0.0f;
        int lastLevelTime_ = 0;
        bool ended_ = false;
        double endTime_ = 0.0;
        System::Random random_;
    };
}
