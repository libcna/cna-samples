// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Tank.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "RollingAverage.hpp"
#include "System/TimeSpan.hpp"

namespace Microsoft::Xna::Framework
{
    class GameTime;
}

namespace Microsoft::Xna::Framework::Content
{
    class ContentManager;
}

namespace Microsoft::Xna::Framework::Graphics
{
    class SpriteBatch;
}

namespace Microsoft::Xna::Framework::Net
{
    class PacketReader;
    class PacketWriter;
}

namespace NetworkPrediction
{
    /**
     * @brief Moves and draws one networked tank with prediction and smoothing.
     */
    class Tank
    {
    public:
        /**
         * @brief Creates a tank at the starting position selected by its gamer index.
         *
         * @param gamerIndex Index of the owning gamer in the session roster.
         * @param content Content manager used to load the two tank textures.
         * @param screenWidth Width of the movement area.
         * @param screenHeight Height of the movement area.
         */
        Tank(
            int gamerIndex,
            Microsoft::Xna::Framework::Content::ContentManager& content,
            int screenWidth,
            int screenHeight);

        /**
         * @brief Gets the smoothed position currently displayed.
         *
         * @return Current display position.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector2 getPositionProperty() const;

        /**
         * @brief Advances a locally controlled tank using its latest inputs.
         *
         * @param tankInput Requested tank direction.
         * @param turretInput Requested turret direction.
         */
        void UpdateLocal(
            Microsoft::Xna::Framework::Vector2 tankInput,
            Microsoft::Xna::Framework::Vector2 turretInput);

        /**
         * @brief Advances prediction and smoothing for a remotely controlled tank.
         *
         * @param framesBetweenPackets Expected packet interval in frames.
         * @param enablePrediction Whether local prediction is enabled.
         */
        void UpdateRemote(int framesBetweenPackets, bool enablePrediction);

        /**
         * @brief Writes the complete local tank state and current inputs to a packet.
         *
         * @param packetWriter Packet writer receiving the state.
         * @param gameTime Current timing snapshot.
         */
        void WriteNetworkPacket(
            Microsoft::Xna::Framework::Net::PacketWriter& packetWriter,
            const Microsoft::Xna::Framework::GameTime& gameTime);

        /**
         * @brief Reads a remote state packet and applies optional prediction and smoothing.
         *
         * @param packetReader Packet reader containing the remote state.
         * @param gameTime Current timing snapshot.
         * @param latency Estimated one-way packet latency.
         * @param enablePrediction Whether latency prediction is enabled.
         * @param enableSmoothing Whether interpolation from the previous display state is enabled.
         */
        void ReadNetworkPacket(
            Microsoft::Xna::Framework::Net::PacketReader& packetReader,
            const Microsoft::Xna::Framework::GameTime& gameTime,
            System::TimeSpan latency,
            bool enablePrediction,
            bool enableSmoothing);

        /**
         * @brief Draws the tank body and turret.
         *
         * @param spriteBatch Active sprite batch.
         */
        void Draw(Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch);

    private:
        static constexpr float TankTurnRate = 0.01f;
        static constexpr float TurretTurnRate = 0.03f;
        static constexpr float TankSpeed = 0.3f;
        static constexpr float TankFriction = 0.9f;

        struct TankState
        {
            Microsoft::Xna::Framework::Vector2 Position;
            Microsoft::Xna::Framework::Vector2 Velocity;
            float TankRotation = 0.0f;
            float TurretRotation = 0.0f;
        };

        TankState simulationState;
        TankState previousState;
        TankState displayState;
        float currentSmoothing = 0.0f;
        RollingAverage clockDelta{100};
        Microsoft::Xna::Framework::Vector2 tankInput;
        Microsoft::Xna::Framework::Vector2 turretInput;
        Microsoft::Xna::Framework::Graphics::Texture2D tankTexture;
        Microsoft::Xna::Framework::Graphics::Texture2D turretTexture;
        Microsoft::Xna::Framework::Vector2 screenSize;

        void ApplySmoothing();
        void ApplyPrediction(
            const Microsoft::Xna::Framework::GameTime& gameTime,
            System::TimeSpan latency,
            float packetSendTime);
        void UpdateState(TankState& state);
        [[nodiscard]] static float TurnToFace(
            float rotation,
            Microsoft::Xna::Framework::Vector2 target,
            float turnRate);
    };
}
