// SPDX-License-Identifier: MS-PL
#pragma once

// EntityList.hpp — C++ port of ShipGame/EntityList.cs (XNA 4.0 Ship Game
// Starter Kit). A named list of transform matrices, loaded from and saved to
// the same XML documents the original ships.

#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/Collections/Generic/List.hpp"
#include "System/IO/File.hpp"
#include "System/IO/FileNotFoundException.hpp"
#include "System/IO/FileStream.hpp"
#include "System/Random.hpp"
#include "System/String.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Matrix;

// Port of ShipGame/EntityList.cs.
struct Entity {
    System::String name;       // entity name
    Matrix transform;          // entity transform matrix

    Entity() = default;

    // Create a new entity with given name and transform matrix
    Entity(System::String entityName, Matrix entityTransform)
        : name(std::move(entityName)), transform(entityTransform) {}

    SHARP_XML_SERIALIZABLE(Entity, "Entity",
                           SHARP_XML_M(Entity, name),
                           SHARP_XML_M(Entity, transform))
};

class EntityList {
public:
    // entities list
    System::Collections::Generic::List<Entity> entities;

    // Get the entity transform matrix
    Matrix GetTransform(const System::String& name) {
        for (const Entity& e : entities) {
            if (e.name == name) {
                return e.transform;
            }
        }

        return Matrix::Identity;
    }

    // Get a random transform matrix from the list preventing repetiton
    Matrix GetTransformRandom(System::Random& random) {
        // if no itens return indentity
        if (entities.getCountProperty() == 0)
            return Matrix::Identity;

        // if only one item available return it
        if (entities.getCountProperty() == 1)
            return entities[0].transform;

        // pick a random item different from the last one
        int rnd;
        do {
            rnd = random.Next(entities.getCountProperty());
        } while (rnd == lastRandom);

        // set new last random number
        lastRandom = rnd;

        // return transform for random pick
        return entities[rnd].transform;
    }

    // Get the list of entities
    System::Collections::Generic::List<Entity>& Entities() { return entities; }

    // Save the list to a xml file
    bool Save(const System::String& filename) {
        // open stream
        System::IO::FileStream stream = System::IO::File::Create(filename);

        // serialize
        System::Xml::Serialization::XmlSerializer<EntityList> serializer;
        serializer.Serialize(stream, *this);

        // close
        stream.Close();

        return true;
    }

    // Static function to load a entity list from a xml file
    static std::shared_ptr<EntityList> Load(const System::String& filename) {
        // open file
        std::unique_ptr<System::IO::FileStream> stream;
        try {
            stream = std::make_unique<System::IO::FileStream>(System::IO::File::OpenRead(filename));
        } catch (const System::IO::FileNotFoundException& e) {
            System::Console::WriteLine(System::String("EntityList load error:") + e.getMessageProperty());
            stream = nullptr;
        }
        if (stream == nullptr)
            return nullptr;

        // serialize
        System::Xml::Serialization::XmlSerializer<EntityList> serializer;
        auto entityList = std::make_shared<EntityList>(serializer.Deserialize(*stream));

        // close
        stream->Close();
        stream = nullptr;

        return entityList;
    }

    SHARP_XML_SERIALIZABLE(EntityList, "EntityList",
                           SHARP_XML_M(EntityList, entities))

private:
    // last random number generated (to prevent repetition)
    int lastRandom = -1;
};

} // namespace ShipGame
