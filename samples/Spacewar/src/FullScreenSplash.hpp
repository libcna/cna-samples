// SPDX-License-Identifier: MS-PL
#pragma once

#include <string>

#include "Screen.hpp"

namespace Spacewar
{
    class FullScreenSplash : public Screen
    {
    public:
        FullScreenSplash(Microsoft::Xna::Framework::Game* game, std::string textureName,
                         System::TimeSpan timeoutSpan, GameState nextState);

        GameState Update(System::TimeSpan time, System::TimeSpan elapsedTime) override;
        void Render() override;

    protected:
        FullScreenSplash(Microsoft::Xna::Framework::Game* game, std::string textureName);

    private:
        void SetTexture(std::string textureName);

        std::string textureName_;
        double timeout_ = 0.0;
        double endTime_ = -1.0;
        GameState nextState_ = GameState::None;
    };
}
