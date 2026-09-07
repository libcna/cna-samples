#pragma once

// DataModel.hpp -- C++ port of YachtServices/DataModel.cs.
//
// The model both products share. The server is authoritative over it and the client mirrors it,
// which is why it lives in the service's namespace rather than the game's even though the
// offline game -- which never talks to a server -- is built on it too.
//
// Every type here writes its own XML rather than letting a serializer derive one, so each
// implements System::Xml::Serialization::IXmlSerializable. Two of them do not follow .NET's own
// rule that WriteXml writes only the content of an element the caller opened: DiceState,
// GameState, Message and AvailableGames write their own wrapping element. That is what the
// original does, and both ends of this format are ported from it, so it is reproduced rather
// than corrected -- a "fixed" writer here would produce documents the original server rejects.

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/Guid.hpp"
#include "System/Xml/Serialization/IXmlSerializable.hpp"
#include "System/Xml/XmlReader.hpp"
#include "System/Xml/XmlWriter.hpp"

#include "ServiceConstants.hpp"

namespace YachtServices {

using System::Xml::Serialization::IXmlSerializable;
using System::Xml::XmlReader;
using System::Xml::XmlWriter;

/** @brief Possible game types. */
enum class GameTypes {
    /** @brief A game against local opponents, with no server involved. */
    Offline,
    /** @brief A game played against other people through the game server. */
    Online
};

/** @brief The name a GameTypes value carries in the wire format. */
[[nodiscard]] inline std::string ToString(GameTypes value)
{
    return value == GameTypes::Online ? "Online" : "Offline";
}

/** @brief The GameTypes value a wire-format name denotes. */
[[nodiscard]] inline GameTypes ParseGameTypes(const std::string& name)
{
    return name == "Online" ? GameTypes::Online : GameTypes::Offline;
}

/** @brief Possible message types sent by the game server. */
enum class MessageContentType {
    /** @brief The body is a GameState. */
    GameState = 1,
    /** @brief The body is a SimpleType. */
    SimpleType = 2,
    /** @brief The body is an AvailableGames. */
    AvailableGames = 3,
    /** @brief The body is an EndGameInformation. */
    EndGameInformation = 4
};

/** @brief The name a MessageContentType value carries in the wire format. */
[[nodiscard]] inline std::string ToString(MessageContentType value)
{
    switch (value) {
    case MessageContentType::GameState: return "GameState";
    case MessageContentType::SimpleType: return "SimpleType";
    case MessageContentType::AvailableGames: return "AvailableGames";
    case MessageContentType::EndGameInformation: return "EndGameInformation";
    }
    return "GameState";
}

/** @brief The MessageContentType value a wire-format name denotes. */
[[nodiscard]] inline MessageContentType ParseMessageContentType(const std::string& name)
{
    if (name == "SimpleType") return MessageContentType::SimpleType;
    if (name == "AvailableGames") return MessageContentType::AvailableGames;
    if (name == "EndGameInformation") return MessageContentType::EndGameInformation;
    return MessageContentType::GameState;
}

/**
 * @brief A simple type message containing a name and a value.
 */
class SimpleType : public IXmlSerializable {
public:
    /** @brief Message's value. */
    std::string Value;

    /** @brief Message's type. */
    std::string Name;

    /**
     * @brief Not implemented.
     *
     * @return Never returns.
     * @throws std::logic_error always, as the original throws NotImplementedException.
     */
    [[nodiscard]] System::Xml::Schema::XmlSchema* GetSchema() const override
    {
        throw std::logic_error("SimpleType::GetSchema is not implemented.");
    }

    /**
     * @brief Reads a serialized simple type message and populates this instance.
     *
     * @param reader The XML reader from which to read the data.
     */
    void ReadXml(XmlReader& reader) override
    {
        reader.ReadStartElement();

        Name = reader.GetAttribute("Name");
        Value = reader.GetAttribute("Value");
    }

    /**
     * @brief Serializes this instance into a specified XML writer.
     *
     * @param writer The XML writer into which this instance is to be serialized.
     */
    void WriteXml(XmlWriter& writer) const override
    {
        writer.WriteStartElement("SimpleType");

        writer.WriteAttributeString("Name", Name);

        writer.WriteAttributeString("Value", Value);
    }
};

/**
 * @brief Contains player information.
 */
class PlayerInformation {
public:
    /** @brief The player's name. */
    std::string Name;

    /** @brief The player's score card, containing 12 scores. */
    std::vector<SharpRuntime::bytecs> ScoreCard;

    /** @brief The player's total score. */
    int TotalScore = 0;

    /** @brief The player's ID on the server. */
    int PlayerID = 0;

    /**
     * @brief Populates this instance according to serialized XML data.
     *
     * @param reader   XML reader positioned just before the serialized information.
     * @param gameType The type of game the player participates in, which decides whether score
     *                 card information is present.
     */
    void Deserialize(XmlReader& reader, GameTypes gameType)
    {
        // Read the start element
        reader.Read();

        Name = reader.GetAttribute("Name");
        PlayerID = std::stoi(reader.GetAttribute("ID"));
        TotalScore = std::stoi(reader.GetAttribute("Score"));

        if (gameType == GameTypes::Offline) {
            // Read score card information
            reader.Read();
            ScoreCard.assign(12, 0);
            reader.ReadContentAsBase64(ScoreCard, 0, 12);
        } else {
            // Read end element
            reader.Read();
        }
    }

    /**
     * @brief Serializes this instance into a specified XML writer.
     *
     * @param writer   The XML writer into which to write.
     * @param gameType The type of game the player participates in, which decides whether score
     *                 card information is written.
     */
    void Serialize(XmlWriter& writer, GameTypes gameType) const
    {
        // Write start element
        writer.WriteStartElement("PlayerInformation");
        writer.WriteAttributeString("Name", Name);
        writer.WriteAttributeString("ID", std::to_string(PlayerID));
        writer.WriteAttributeString("Score", std::to_string(TotalScore));

        if (gameType == GameTypes::Offline) {
            // Write score card information
            writer.WriteBase64(ScoreCard, 0, static_cast<SharpRuntime::intcs>(ScoreCard.size()));
        }

        // Write end element
        writer.WriteFullEndElement();
    }
};

/**
 * @brief Represents the entire state of a Yacht game.
 */
class GameState : public IXmlSerializable {
public:
    /** @brief A list of all players participating in the game. */
    std::vector<PlayerInformation> Players;

    /** @brief The amount of steps performed in the game. */
    SharpRuntime::bytecs StepsMade = 0;

    /** @brief The game's name. */
    std::string Name;

    /** @brief The ID of the currently active player. */
    int CurrentPlayer = 0;

    /** @brief The game's unique identifier. */
    System::Guid GameID;

    /** @brief Whether or not the game has started. */
    bool IsStarted = false;

    /** @brief The game's type. */
    GameTypes GameType = GameTypes::Offline;

    /** @brief Creates a new instance of the class. */
    GameState() = default;

    /**
     * @brief Not implemented.
     *
     * @return Never returns.
     * @throws std::logic_error always, as the original throws NotImplementedException.
     */
    [[nodiscard]] System::Xml::Schema::XmlSchema* GetSchema() const override
    {
        throw std::logic_error("GameState::GetSchema is not implemented.");
    }

    /**
     * @brief Serializes this instance into a specified XML writer.
     *
     * @param writer The XML writer into which to write the serialized instance.
     */
    void Serialize(XmlWriter& writer) const
    {
        writer.WriteStartElement("GameState");

        writer.WriteAttributeString("Type", YachtServices::ToString(GameType));
        writer.WriteAttributeString("IsStarted", IsStarted ? "True" : "False");
        writer.WriteAttributeString("GameID", GameID.ToString());
        writer.WriteAttributeString("CurrentPlayer", std::to_string(CurrentPlayer));
        writer.WriteAttributeString("Name", Name);
        writer.WriteAttributeString("StepsMade", std::to_string(static_cast<int>(StepsMade)));
        writer.WriteAttributeString("NumberOfPlayers", std::to_string(Players.size()));

        for (std::size_t i = 0; i < Players.size(); i++) {
            Players[i].Serialize(writer, GameType);
        }

        writer.WriteEndElement();
    }

    /**
     * @brief Reads a serialized GameState object.
     *
     * Unlike ReadXml, the reader is assumed to have already read the node containing the start
     * element. This moves the reader past the representation's end element.
     *
     * @param reader The XML reader from which to read.
     */
    void Deserialize(XmlReader& reader)
    {
        GameType = ParseGameTypes(reader.GetAttribute("Type"));
        GameID = System::Guid(reader.GetAttribute("GameID"));
        IsStarted = ParseBoolean(reader.GetAttribute("IsStarted"));
        CurrentPlayer = std::stoi(reader.GetAttribute("CurrentPlayer"));
        Name = reader.GetAttribute("Name");
        StepsMade = static_cast<SharpRuntime::bytecs>(std::stoi(reader.GetAttribute("StepsMade")));
        const int numberOfPlayers = std::stoi(reader.GetAttribute("NumberOfPlayers"));

        for (int i = 0; i < numberOfPlayers; i++) {
            PlayerInformation playerInformation;
            playerInformation.Deserialize(reader, GameType);
            Players.push_back(playerInformation);
        }

        // Read the end element
        reader.Read();
    }

    /**
     * @brief Populates this instance according to serialized XML data.
     *
     * @param reader XML reader from which to read the serialized message data.
     */
    void ReadXml(XmlReader& reader) override
    {
        // Read the start element
        reader.Read();

        Deserialize(reader);
    }

    /**
     * @brief Serializes this instance into a specified XML writer.
     *
     * @param writer The XML writer into which to write the serialized instance.
     */
    void WriteXml(XmlWriter& writer) const override
    {
        Serialize(writer);
    }

    /**
     * @brief The name of the root element in the object's XML representation.
     *
     * @return The element name.
     */
    [[nodiscard]] std::string getRootNameProperty() const
    {
        return "GameState";
    }

private:
    // .NET's bool.Parse is case-insensitive and its ToString writes "True"/"False", so a
    // document written by the original server and one written here have to read the same.
    [[nodiscard]] static bool ParseBoolean(const std::string& text)
    {
        return text == "true" || text == "True" || text == "TRUE";
    }
};

/**
 * @brief Represents a Yacht step: one player scoring one line.
 */
class YachtStep : public IXmlSerializable {
public:
    /** @brief The index of the player who made the step. */
    int PlayerIndex = 0;

    /** @brief The score line that was scored. */
    int ScoreLine = 0;

    /** @brief The score placed on that line. */
    SharpRuntime::bytecs Score = 0;

    /** @brief The step's sequence number within the game. */
    int StepNumber = 0;

    /** @brief Creates an empty step. */
    YachtStep() = default;

    /**
     * @brief Creates a step.
     *
     * @param scoreLine   The score line that was scored.
     * @param score       The score placed on that line.
     * @param playerIndex The index of the player who made the step.
     * @param stepNumber  The step's sequence number within the game.
     */
    YachtStep(int scoreLine, SharpRuntime::bytecs score, int playerIndex, int stepNumber)
        : PlayerIndex(playerIndex), ScoreLine(scoreLine), Score(score), StepNumber(stepNumber)
    {
    }

    /**
     * @brief Not implemented.
     *
     * @return Never returns.
     * @throws std::logic_error always, as the original throws NotImplementedException.
     */
    [[nodiscard]] System::Xml::Schema::XmlSchema* GetSchema() const override
    {
        throw std::logic_error("YachtStep::GetSchema is not implemented.");
    }

    /**
     * @brief Populates this instance according to serialized XML data.
     *
     * @param reader XML reader positioned on the step's element.
     */
    void ReadXml(XmlReader& reader) override
    {
        PlayerIndex = std::stoi(reader.GetAttribute("PlayerIndex"));
        ScoreLine = std::stoi(reader.GetAttribute("ScoreLine"));
        Score = static_cast<SharpRuntime::bytecs>(std::stoi(reader.GetAttribute("Score")));
        StepNumber = std::stoi(reader.GetAttribute("StepNumber"));
    }

    /**
     * @brief Serializes this instance into a specified XML writer.
     *
     * @param writer The XML writer into which to write the serialized instance.
     */
    void WriteXml(XmlWriter& writer) const override
    {
        writer.WriteStartElement("YachtStep");
        writer.WriteAttributeString("PlayerIndex", std::to_string(PlayerIndex));
        writer.WriteAttributeString("ScoreLine", std::to_string(ScoreLine));
        writer.WriteAttributeString("Score", std::to_string(static_cast<int>(Score)));
        writer.WriteAttributeString("StepNumber", std::to_string(StepNumber));
    }
};

/**
 * @brief Identifies one game a player may join.
 */
struct GameInformation {
    /** @brief The game's unique identifier. */
    System::Guid GameID;

    /** @brief The game's name. */
    std::string Name;
};

/**
 * @brief The list of games a player may join.
 */
class AvailableGames : public IXmlSerializable {
public:
    /** @brief The games that are available. */
    std::vector<GameInformation> Games;

    /** @brief Creates an empty list. */
    AvailableGames() = default;

    /**
     * @brief Not implemented.
     *
     * @return Never returns.
     * @throws std::logic_error always, as the original throws NotImplementedException.
     */
    [[nodiscard]] System::Xml::Schema::XmlSchema* GetSchema() const override
    {
        throw std::logic_error("AvailableGames::GetSchema is not implemented.");
    }

    /**
     * @brief Populates this instance according to serialized XML data.
     *
     * @param reader XML reader from which to read the serialized data.
     */
    void ReadXml(XmlReader& reader) override
    {
        // Read start element
        reader.Read();

        const int count = std::stoi(reader.GetAttribute("Count"));

        for (int i = 0; i < count; i++) {
            reader.Read();
            GameInformation gi;
            gi.GameID = System::Guid(reader.GetAttribute("GameID"));
            gi.Name = reader.GetAttribute("Name");
            Games.push_back(gi);

            // A game element does not have an individual end tag
        }

        // Read end element
        reader.Read();
    }

    /**
     * @brief Serializes this instance into a specified XML writer.
     *
     * @param writer The XML writer into which to write the serialized instance.
     */
    void WriteXml(XmlWriter& writer) const override
    {
        writer.WriteStartElement("AvailableGames");
        writer.WriteAttributeString("Count", std::to_string(Games.size()));

        for (std::size_t i = 0; i < Games.size(); i++) {
            writer.WriteStartElement("Game");
            writer.WriteAttributeString("GameID", Games[i].GameID.ToString());
            writer.WriteAttributeString("Name", Games[i].Name);
            writer.WriteEndElement();
        }

        writer.WriteFullEndElement();
    }
};

/**
 * @brief The winning player's final score card.
 */
class EndGameInformation : public IXmlSerializable {
public:
    /** @brief The player's final score card, containing 12 scores. */
    std::vector<SharpRuntime::bytecs> ScoreCard;

    /** @brief The player's ID on the server. */
    int PlayerID = 0;

    /**
     * @brief Not implemented.
     *
     * @return Never returns.
     * @throws std::logic_error always, as the original throws NotImplementedException.
     */
    [[nodiscard]] System::Xml::Schema::XmlSchema* GetSchema() const override
    {
        throw std::logic_error("EndGameInformation::GetSchema is not implemented.");
    }

    /**
     * @brief Populates this instance according to serialized XML data.
     *
     * @param reader XML reader from which to read the serialized data.
     */
    void ReadXml(XmlReader& reader) override
    {
        reader.Read();

        PlayerID = std::stoi(reader.GetAttribute("ID"));
        ScoreCard.assign(12, 0);

        reader.ReadStartElement();
        reader.ReadContentAsBinHex(ScoreCard, 0, 12);
    }

    /**
     * @brief Serializes this instance into a specified XML writer.
     *
     * @param writer The XML writer into which to write the serialized instance.
     */
    void WriteXml(XmlWriter& writer) const override
    {
        writer.WriteStartElement("EndGameState");
        writer.WriteAttributeString("ID", std::to_string(PlayerID));
        writer.WriteBinHex(ScoreCard, 0, static_cast<SharpRuntime::intcs>(ScoreCard.size()));
    }
};

/**
 * @brief A message sent from the game server to a client.
 */
class Message : public IXmlSerializable {
public:
    /** @brief The message's type. */
    MessageContentType ContentType = MessageContentType::GameState;

    /** @brief The message's sequence number. */
    int SequenceNumber = 0;

    /** @brief The message's body, which contains XML serialized data. */
    std::shared_ptr<IXmlSerializable> Body;

    /**
     * @brief Not implemented.
     *
     * @return Never returns.
     * @throws std::logic_error always, as the original throws NotImplementedException.
     */
    [[nodiscard]] System::Xml::Schema::XmlSchema* GetSchema() const override
    {
        throw std::logic_error("Message::GetSchema is not implemented.");
    }

    /**
     * @brief Populates this instance according to serialized XML data.
     *
     * @param reader XML reader from which to read the serialized message data.
     */
    void ReadXml(XmlReader& reader) override
    {
        // Read the start element
        reader.Read();

        ContentType = ParseMessageContentType(reader.GetAttribute("ContentType"));

        SequenceNumber = std::stoi(reader.GetAttribute("SequenceNumber"));

        // Read body start element
        reader.Read();

        // Create the message according to the content type.
        switch (ContentType) {
        case MessageContentType::GameState:
            Body = std::make_shared<GameState>();
            break;
        case MessageContentType::SimpleType:
            Body = std::make_shared<SimpleType>();
            break;
        case MessageContentType::AvailableGames:
            Body = std::make_shared<AvailableGames>();
            break;
        case MessageContentType::EndGameInformation:
            Body = std::make_shared<EndGameInformation>();
            break;
        default:
            break;
        }

        Body->ReadXml(reader);

        // Read body end element
        reader.Read();

        // Read message end element
        reader.Read();
    }

    /**
     * @brief Serializes this instance into a specified XML writer.
     *
     * @param writer The XML writer into which to write the serialized instance.
     */
    void WriteXml(XmlWriter& writer) const override
    {
        writer.WriteStartElement("Message");

        writer.WriteAttributeString("ContentType", YachtServices::ToString(ContentType));
        writer.WriteAttributeString("SequenceNumber", std::to_string(SequenceNumber));

        writer.WriteStartElement("Body");
        Body->WriteXml(writer);
        writer.WriteEndElement();

        writer.WriteEndElement();
    }
};

} // namespace YachtServices
