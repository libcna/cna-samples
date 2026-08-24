// SPDX-License-Identifier: MS-PL
#pragma once

#include "SpacewarScreen.hpp"

namespace Spacewar
{
    class RetroScreen final : public SpacewarScreen
    {
    public:
        explicit RetroScreen(Microsoft::Xna::Framework::Game* game);
        void Render() override;
        GameState Update(System::TimeSpan time, System::TimeSpan elapsedTime) override;
        void OnCreateDevice() override;
    };
}
