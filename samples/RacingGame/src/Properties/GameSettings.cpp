// SPDX-License-Identifier: MS-PL

#include "Properties/GameSettings.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Storage/StorageContainer.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageDevice.hpp"
#include "System/IO/Stream.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"

namespace RacingGame::Properties
{
    using Microsoft::Xna::Framework::Storage::StorageContainer;
    using Microsoft::Xna::Framework::Storage::StorageDevice;
    using System::Xml::Serialization::XmlSerializationOptions;
    using System::Xml::Serialization::XmlSerializer;

    namespace
    {
        std::unique_ptr<StorageDevice> OpenDevice()
        {
            auto selection = StorageDevice::BeginShowSelector(nullptr, nullptr);
            return StorageDevice::EndShowSelector(selection.get());
        }

        std::unique_ptr<StorageContainer> OpenContainer(StorageDevice& device)
        {
            auto operation = device.BeginOpenContainer(
                "RacingGame", nullptr, nullptr);
            return device.EndOpenContainer(operation.get());
        }

        std::string ReadAll(System::IO::Stream& stream)
        {
            std::vector<SharpRuntime::bytecs> bytes(
                static_cast<std::size_t>(stream.getLengthProperty()));
            int offset = 0;
            while (offset < static_cast<int>(bytes.size()))
            {
                const int count = stream.Read(
                    bytes.data(), offset,
                    static_cast<int>(bytes.size()) - offset);
                if (count <= 0) break;
                offset += count;
            }
            return std::string(
                reinterpret_cast<const char*>(bytes.data()),
                static_cast<std::size_t>(offset));
        }
    }

    void GameSettings::Load()
    {
        needSave = false;
        bool saveImmediately = false;
        try
        {
            auto device = OpenDevice();
            if (!device || !device->getIsConnectedProperty()) return;
            auto container = OpenContainer(*device);
            if (container->FileExists(SettingsFilename))
            {
                auto stream = container->OpenFile(
                    SettingsFilename, System::IO::FileMode::Open,
                    System::IO::FileAccess::Read,
                    System::IO::FileShare::ReadWrite);
                if (stream->getLengthProperty() > 0)
                {
                    *this = XmlSerializer<GameSettings>{}.Deserialize(
                        ReadAll(*stream));
                    needSave = false;
                }
                else
                {
                    needSave = true;
                    saveImmediately = true;
                }
            }
            else
            {
                needSave = true;
            }
        }
        catch (const std::exception&)
        {
            // The original keeps the in-memory defaults when storage is unavailable or invalid.
        }
        if (saveImmediately) Save();
    }

    void GameSettings::Save()
    {
        if (!needSave) return;
        needSave = false;
        try
        {
            auto device = OpenDevice();
            if (!device || !device->getIsConnectedProperty()) return;
            auto container = OpenContainer(*device);
            XmlSerializationOptions options;
            options.Indent = true;
            options.WriteEncodingAttribute = true;
            const std::string xml =
                XmlSerializer<GameSettings>{}.Serialize(*this, options);
            auto stream = container->CreateFile(SettingsFilename);
            stream->Write(
                reinterpret_cast<const SharpRuntime::bytecs*>(xml.data()),
                0, static_cast<int>(xml.size()));
            stream->Flush();
            stream->Close();
        }
        catch (const std::exception&)
        {
            // The original treats this save request as consumed even when storage fails.
        }
    }

    void GameSettings::SetMinimumGraphics()
    {
        setResolutionWidthProperty(MinimumResolutionWidth);
        setResolutionHeightProperty(MinimumResolutionHeight);
        setShadowMappingProperty(false);
        setHighDetailProperty(false);
        setPostScreenEffectsProperty(false);
        Save();
    }

    const std::string& GameSettings::getHighscoresProperty() const
    {
        return highscores;
    }

    void GameSettings::setHighscoresProperty(const std::string& value)
    {
        if (highscores != value) needSave = true;
        highscores = value;
    }

    const std::string& GameSettings::getPlayerNameProperty() const
    {
        return playerName;
    }

    void GameSettings::setPlayerNameProperty(const std::string& value)
    {
        if (playerName != value) needSave = true;
        playerName = value;
    }

    int GameSettings::getResolutionWidthProperty() const
    {
        return resolutionWidth;
    }

    void GameSettings::setResolutionWidthProperty(const int value)
    {
        if (resolutionWidth != value) needSave = true;
        resolutionWidth = value;
    }

    int GameSettings::getResolutionHeightProperty() const
    {
        return resolutionHeight;
    }

    void GameSettings::setResolutionHeightProperty(const int value)
    {
        if (resolutionHeight != value) needSave = true;
        resolutionHeight = value;
    }

    bool GameSettings::getFullscreenProperty() const { return fullscreen; }
    void GameSettings::setFullscreenProperty(const bool value)
    {
        if (fullscreen != value) needSave = true;
        fullscreen = value;
    }
    bool GameSettings::getPostScreenEffectsProperty() const
    {
        return postScreenEffects;
    }
    void GameSettings::setPostScreenEffectsProperty(const bool value)
    {
        if (postScreenEffects != value) needSave = true;
        postScreenEffects = value;
    }
    bool GameSettings::getShadowMappingProperty() const { return shadowMapping; }
    void GameSettings::setShadowMappingProperty(const bool value)
    {
        if (shadowMapping != value) needSave = true;
        shadowMapping = value;
    }
    bool GameSettings::getHighDetailProperty() const { return highDetail; }
    void GameSettings::setHighDetailProperty(const bool value)
    {
        if (highDetail != value) needSave = true;
        highDetail = value;
    }
    float GameSettings::getSoundVolumeProperty() const { return soundVolume; }
    void GameSettings::setSoundVolumeProperty(const float value)
    {
        if (soundVolume != value) needSave = true;
        soundVolume = value;
    }
    float GameSettings::getMusicVolumeProperty() const { return musicVolume; }
    void GameSettings::setMusicVolumeProperty(const float value)
    {
        if (musicVolume != value) needSave = true;
        musicVolume = value;
    }
    float GameSettings::getControllerSensitivityProperty() const
    {
        return controllerSensitivity;
    }
    void GameSettings::setControllerSensitivityProperty(const float value)
    {
        if (controllerSensitivity != value) needSave = true;
        controllerSensitivity = value;
    }
    bool GameSettings::getNeedSaveProperty() const { return needSave; }
}
