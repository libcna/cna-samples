// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <memory>

#include "Screen.hpp"
#include "Microsoft/Xna/Framework/Audio/Cue.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"

namespace Spacewar
{
    class ShipUpgradeScreen final : public Screen
    {
    public:
        explicit ShipUpgradeScreen(Microsoft::Xna::Framework::Game* game);
        GameState Update(System::TimeSpan time, System::TimeSpan elapsedTime) override;
        void Shutdown() override;
        void Render() override;
        void OnCreateDevice() override;

    private:
        [[nodiscard]] static Microsoft::Xna::Framework::Rectangle SelectedSprite(int player, int weapon);
        [[nodiscard]] static Microsoft::Xna::Framework::Rectangle DisabledSprite(int player, int weapon);
        [[nodiscard]] static Microsoft::Xna::Framework::Rectangle ReadySprite(int player, int weapon);
        [[nodiscard]] static std::string Grouped(int value);

        inline static const std::string UpgradeTexture = "textures/weapon_select_FINAL";
        static constexpr int CountSpeed = 3;
        bool player1Ready_ = false;
        bool player2Ready_ = false;
        bool playingTallySound_ = false;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::Cue> tallySound_;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::Cue> menuMusic_;
        std::array<int, 2> playerCashCount_{};
        std::array<float, 2> flashPercent_{};
        std::array<System::TimeSpan, 2> flashEndTime_{};
        System::TimeSpan flashTime_{0, 0, 3};
        std::array<SceneItem*, 2> weapons_{};
        std::array<ProjectileType, 2> purchasedWeapon_{
            ProjectileType::Peashooter, ProjectileType::Peashooter};
        std::array<std::array<Microsoft::Xna::Framework::Vector2, 4>, 2> weaponPositions_{{
            {{ {248, 133}, {75, 314}, {444, 314}, {248, 501} }},
            {{ {864, 133}, {686, 314}, {1055, 314}, {864, 501} }},
        }};
        Microsoft::Xna::Framework::Vector4 upgradeFontColor_{0.882f, 0.596f, 0.286f, 1.0f};
    };
}
