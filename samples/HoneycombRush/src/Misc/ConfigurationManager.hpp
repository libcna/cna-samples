// SPDX-License-Identifier: MS-PL
#pragma once

#include <algorithm>
#include <cctype>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "System/Int32.hpp"
#include "System/TimeSpan.hpp"
#include "System/Xml/Linq/XAttribute.hpp"
#include "System/Xml/Linq/XDocument.hpp"
#include "System/Xml/Linq/XElement.hpp"

namespace HoneycombRush {

enum class DifficultyMode { Easy = 1, Medium = 2, Hard = 3 };

inline DifficultyMode& operator++(DifficultyMode& mode) {
    mode = static_cast<DifficultyMode>(static_cast<int>(mode) + 1);
    return mode;
}

struct Configuration {
    System::TimeSpan GameElapsed;
    float MinWorkerBeeVelocity = 0.0f;
    float MaxWorkerBeeVelocity = 0.0f;
    float MinSoldierBeeVelocity = 0.0f;
    float MaxSoldierBeeVelocity = 0.0f;
    int TotalSmokeAmount = 0;
    int DecreaseAmountSpeed = 0;
    int IncreaseAmountSpeed = 0;
    int HighScoreFactor = 0;
};

class ConfigurationManager {
public:
    static const std::unordered_map<DifficultyMode, Configuration>& ModesConfiguration() {
        return modesConfiguration_;
    }

    static bool IsLoaded() { return isLoaded_; }

    static std::optional<DifficultyMode>& DifficultyModeProperty() { return difficultyMode_; }

    static void LoadConfiguration(const std::shared_ptr<System::Xml::Linq::XDocument>& document) {
        if (!document)
            throw std::invalid_argument("document");

        std::unordered_map<DifficultyMode, Configuration> loaded;
        const auto difficulties = document->Element("Difficulties");
        if (!difficulties)
            throw std::runtime_error("Configuration is missing the Difficulties element");

        for (const auto& element : difficulties->Elements("Difficulty")) {
            const auto id = element->Attribute("ID");
            if (!id)
                throw std::runtime_error("Difficulty is missing its ID attribute");

            Configuration configuration;
            configuration.DecreaseAmountSpeed = ParseInt(element, "DecreaseAmountSpeed");
            configuration.GameElapsed = System::TimeSpan::Parse(Value(element, "GameElapsed"));
            configuration.IncreaseAmountSpeed = ParseInt(element, "IncreaseAmountSpeed");
            configuration.MaxSoldierBeeVelocity = (float)ParseInt(element, "MaxSoldierBeeVelocity");
            configuration.MaxWorkerBeeVelocity = (float)ParseInt(element, "MaxWorkerBeeVelocity");
            configuration.MinSoldierBeeVelocity = (float)ParseInt(element, "MinSoldierBeeVelocity");
            configuration.MinWorkerBeeVelocity = (float)ParseInt(element, "MinWorkerBeeVelocity");
            configuration.TotalSmokeAmount = ParseInt(element, "TotalSmokeAmount");
            configuration.HighScoreFactor = ParseInt(element, "HighScoreFactor");
            loaded.emplace(ParseDifficultyMode(id->getValueProperty()), configuration);
        }

        modesConfiguration_ = std::move(loaded);
        isLoaded_ = true;
    }

private:
    static std::string Value(const std::shared_ptr<System::Xml::Linq::XElement>& parent,
                             const char* elementName) {
        const auto element = parent->Element(elementName);
        if (!element)
            throw std::runtime_error(std::string("Difficulty is missing ") + elementName);
        return element->getValueProperty();
    }

    static int ParseInt(const std::shared_ptr<System::Xml::Linq::XElement>& parent,
                        const char* elementName) {
        return System::Int32::Parse(Value(parent, elementName));
    }

    static DifficultyMode ParseDifficultyMode(const std::string& value) {
        std::string normalized = value;
        std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                       [](unsigned char character) { return (char)std::tolower(character); });
        if (normalized == "easy") return DifficultyMode::Easy;
        if (normalized == "medium") return DifficultyMode::Medium;
        if (normalized == "hard") return DifficultyMode::Hard;
        throw std::runtime_error("Unknown difficulty mode: " + value);
    }

    static inline std::unordered_map<DifficultyMode, Configuration> modesConfiguration_;
    static inline bool isLoaded_ = false;
    static inline std::optional<DifficultyMode> difficultyMode_;
};

} // namespace HoneycombRush
