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
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Net/PacketReader.hpp"
#include "Microsoft/Xna/Framework/Net/PacketWriter.hpp"

namespace NetworkPrediction
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Content;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Net;

    Tank::Tank(int gamerIndex, ContentManager& content, int screenWidth, int screenHeight)
    {
        const float x = static_cast<float>(
            screenWidth / 4 + (gamerIndex % 5) * screenWidth / 8);
        const float y = static_cast<float>(
            screenHeight / 4 + (gamerIndex / 5) * screenHeight / 5);

        simulationState.Position = Vector2(x, y);
        simulationState.TankRotation = -MathHelper::PiOver2;
        simulationState.TurretRotation = -MathHelper::PiOver2;

        previousState = simulationState;
        displayState = simulationState;

        tankTexture = content.Load<Texture2D>("Tank");
        turretTexture = content.Load<Texture2D>("Turret");

        screenSize = Vector2(static_cast<float>(screenWidth), static_cast<float>(screenHeight));
    }

    Vector2 Tank::getPositionProperty() const
    {
        return displayState.Position;
    }

    void Tank::UpdateLocal(Vector2 newTankInput, Vector2 newTurretInput)
    {
        tankInput = newTankInput;
        turretInput = newTurretInput;

        UpdateState(simulationState);
        displayState = simulationState;
    }

    void Tank::UpdateRemote(int framesBetweenPackets, bool enablePrediction)
    {
        const float smoothingDecay = 1.0f / static_cast<float>(framesBetweenPackets);

        currentSmoothing -= smoothingDecay;

        if (currentSmoothing < 0.0f)
        {
            currentSmoothing = 0.0f;
        }

        if (enablePrediction)
        {
            UpdateState(simulationState);

            if (currentSmoothing > 0.0f)
            {
                UpdateState(previousState);
            }
        }

        if (currentSmoothing > 0.0f)
        {
            ApplySmoothing();
        }
        else
        {
            displayState = simulationState;
        }
    }

    void Tank::ApplySmoothing()
    {
        displayState.Position = Vector2::Lerp(
            simulationState.Position,
            previousState.Position,
            currentSmoothing);

        displayState.Velocity = Vector2::Lerp(
            simulationState.Velocity,
            previousState.Velocity,
            currentSmoothing);

        displayState.TankRotation = MathHelper::Lerp(
            simulationState.TankRotation,
            previousState.TankRotation,
            currentSmoothing);

        displayState.TurretRotation = MathHelper::Lerp(
            simulationState.TurretRotation,
            previousState.TurretRotation,
            currentSmoothing);
    }

    void Tank::WriteNetworkPacket(PacketWriter& packetWriter, const GameTime& gameTime)
    {
        packetWriter.Write(static_cast<float>(
            gameTime.getTotalGameTimeProperty().getTotalSecondsProperty()));

        packetWriter.Write(simulationState.Position);
        packetWriter.Write(simulationState.Velocity);
        packetWriter.Write(simulationState.TankRotation);
        packetWriter.Write(simulationState.TurretRotation);

        packetWriter.Write(tankInput);
        packetWriter.Write(turretInput);
    }

    void Tank::ReadNetworkPacket(
        PacketReader& packetReader,
        const GameTime& gameTime,
        TimeSpan latency,
        bool enablePrediction,
        bool enableSmoothing)
    {
        if (enableSmoothing)
        {
            previousState = displayState;
            currentSmoothing = 1.0f;
        }
        else
        {
            currentSmoothing = 0.0f;
        }

        const float packetSendTime = packetReader.ReadSingle();

        simulationState.Position = packetReader.ReadVector2();
        simulationState.Velocity = packetReader.ReadVector2();
        simulationState.TankRotation = packetReader.ReadSingle();
        simulationState.TurretRotation = packetReader.ReadSingle();

        tankInput = packetReader.ReadVector2();
        turretInput = packetReader.ReadVector2();

        if (enablePrediction)
        {
            ApplyPrediction(gameTime, latency, packetSendTime);
        }
    }

    void Tank::ApplyPrediction(
        const GameTime& gameTime,
        TimeSpan latency,
        float packetSendTime)
    {
        const float localTime = static_cast<float>(
            gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());

        const float timeDelta = localTime - packetSendTime;
        clockDelta.AddValue(timeDelta);

        const float timeDeviation = timeDelta - clockDelta.getAverageValueProperty();
        latency = latency + TimeSpan::FromSeconds(timeDeviation);

        const TimeSpan oneFrame = TimeSpan::FromSeconds(1.0 / 60.0);

        while (latency >= oneFrame)
        {
            UpdateState(simulationState);
            latency = latency - oneFrame;
        }
    }

    void Tank::UpdateState(TankState& state)
    {
        state.TankRotation = TurnToFace(state.TankRotation, tankInput, TankTurnRate);
        state.TurretRotation = TurnToFace(state.TurretRotation, turretInput, TurretTurnRate);

        const Vector2 tankForward(
            static_cast<float>(std::cos(state.TankRotation)),
            static_cast<float>(std::sin(state.TankRotation)));

        const Vector2 targetForward(tankInput.X, -tankInput.Y);
        const float facingForward = Vector2::Dot(tankForward, targetForward);

        if (facingForward > 0.0f)
        {
            const float speed = facingForward * facingForward * TankSpeed;
            state.Velocity = state.Velocity + tankForward * speed;
        }

        state.Position = state.Position + state.Velocity;
        state.Velocity = state.Velocity * TankFriction;
        state.Position = Vector2::Clamp(state.Position, Vector2::Zero, screenSize);
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
            displayState.Position,
            std::nullopt,
            Color::White,
            displayState.TankRotation,
            origin,
            1.0f,
            SpriteEffects::None,
            0.0f);

        spriteBatch.Draw(
            turretTexture,
            displayState.Position,
            std::nullopt,
            Color::White,
            displayState.TurretRotation,
            origin,
            1.0f,
            SpriteEffects::None,
            0.0f);
    }
}
