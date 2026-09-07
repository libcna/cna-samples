#pragma once

// ListScreen.hpp -- C++ port of GameScreens/ListScreen.cs.
//
// The original is `abstract class ListScreen<T>`; this is the same class template. C#'s
// ReadOnlyCollection<T> return becomes a const reference to the live vector, which is what a
// read-only view over an existing list is in C++ -- the original never copies the list either.

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/Int32.hpp"

#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"
#include "../Session/Session.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Texture2D;

template <typename T>
class ListScreen : public GameScreen {
public:
    // Constructs a new ListScreen object.
    ListScreen() { SetIsPopup(true); }

    // Get the list that this screen displays.
    virtual const std::vector<T>& GetDataList() = 0;

    // The index of the selected entry.
    int SelectedIndex() const { return selectedIndex_; }
    void SetSelectedIndex(int value) {
        if (selectedIndex_ != value) {
            selectedIndex_ = value;
            EnsureVisible(selectedIndex_);
        }
    }

    // Ensure that the given index is visible on the screen.
    void EnsureVisible(int index) {
        if (index < startIndex_) {
            // if it's above the current selection, set the first entry
            startIndex_ = index;
        }
        if (selectedIndex_ > (endIndex_ - 1)) {
            startIndex_ += selectedIndex_ - (endIndex_ - 1);
        }
        // otherwise, it should be in the current selection already
        // -- note that the start and end indices are checked in Draw.
    }

    // The first index displayed on the screen from the list.
    int StartIndex() const { return startIndex_; }
    void SetStartIndex(int value) { startIndex_ = value; } // safety-checked in Draw

    // The last index displayed on the screen from the list.
    int EndIndex() const { return endIndex_; }
    void SetEndIndex(int value) { endIndex_ = value; } // safety-checked in Draw

    // The maximum number of list entries that the screen can show at once.
    static constexpr int MaximumListEntries = 4;

    void LoadContent() override {
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();

        // load the background textures
        fadeTexture_ = content.Load<Texture2D>("Textures/GameScreens/FadeScreen");
        backgroundTexture_ = content.Load<Texture2D>("Textures/GameScreens/GameScreenBkgd");
        listTexture_ = content.Load<Texture2D>("Textures/GameScreens/InfoDisplay");
        plankTexture_ = content.Load<Texture2D>("Textures/MainMenu/MainMenuPlank03");
        goldTexture_ = content.Load<Texture2D>("Textures/GameScreens/GoldIcon");

        // load the foreground textures
        highlightTexture_ = content.Load<Texture2D>("Textures/GameScreens/HighlightLarge");
        selectionArrowTexture_ = content.Load<Texture2D>("Textures/GameScreens/SelectionArrow");

        // load the trigger images
        leftTriggerTexture_ = content.Load<Texture2D>("Textures/Buttons/LeftTriggerButton");
        rightTriggerTexture_ = content.Load<Texture2D>("Textures/Buttons/RightTriggerButton");
        leftQuantityArrowTexture_ = content.Load<Texture2D>("Textures/Buttons/QuantityArrowLeft");
        rightQuantityArrowTexture_ = content.Load<Texture2D>("Textures/Buttons/QuantityArrowRight");
        backButtonTexture_ = content.Load<Texture2D>("Textures/Buttons/BButton");
        selectButtonTexture_ = content.Load<Texture2D>("Textures/Buttons/AButton");
        xButtonTexture_ = content.Load<Texture2D>("Textures/Buttons/XButton");
        yButtonTexture_ = content.Load<Texture2D>("Textures/Buttons/YButton");

        // calculate the centered positions
        auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        plankTexturePosition_ =
            Vector2((float)viewport.getXProperty() + (float)(viewport.getWidthProperty() -
                                                             plankTexture_.getWidthProperty()) / 2.0f,
                    67.0f);

        // adjust positions for texture sizes
        backButtonTextPosition_.X += (float)backButtonTexture_.getWidthProperty();
        xButtonTextPosition_.X += (float)xButtonTexture_.getWidthProperty();

        GameScreen::LoadContent();
    }

    void HandleInput() override {
        if (InputManager::IsActionTriggered(InputManager::Action::PageLeft)) {
            PageScreenLeft();
        } else if (InputManager::IsActionTriggered(InputManager::Action::PageRight)) {
            PageScreenRight();
        } else if (InputManager::IsActionTriggered(InputManager::Action::CursorUp)) {
            MoveCursorUp();
        } else if (InputManager::IsActionTriggered(InputManager::Action::CursorDown)) {
            MoveCursorDown();
        } else if (InputManager::IsActionTriggered(InputManager::Action::IncreaseAmount)) {
            MoveCursorRight();
        } else if (InputManager::IsActionTriggered(InputManager::Action::DecreaseAmount)) {
            MoveCursorLeft();
        } else if (InputManager::IsActionTriggered(InputManager::Action::Back)) {
            BackTriggered();
        } else if (InputManager::IsActionTriggered(InputManager::Action::Ok)) {
            const std::vector<T>& dataList = GetDataList();
            if (selectedIndex_ >= 0 && selectedIndex_ < (int)dataList.size()) {
                SelectTriggered(dataList[(std::size_t)selectedIndex_]);
            }
        } else if (InputManager::IsActionTriggered(InputManager::Action::DropUnEquip)) {
            const std::vector<T>& dataList = GetDataList();
            if (selectedIndex_ >= 0 && selectedIndex_ < (int)dataList.size()) {
                ButtonXPressed(dataList[(std::size_t)selectedIndex_]);
            }
        } else if (InputManager::IsActionTriggered(InputManager::Action::TakeView)) {
            const std::vector<T>& dataList = GetDataList();
            if (selectedIndex_ >= 0 && selectedIndex_ < (int)dataList.size()) {
                ButtonYPressed(dataList[(std::size_t)selectedIndex_]);
            }
        }
        GameScreen::HandleInput();
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        // get the content list
        const std::vector<T>& dataList = GetDataList();

        // turn off the buttons if the list is empty
        if (dataList.empty()) {
            selectButtonText_.clear();
            xButtonText_.clear();
            yButtonText_.clear();
        }

        // fix the indices for the current list size
        SetSelectedIndex(
            (int)MathHelper::Clamp((float)SelectedIndex(), 0.0f, (float)dataList.size() - 1.0f));
        startIndex_ = (int)MathHelper::Clamp((float)startIndex_, 0.0f,
                                             (float)dataList.size() - (float)MaximumListEntries);
        endIndex_ = std::min(startIndex_ + MaximumListEntries, (int)dataList.size());

        spriteBatch.Begin();

        DrawBackground();
        if (!dataList.empty()) {
            DrawListPosition(SelectedIndex() + 1, (int)dataList.size());
        }
        DrawButtons();
        DrawPartyGold();
        DrawColumnHeaders();
        DrawTitle();

        // draw each item currently shown
        Vector2 position = listEntryStartPosition_ + Vector2(0.0f, (float)listLineSpacing / 2.0f);
        if (startIndex_ >= 0) {
            for (int index = startIndex_; index < endIndex_; index++) {
                const T& entry = dataList[(std::size_t)index];
                if (index == selectedIndex_) {
                    DrawSelection(position);
                    DrawEntry(entry, position, true);
                    DrawSelectedDescription(entry);
                } else {
                    DrawEntry(entry, position, false);
                }
                position.Y += (float)listLineSpacing;
            }
        }

        spriteBatch.End();
    }

protected:
    // Move the current selection up one entry.
    virtual void MoveCursorUp() {
        if (SelectedIndex() > 0) {
            SetSelectedIndex(SelectedIndex() - 1);
        }
    }

    // Move the current selection down one entry.
    virtual void MoveCursorDown() {
        SetSelectedIndex(SelectedIndex() + 1); // safety-checked in Draw()
    }

    // Decrease the selected quantity by one.
    virtual void MoveCursorLeft() {}

    // Increase the selected quantity by one.
    virtual void MoveCursorRight() {}

    // Switch to the screen to the "left" of this one in the UI, if any.
    virtual void PageScreenLeft() {}

    // Switch to the screen to the "right" of this one in the UI, if any.
    virtual void PageScreenRight() {}

    // Respond to the triggering of the Back action.
    virtual void BackTriggered() { ExitScreen(); }

    // Respond to the triggering of the Select action.
    virtual void SelectTriggered(const T& entry) { (void)entry; }

    // Respond to the triggering of the X button (and related key).
    virtual void ButtonXPressed(const T& entry) { (void)entry; }

    // Respond to the triggering of the Y button (and related key).
    virtual void ButtonYPressed(const T& entry) { (void)entry; }

    // Draw the entry at the given position in the list.
    virtual void DrawEntry(const T& entry, const Vector2& position, bool isSelected) = 0;

    // Draw the description of the selected item.
    virtual void DrawSelectedDescription(const T& entry) = 0;

    // Draw the column headers above the list.
    virtual void DrawColumnHeaders() = 0;

    // Draw the selection graphics over the selected item.
    virtual void DrawSelection(const Vector2& position) {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        spriteBatch.Draw(highlightTexture_, Vector2(highlightStartPosition_.X, position.Y),
                         Color::White);
        spriteBatch.Draw(selectionArrowTexture_,
                         Vector2(selectionArrowPosition_.X, position.Y + 10.0f), Color::White);
    }

    // Draw the background of the screen.
    virtual void DrawBackground() {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        spriteBatch.Draw(fadeTexture_, backgroundDestination_, Color::White);
        spriteBatch.Draw(backgroundTexture_, backgroundDestination_, Color::White);
        spriteBatch.Draw(listTexture_, listTexturePosition_, Color::White);
    }

    // Draw the current list position in the appropriate location on the screen.
    virtual void DrawListPosition(int position, int total) {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        // draw the top number - the current position in the list
        std::string listPositionTopText = System::Int32::ToString(position);
        Vector2 drawPosition = listPositionTopPosition_;
        drawPosition.X -=
            std::ceil(Fonts::GearInfoFont().MeasureString(listPositionTopText).X / 2.0f);
        spriteBatch.DrawString(Fonts::GearInfoFont(), listPositionTopText, drawPosition,
                               Fonts::CountColor);

        // draw the bottom number - the current position in the list
        std::string listPositionBottomText = System::Int32::ToString(total);
        drawPosition = listPositionBottomPosition_;
        drawPosition.X -=
            std::ceil(Fonts::GearInfoFont().MeasureString(listPositionBottomText).X / 2.0f);
        spriteBatch.DrawString(Fonts::GearInfoFont(), listPositionBottomText, drawPosition,
                               Fonts::CountColor);
    }

    // Draw the party gold text.
    virtual void DrawPartyGold() {
        if (!IsActive()) {
            return;
        }

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        spriteBatch.Draw(goldTexture_, goldTexturePosition_, Color::White);
        spriteBatch.DrawString(Fonts::ButtonNamesFont(),
                               Fonts::GetGoldString(Session::GetParty()->PartyGold()),
                               goldTextPosition_, Color::White);
    }

    // Draw all of the buttons used by the screen.
    virtual void DrawButtons() {
        if (!IsActive()) {
            return;
        }

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        // draw the left trigger texture and text
        if (!leftTriggerText_.empty()) {
            Vector2 position =
                leftTriggerTexturePosition_ +
                Vector2((float)leftTriggerTexture_.getWidthProperty() / 2.0f -
                            std::ceil(Fonts::PlayerStatisticsFont().MeasureString(leftTriggerText_).X /
                                      2.0f),
                        90.0f);
            spriteBatch.Draw(leftTriggerTexture_, leftTriggerTexturePosition_, Color::White);
            spriteBatch.DrawString(Fonts::PlayerStatisticsFont(), leftTriggerText_, position,
                                   Color::Black);
        }

        // draw the right trigger texture and text
        if (!rightTriggerText_.empty()) {
            Vector2 position =
                rightTriggerTexturePosition_ +
                Vector2((float)rightTriggerTexture_.getWidthProperty() / 2.0f -
                            std::ceil(
                                Fonts::PlayerStatisticsFont().MeasureString(rightTriggerText_).X /
                                2.0f),
                        90.0f);
            spriteBatch.Draw(rightTriggerTexture_, rightTriggerTexturePosition_, Color::White);
            spriteBatch.DrawString(Fonts::PlayerStatisticsFont(), rightTriggerText_, position,
                                   Color::Black);
        }

        // draw the back button texture and text
        if (!backButtonText_.empty()) {
            spriteBatch.Draw(backButtonTexture_, backButtonTexturePosition_, Color::White);
            spriteBatch.DrawString(Fonts::ButtonNamesFont(), backButtonText_,
                                   backButtonTextPosition_, Color::White);
        }

        // draw the select button texture and text
        if (!selectButtonText_.empty()) {
            spriteBatch.Draw(selectButtonTexture_, selectButtonTexturePosition_, Color::White);
            Vector2 position =
                selectButtonTexturePosition_ -
                Vector2(Fonts::ButtonNamesFont().MeasureString(selectButtonText_).X, 0.0f) +
                Vector2(0.0f, 5.0f);
            spriteBatch.DrawString(Fonts::ButtonNamesFont(), selectButtonText_, position,
                                   Color::White);
        }

        // draw the X button texture and text
        if (!xButtonText_.empty()) {
            spriteBatch.Draw(xButtonTexture_, xButtonTexturePosition_, Color::White);
            spriteBatch.DrawString(Fonts::ButtonNamesFont(), xButtonText_, xButtonTextPosition_,
                                   Color::White);
        }

        // draw the Y button texture and text
        if (!yButtonText_.empty()) {
            spriteBatch.Draw(yButtonTexture_, yButtonTexturePosition_, Color::White);
            Vector2 position =
                yButtonTexturePosition_ -
                Vector2(Fonts::ButtonNamesFont().MeasureString(yButtonText_).X, 0.0f) +
                Vector2(0.0f, 5.0f);
            spriteBatch.DrawString(Fonts::ButtonNamesFont(), yButtonText_, position, Color::White);
        }
    }

    // Draw the title of the screen, if any.
    virtual void DrawTitle() {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        if (!titleText_.empty()) {
            Vector2 titleTextSize = Fonts::HeaderFont().MeasureString(titleText_);
            auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
            Vector2 position(std::floor((float)viewport.getXProperty() +
                                        (float)(viewport.getWidthProperty() / 2) -
                                        titleTextSize.X / 2.0f),
                             90.0f);
            spriteBatch.Draw(plankTexture_, plankTexturePosition_, Color::White);
            spriteBatch.DrawString(Fonts::HeaderFont(), titleText_, position, Fonts::TitleColor);
        }
    }

    const Vector2 iconOffset_{0.0f, 0.0f};
    const Vector2 descriptionTextPosition_{200.0f, 550.0f};

    const Vector2 listEntryStartPosition_{200.0f, 202.0f};
    static constexpr int listLineSpacing = 76;

    std::string titleText_;

    Texture2D goldTexture_;
    std::string goldText_;

    std::string leftTriggerText_;
    std::string rightTriggerText_;
    std::string backButtonText_;
    std::string selectButtonText_;
    std::string xButtonText_;
    std::string yButtonText_;

    Texture2D leftQuantityArrowTexture_;
    Texture2D rightQuantityArrowTexture_;

private:
    const Vector2 listPositionTopPosition_{1160.0f, 354.0f};
    const Vector2 listPositionBottomPosition_{1160.0f, 384.0f};

    Texture2D backgroundTexture_;
    const Rectangle backgroundDestination_{0, 0, 1280, 720};
    Texture2D fadeTexture_;

    Texture2D listTexture_;
    const Vector2 listTexturePosition_{187.0f, 180.0f};

    Texture2D plankTexture_;
    Vector2 plankTexturePosition_;

    const Vector2 goldTexturePosition_{490.0f, 640.0f};
    const Vector2 goldTextPosition_{565.0f, 648.0f};

    Texture2D highlightTexture_;
    const Vector2 highlightStartPosition_{170.0f, 237.0f};
    Texture2D selectionArrowTexture_;
    const Vector2 selectionArrowPosition_{135.0f, 245.0f};

    Texture2D leftTriggerTexture_;
    const Vector2 leftTriggerTexturePosition_{340.0f, 50.0f};

    Texture2D rightTriggerTexture_;
    const Vector2 rightTriggerTexturePosition_{900.0f, 50.0f};

    Texture2D backButtonTexture_;
    const Vector2 backButtonTexturePosition_{80.0f, 640.0f};
    Vector2 backButtonTextPosition_{90.0f, 645.0f}; // + tex width

    Texture2D selectButtonTexture_;
    const Vector2 selectButtonTexturePosition_{1150.0f, 640.0f};

    Texture2D xButtonTexture_;
    const Vector2 xButtonTexturePosition_{240.0f, 640.0f};
    Vector2 xButtonTextPosition_{250.0f, 645.0f}; // + tex width

    Texture2D yButtonTexture_;
    const Vector2 yButtonTexturePosition_{890.0f, 640.0f};

    int selectedIndex_ = 0;
    int startIndex_ = 0;
    int endIndex_ = 0;
};

} // namespace RolePlaying
