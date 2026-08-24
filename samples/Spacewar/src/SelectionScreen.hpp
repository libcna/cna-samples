// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <memory>

#include "Screen.hpp"
#include "Microsoft/Xna/Framework/Audio/Cue.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"

namespace Spacewar
{
    class SelectionScreen final : public Screen
    {
    public:
        explicit SelectionScreen(Microsoft::Xna::Framework::Game* game);
        GameState Update(System::TimeSpan time, System::TimeSpan elapsedTime) override;
        void Shutdown() override;
        void Render() override;
        void OnCreateDevice() override;

    private:
        inline static const std::string SelectionTexture = "textures/ship_select_FINAL";
        std::array<SceneItem*, 2> ships_{};
        std::array<int, 2> selectedShip_{};
        std::array<int, 2> selectedSkin_{};
        bool player1Ready_ = false;
        bool player2Ready_ = false;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::Cue> menuMusic_;
    };
}
