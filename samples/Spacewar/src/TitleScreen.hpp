// SPDX-License-Identifier: MS-PL
#pragma once

#include "FullScreenSplash.hpp"

namespace Spacewar
{
    class TitleScreen final : public FullScreenSplash
    {
    public:
        explicit TitleScreen(Microsoft::Xna::Framework::Game* game);

        GameState Update(System::TimeSpan time, System::TimeSpan elapsedTime) override;
        void Render() override;
        void OnCreateDevice() override;

    private:
        bool showInfo_ = false;
        bool playRetro_ = false;
    };
}
