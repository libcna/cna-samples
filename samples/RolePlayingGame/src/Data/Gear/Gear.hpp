#pragma once

// Gear.hpp -- C++ port of RolePlayingGameData/Gear/Gear.cs.

#include <optional>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "System/ArgumentException.hpp"
#include "../ContentObject.hpp"
#include "System/Int32.hpp"

namespace RolePlayingGameData {

class FightingCharacter;

// An inventory element -- items, equipment, etc.
class Gear : public ContentObject {
public:
    ~Gear() override = default;

    std::string Name;
    std::string Description;

    virtual std::string GetPowerText() const { return ""; }

    // If the value is less than zero, it cannot be sold.
    int GoldValue = 0;
    // If true, the gear can be dropped. If false, it cannot ever be dropped.
    bool IsDroppable = false;

    int MinimumCharacterLevel = 0;
    // Class names are compared case-insensitive.
    std::vector<std::string> SupportedClasses;

    virtual bool CheckRestrictions(const FightingCharacter& fightingCharacter) const;

    virtual std::string GetRestrictionsText() const {
        std::string sb;
        if (MinimumCharacterLevel > 0) {
            sb += "Level - " + System::Int32::ToString(MinimumCharacterLevel) + "; ";
        }
        if (!SupportedClasses.empty()) {
            sb += "Class - ";
            bool first = true;
            for (auto& className : SupportedClasses) {
                if (first) first = false; else sb += ",";
                sb += className;
            }
        }
        return sb;
    }

    std::string IconTextureName;
    std::shared_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> IconTexture;

    virtual void DrawIcon(Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch,
                          Microsoft::Xna::Framework::Vector2 position) const {
        if (IconTexture) {
            spriteBatch.Draw(*IconTexture, position, Microsoft::Xna::Framework::Color(255, 255, 255, 255));
        }
    }

    virtual void DrawDescription(Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch,
                                 Microsoft::Xna::Framework::Graphics::SpriteFont& spriteFont,
                                 Microsoft::Xna::Framework::Color color,
                                 Microsoft::Xna::Framework::Vector2 position,
                                 int maximumCharactersPerLine, int maximumLines) const {
        if (Description.empty()) return;
        spriteBatch.DrawString(spriteFont, Description, position, color);
    }
};

// Reads a Gear object from the content pipeline.
//
// Gear is abstract in the original: the concrete reader that owns the instance calls this one
// through ReadRawObject, so an existing instance is required and never created here.
class GearReader
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Gear>> {
public:
    explicit GearReader(const std::string& typeName = "RolePlayingGameData.Gear")
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Gear>>(typeName) {}

protected:
    std::shared_ptr<Gear> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Gear>> existingInstance) override {
        std::shared_ptr<Gear> gear =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (gear == nullptr) {
            throw System::ArgumentException("Unable to create new Gear objects.");
        }

        gear->SetAssetName(input.getAssetNameProperty());

        // read gear settings
        gear->Name = input.ReadString();
        gear->Description = input.ReadString();
        gear->GoldValue = static_cast<int>(input.ReadInt32());
        gear->IsDroppable = input.ReadBoolean();
        gear->MinimumCharacterLevel = static_cast<int>(input.ReadInt32());
        const auto supportedClasses = input.ReadObject<std::vector<std::string>>();
        gear->SupportedClasses.insert(
            gear->SupportedClasses.end(), supportedClasses.begin(), supportedClasses.end());
        gear->IconTextureName = input.ReadString();
        gear->IconTexture = std::make_shared<Microsoft::Xna::Framework::Graphics::Texture2D>(
            input.getContentManagerProperty()
                ->Load<Microsoft::Xna::Framework::Graphics::Texture2D>(
                    "Textures/Gear/" + gear->IconTextureName));

        return gear;
    }
};

} // namespace RolePlayingGameData
