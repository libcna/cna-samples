// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Tank.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Tank.hpp"

#include <algorithm>
#include <cmath>
#include <optional>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"

namespace PeerToPeer
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Content;
    using namespace Microsoft::Xna::Framework::Graphics;

    Tank::Tank(int gamerIndex, ContentManager& content, int screenWidth, int screenHeight)
    {
        Position.X = static_cast<float>(
            screenWidth / 4 + (gamerIndex % 5) * screenWidth / 8);
        Position.Y = static_cast<float>(
            screenHeight / 4 + (gamerIndex / 5) * screenHeight / 5);

        TankRotation = -MathHelper::PiOver2;
        TurretRotation = -MathHelper::PiOver2;

        tankTexture = content.Load<Texture2D>("Tank");
        turretTexture = content.Load<Texture2D>("Turret");

        screenSize = Vector2(
            static_cast<float>(screenWidth),
            static_cast<float>(screenHeight));
    }

    void Tank::Update()
    {
        TankRotation = TurnToFace(TankRotation, TankInput, TankTurnRate);
        TurretRotation = TurnToFace(TurretRotation, TurretInput, TurretTurnRate);

        const Vector2 tankForward(
            static_cast<float>(std::cos(TankRotation)),
            static_cast<float>(std::sin(TankRotation)));
        const Vector2 targetForward(TankInput.X, -TankInput.Y);
        const float facingForward = Vector2::Dot(tankForward, targetForward);

        if (facingForward > 0.0f)
        {
            Velocity = Velocity +
                       tankForward * facingForward * facingForward * TankSpeed;
        }

        Position = Position + Velocity;
        Velocity = Velocity * TankFriction;
        Position = Vector2::Clamp(Position, Vector2::Zero, screenSize);
    }

    float Tank::TurnToFace(float rotation, Vector2 target, float turnRate)
    {
        if (target == Vector2::Zero)
        {
            return rotation;
        }

        const float angle = static_cast<float>(std::atan2(-target.Y, target.X));
        float difference = rotation - angle;

        while (difference > MathHelper::Pi)
        {
            difference -= MathHelper::TwoPi;
        }

        while (difference < -MathHelper::Pi)
        {
            difference += MathHelper::TwoPi;
        }

        turnRate *= std::abs(difference);

        if (difference < 0.0f)
        {
            return rotation + std::min(turnRate, -difference);
        }

        return rotation - std::min(turnRate, difference);
    }

    void Tank::Draw(SpriteBatch& spriteBatch)
    {
        const Vector2 origin(
            static_cast<float>(tankTexture.getWidthProperty() / 2),
            static_cast<float>(tankTexture.getHeightProperty() / 2));

        spriteBatch.Draw(
            tankTexture,
            Position,
            std::nullopt,
            Color::White,
            TankRotation,
            origin,
            1.0f,
            SpriteEffects::None,
            0.0f);

        spriteBatch.Draw(
            turretTexture,
            Position,
            std::nullopt,
            Color::White,
            TurretRotation,
            origin,
            1.0f,
            SpriteEffects::None,
            0.0f);
    }
}
