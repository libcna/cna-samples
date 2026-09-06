// SPDX-License-Identifier: MS-PL
#pragma once

// LightList.hpp — C++ port of ShipGame/Graphics/LightList.cs (XNA 4.0 Ship Game
// Starter Kit). The ambient colour and point lights a level or screen ships,
// loaded from the same XML documents the original does.

#include <memory>
#include <vector>
#include <string>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/XmlSerializationEXT.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "System/Console.hpp"
#include "System/IO/File.hpp"
#include "System/IO/FileNotFoundException.hpp"
#include "System/IO/FileStream.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Vector4;
using Microsoft::Xna::Framework::Graphics::EffectParameter;

// Port of the Light struct in ShipGame/Graphics/LightList.cs.
struct Light {
    Vector3 position;    // position
    float radius = 0.0f; // radius
    Vector3 color;       // color

    Light() = default;

    // Create a new list of lights
    Light(Vector3 lightPosition, float lightRadius, Vector3 lightColor)
        : position(lightPosition), radius(lightRadius), color(lightColor) {}

    // Set light properties to given effect
    void SetEffect(EffectParameter* effectLightPosition, EffectParameter* effectLightColor,
                   Matrix worldInverse) const {
        Vector4 positionRadius(Vector3::Transform(position, worldInverse), radius);
        if (effectLightPosition != nullptr) {
            effectLightPosition->SetValue(positionRadius);
        }
        if (effectLightColor != nullptr) {
            effectLightColor->SetValue(color);
        }
    }

    SHARP_XML_SERIALIZABLE(Light, "Light",
                           SHARP_XML_M(Light, position),
                           SHARP_XML_M(Light, radius),
                           SHARP_XML_M(Light, color))
};

// Port of ShipGame/Graphics/LightList.cs.
class LightList {
public:
    // ambient light
    Vector3 ambient{0.3f, 0.3f, 0.3f};

    // list of lights
    std::vector<Light> lights;

    // Saves the light list to a xml file
    bool Save(const std::string& filename) {
        // create stream
        System::IO::FileStream stream = System::IO::File::Create(filename);

        // serialize
        System::Xml::Serialization::XmlSerializer<LightList> serializer;
        const std::string document = serializer.Serialize(*this);
        stream.Write(reinterpret_cast<const SharpRuntime::bytecs*>(document.data()), 0,
                     static_cast<SharpRuntime::intcs>(document.size()));

        // close
        stream.Close();

        return true;
    }

    // Static method to load a light list from a file
    static std::shared_ptr<LightList> Load(const std::string& filename) {
        // open file. The stream is a scope in C++ rather than a nullable handle, so the
        // read happens inside the same try the original opens the file in; the log line and
        // the null return on a missing file are the original's.
        try {
            System::IO::FileStream stream = System::IO::File::OpenRead(filename);

            // serialize
            System::Xml::Serialization::XmlSerializer<LightList> serializer;
            auto environmentLights = std::make_shared<LightList>(serializer.Deserialize(stream));

            // close
            stream.Close();

            return environmentLights;
        } catch (const System::IO::FileNotFoundException& e) {
            System::Console::WriteLine(std::string("LightList load error:") + e.getMessageProperty());
            return nullptr;
        }
    }

    SHARP_XML_SERIALIZABLE(LightList, "LightList",
                           SHARP_XML_M(LightList, ambient),
                           SHARP_XML_M(LightList, lights))
};

} // namespace ShipGame
