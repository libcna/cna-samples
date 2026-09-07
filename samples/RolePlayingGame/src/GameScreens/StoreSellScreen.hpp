#pragma once

// StoreSellScreen.hpp -- C++ port of GameScreens/StoreSellScreen.cs.

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/Int32.hpp"

#include "../Data/Map/Store.hpp"
#include "../Session/Session.hpp"
#include "InventoryScreen.hpp"

namespace RolePlaying {

using RolePlayingGameData::Store;

// Displays the gear in the party inventory and allows the user to sell them.
class StoreSellScreen : public InventoryScreen {
public:
    // Creates a new StoreSellScreen object for the given store.
    explicit StoreSellScreen(const std::shared_ptr<Store>& store)
        : InventoryScreen(true), store_(store) {
        // check the parameter
        if (store == nullptr || store->StoreCategories.empty()) {
            throw System::ArgumentNullException("store");
        }

        // configure the menu text
        selectButtonText_ = "Sell";
        backButtonText_ = "Back";
        xButtonText_.clear();
        yButtonText_.clear();

        ResetQuantities();
    }

    void LoadContent() override {
        InventoryScreen::LoadContent();

        auto& content = GetScreenManager()->getGameProperty().getContentProperty();

        leftQuantityArrow_ = content.Load<Texture2D>("Textures/Buttons/QuantityArrowLeft");
        rightQuantityArrow_ = content.Load<Texture2D>("Textures/Buttons/QuantityArrowRight");
    }

protected:
    // Move the current selection up one entry.
    void MoveCursorUp() override {
        InventoryScreen::MoveCursorUp();
        ResetQuantities();
    }

    // Move the current selection down one entry.
    void MoveCursorDown() override {
        InventoryScreen::MoveCursorDown();
        ResetQuantities();
    }

    // Decrease the selected quantity by one.
    void MoveCursorLeft() override {
        const std::vector<std::shared_ptr<ContentEntry<Gear>>>& entries = GetDataList();
        if (!entries.empty()) {
            // loop to one if the selected quantity is already at maximum.
            if (selectedQuantity_ > 1) {
                selectedQuantity_--;
            } else {
                selectedQuantity_ = entries[(std::size_t)SelectedIndex()]->Count;
            }
        } else {
            selectedQuantity_ = 0;
        }
    }

    // Increase the selected quantity by one.
    void MoveCursorRight() override {
        const std::vector<std::shared_ptr<ContentEntry<Gear>>>& entries = GetDataList();
        if (!entries.empty()) {
            // loop to one if the selected quantity is already at maximum.
            selectedQuantity_ =
                selectedQuantity_ < entries[(std::size_t)SelectedIndex()]->Count
                    ? selectedQuantity_ + 1
                    : 1;
        } else {
            selectedQuantity_ = 0;
        }
    }

    // Respond to the triggering of the Select action.
    void SelectTriggered(const std::shared_ptr<ContentEntry<Gear>>& entry) override {
        // check the parameter
        if (entry == nullptr || entry->Content == nullptr) {
            return;
        }

        // make sure the selected quantity is valid
        selectedQuantity_ = std::min(selectedQuantity_, entry->Count);

        // add the gold to the party's inventory
        Session::GetParty()->AddPartyGold(
            selectedQuantity_ *
            (int)std::ceil((float)entry->Content->GoldValue * store_->SellMultiplier));

        // remove the items from the party's inventory
        Session::GetParty()->RemoveFromInventory(entry->Content, selectedQuantity_);

        // reset the quantities - either gold has gone down or the total was bad
        ResetQuantities();
    }

    // Switch to the previous store category.
    void PageScreenLeft() override {
        isItems_ = !isItems_;
        ResetTriggerText();
        ResetQuantities();
    }

    // Switch to the next store category.
    void PageScreenRight() override {
        isItems_ = !isItems_;
        ResetTriggerText();
        ResetQuantities();
    }

    // Reset the trigger button text to the names of the previous and next UI screens.
    void ResetTriggerText() override {
        leftTriggerText_ = rightTriggerText_ = isItems_ ? "Equipment" : "Items";
    }

    // Draw the entry at the given position in the list.
    void DrawEntry(const std::shared_ptr<ContentEntry<Gear>>& entry, const Vector2& position,
                   bool isSelected) override {
        // check the parameter
        if (entry == nullptr || entry->Content == nullptr) {
            throw System::ArgumentNullException("entry");
        }

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 drawPosition = position;

        // draw the icon
        spriteBatch.Draw(*entry->Content->IconTexture, drawPosition + iconOffset_, Color::White);

        // draw the name
        Color color = isSelected ? Fonts::HighlightColor : Fonts::DisplayColor;
        drawPosition.Y += (float)listLineSpacing / 4.0f;
        drawPosition.X += (float)NameColumnInterval;
        spriteBatch.DrawString(Fonts::GearInfoFont(), entry->Content->Name, drawPosition, color);

        // draw the power
        drawPosition.X += (float)PowerColumnInterval;
        std::string powerText = entry->Content->GetPowerText();
        Vector2 powerTextSize = Fonts::GearInfoFont().MeasureString(powerText);
        Vector2 powerPosition = drawPosition;
        powerPosition.Y -= std::ceil((powerTextSize.Y - 30.0f) / 2.0f);
        spriteBatch.DrawString(Fonts::GearInfoFont(), powerText, powerPosition, color);

        // draw the quantity
        drawPosition.X += (float)SellQuantityColumnInterval;
        if (isSelected) {
            Vector2 quantityPosition = drawPosition;
            // draw the left selection arrow
            quantityPosition.X -= (float)leftQuantityArrow_.getWidthProperty();
            spriteBatch.Draw(leftQuantityArrow_,
                             Vector2(quantityPosition.X, quantityPosition.Y - 4.0f), Color::White);
            quantityPosition.X += (float)leftQuantityArrow_.getWidthProperty();
            // draw the selected quantity ratio
            std::string quantityText = System::Int32::ToString(selectedQuantity_) + "/" +
                                       System::Int32::ToString(entry->Count);
            spriteBatch.DrawString(Fonts::GearInfoFont(), quantityText, quantityPosition, color);
            quantityPosition.X += Fonts::GearInfoFont().MeasureString(quantityText).X;
            // draw the right selection arrow
            spriteBatch.Draw(rightQuantityArrow_,
                             Vector2(quantityPosition.X, quantityPosition.Y - 4.0f), Color::White);
            quantityPosition.X += (float)rightQuantityArrow_.getWidthProperty();
            // draw the sell button
            selectButtonText_ = "Sell";
        } else {
            spriteBatch.DrawString(Fonts::GearInfoFont(), System::Int32::ToString(entry->Count),
                                   drawPosition, color);
        }

        // draw the price
        drawPosition.X += (float)PriceColumnInterval;
        std::string priceText;
        if (isSelected) {
            int totalPrice =
                selectedQuantity_ *
                (int)std::ceil((float)entry->Content->GoldValue * store_->SellMultiplier);
            priceText = System::Int32::ToString(totalPrice);
        } else {
            priceText = System::Int32::ToString(
                (int)std::ceil((float)entry->Content->GoldValue * store_->SellMultiplier));
        }
        spriteBatch.DrawString(Fonts::GearInfoFont(), priceText, drawPosition, color);
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

        position.X += (float)PowerColumnInterval;
        if (!powerColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), powerColumnText_, position,
                                   Fonts::CaptionColor);
        }

        position.X += (float)SellQuantityColumnInterval;
        if (!quantityColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), quantityColumnText_, position,
                                   Fonts::CaptionColor);
        }

        position.X += (float)PriceColumnInterval;
        if (!priceColumnText_.empty()) {
            spriteBatch.DrawString(Fonts::CaptionFont(), priceColumnText_, position,
                                   Fonts::CaptionColor);
        }
    }

private:
    // Resets the selected quantity to the maximum value for the selected entry.
    void ResetQuantities() { selectedQuantity_ = 1; }

    // The left-facing quantity arrow.
    Texture2D leftQuantityArrow_;

    // The right-facing quantity arrow.
    Texture2D rightQuantityArrow_;

    // This screen's own column layout; only the quantity column differs from InventoryScreen's.
    static constexpr int SellQuantityColumnInterval = 340;

    std::string priceColumnText_ = "Price";
    static constexpr int PriceColumnInterval = 120;

    // The store whose goods are being displayed.
    std::shared_ptr<Store> store_;

    // The selected quantity of the current entry.
    int selectedQuantity_ = 0;
};

} // namespace RolePlaying
