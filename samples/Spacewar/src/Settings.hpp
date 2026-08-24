// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

#include <array>
#include <string>

namespace Spacewar
{
    class Settings
    {
    public:
        struct PlayerShipInfo
        {
            Microsoft::Xna::Framework::Vector2 StartPosition;
            double StartAngle;
        };

        struct WeaponInfo
        {
            int Cost;
            double Lifetime;
            int Max;
            int Burst;
            float Acceleration;
            int Damage;
        };

        struct ShipLighting
        {
            Microsoft::Xna::Framework::Vector4 Ambient;
            Microsoft::Xna::Framework::Vector4 DirectionalDirection;
            Microsoft::Xna::Framework::Vector4 DirectionalColor;
            Microsoft::Xna::Framework::Vector4 PointPosition;
            Microsoft::Xna::Framework::Vector4 PointColor;
            float PointFactor;
        };

        std::string MediaPath;
        std::string WindowTitle = "Spacewar";
        int LevelTime = 30;
        float ThrustPower = 100.0f;
        float FrictionFactor = 0.1f;
        float MaxSpeed = 200.0f;
        float ShipRecoveryTime = 1.6f;

        Microsoft::Xna::Framework::Vector2 SunPosition{0.0f, 0.0f};
        double GravityStrength = 500000.0;
        int GravityPower = 2;
        float ColorDistribution = 3.0f;
        float Fade = 4.0f;
        float FlameSpeed = 0.22f;
        float Spread = 0.50f;
        float Flamability = 1.74f;
        float Size = 70.0f;

        float AsteroidScale = 0.02f;
        float BulletScale = 0.02f;
        float ShipScale = 0.02f;
        std::array<PlayerShipInfo, 2> Ships{{
            {{-300.0f, 0.0f}, 90.0},
            {{300.0f, 0.0f}, 90.0},
        }};

        std::array<WeaponInfo, 5> Weapons{{
            {0, 3.0, 5, 1, 0.0f, 1},
            {1000, 3.0, 4, 3, 0.0f, 1},
            {2000, 3.0, 3, 3, 0.0f, 1},
            {3000, 2.0, 1, 1, 1.0f, 5},
            {4000, 2.0, 3, 1, 0.0f, 5},
        }};

        float CrossFadeSpeed = 0.2f;
        float OffsetSpeed = 0.1f;
        std::array<ShipLighting, 2> ShipLights{{
            {{1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 0.0f},
             {0.4f, 0.4f, 0.8f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f},
             {0.8f, 0.6f, 0.0f, 1.0f}, 0.01f},
            {{0.2f, 0.2f, 0.2f, 1.0f}, {1.0f, 1.0f, 1.0f, 0.0f},
             {0.4f, 0.4f, 0.8f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f},
             {0.8f, 0.6f, 0.0f, 1.0f}, 0.008f},
        }};

        Microsoft::Xna::Framework::Input::Keys Player1Start = Microsoft::Xna::Framework::Input::Keys::LeftControl;
        Microsoft::Xna::Framework::Input::Keys Player1Back = Microsoft::Xna::Framework::Input::Keys::LeftShift;
        Microsoft::Xna::Framework::Input::Keys Player1A = Microsoft::Xna::Framework::Input::Keys::V;
        Microsoft::Xna::Framework::Input::Keys Player1B = Microsoft::Xna::Framework::Input::Keys::G;
        Microsoft::Xna::Framework::Input::Keys Player1X = Microsoft::Xna::Framework::Input::Keys::F;
        Microsoft::Xna::Framework::Input::Keys Player1Y = Microsoft::Xna::Framework::Input::Keys::T;
        Microsoft::Xna::Framework::Input::Keys Player1ThumbstickLeftXmin = Microsoft::Xna::Framework::Input::Keys::A;
        Microsoft::Xna::Framework::Input::Keys Player1ThumbstickLeftXmax = Microsoft::Xna::Framework::Input::Keys::D;
        Microsoft::Xna::Framework::Input::Keys Player1ThumbstickLeftYmin = Microsoft::Xna::Framework::Input::Keys::S;
        Microsoft::Xna::Framework::Input::Keys Player1ThumbstickLeftYmax = Microsoft::Xna::Framework::Input::Keys::W;
        Microsoft::Xna::Framework::Input::Keys Player1Left = Microsoft::Xna::Framework::Input::Keys::A;
        Microsoft::Xna::Framework::Input::Keys Player1Right = Microsoft::Xna::Framework::Input::Keys::D;
        Microsoft::Xna::Framework::Input::Keys Player1Down = Microsoft::Xna::Framework::Input::Keys::S;
        Microsoft::Xna::Framework::Input::Keys Player1Up = Microsoft::Xna::Framework::Input::Keys::W;
        Microsoft::Xna::Framework::Input::Keys Player1LeftTrigger = Microsoft::Xna::Framework::Input::Keys::Q;
        Microsoft::Xna::Framework::Input::Keys Player1RightTrigger = Microsoft::Xna::Framework::Input::Keys::E;

        Microsoft::Xna::Framework::Input::Keys Player2Start = Microsoft::Xna::Framework::Input::Keys::RightControl;
        Microsoft::Xna::Framework::Input::Keys Player2Back = Microsoft::Xna::Framework::Input::Keys::RightShift;
        Microsoft::Xna::Framework::Input::Keys Player2A = Microsoft::Xna::Framework::Input::Keys::Home;
        Microsoft::Xna::Framework::Input::Keys Player2B = Microsoft::Xna::Framework::Input::Keys::End;
        Microsoft::Xna::Framework::Input::Keys Player2X = Microsoft::Xna::Framework::Input::Keys::PageUp;
        Microsoft::Xna::Framework::Input::Keys Player2Y = Microsoft::Xna::Framework::Input::Keys::PageDown;
        Microsoft::Xna::Framework::Input::Keys Player2ThumbstickLeftXmin = Microsoft::Xna::Framework::Input::Keys::Left;
        Microsoft::Xna::Framework::Input::Keys Player2ThumbstickLeftXmax = Microsoft::Xna::Framework::Input::Keys::Right;
        Microsoft::Xna::Framework::Input::Keys Player2ThumbstickLeftYmin = Microsoft::Xna::Framework::Input::Keys::Down;
        Microsoft::Xna::Framework::Input::Keys Player2ThumbstickLeftYmax = Microsoft::Xna::Framework::Input::Keys::Up;
        Microsoft::Xna::Framework::Input::Keys Player2Left = Microsoft::Xna::Framework::Input::Keys::Left;
        Microsoft::Xna::Framework::Input::Keys Player2Right = Microsoft::Xna::Framework::Input::Keys::Right;
        Microsoft::Xna::Framework::Input::Keys Player2Down = Microsoft::Xna::Framework::Input::Keys::Down;
        Microsoft::Xna::Framework::Input::Keys Player2Up = Microsoft::Xna::Framework::Input::Keys::Up;
        Microsoft::Xna::Framework::Input::Keys Player2LeftTrigger = Microsoft::Xna::Framework::Input::Keys::Insert;
        Microsoft::Xna::Framework::Input::Keys Player2RightTrigger = Microsoft::Xna::Framework::Input::Keys::Delete;

        void Save(const std::string& filename) const;
        [[nodiscard]] static Settings Load(const std::string& filename);
    };
}
