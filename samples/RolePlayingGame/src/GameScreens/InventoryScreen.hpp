#pragma once

// InventoryScreen.hpp -- C++ port of GameScreens/InventoryScreen.cs.

#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../Combat/CombatEngine.hpp"
#include "../Data/ContentEntry.hpp"
#include "../Data/Gear/Equipment.hpp"
#include "../Data/Gear/Gear.hpp"
#include "../Data/Gear/Item.hpp"
#include "../MenuScreens/MessageBoxScreen.hpp"
#include "../Session/Session.hpp"
#include "ListScreen.hpp"

namespace RolePlaying {

using RolePlayingGameData::ContentEntry;
using RolePlayingGameData::Equipment;
using RolePlayingGameData::Gear;
using RolePlayingGameData::Item;

class StatisticsScreen;      // fwd decl -- constructed in PageScreenLeft
class QuestLogScreen;        // fwd decl -- constructed in PageScreenRight
class PlayerSelectionScreen; // fwd decl -- constructed in SelectTriggered

// Displays the inventory of the party, either showing items or equipment.
class InventoryScreen : public ListScreen<std::shared_ptr<ContentEntry<Gear>>> {
public:
    // Responds when an item is selected by this menu. Typically used by the calling menu, like
    // the combat HUD menu, to respond to selection.
    std::function<void(const std::shared_ptr<Gear>&)> GearSelected;

    // Constructs a new InventoryScreen object.
    explicit InventoryScreen(bool isItems) : isItems_(isItems) {
        // configure the menu text
        titleText_ = "Inventory";
        selectButtonText_ = "Select";
        backButtonText_ = "Back";
        xButtonText_ = "Drop";
        yButtonText_.clear();
        ResetTriggerText();
    }

    // Retrieve the list of gear shown in this menu.
    const std::vector<std::shared_ptr<ContentEntry<Gear>>>& GetDataList() override {
        dataList_.clear();
        const std::vector<std::shared_ptr<ContentEntry<Gear>>>& inventory =
            Session::GetParty()->Inventory();

        // build a new list of only the desired gear
        for (const std::shared_ptr<ContentEntry<Gear>>& gearEntry : inventory) {
            if (isItems_) {
                if (std::dynamic_pointer_cast<Item>(gearEntry->Content) != nullptr) {
                    dataList_.push_back(gearEntry);
                }
            } else {
                if (std::dynamic_pointer_cast<Equipment>(gearEntry->Content) != nullptr) {
                    dataList_.push_back(gearEntry);
                }
            }
        }

        // sort the list by name
        std::stable_sort(dataList_.begin(), dataList_.end(),
                         [](const std::shared_ptr<ContentEntry<Gear>>& gearEntry1,
                            const std::shared_ptr<ContentEntry<Gear>>& gearEntry2) {
                             // handle null values
                             if (gearEntry1 == nullptr || gearEntry1->Content == nullptr) {
                                 return false;
                             }
                             if (gearEntry2 == nullptr || gearEntry2->Content == nullptr) {
                                 return true;
                             }
                             return gearEntry1->Content->Name < gearEntry2->Content->Name;
                         });

        return dataList_;
    }

protected:
    // Respond to the triggering of the Select action (and related key).
    // Defined out-of-line -- it constructs a PlayerSelectionScreen.
    void SelectTriggered(const std::shared_ptr<ContentEntry<Gear>>& entry) override;

    // Respond to the triggering of the X button (and related key).
    void ButtonXPressed(const std::shared_ptr<ContentEntry<Gear>>& entry) override {
        // check the parameter
        if (entry == nullptr || entry->Content == nullptr) {
            return;
        }

        // check whether the gear could be dropped
        if (!entry->Content->IsDroppable) {
            return;
        }

        // add a message box confirming the drop
        auto dropEquipmentConfirmationScreen = std::make_shared<MessageBoxScreen>(
            "Are you sure you want to drop the " + entry->Content->Name + "?");
        std::shared_ptr<Gear> content = entry->Content;
        dropEquipmentConfirmationScreen->Accepted += [content](System::Object*,
                                                               const System::EventArgs&) {
            Session::GetParty()->RemoveFromInventory(content, 1);
        };
        GetScreenManager()->AddScreen(dropEquipmentConfirmationScreen);
    }

    // Switch to the screen to the "left" of this one in the UI.
    // Defined out-of-line -- it constructs a StatisticsScreen.
    void PageScreenLeft() override;

    // Switch to the screen to the "right" of this one in the UI.
    // Defined out-of-line -- it constructs a QuestLogScreen.
    void PageScreenRight() override;

    // Reset the trigger button text to the names of the previous and next UI screens.
    virtual void ResetTriggerText() {
        if (CombatEngine::IsActive()) {
            leftTriggerText_.clear();
            rightTriggerText_.clear();
        } else {
            if (isItems_) {
                leftTriggerText_ = "Statistics";
                rightTriggerText_ = "Equipment";
            } else {
                leftTriggerText_ = "Items";
                rightTriggerText_ = "Quests";
            }
        }
    }

    // Draw the gear's content entry at the given position in the list.
    void DrawEntry(const std::shared_ptr<ContentEntry<Gear>>& entry, const Vector2& position,
                   bool isSelected) override {
        // check the parameter
        if (entry == nullptr) {
            throw System::ArgumentNullException("entry");
        }
        const std::shared_ptr<Gear>& gear = entry->Content;
        if (gear == nullptr) {
            return;
        }

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 drawPosition = position;

        // draw the icon
        spriteBatch.Draw(*gear->IconTexture, drawPosition + iconOffset_, Color::White);

        // draw the name
        Color color = isSelected ? Fonts::HighlightColor : Fonts::DisplayColor;
        drawPosition.Y += (float)listLineSpacing / 4.0f;
        drawPosition.X += (float)NameColumnInterval;
        spriteBatch.DrawString(Fonts::GearInfoFont(), gear->Name, drawPosition, color);

        // draw the power
        drawPosition.X += (float)PowerColumnInterval;
        std::string powerText = gear->GetPowerText();
        Vector2 powerTextSize = Fonts::GearInfoFont().MeasureString(powerText);
        Vector2 powerPosition = drawPosition;
        powerPosition.Y -= std::ceil((powerTextSize.Y - 30.0f) / 2.0f);
        spriteBatch.DrawString(Fonts::GearInfoFont(), powerText, powerPosition, color);

        // draw the quantity
        drawPosition.X += (float)QuantityColumnInterval;
        spriteBatch.DrawString(Fonts::GearInfoFont(), System::Int32::ToString(entry->Count),
                               drawPosition, color);

        // turn on or off the select and drop buttons
        if (isSelected) {
            selectButtonText_ = "Select";
            xButtonText_ = entry->Content->IsDroppable ? "Drop" : std::string();
        }
    }

    // Draw the description of the selected item.
    void DrawSelectedDescription(const std::shared_ptr<ContentEntry<Gear>>& entry) override {
        // check the parameter
        if (entry == nullptr) {
            throw System::ArgumentNullException("entry");
        }
        const std::shared_ptr<Gear>& gear = entry->Content;
        if (gear == nullptr) {
            return;
        }

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 position = descriptionTextPosition_;

        // draw the description
        // -- it's up to the content owner to fit the description
        std::string text = gear->Description;
        if (!text.empty()) {
            spriteBatch.DrawString(Fonts::DescriptionFont(), text, position,
                                   Fonts::DescriptionColor);
            position.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
        }

        // draw additional information for equipment
        auto equipment = std::dynamic_pointer_cast<Equipment>(entry->Content);
        if (equipment != nullptr) {
            // draw the modifiers
            text = equipment->OwnerBuffStatistics.GetModifierString();
            if (!text.empty()) {
                spriteBatch.DrawString(Fonts::DescriptionFont(), text, position,
                                       Fonts::DescriptionColor);
                position.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
            }
        }

        // draw the restrictions
        text = entry->Content->GetRestrictionsText();
        if (!text.empty()) {
            spriteBatch.DrawString(Fonts::DescriptionFont(), text, position,
                                   Fonts::DescriptionColor);
            position.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
        }
    }

    // Draw the column headers above the gear list.
    void DrawColumnHeaders() override {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 position = listEntryStartPosition_;

        position.X += (float)NameColumnInterval;
        if (!nameColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), nameColumnText_, position,
                                   Fonts::CaptionColor);
        }

        position.X += (float)PowerColumnInterval;
        if (!powerColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), powerColumnText_, position,
                                   Fonts::CaptionColor);
        }

        position.X += (float)QuantityColumnInterval;
        if (!quantityColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), quantityColumnText_, position,
                                   Fonts::CaptionColor);
        }
    }

    std::string nameColumnText_ = "Name";
    static constexpr int NameColumnInterval = 80;

    std::string powerColumnText_ = "Power (min, max)";
    static constexpr int PowerColumnInterval = 270;

    std::string quantityColumnText_ = "Qty";
    static constexpr int QuantityColumnInterval = 450;

    // If true, the menu is only displaying items; otherwise, only equipment.
    bool isItems_;

private:
    std::vector<std::shared_ptr<ContentEntry<Gear>>> dataList_;
};

} // namespace RolePlaying
