// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/XmlSerializationEXT.hpp"
#include "Microsoft/Xna/Framework/XmlSerializationEXT.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"

#include <string>
#include <vector>

namespace Spacewar
{
    class Settings
    {
    public:
        struct PlayerShipInfo
        {
            Microsoft::Xna::Framework::Vector2 StartPosition;
            double StartAngle;
            SHARP_XML_SERIALIZABLE(PlayerShipInfo, "PlayerShipInfo",
                                   SHARP_XML_M(PlayerShipInfo, StartPosition),
                                   SHARP_XML_M(PlayerShipInfo, StartAngle))
        };

        struct WeaponInfo
        {
            int Cost;
            double Lifetime;
            int Max;
            int Burst;
            float Acceleration;
            int Damage;
            SHARP_XML_SERIALIZABLE(WeaponInfo, "WeaponInfo",
                                   SHARP_XML_M(WeaponInfo, Cost),
                                   SHARP_XML_M(WeaponInfo, Lifetime),
                                   SHARP_XML_M(WeaponInfo, Max),
                                   SHARP_XML_M(WeaponInfo, Burst),
                                   SHARP_XML_M(WeaponInfo, Acceleration),
                                   SHARP_XML_M(WeaponInfo, Damage))
        };

        struct ShipLighting
        {
            Microsoft::Xna::Framework::Vector4 Ambient;
            Microsoft::Xna::Framework::Vector4 DirectionalDirection;
            Microsoft::Xna::Framework::Vector4 DirectionalColor;
            Microsoft::Xna::Framework::Vector4 PointPosition;
            Microsoft::Xna::Framework::Vector4 PointColor;
            float PointFactor;
            SHARP_XML_SERIALIZABLE(ShipLighting, "ShipLighting",
                                   SHARP_XML_M(ShipLighting, Ambient),
                                   SHARP_XML_M(ShipLighting, DirectionalDirection),
                                   SHARP_XML_M(ShipLighting, DirectionalColor),
                                   SHARP_XML_M(ShipLighting, PointPosition),
                                   SHARP_XML_M(ShipLighting, PointColor),
                                   SHARP_XML_M(ShipLighting, PointFactor))
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
        std::vector<PlayerShipInfo> Ships{
            {{-300.0f, 0.0f}, 90.0},
            {{300.0f, 0.0f}, 90.0},
        };

        std::vector<WeaponInfo> Weapons{
            {0, 3.0, 5, 1, 0.0f, 1},
            {1000, 3.0, 4, 3, 0.0f, 1},
            {2000, 3.0, 3, 3, 0.0f, 1},
            {3000, 2.0, 1, 1, 1.0f, 5},
            {4000, 2.0, 3, 1, 0.0f, 5},
        };

        float CrossFadeSpeed = 0.2f;
        float OffsetSpeed = 0.1f;
        std::vector<ShipLighting> ShipLights{
            {{1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 0.0f},
             {0.4f, 0.4f, 0.8f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f},
             {0.8f, 0.6f, 0.0f, 1.0f}, 0.01f},
            {{0.2f, 0.2f, 0.2f, 1.0f}, {1.0f, 1.0f, 1.0f, 0.0f},
             {0.4f, 0.4f, 0.8f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f},
             {0.8f, 0.6f, 0.0f, 1.0f}, 0.008f},
        };

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

        SHARP_XML_SERIALIZABLE(Settings, "Settings",
                               SHARP_XML_M(Settings, MediaPath),
                               SHARP_XML_M(Settings, WindowTitle),
                               SHARP_XML_M(Settings, LevelTime),
                               SHARP_XML_M(Settings, ThrustPower),
                               SHARP_XML_M(Settings, FrictionFactor),
                               SHARP_XML_M(Settings, MaxSpeed),
                               SHARP_XML_M(Settings, ShipRecoveryTime),
                               SHARP_XML_M(Settings, SunPosition),
                               SHARP_XML_M(Settings, GravityStrength),
                               SHARP_XML_M(Settings, GravityPower),
                               SHARP_XML_M(Settings, ColorDistribution),
                               SHARP_XML_M(Settings, Fade),
                               SHARP_XML_M(Settings, FlameSpeed),
                               SHARP_XML_M(Settings, Spread),
                               SHARP_XML_M(Settings, Flamability),
                               SHARP_XML_M(Settings, Size),
                               SHARP_XML_M(Settings, AsteroidScale),
                               SHARP_XML_M(Settings, BulletScale),
                               SHARP_XML_M(Settings, ShipScale),
                               SHARP_XML_M(Settings, Ships),
                               SHARP_XML_M(Settings, Weapons),
                               SHARP_XML_M(Settings, CrossFadeSpeed),
                               SHARP_XML_M(Settings, OffsetSpeed),
                               SHARP_XML_M(Settings, ShipLights),
                               SHARP_XML_M(Settings, Player1Start),
                               SHARP_XML_M(Settings, Player1Back),
                               SHARP_XML_M(Settings, Player1A),
                               SHARP_XML_M(Settings, Player1B),
                               SHARP_XML_M(Settings, Player1X),
                               SHARP_XML_M(Settings, Player1Y),
                               SHARP_XML_M(Settings, Player1ThumbstickLeftXmin),
                               SHARP_XML_M(Settings, Player1ThumbstickLeftXmax),
                               SHARP_XML_M(Settings, Player1ThumbstickLeftYmin),
                               SHARP_XML_M(Settings, Player1ThumbstickLeftYmax),
                               SHARP_XML_M(Settings, Player1Left),
                               SHARP_XML_M(Settings, Player1Right),
                               SHARP_XML_M(Settings, Player1Down),
                               SHARP_XML_M(Settings, Player1Up),
                               SHARP_XML_M(Settings, Player1LeftTrigger),
                               SHARP_XML_M(Settings, Player1RightTrigger),
                               SHARP_XML_M(Settings, Player2Start),
                               SHARP_XML_M(Settings, Player2Back),
                               SHARP_XML_M(Settings, Player2A),
                               SHARP_XML_M(Settings, Player2B),
                               SHARP_XML_M(Settings, Player2X),
                               SHARP_XML_M(Settings, Player2Y),
                               SHARP_XML_M(Settings, Player2ThumbstickLeftXmin),
                               SHARP_XML_M(Settings, Player2ThumbstickLeftXmax),
                               SHARP_XML_M(Settings, Player2ThumbstickLeftYmin),
                               SHARP_XML_M(Settings, Player2ThumbstickLeftYmax),
                               SHARP_XML_M(Settings, Player2Left),
                               SHARP_XML_M(Settings, Player2Right),
                               SHARP_XML_M(Settings, Player2Down),
                               SHARP_XML_M(Settings, Player2Up),
                               SHARP_XML_M(Settings, Player2LeftTrigger),
                               SHARP_XML_M(Settings, Player2RightTrigger))

        void Save(const std::string& filename) const;
        [[nodiscard]] static Settings Load(const std::string& filename);
    };
}
