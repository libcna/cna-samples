#pragma once

// DiceState.hpp -- C++ port of Yacht/Objects/DiceState.cs.

#include <array>
#include <memory>
#include <stdexcept>
#include <string>

#include "System/Xml/Serialization/IXmlSerializable.hpp"
#include "System/Xml/XmlReader.hpp"
#include "System/Xml/XmlWriter.hpp"

#include "Dice.hpp"

namespace Yacht {

/**
 * @brief The state of a player's dice.
 *
 * Persisted when the game is moved to the background, so a player who takes a call mid-turn
 * comes back to the same dice, with the same rolls already spent.
 *
 * @note A slot may be empty: `RollingDice` and `HoldingDice` are the two halves of one set of
 * five, and a die is in exactly one of them, so each array has holes. The wire format writes an
 * empty string for a hole, which is why the attributes are read as strings rather than numbers.
 */
class DiceState : public System::Xml::Serialization::IXmlSerializable {
public:
    /** @brief Amount of dice the player has. */
    static constexpr int DieAmount = 5;

    /** @brief How many times the dice have been rolled. */
    int Rolls = 0;

    /** @brief The number of the turn for which the dice state is valid. */
    int ValidForTurn = 0;

    /** @brief Dice that the player is rolling. */
    std::array<std::shared_ptr<Dice>, DieAmount> RollingDice{};

    /** @brief Dice that the player is holding. */
    std::array<std::shared_ptr<Dice>, DieAmount> HoldingDice{};

    /**
     * @brief Not implemented.
     *
     * @return Never returns.
     * @throws std::logic_error always, as the original throws NotImplementedException.
     */
    [[nodiscard]] System::Xml::Schema::XmlSchema* GetSchema() const override
    {
        throw std::logic_error("DiceState::GetSchema is not implemented.");
    }

    /**
     * @brief Reads a serialized DiceState object.
     *
     * @param reader The xml reader from which to read.
     */
    void ReadXml(System::Xml::XmlReader& reader) override
    {
        // Read the start element
        reader.Read();

        Deserialize(reader);
    }

    /**
     * @brief Serializes the DiceState object.
     *
     * @param writer The xml writer to use when writing.
     */
    void WriteXml(System::Xml::XmlWriter& writer) const override
    {
        writer.WriteStartElement("DiceState");

        writer.WriteAttributeString("Rolls", std::to_string(Rolls));
        writer.WriteAttributeString("ValidForTurn", std::to_string(ValidForTurn));

        // Serialize rolling dice
        writer.WriteStartElement("RollingDice");

        for (int i = 0; i < DieAmount; i++) {
            writer.WriteAttributeString(
                "D" + std::to_string(i),
                RollingDice[static_cast<std::size_t>(i)] == nullptr
                    ? ""
                    : std::to_string(static_cast<int>(
                          RollingDice[static_cast<std::size_t>(i)]->getValueProperty())));
        }

        writer.WriteEndElement();

        // Serialize held dice
        writer.WriteStartElement("HoldingDice");

        for (int i = 0; i < DieAmount; i++) {
            writer.WriteAttributeString(
                "D" + std::to_string(i),
                HoldingDice[static_cast<std::size_t>(i)] == nullptr
                    ? ""
                    : std::to_string(static_cast<int>(
                          HoldingDice[static_cast<std::size_t>(i)]->getValueProperty())));
        }

        writer.WriteEndElement();

        // End element for the "DiceState" element
        writer.WriteEndElement();
    }

    /**
     * @brief Reads a serialized DiceState object.
     *
     * Unlike ReadXml, the reader is assumed to have already read the node which contains the
     * start element. This moves the reader past the representation's end element.
     *
     * @param reader The xml reader from which to read.
     */
    void Deserialize(System::Xml::XmlReader& reader)
    {
        // Get the amount of rolls performed
        Rolls = std::stoi(reader.GetAttribute("Rolls"));

        // Get the turn for which the state is valid
        ValidForTurn = std::stoi(reader.GetAttribute("ValidForTurn"));

        // Get the rolling dice
        reader.Read();
        RollingDice = {};

        for (int i = 0; i < DieAmount; i++) {
            const std::string value = reader.GetAttribute("D" + std::to_string(i));
            if (!value.empty()) {
                RollingDice[static_cast<std::size_t>(i)] = std::make_shared<Dice>();
                RollingDice[static_cast<std::size_t>(i)]->setValueProperty(
                    static_cast<DiceValue>(std::stoi(value)));
            }
        }

        // The rolling dice element does not close explicitly

        // Get the held dice
        reader.Read();
        HoldingDice = {};

        for (int i = 0; i < DieAmount; i++) {
            const std::string value = reader.GetAttribute("D" + std::to_string(i));
            if (!value.empty()) {
                HoldingDice[static_cast<std::size_t>(i)] = std::make_shared<Dice>();
                HoldingDice[static_cast<std::size_t>(i)]->setValueProperty(
                    static_cast<DiceValue>(std::stoi(value)));
            }
        }

        // The held dice element does not close explicitly

        // Read end element
        reader.Read();
    }
};

} // namespace Yacht
