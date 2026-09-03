// SPDX-License-Identifier: MS-PL

#include <cstdio>
#include <exception>
#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Storage/StorageContainer.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageDevice.hpp"
#include "Properties/GameSettings.hpp"
#include "System/IO/Stream.hpp"

namespace
{
    using Microsoft::Xna::Framework::Storage::StorageDevice;
    using RacingGame::Properties::GameSettings;

    bool Check(const bool condition, const char* label)
    {
        std::printf("[%s] %s\n", condition ? "PASS" : "FAIL", label);
        return condition;
    }

    std::string ReadPersistedXml()
    {
        auto selection = StorageDevice::BeginShowSelector(nullptr, nullptr);
        auto device = StorageDevice::EndShowSelector(selection.get());
        auto opening = device->BeginOpenContainer(
            "RacingGame", nullptr, nullptr);
        auto container = device->EndOpenContainer(opening.get());
        auto stream = container->OpenFile(
            "RacingGameSettings.xml", System::IO::FileMode::Open,
            System::IO::FileAccess::Read);
        std::string xml(static_cast<std::size_t>(stream->getLengthProperty()),
                        '\0');
        int offset = 0;
        while (offset < static_cast<int>(xml.size()))
        {
            const int count = stream->Read(
                reinterpret_cast<SharpRuntime::bytecs*>(xml.data()), offset,
                static_cast<int>(xml.size()) - offset);
            if (count <= 0) break;
            offset += count;
        }
        xml.resize(static_cast<std::size_t>(offset));
        return xml;
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::fprintf(stderr, "usage: RacingGameSettingsProbe STORAGE_APP\n");
        return 2;
    }
    try
    {
        StorageDevice::SetAppNameEXT(argv[1]);
        GameSettings settings;
        settings.Load();
        settings.setHighscoresProperty("CNA Racer:42424");
        settings.setPlayerNameProperty("CNA Racer");
        settings.setResolutionWidthProperty(1280);
        settings.setResolutionHeightProperty(720);
        settings.setFullscreenProperty(false);
        settings.setPostScreenEffectsProperty(false);
        settings.setShadowMappingProperty(false);
        settings.setHighDetailProperty(false);
        settings.setSoundVolumeProperty(0.35f);
        settings.setMusicVolumeProperty(0.45f);
        settings.setControllerSensitivityProperty(0.65f);
        settings.Save();

        bool passed = true;
        const std::string xml = ReadPersistedXml();
        passed = Check(xml.find("<GameSettings xmlns:xsi=") != std::string::npos,
                       "settings use the XmlSerializer root and schema namespaces") && passed;
        passed = Check(xml.find("<PlayerName>CNA Racer</PlayerName>") !=
                           std::string::npos,
                       "settings preserve the original property element names") && passed;

        GameSettings loaded;
        loaded.Load();
        passed = Check(loaded.getHighscoresProperty() == "CNA Racer:42424" &&
                           loaded.getPlayerNameProperty() == "CNA Racer",
                       "highscores and player name survive a storage round trip") && passed;
        passed = Check(loaded.getResolutionWidthProperty() == 1280 &&
                           loaded.getResolutionHeightProperty() == 720 &&
                           !loaded.getFullscreenProperty(),
                       "display settings survive a storage round trip") && passed;
        passed = Check(!loaded.getPostScreenEffectsProperty() &&
                           !loaded.getShadowMappingProperty() &&
                           !loaded.getHighDetailProperty(),
                       "graphics quality settings survive a storage round trip") && passed;
        passed = Check(loaded.getSoundVolumeProperty() == 0.35f &&
                           loaded.getMusicVolumeProperty() == 0.45f &&
                           loaded.getControllerSensitivityProperty() == 0.65f,
                       "audio and controller settings survive a storage round trip") && passed;
        loaded.SetMinimumGraphics();
        GameSettings minimum;
        minimum.Load();
        passed = Check(
            minimum.getResolutionWidthProperty() ==
                GameSettings::MinimumResolutionWidth &&
            minimum.getResolutionHeightProperty() ==
                GameSettings::MinimumResolutionHeight,
            "minimum graphics settings are persisted") && passed;
        std::printf("=== Racing Settings: %s ===\n",
                    passed ? "PASS" : "FAIL");
        return passed ? 0 : 1;
    }
    catch (const std::exception& exception)
    {
        std::fprintf(stderr, "[FAIL] settings: %s\n", exception.what());
        return 1;
    }
}
