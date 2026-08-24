// SPDX-License-Identifier: MS-PL
#pragma once

#include "Screen.hpp"

namespace Spacewar
{
    class Particles;
    class Projectiles;
    class Ship;

    class SpacewarScreen : public Screen
    {
    public:
        explicit SpacewarScreen(Microsoft::Xna::Framework::Game* game);
        void Shutdown() override;
        void OnCreateDevice() override;

    protected:
        virtual void HandleCollisions(System::TimeSpan gameTime);
        void HitPlayer2(System::TimeSpan gameTime, int damage);
        void HitPlayer1(System::TimeSpan gameTime, int damage);

        Projectiles* bullets_ = nullptr;
        Ship* ship1_ = nullptr;
        Ship* ship2_ = nullptr;
        SceneItem* sun_ = nullptr;
        bool paused_ = true;
        int player1Score_ = 0;
        int player2Score_ = 0;
        Particles* particles_ = nullptr;
        SceneItem* backdrop_ = nullptr;

    private:
        void ResetShips(System::TimeSpan gameTime);
    };
}
