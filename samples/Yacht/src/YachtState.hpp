#pragma once

// YachtState.hpp -- C++ port of Yacht/YachtState.cs.

#include <memory>
#include <stdexcept>
#include <string>

#include "System/Xml/Serialization/IXmlSerializable.hpp"
#include "System/Xml/XmlNodeType.hpp"
#include "System/Xml/XmlReader.hpp"
#include "System/Xml/XmlWriter.hpp"

#include "Misc/NetworkManager.hpp"
#include "Objects/DiceState.hpp"
#include "YachtServices/DataModel.hpp"

namespace Yacht {

/**
 * @brief Everything the game has to remember when it is moved to the background.
 *
 * Three parts, any of which may be absent: the game itself, this player's dice, and how to find
 * the server again. A game that was not in progress writes none of them, which is why the
 * reader looks at whatever sub-elements it finds rather than expecting a fixed shape.
 */
class YachtState : public System::Object,
                   public System::Xml::Serialization::IXmlSerializable {
public:
    /** @brief The state of the ongoing game. Can be null if no game is in progress. */
    std::shared_ptr<YachtServices::GameState> YachGameState;

    /**
     * @brief The state of the player's dice.
     *
     * Can be null if no game is in progress, or the player does not manage his own dice.
     */
    std::shared_ptr<DiceState> PlayerDiceState;

    /** @brief Network related state information for contacting the game server. */
    NetworkManager* NetworkManagerState = nullptr;

    /** @brief The type's name. @return "YachtState". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "YachtState";
        return name;
    }

    /**
     * @brief Not implemented.
     *
     * @return Never returns.
     * @throws std::logic_error always, as the original throws NotImplementedException.
     */
    [[nodiscard]] System::Xml::Schema::XmlSchema* GetSchema() const override
    {
        throw std::logic_error("YachtState::GetSchema is not implemented.");
    }

    /**
     * @brief Generates a Yacht state from its XML representation.
     *
     * @param reader The reader from which the object is deserialized.
     *
     * @note This updates the singleton NetworkManager with the data read, rather than building
     * a second one: there is only ever one connection, and what is stored is its identity.
     */
    void ReadXml(System::Xml::XmlReader& reader) override
    {
        // Read the start element
        reader.Read();

        YachGameState = nullptr;
        PlayerDiceState = nullptr;
        NetworkManagerState = NetworkManager::getInstanceProperty();

        // Read the start element of the first sub-element
        reader.Read();

        // Read the yacht state's sub-elements
        while (reader.IsStartElement()) {
            if (reader.getLocalNameProperty() == "GameState") {
                YachGameState = std::make_shared<YachtServices::GameState>();
                YachGameState->Deserialize(reader);
            } else if (reader.getLocalNameProperty() == "DiceState") {
                PlayerDiceState = std::make_shared<DiceState>();
                PlayerDiceState->Deserialize(reader);
            } else if (reader.getLocalNameProperty() == "NetworkManager") {
                if (NetworkManagerState != nullptr) {
                    NetworkManagerState->Deserialize(reader);
                }
            }

            // Read the next sub-element start element
            reader.Read();
        }

        // Read the end element
        reader.Read();
    }

    /**
     * @brief Converts a Yacht state into its XML representation.
     *
     * @param writer The writer into which the object is serialized.
     */
    void WriteXml(System::Xml::XmlWriter& writer) const override
    {
        // Write the start element
        writer.WriteStartElement("YachtState");

        // Serialize the available yacht state sub-elements
        if (YachGameState != nullptr) {
            YachGameState->WriteXml(writer);
        }
        if (PlayerDiceState != nullptr) {
            PlayerDiceState->WriteXml(writer);
        }
        if (NetworkManagerState != nullptr) {
            NetworkManagerState->WriteXml(writer);
        }

        // Write the end element
        writer.WriteEndElement();
    }
};

} // namespace Yacht
