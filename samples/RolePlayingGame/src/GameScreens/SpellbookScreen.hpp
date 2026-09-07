#pragma once

// SpellbookScreen.hpp -- C++ port of GameScreens/SpellbookScreen.cs.

#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "System/ArgumentNullException.hpp"

#include "../Combat/CombatEngine.hpp"
#include "../Data/Characters/FightingCharacter.hpp"
#include "../Data/Spell.hpp"
#include "../Data/StatisticsValue.hpp"
#include "ListScreen.hpp"

namespace RolePlaying {

using RolePlayingGameData::FightingCharacter;
using RolePlayingGameData::Spell;
using RolePlayingGameData::StatisticsValue;

// Lists the spells available to the character.
class SpellbookScreen : public ListScreen<std::shared_ptr<Spell>> {
public:
    // Responds when a spell is selected by this menu. Typically used by the calling menu, like
    // the combat HUD menu, to respond to selection.
    std::function<void(const std::shared_ptr<Spell>&)> SpellSelected;

    // Creates a new SpellbookScreen object for the given player and statistics.
    SpellbookScreen(const std::shared_ptr<FightingCharacter>& fightingCharacter,
                    const StatisticsValue& statistics)
        : fightingCharacter_(fightingCharacter), statistics_(statistics) {
        // check the parameter
        if (fightingCharacter == nullptr) {
            throw System::ArgumentNullException("fightingCharacter");
        }

        // sort the player's spell
        std::stable_sort(fightingCharacter_->Spells().begin(), fightingCharacter_->Spells().end(),
                         [](const std::shared_ptr<Spell>& spell1,
                            const std::shared_ptr<Spell>& spell2) {
                             // handle null values
                             if (spell1 == nullptr) {
                                 return false;
                             }
                             if (spell2 == nullptr) {
                                 return true;
                             }

                             // sort by name
                             return spell1->Name < spell2->Name;
                         });

        // configure the menu text
        titleText_ = "Spell Book";
        selectButtonText_ = "Cast";
        backButtonText_ = "Back";
        xButtonText_.clear();
        yButtonText_.clear();
        leftTriggerText_.clear();
        rightTriggerText_.clear();
    }

    // Get the list that this screen displays.
    const std::vector<std::shared_ptr<Spell>>& GetDataList() override {
        return fightingCharacter_->Spells();
    }

protected:
    // Respond to the triggering of the Select action (and related key).
    void SelectTriggered(const std::shared_ptr<Spell>& entry) override {
        // check the parameter
        if (entry == nullptr) {
            return;
        }

        // make sure the spell can be selected
        if (!CanSelectEntry(entry)) {
            return;
        }

        // if the event is valid, fire it and exit this screen
        if (SpellSelected) {
            SpellSelected(entry);
            ExitScreen();
            return;
        }
    }

    // Draw the spell at the given position in the list.
    void DrawEntry(const std::shared_ptr<Spell>& entry, const Vector2& position,
                   bool isSelected) override {
        // check the parameter
        if (entry == nullptr) {
            throw System::ArgumentNullException("entry");
        }

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 drawPosition = position;
        Color color = isSelected ? Fonts::HighlightColor : Fonts::DisplayColor;

        // draw the icon
        spriteBatch.Draw(*entry->IconTexture, drawPosition + iconOffset_, Color::White);

        // draw the name
        drawPosition.Y += (float)listLineSpacing / 4.0f;
        drawPosition.X += (float)NameColumnInterval;
        spriteBatch.DrawString(Fonts::GearInfoFont(), entry->Name, drawPosition, color);

        // draw the level
        drawPosition.X += (float)LevelColumnInterval;
        spriteBatch.DrawString(Fonts::GearInfoFont(), System::Int32::ToString(entry->Level()),
                               drawPosition, color);

        // draw the power
        drawPosition.X += (float)PowerColumnInterval;
        std::string powerText = entry->GetPowerText();
        Vector2 powerTextSize = Fonts::GearInfoFont().MeasureString(powerText);
        Vector2 powerPosition = drawPosition;
        powerPosition.Y -= std::ceil((powerTextSize.Y - 30.0f) / 2.0f);
        spriteBatch.DrawString(Fonts::GearInfoFont(), powerText, powerPosition, color);

        // draw the quantity
        drawPosition.X += (float)MagicCostColumnInterval;
        spriteBatch.DrawString(Fonts::GearInfoFont(),
                               System::Int32::ToString(entry->MagicPointCost), drawPosition, color);

        // draw the cast button if needed
        if (isSelected) {
            selectButtonText_ =
                (CanSelectEntry(entry) && SpellSelected) ? "Cast" : std::string();
        }
    }

    // Draw the description of the selected item.
    void DrawSelectedDescription(const std::shared_ptr<Spell>& entry) override {
        // check the parameter
        if (entry == nullptr) {
            throw System::ArgumentNullException("entry");
        }

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        // draw the insufficient-mp warning
        if (CombatEngine::IsActive() && entry->MagicPointCost > statistics_.MagicPoints) {
            spriteBatch.DrawString(Fonts::DescriptionFont(), "Not enough MP to Cast Spell",
                                   warningMessagePosition_, Color::Red);
        }

        // draw the description
        spriteBatch.DrawString(Fonts::DescriptionFont(),
                               Fonts::BreakTextIntoLines(entry->Description, 90, 3),
                               spellDescriptionPosition_, Fonts::DescriptionColor);
    }

    // Draw the column headers above the list.
    void DrawColumnHeaders() override {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 position = listEntryStartPosition_;

        position.X += (float)NameColumnInterval;
        if (!nameColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), nameColumnText_, position,
                                   Fonts::CaptionColor);
        }

        position.X += (float)LevelColumnInterval;
        if (!levelColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), levelColumnText_, position,
                                   Fonts::CaptionColor);
        }

        position.X += (float)PowerColumnInterval;
        if (!powerColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), powerColumnText_, position,
                                   Fonts::CaptionColor);
        }

        position.X += (float)MagicCostColumnInterval;
        if (!magicCostColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), magicCostColumnText_, position,
                                   Fonts::CaptionColor);
        }
    }

private:
    // Returns true if the specified spell can be selected.
    bool CanSelectEntry(const std::shared_ptr<Spell>& entry) const {
        if (entry == nullptr) {
            return false;
        }

        return (statistics_.MagicPoints >= entry->MagicPointCost) &&
               (!entry->IsOffensive || CombatEngine::IsActive());
    }

    const Vector2 spellDescriptionPosition_{200.0f, 550.0f};
    const Vector2 warningMessagePosition_{200.0f, 580.0f};

    std::string nameColumnText_ = "Name";
    static constexpr int NameColumnInterval = 80;

    std::string levelColumnText_ = "Level";
    static constexpr int LevelColumnInterval = 240;

    std::string powerColumnText_ = "Power (min, max)";
    static constexpr int PowerColumnInterval = 110;

    std::string magicCostColumnText_ = "MP";
    static constexpr int MagicCostColumnInterval = 380;

    // The FightingCharacter object whose spells are displayed.
    std::shared_ptr<FightingCharacter> fightingCharacter_;

    // The statistics of the character, for calculating the eligibility of spells. Needed because
    // combat statistics override character statistics.
    StatisticsValue statistics_;
};

} // namespace RolePlaying
