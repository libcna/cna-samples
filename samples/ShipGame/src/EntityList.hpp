// SPDX-License-Identifier: MS-PL
#pragma once

// EntityList.hpp — C++ port of ShipGame/EntityList.cs (XNA 4.0 Ship Game
// Starter Kit). A named list of transform matrices, loaded from and saved to
// the same XML documents the original ships.

#include <memory>
#include <vector>
#include <string>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/XmlSerializationEXT.hpp"
#include "System/Console.hpp"
#include "System/IO/File.hpp"
#include "System/IO/FileNotFoundException.hpp"
#include "System/IO/FileStream.hpp"
#include "System/Random.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Matrix;

// Port of ShipGame/EntityList.cs.
struct Entity {
    std::string name;       // entity name
    Matrix transform;          // entity transform matrix

    Entity() = default;

    // Create a new entity with given name and transform matrix
    Entity(std::string entityName, Matrix entityTransform)
        : name(std::move(entityName)), transform(entityTransform) {}

    SHARP_XML_SERIALIZABLE(Entity, "Entity",
                           SHARP_XML_M(Entity, name),
                           SHARP_XML_M(Entity, transform))
};

class EntityList {
public:
    // entities list
    std::vector<Entity> entities;

    // Get the entity transform matrix
    Matrix GetTransform(const std::string& name) {
        for (const Entity& e : entities) {
            if (e.name == name) {
                return e.transform;
            }
        }

        return Matrix::getIdentityProperty();
    }

    // Get a random transform matrix from the list preventing repetiton
    Matrix GetTransformRandom(System::Random& random) {
        // if no itens return indentity
        if ((int)entities.size() == 0)
            return Matrix::getIdentityProperty();

        // if only one item available return it
        if ((int)entities.size() == 1)
            return entities[0].transform;

        // pick a random item different from the last one
        int rnd;
        do {
            rnd = random.Next((int)entities.size());
        } while (rnd == lastRandom);

        // set new last random number
        lastRandom = rnd;

        // return transform for random pick
        return entities[rnd].transform;
    }

    // Get the list of entities
    std::vector<Entity>& Entities() { return entities; }

    // Save the list to a xml file
    bool Save(const std::string& filename) {
        // open stream
        System::IO::FileStream stream = System::IO::File::Create(filename);

        // serialize
        System::Xml::Serialization::XmlSerializer<EntityList> serializer;
        const std::string document = serializer.Serialize(*this);
        stream.Write(reinterpret_cast<const SharpRuntime::bytecs*>(document.data()), 0,
                     static_cast<SharpRuntime::intcs>(document.size()));

        // close
        stream.Close();

        return true;
    }

    // Static function to load a entity list from a xml file
    static std::shared_ptr<EntityList> Load(const std::string& filename) {
        // open file. The stream is a scope in C++ rather than a nullable handle, so the
        // read happens inside the same try the original opens the file in; the log line and
        // the null return on a missing file are the original's.
        try {
            System::IO::FileStream stream = System::IO::File::OpenRead(filename);

            // serialize
            System::Xml::Serialization::XmlSerializer<EntityList> serializer;
            auto entityList = std::make_shared<EntityList>(serializer.Deserialize(stream));

            // close
            stream.Close();

            return entityList;
        } catch (const System::IO::FileNotFoundException& e) {
            System::Console::WriteLine(std::string("EntityList load error:") + e.getMessageProperty());
            return nullptr;
        }
    }

    SHARP_XML_SERIALIZABLE(EntityList, "EntityList",
                           SHARP_XML_M(EntityList, entities))

private:
    // last random number generated (to prevent repetition)
    int lastRandom = -1;
};

} // namespace ShipGame
