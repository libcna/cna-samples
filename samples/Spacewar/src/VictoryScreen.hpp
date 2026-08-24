// SPDX-License-Identifier: MS-PL
#pragma once

#include "FullScreenSplash.hpp"

namespace Spacewar
{
    class VictoryScreen final : public FullScreenSplash
    {
    public:
        explicit VictoryScreen(Microsoft::Xna::Framework::Game* game);
        GameState Update(System::TimeSpan time, System::TimeSpan elapsedTime) override;
        void Render() override;
        void OnCreateDevice() override;

    private:
        inline static const std::string VictoryTexture = "textures/victory";
        int winningPlayerNumber_ = 0;
        SceneItem* ship_ = nullptr;
    };
}
