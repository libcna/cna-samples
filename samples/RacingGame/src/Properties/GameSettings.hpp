// SPDX-License-Identifier: MS-PL

#pragma once

#include <string>

#include "System/Xml/Serialization/detail/XmlMember.hpp"

namespace RacingGame::Properties
{
    /** @brief Persistent Racing settings stored in the original XML shape. */
    class GameSettings final
    {
    public:
        /** @brief Minimum supported backbuffer width. */
        static constexpr int MinimumResolutionWidth = 640;
        /** @brief Minimum supported backbuffer height. */
        static constexpr int MinimumResolutionHeight = 480;

        /** @brief Creates the original default settings. */
        GameSettings() = default;

        /** @brief Loads settings from the XNA RacingGame storage container. */
        void Load();
        /** @brief Saves settings when at least one value changed. */
        void Save();
        /** @brief Selects all minimum graphics options and saves them. */
        void SetMinimumGraphics();

        /** @brief Gets the serialized highscore table. */
        [[nodiscard]] const std::string& getHighscoresProperty() const;
        /** @brief Replaces the serialized highscore table. */
        void setHighscoresProperty(const std::string& value);
        /** @brief Gets the local player name. */
        [[nodiscard]] const std::string& getPlayerNameProperty() const;
        /** @brief Replaces the local player name. */
        void setPlayerNameProperty(const std::string& value);
        /** @brief Gets the requested resolution width. */
        [[nodiscard]] int getResolutionWidthProperty() const;
        /** @brief Sets the requested resolution width. */
        void setResolutionWidthProperty(int value);
        /** @brief Gets the requested resolution height. */
        [[nodiscard]] int getResolutionHeightProperty() const;
        /** @brief Sets the requested resolution height. */
        void setResolutionHeightProperty(int value);
        /** @brief Gets whether fullscreen is requested. */
        [[nodiscard]] bool getFullscreenProperty() const;
        /** @brief Sets whether fullscreen is requested. */
        void setFullscreenProperty(bool value);
        /** @brief Gets whether post-screen effects are enabled. */
        [[nodiscard]] bool getPostScreenEffectsProperty() const;
        /** @brief Sets whether post-screen effects are enabled. */
        void setPostScreenEffectsProperty(bool value);
        /** @brief Gets whether shadow mapping is enabled. */
        [[nodiscard]] bool getShadowMappingProperty() const;
        /** @brief Sets whether shadow mapping is enabled. */
        void setShadowMappingProperty(bool value);
        /** @brief Gets whether high-detail rendering is enabled. */
        [[nodiscard]] bool getHighDetailProperty() const;
        /** @brief Sets whether high-detail rendering is enabled. */
        void setHighDetailProperty(bool value);
        /** @brief Gets effects volume. */
        [[nodiscard]] float getSoundVolumeProperty() const;
        /** @brief Sets effects volume. */
        void setSoundVolumeProperty(float value);
        /** @brief Gets music volume. */
        [[nodiscard]] float getMusicVolumeProperty() const;
        /** @brief Sets music volume. */
        void setMusicVolumeProperty(float value);
        /** @brief Gets controller sensitivity. */
        [[nodiscard]] float getControllerSensitivityProperty() const;
        /** @brief Sets controller sensitivity. */
        void setControllerSensitivityProperty(float value);
        /** @brief Gets whether a future Save call has work to do. */
        [[nodiscard]] bool getNeedSaveProperty() const;

    private:
        static constexpr const char* SettingsFilename =
            "RacingGameSettings.xml";

        std::string highscores;
        std::string playerName = "Player";
        int resolutionWidth = 0;
        int resolutionHeight = 0;
        bool fullscreen = true;
        bool postScreenEffects = true;
        bool shadowMapping = true;
        bool highDetail = true;
        float soundVolume = 0.8f;
        float musicVolume = 0.6f;
        float controllerSensitivity = 0.5f;
        bool needSave = false;

        SHARP_XML_SERIALIZABLE(
            GameSettings, "GameSettings",
            ::System::Xml::Serialization::detail::MakeMember(
                "Highscores", &GameSettings::highscores),
            ::System::Xml::Serialization::detail::MakeMember(
                "PlayerName", &GameSettings::playerName),
            ::System::Xml::Serialization::detail::MakeMember(
                "ResolutionWidth", &GameSettings::resolutionWidth),
            ::System::Xml::Serialization::detail::MakeMember(
                "ResolutionHeight", &GameSettings::resolutionHeight),
            ::System::Xml::Serialization::detail::MakeMember(
                "Fullscreen", &GameSettings::fullscreen),
            ::System::Xml::Serialization::detail::MakeMember(
                "PostScreenEffects", &GameSettings::postScreenEffects),
            ::System::Xml::Serialization::detail::MakeMember(
                "ShadowMapping", &GameSettings::shadowMapping),
            ::System::Xml::Serialization::detail::MakeMember(
                "HighDetail", &GameSettings::highDetail),
            ::System::Xml::Serialization::detail::MakeMember(
                "SoundVolume", &GameSettings::soundVolume),
            ::System::Xml::Serialization::detail::MakeMember(
                "MusicVolume", &GameSettings::musicVolume),
            ::System::Xml::Serialization::detail::MakeMember(
                "ControllerSensitivity", &GameSettings::controllerSensitivity))
    };
}
