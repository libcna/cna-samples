#pragma once

// SessionSaveLoad.hpp -- the save/load half of RolePlayingGame/Session/Session.cs.
//
// It is a separate header only because the save data types are built from a live Session and a
// live Party, so Session.hpp cannot include them; the bodies are the original's.
//
// The document written here is the original's, measured rather than guessed: a probe compiled
// against the real RolePlayingGameDataWindows.dll under the XNA 4.0 Wine prefix serialized each
// of these types and this port follows what came back, element for element. See missing.md.

#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageContainer.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageDevice.hpp"
#include "System/Convert.hpp"
#include "System/DateTime.hpp"
#include "System/IO/FileMode.hpp"
#include "System/IO/Path.hpp"
#include "System/IO/StreamReader.hpp"
#include "System/IO/StreamWriter.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"
#include "System/Xml/XmlDocument.hpp"

#include "../Data/Quests/Quest.hpp"
#include "../Data/Quests/QuestLine.hpp"
#include "../TileEngine/TileEngine.hpp"
#include "PartySaveData.hpp"
#include "PartySaveRestore.hpp"
#include "SaveGameDescription.hpp"
#include "Session.hpp"

namespace RolePlaying {

namespace SaveLoad {

using Microsoft::Xna::Framework::Storage::StorageContainer;
using Microsoft::Xna::Framework::Storage::StorageDevice;
using System::Xml::Serialization::XmlSerializer;

/**
 * @brief Opens the game's storage container, waiting for the asynchronous open as the original
 *        does with the same three calls.
 */
inline std::unique_ptr<StorageContainer> OpenContainer(StorageDevice& storageDevice) {
    auto result = storageDevice.BeginOpenContainer(Session::SaveGameContainerName, nullptr, nullptr);
    result->getAsyncWaitHandleProperty().WaitOne();
    return storageDevice.EndOpenContainer(result.get());
}

/** @brief Writes one element with text content, the counterpart of WriteElementString. */
inline void WriteElementString(System::Xml::XmlDocument& doc, System::Xml::XmlElement* parent,
                               const std::string& name, const std::string& text) {
    System::Xml::XmlElement* element = doc.CreateElement(name);
    element->setInnerTextProperty(text);
    parent->AppendChild(element);
}

/** @brief The first child element with the given name, or nullptr. */
[[nodiscard]] inline System::Xml::XmlElement* Child(System::Xml::XmlNode* parent,
                                                    const std::string& name) {
    for (auto* child = parent->getFirstChildProperty(); child != nullptr;
         child = child->getNextSiblingProperty()) {
        if (child->getNodeTypeProperty() == System::Xml::XmlNodeType::Element &&
            child->getNameProperty() == name) {
            return static_cast<System::Xml::XmlElement*>(child);
        }
    }
    return nullptr;
}

/** @brief Reads one element's text, the counterpart of ReadElementString. */
[[nodiscard]] inline std::string ReadElementString(System::Xml::XmlNode* parent,
                                                   const std::string& name) {
    System::Xml::XmlElement* element = Child(parent, name);
    return element == nullptr ? std::string() : element->getInnerTextProperty();
}

/**
 * @brief Reads the one element a given serializer wrote, located by that serializer's own root
 *        element name.
 *
 * The original hands an open XmlReader to each XmlSerializer in turn and relies on the reader's
 * position; this port has the whole document in hand, so it locates each object by the element
 * name .NET gave it. Same elements, same order, no positional coupling.
 */
template <typename T>
[[nodiscard]] T ReadObject(System::Xml::XmlNode* parent) {
    System::Xml::XmlElement* element = Child(parent, XmlSerializer<T>::RootElementName());
    if (element == nullptr) {
        return T{};
    }
    return XmlSerializer<T>().DeserializeFrom(element);
}

} // namespace SaveLoad

// The Session members below live in RolePlaying, not in SaveLoad, so they name the serializer in
// full; this alias keeps the original's statements readable at the same width.
template <typename T>
using SaveSerializer = System::Xml::Serialization::XmlSerializer<T>;

// -- storage ---------------------------------------------------------------------------------

inline void Session::GetStorageDevice(const StorageDeviceDelegate& retrievalDelegate) {
    // check the parameter
    if (!retrievalDelegate) {
        throw System::ArgumentNullException("retrievalDelegate");
    }

    // check the stored storage device
    if (storageDevice_ && storageDevice_->getIsConnectedProperty()) {
        retrievalDelegate(*storageDevice_);
        return;
    }

    // the storage device must be retrieved
    if (!Microsoft::Xna::Framework::GamerServices::Guide::getIsVisibleProperty()) {
        // Reset the device
        storageDevice_.reset();
        auto result = Microsoft::Xna::Framework::Storage::StorageDevice::BeginShowSelector(
            nullptr, nullptr);
        // CNA completes the selector synchronously, so the original's asynchronous callback is
        // the statement after it rather than a separate method.
        if (result == nullptr || !result->getIsCompletedProperty()) {
            return;
        }
        storageDevice_ =
            Microsoft::Xna::Framework::Storage::StorageDevice::EndShowSelector(result.get());
        if (storageDevice_ && storageDevice_->getIsConnectedProperty()) {
            retrievalDelegate(*storageDevice_);
        }
    }
}

// -- loading a session -----------------------------------------------------------------------

inline void Session::LoadSession(const SaveGameDescription& saveGameDescription,
                                 ScreenManager& screenManager, GameplayScreen& gameplayScreen) {
    // end any existing session
    EndSession();

    // create the new session
    singleton_ = new Session(screenManager, gameplayScreen);

    // get the storage device and load the session
    GetStorageDevice([&saveGameDescription](
                         Microsoft::Xna::Framework::Storage::StorageDevice& storageDevice) {
        LoadSessionResult(storageDevice, saveGameDescription);
    });
}

inline void Session::LoadSessionResult(
    Microsoft::Xna::Framework::Storage::StorageDevice& storageDevice,
    const SaveGameDescription& saveGameDescription) {
    // check the parameter
    if (!storageDevice.getIsConnectedProperty()) {
        return;
    }

    // open the container
    std::unique_ptr<Microsoft::Xna::Framework::Storage::StorageContainer> storageContainer =
        SaveLoad::OpenContainer(storageDevice);
    System::Xml::XmlDocument document;
    {
        std::unique_ptr<System::IO::Stream> stream =
            storageContainer->OpenFile(saveGameDescription.FileName, System::IO::FileMode::Open);
        System::IO::StreamReader reader(stream.get(), true);
        document.LoadXml(reader.ReadToEnd());
    }

    // <rolePlayingGameSaveData>
    System::Xml::XmlElement* root = document.getDocumentElementProperty();
    if (root == nullptr || root->getNameProperty() != "rolePlayingGameSaveData") {
        throw System::Xml::XmlException("Session::LoadSessionResult: not a save game document.");
    }

    // read the map information
    System::Xml::XmlElement* mapData = SaveLoad::Child(root, "mapData");
    std::string mapAssetName = SaveLoad::ReadElementString(mapData, "mapContentName");
    PlayerPosition playerPosition = SaveLoad::ReadObject<PlayerPosition>(mapData);
    singleton_->removedMapChests_ =
        SaveLoad::ReadObject<std::vector<WorldEntry<Chest>>>(mapData);
    singleton_->removedMapFixedCombats_ =
        SaveLoad::ReadObject<std::vector<WorldEntry<FixedCombat>>>(mapData);
    singleton_->removedMapPlayerNpcs_ =
        SaveLoad::ReadObject<std::vector<WorldEntry<Player>>>(mapData);
    singleton_->modifiedMapChests_ =
        SaveLoad::ReadObject<std::vector<ModifiedChestEntry>>(mapData);
    ChangeMap(mapAssetName, nullptr);
    TileEngine::PartyLeaderPosition() = playerPosition;

    // read the quest information
    Microsoft::Xna::Framework::Content::ContentManager& content = Content();
    System::Xml::XmlElement* questData = SaveLoad::Child(root, "questData");
    singleton_->questLine_ =
        content
            .Load<std::shared_ptr<QuestLine>>(
                SaveLoad::ReadElementString(questData, "questLineContentName"))
            ->Clone();
    singleton_->currentQuestIndex_ =
        System::Convert::ToInt32(SaveLoad::ReadElementString(questData, "currentQuestIndex"));
    for (int i = 0; i < singleton_->currentQuestIndex_; i++) {
        singleton_->questLine_->Quests[i]->Stage = Quest::QuestStage::Completed;
    }
    singleton_->removedQuestChests_ =
        SaveLoad::ReadObject<std::vector<WorldEntry<Chest>>>(questData);
    singleton_->removedQuestFixedCombats_ =
        SaveLoad::ReadObject<std::vector<WorldEntry<FixedCombat>>>(questData);
    singleton_->modifiedQuestChests_ =
        SaveLoad::ReadObject<std::vector<ModifiedChestEntry>>(questData);
    Quest::QuestStage questStage = System::Xml::Serialization::detail::FromXmlText<
        Quest::QuestStage>(SaveLoad::ReadElementString(questData, "currentQuestStage"));
    if (singleton_->questLine_ && !IsQuestLineComplete()) {
        singleton_->quest_ = singleton_->questLine_->Quests[CurrentQuestIndex()];
        singleton_->ModifyQuest(*singleton_->quest_);
        singleton_->quest_->Stage = questStage;
    }

    // read the party data
    singleton_->party_ = std::make_unique<Party>(
        SaveLoad::ReadObject<PartySaveData>(root), content);
}

// -- saving a session ------------------------------------------------------------------------

inline void Session::SaveSession(const SaveGameDescription* overwriteDescription) {
    // The description is copied because the original's list is rebuilt by the refresh that the
    // save triggers, which would leave a pointer into it dangling.
    std::optional<SaveGameDescription> overwrite;
    if (overwriteDescription != nullptr) {
        overwrite = *overwriteDescription;
    }

    // retrieve the storage device, asynchronously
    GetStorageDevice([&overwrite](Microsoft::Xna::Framework::Storage::StorageDevice& storageDevice) {
        SaveSessionResult(storageDevice, overwrite ? &*overwrite : nullptr);
    });
}

inline void Session::SaveSessionResult(
    Microsoft::Xna::Framework::Storage::StorageDevice& storageDevice,
    const SaveGameDescription* overwriteDescription) {
    // check the parameter
    if (!storageDevice.getIsConnectedProperty()) {
        return;
    }

    // open the container
    std::unique_ptr<Microsoft::Xna::Framework::Storage::StorageContainer> storageContainer =
        SaveLoad::OpenContainer(storageDevice);

    std::string filename;
    std::string descriptionFilename;
    // get the filenames
    if (overwriteDescription == nullptr) {
        int saveGameIndex = 0;
        std::string testFilename;
        do {
            saveGameIndex++;
            testFilename = "SaveGame" + std::to_string(saveGameIndex) + ".xml";
        } while (storageContainer->FileExists(testFilename));
        filename = testFilename;
        descriptionFilename = "SaveGameDescription" + std::to_string(saveGameIndex) + ".xml";
    } else {
        filename = overwriteDescription->FileName;
        descriptionFilename =
            "SaveGameDescription" +
            System::IO::Path::GetFileNameWithoutExtension(overwriteDescription->FileName)
                .substr(8) +
            ".xml";
    }

    System::Xml::XmlDocument document;

    // <rolePlayingGameData>
    System::Xml::XmlElement* root = document.CreateElement("rolePlayingGameSaveData");
    document.AppendChild(root);

    // write the map information
    System::Xml::XmlElement* mapData = document.CreateElement("mapData");
    root->AppendChild(mapData);
    SaveLoad::WriteElementString(document, mapData, "mapContentName",
                                 TileEngine::Map()->AssetName());
    SaveSerializer<PlayerPosition>().SerializeInto(document, mapData,
                                                  TileEngine::PartyLeaderPosition());
    SaveSerializer<std::vector<WorldEntry<Chest>>>().SerializeInto(document, mapData,
                                                                  singleton_->removedMapChests_);
    SaveSerializer<std::vector<WorldEntry<FixedCombat>>>().SerializeInto(
        document, mapData, singleton_->removedMapFixedCombats_);
    SaveSerializer<std::vector<WorldEntry<Player>>>().SerializeInto(
        document, mapData, singleton_->removedMapPlayerNpcs_);
    SaveSerializer<std::vector<ModifiedChestEntry>>().SerializeInto(
        document, mapData, singleton_->modifiedMapChests_);

    // write the quest information
    System::Xml::XmlElement* questData = document.CreateElement("questData");
    root->AppendChild(questData);
    SaveLoad::WriteElementString(document, questData, "questLineContentName",
                                 singleton_->questLine_->AssetName());
    SaveLoad::WriteElementString(document, questData, "currentQuestIndex",
                                 std::to_string(singleton_->currentQuestIndex_));
    SaveSerializer<std::vector<WorldEntry<Chest>>>().SerializeInto(document, questData,
                                                                  singleton_->removedQuestChests_);
    SaveSerializer<std::vector<WorldEntry<FixedCombat>>>().SerializeInto(
        document, questData, singleton_->removedQuestFixedCombats_);
    SaveSerializer<std::vector<ModifiedChestEntry>>().SerializeInto(
        document, questData, singleton_->modifiedQuestChests_);
    SaveLoad::WriteElementString(
        document, questData, "currentQuestStage",
        System::Xml::Serialization::detail::ToXmlText(
            IsQuestLineComplete() ? Quest::QuestStage::NotStarted : singleton_->quest_->Stage));

    // write the party data
    SaveSerializer<PartySaveData>().SerializeInto(document, root,
                                                 PartySaveData(singleton_->party_.get()));

    {
        std::unique_ptr<System::IO::Stream> stream =
            storageContainer->OpenFile(filename, System::IO::FileMode::Create);
        System::IO::StreamWriter writer(stream.get(), true);
        writer.Write("<?xml version=\"1.0\" encoding=\"utf-8\"?>" +
                     document.getOuterXmlProperty());
        writer.Flush();
    }

    // create the save game description
    SaveGameDescription description;
    description.FileName = System::IO::Path::GetFileName(filename);
    description.ChapterName =
        IsQuestLineComplete() ? std::string("Quest Line Complete") : GetQuest()->Name;
    description.Description = System::DateTime::getNowProperty().ToString();
    {
        std::unique_ptr<System::IO::Stream> stream =
            storageContainer->OpenFile(descriptionFilename, System::IO::FileMode::Create);
        System::IO::StreamWriter writer(stream.get(), true);
        writer.Write(SaveSerializer<SaveGameDescription>().Serialize(description));
        writer.Flush();
    }
}

// -- deleting a save game --------------------------------------------------------------------

inline void Session::DeleteSaveGame(const SaveGameDescription& saveGameDescription) {
    SaveGameDescription copy = saveGameDescription;

    // get the storage device and delete the save game
    GetStorageDevice([&copy](Microsoft::Xna::Framework::Storage::StorageDevice& storageDevice) {
        DeleteSaveGameResult(storageDevice, copy);
    });
}

inline void Session::DeleteSaveGameResult(
    Microsoft::Xna::Framework::Storage::StorageDevice& storageDevice,
    const SaveGameDescription& saveGameDescription) {
    // check the parameter
    if (!storageDevice.getIsConnectedProperty()) {
        return;
    }

    // open the container
    {
        std::unique_ptr<Microsoft::Xna::Framework::Storage::StorageContainer> storageContainer =
            SaveLoad::OpenContainer(storageDevice);
        storageContainer->DeleteFile(saveGameDescription.FileName);
        storageContainer->DeleteFile(
            "SaveGameDescription" +
            System::IO::Path::GetFileNameWithoutExtension(saveGameDescription.FileName).substr(8) +
            ".xml");
    }

    // refresh the save game descriptions
    Session::RefreshSaveGameDescriptions();
}

// -- save game descriptions ------------------------------------------------------------------

inline void Session::RefreshSaveGameDescriptions() {
    // clear the list
    saveGameDescriptions_.reset();

    // retrieve the storage device, asynchronously
    GetStorageDevice(&Session::RefreshSaveGameDescriptionsResult);
}

inline void Session::RefreshSaveGameDescriptionsResult(
    Microsoft::Xna::Framework::Storage::StorageDevice& storageDevice) {
    // check the parameter
    if (!storageDevice.getIsConnectedProperty()) {
        return;
    }

    // open the container
    std::unique_ptr<Microsoft::Xna::Framework::Storage::StorageContainer> storageContainer =
        SaveLoad::OpenContainer(storageDevice);
    saveGameDescriptions_.emplace();
    // get the description list
    std::vector<std::string> filenames = storageContainer->GetFileNames("SaveGameDescription*.xml");
    std::sort(filenames.begin(), filenames.end());
    // add each entry to the list
    for (const std::string& filename : filenames) {
        // check the size of the list
        if (static_cast<int>(saveGameDescriptions_->size()) >= MaximumSaveGameDescriptions) {
            break;
        }
        // open the file stream
        std::unique_ptr<System::IO::Stream> fileStream =
            storageContainer->OpenFile(filename, System::IO::FileMode::Open);
        // deserialize the object
        System::IO::StreamReader reader(fileStream.get(), true);
        saveGameDescriptions_->push_back(
            SaveSerializer<SaveGameDescription>().Deserialize(reader.ReadToEnd()));
    }
}

} // namespace RolePlaying
