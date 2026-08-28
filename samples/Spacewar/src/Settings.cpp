// SPDX-License-Identifier: MS-PL

#include "Settings.hpp"

#include "System/Int32.hpp"
#include "System/Single.hpp"
#include "System/Double.hpp"
#include "System/Xml/Linq/XDocument.hpp"
#include "System/Xml/Linq/XElement.hpp"

#include <stdexcept>
#include <unordered_map>

namespace Spacewar
{
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector4;
    using Microsoft::Xna::Framework::Input::Keys;
    using System::Xml::Linq::XDocument;
    using System::Xml::Linq::XElement;

    namespace
    {
        std::shared_ptr<XElement> Required(const std::shared_ptr<XElement>& parent,
                                           const std::string& name)
        {
            auto child = parent->Element(name);
            if (!child)
                throw std::runtime_error("Missing Settings XML element: " + name);
            return child;
        }

        std::string Text(const std::shared_ptr<XElement>& parent, const std::string& name)
        {
            return Required(parent, name)->getValueProperty();
        }

        float Float(const std::shared_ptr<XElement>& parent, const std::string& name)
        {
            return System::Single::Parse(Text(parent, name));
        }

        double Double(const std::shared_ptr<XElement>& parent, const std::string& name)
        {
            return System::Double::Parse(Text(parent, name));
        }

        int Int(const std::shared_ptr<XElement>& parent, const std::string& name)
        {
            return System::Int32::Parse(Text(parent, name));
        }

        Vector2 ReadVector2(const std::shared_ptr<XElement>& parent, const std::string& name)
        {
            const auto element = Required(parent, name);
            return {Float(element, "X"), Float(element, "Y")};
        }

        Vector4 ReadVector4(const std::shared_ptr<XElement>& parent, const std::string& name)
        {
            const auto element = Required(parent, name);
            const auto w = element->Element("W");
            return {Float(element, "X"), Float(element, "Y"), Float(element, "Z"),
                    w ? System::Single::Parse(w->getValueProperty()) : 0.0f};
        }

        Keys ReadKey(const std::string& value)
        {
            static const std::unordered_map<std::string, Keys> keys{
                {"LeftControl", Keys::LeftControl}, {"RightControl", Keys::RightControl},
                {"LeftShift", Keys::LeftShift}, {"RightShift", Keys::RightShift},
                {"Home", Keys::Home}, {"End", Keys::End}, {"PageUp", Keys::PageUp},
                {"PageDown", Keys::PageDown}, {"Left", Keys::Left}, {"Right", Keys::Right},
                {"Up", Keys::Up}, {"Down", Keys::Down}, {"Insert", Keys::Insert},
                {"Delete", Keys::Delete}, {"A", Keys::A}, {"D", Keys::D}, {"E", Keys::E},
                {"F", Keys::F}, {"G", Keys::G}, {"Q", Keys::Q}, {"S", Keys::S},
                {"T", Keys::T}, {"V", Keys::V}, {"W", Keys::W},
            };
            const auto found = keys.find(value);
            if (found == keys.end())
                throw std::runtime_error("Unsupported Settings key value: " + value);
            return found->second;
        }

        const char* KeyName(Keys key)
        {
            switch (key)
            {
                case Keys::LeftControl: return "LeftControl";
                case Keys::RightControl: return "RightControl";
                case Keys::LeftShift: return "LeftShift";
                case Keys::RightShift: return "RightShift";
                case Keys::Home: return "Home";
                case Keys::End: return "End";
                case Keys::PageUp: return "PageUp";
                case Keys::PageDown: return "PageDown";
                case Keys::Left: return "Left";
                case Keys::Right: return "Right";
                case Keys::Up: return "Up";
                case Keys::Down: return "Down";
                case Keys::Insert: return "Insert";
                case Keys::Delete: return "Delete";
                case Keys::A: return "A";
                case Keys::D: return "D";
                case Keys::E: return "E";
                case Keys::F: return "F";
                case Keys::G: return "G";
                case Keys::Q: return "Q";
                case Keys::S: return "S";
                case Keys::T: return "T";
                case Keys::V: return "V";
                case Keys::W: return "W";
                default: throw std::runtime_error("Unsupported Settings key enum value");
            }
        }

        void Add(const std::shared_ptr<XElement>& parent, const std::string& name,
                 const std::string& value)
        {
            parent->Add(std::make_shared<XElement>(name, value));
        }

        void Add(const std::shared_ptr<XElement>& parent, const std::string& name, float value)
        {
            Add(parent, name, System::Single::ToString(value));
        }

        void Add(const std::shared_ptr<XElement>& parent, const std::string& name, double value)
        {
            Add(parent, name, System::Double::ToString(value));
        }

        void Add(const std::shared_ptr<XElement>& parent, const std::string& name, int value)
        {
            Add(parent, name, System::Int32::ToString(value));
        }

        void AddVector2(const std::shared_ptr<XElement>& parent, const std::string& name,
                        const Vector2& value)
        {
            auto element = std::make_shared<XElement>(name);
            Add(element, "X", value.X);
            Add(element, "Y", value.Y);
            parent->Add(element);
        }

        void AddVector4(const std::shared_ptr<XElement>& parent, const std::string& name,
                        const Vector4& value)
        {
            auto element = std::make_shared<XElement>(name);
            Add(element, "X", value.X);
            Add(element, "Y", value.Y);
            Add(element, "Z", value.Z);
            Add(element, "W", value.W);
            parent->Add(element);
        }
    }

    Settings Settings::Load(const std::string& filename)
    {
        const auto document = XDocument::Load(filename);
        const auto root = document->getRootProperty();
        if (!root || root->getNameProperty().getLocalNameProperty() != "Settings")
            throw std::runtime_error("The settings document has no Settings root element.");

        Settings result;
        result.MediaPath = Text(root, "MediaPath");
        for (char& character : result.MediaPath)
            if (character == '\\') character = '/';
        result.WindowTitle = Text(root, "WindowTitle");
        result.LevelTime = Int(root, "LevelTime");
        result.ThrustPower = Float(root, "ThrustPower");
        result.FrictionFactor = Float(root, "FrictionFactor");
        result.MaxSpeed = Float(root, "MaxSpeed");
        result.ShipRecoveryTime = Float(root, "ShipRecoveryTime");
        result.SunPosition = ReadVector2(root, "SunPosition");
        result.GravityStrength = Double(root, "GravityStrength");
        result.GravityPower = Int(root, "GravityPower");
        result.ColorDistribution = Float(root, "ColorDistribution");
        result.Fade = Float(root, "Fade");
        result.FlameSpeed = Float(root, "FlameSpeed");
        result.Spread = Float(root, "Spread");
        result.Flamability = Float(root, "Flamability");
        result.Size = Float(root, "Size");
        result.AsteroidScale = Float(root, "AsteroidScale");
        result.BulletScale = Float(root, "BulletScale");
        result.ShipScale = Float(root, "ShipScale");

        const auto ships = Required(root, "Ships")->Elements("PlayerShipInfo");
        if (ships.size() != result.Ships.size())
            throw std::runtime_error("Settings.Ships must contain exactly two entries.");
        for (std::size_t i = 0; i < ships.size(); ++i)
        {
            result.Ships[i].StartPosition = ReadVector2(ships[i], "StartPosition");
            result.Ships[i].StartAngle = Double(ships[i], "StartAngle");
        }

        const auto weapons = Required(root, "Weapons")->Elements("WeaponInfo");
        if (weapons.size() != result.Weapons.size())
            throw std::runtime_error("Settings.Weapons must contain exactly five entries.");
        for (std::size_t i = 0; i < weapons.size(); ++i)
        {
            result.Weapons[i] = {Int(weapons[i], "Cost"), Double(weapons[i], "Lifetime"),
                                 Int(weapons[i], "Max"), Int(weapons[i], "Burst"),
                                 Float(weapons[i], "Acceleration"), Int(weapons[i], "Damage")};
        }

        result.CrossFadeSpeed = Float(root, "CrossFadeSpeed");
        result.OffsetSpeed = Float(root, "OffsetSpeed");
        const auto lights = Required(root, "ShipLights")->Elements("ShipLighting");
        if (lights.size() != result.ShipLights.size())
            throw std::runtime_error("Settings.ShipLights must contain exactly two entries.");
        for (std::size_t i = 0; i < lights.size(); ++i)
        {
            result.ShipLights[i] = {
                ReadVector4(lights[i], "Ambient"),
                ReadVector4(lights[i], "DirectionalDirection"),
                ReadVector4(lights[i], "DirectionalColor"),
                ReadVector4(lights[i], "PointPosition"),
                ReadVector4(lights[i], "PointColor"),
                Float(lights[i], "PointFactor"),
            };
        }

#define SPACEWAR_READ_KEY(name) result.name = ReadKey(Text(root, #name))
        SPACEWAR_READ_KEY(Player1Start); SPACEWAR_READ_KEY(Player1Back);
        SPACEWAR_READ_KEY(Player1A); SPACEWAR_READ_KEY(Player1B);
        SPACEWAR_READ_KEY(Player1X); SPACEWAR_READ_KEY(Player1Y);
        SPACEWAR_READ_KEY(Player1ThumbstickLeftXmin); SPACEWAR_READ_KEY(Player1ThumbstickLeftXmax);
        SPACEWAR_READ_KEY(Player1ThumbstickLeftYmin); SPACEWAR_READ_KEY(Player1ThumbstickLeftYmax);
        SPACEWAR_READ_KEY(Player1Left); SPACEWAR_READ_KEY(Player1Right);
        SPACEWAR_READ_KEY(Player1Down); SPACEWAR_READ_KEY(Player1Up);
        SPACEWAR_READ_KEY(Player1LeftTrigger); SPACEWAR_READ_KEY(Player1RightTrigger);
        SPACEWAR_READ_KEY(Player2Start); SPACEWAR_READ_KEY(Player2Back);
        SPACEWAR_READ_KEY(Player2A); SPACEWAR_READ_KEY(Player2B);
        SPACEWAR_READ_KEY(Player2X); SPACEWAR_READ_KEY(Player2Y);
        SPACEWAR_READ_KEY(Player2ThumbstickLeftXmin); SPACEWAR_READ_KEY(Player2ThumbstickLeftXmax);
        SPACEWAR_READ_KEY(Player2ThumbstickLeftYmin); SPACEWAR_READ_KEY(Player2ThumbstickLeftYmax);
        SPACEWAR_READ_KEY(Player2Left); SPACEWAR_READ_KEY(Player2Right);
        SPACEWAR_READ_KEY(Player2Down); SPACEWAR_READ_KEY(Player2Up);
        SPACEWAR_READ_KEY(Player2LeftTrigger); SPACEWAR_READ_KEY(Player2RightTrigger);
#undef SPACEWAR_READ_KEY
        return result;
    }

    void Settings::Save(const std::string& filename) const
    {
        auto root = std::make_shared<XElement>("Settings");
        Add(root, "MediaPath", MediaPath);
        Add(root, "WindowTitle", WindowTitle);
        Add(root, "LevelTime", LevelTime);
        Add(root, "ThrustPower", ThrustPower);
        Add(root, "FrictionFactor", FrictionFactor);
        Add(root, "MaxSpeed", MaxSpeed);
        Add(root, "ShipRecoveryTime", ShipRecoveryTime);
        AddVector2(root, "SunPosition", SunPosition);
        Add(root, "GravityStrength", GravityStrength);
        Add(root, "GravityPower", GravityPower);
        Add(root, "ColorDistribution", ColorDistribution);
        Add(root, "Fade", Fade);
        Add(root, "FlameSpeed", FlameSpeed);
        Add(root, "Spread", Spread);
        Add(root, "Flamability", Flamability);
        Add(root, "Size", Size);
        Add(root, "AsteroidScale", AsteroidScale);
        Add(root, "BulletScale", BulletScale);
        Add(root, "ShipScale", ShipScale);

        auto ships = std::make_shared<XElement>("Ships");
        for (const auto& ship : Ships)
        {
            auto entry = std::make_shared<XElement>("PlayerShipInfo");
            AddVector2(entry, "StartPosition", ship.StartPosition);
            Add(entry, "StartAngle", ship.StartAngle);
            ships->Add(entry);
        }
        root->Add(ships);

        auto weapons = std::make_shared<XElement>("Weapons");
        for (const auto& weapon : Weapons)
        {
            auto entry = std::make_shared<XElement>("WeaponInfo");
            Add(entry, "Cost", weapon.Cost); Add(entry, "Lifetime", weapon.Lifetime);
            Add(entry, "Max", weapon.Max); Add(entry, "Burst", weapon.Burst);
            Add(entry, "Acceleration", weapon.Acceleration); Add(entry, "Damage", weapon.Damage);
            weapons->Add(entry);
        }
        root->Add(weapons);

        Add(root, "CrossFadeSpeed", CrossFadeSpeed);
        Add(root, "OffsetSpeed", OffsetSpeed);
        auto lights = std::make_shared<XElement>("ShipLights");
        for (const auto& light : ShipLights)
        {
            auto entry = std::make_shared<XElement>("ShipLighting");
            AddVector4(entry, "Ambient", light.Ambient);
            AddVector4(entry, "DirectionalDirection", light.DirectionalDirection);
            AddVector4(entry, "DirectionalColor", light.DirectionalColor);
            AddVector4(entry, "PointPosition", light.PointPosition);
            AddVector4(entry, "PointColor", light.PointColor);
            Add(entry, "PointFactor", light.PointFactor);
            lights->Add(entry);
        }
        root->Add(lights);

#define SPACEWAR_WRITE_KEY(name) Add(root, #name, KeyName(name))
        SPACEWAR_WRITE_KEY(Player1Start); SPACEWAR_WRITE_KEY(Player1Back);
        SPACEWAR_WRITE_KEY(Player1A); SPACEWAR_WRITE_KEY(Player1B);
        SPACEWAR_WRITE_KEY(Player1X); SPACEWAR_WRITE_KEY(Player1Y);
        SPACEWAR_WRITE_KEY(Player1ThumbstickLeftXmin); SPACEWAR_WRITE_KEY(Player1ThumbstickLeftXmax);
        SPACEWAR_WRITE_KEY(Player1ThumbstickLeftYmin); SPACEWAR_WRITE_KEY(Player1ThumbstickLeftYmax);
        SPACEWAR_WRITE_KEY(Player1Left); SPACEWAR_WRITE_KEY(Player1Right);
        SPACEWAR_WRITE_KEY(Player1Down); SPACEWAR_WRITE_KEY(Player1Up);
        SPACEWAR_WRITE_KEY(Player1LeftTrigger); SPACEWAR_WRITE_KEY(Player1RightTrigger);
        SPACEWAR_WRITE_KEY(Player2Start); SPACEWAR_WRITE_KEY(Player2Back);
        SPACEWAR_WRITE_KEY(Player2A); SPACEWAR_WRITE_KEY(Player2B);
        SPACEWAR_WRITE_KEY(Player2X); SPACEWAR_WRITE_KEY(Player2Y);
        SPACEWAR_WRITE_KEY(Player2ThumbstickLeftXmin); SPACEWAR_WRITE_KEY(Player2ThumbstickLeftXmax);
        SPACEWAR_WRITE_KEY(Player2ThumbstickLeftYmin); SPACEWAR_WRITE_KEY(Player2ThumbstickLeftYmax);
        SPACEWAR_WRITE_KEY(Player2Left); SPACEWAR_WRITE_KEY(Player2Right);
        SPACEWAR_WRITE_KEY(Player2Down); SPACEWAR_WRITE_KEY(Player2Up);
        SPACEWAR_WRITE_KEY(Player2LeftTrigger); SPACEWAR_WRITE_KEY(Player2RightTrigger);
#undef SPACEWAR_WRITE_KEY

        XDocument document(root);
        document.Save(filename);
    }
}
