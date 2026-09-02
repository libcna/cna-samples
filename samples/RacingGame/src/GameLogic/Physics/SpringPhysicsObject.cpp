// SPDX-License-Identifier: MS-PL

#include "GameLogic/Physics/SpringPhysicsObject.hpp"

namespace RacingGame::GameLogic::Physics
{
    SpringPhysicsObject::SpringPhysicsObject(
        const float setMass, const float setFriction,
        const float setSpringConstant, const float setInitialPos)
        : pos(setInitialPos), mass(setMass), friction(setFriction),
          springConstant(setSpringConstant)
    {
    }

    void SpringPhysicsObject::Simulate(const float timeChange)
    {
        force += -pos * springConstant;
        velocity = force / mass;
        pos += timeChange * velocity;
        force *= 1.0f - timeChange * friction;
    }

    void SpringPhysicsObject::ChangePos(const float change)
    {
        pos += change;
    }
}
