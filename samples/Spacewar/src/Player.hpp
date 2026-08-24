// SPDX-License-Identifier: MS-PL
#pragma once

#include "Enums.hpp"

namespace Spacewar
{
    class Player
    {
    public:
        [[nodiscard]] ShipClass getShipClassProperty() const { return shipClass_; }
        void setShipClassProperty(ShipClass value) { shipClass_ = value; }
        [[nodiscard]] int getCashProperty() const { return cash_; }
        void setCashProperty(int value) { cash_ = value; }
        [[nodiscard]] int getScoreProperty() const { return score_; }
        void setScoreProperty(int value) { score_ = value; }
        [[nodiscard]] int getSkinProperty() const { return skin_; }
        void setSkinProperty(int value) { skin_ = value; }
        [[nodiscard]] int getHealthProperty() const { return health_; }
        void setHealthProperty(int value) { health_ = value; }
        [[nodiscard]] ProjectileType getProjectileTypeProperty() const { return projectileType_; }
        void setProjectileTypeProperty(ProjectileType value) { projectileType_ = value; }

    private:
        ShipClass shipClass_ = ShipClass::Pencil;
        int skin_ = 0;
        int cash_ = 0;
        ProjectileType projectileType_ = ProjectileType::Peashooter;
        int score_ = 0;
        int health_ = 5;
    };
}
