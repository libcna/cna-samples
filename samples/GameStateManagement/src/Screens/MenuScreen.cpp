// SPDX-License-Identifier: MS-PL

#include "Screens/MenuScreen.hpp"

#include <cmath>
#include <utility>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "ScreenManager/InputState.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace GameStateManagement
{
    using namespace Microsoft::Xna::Framework;
    using Microsoft::Xna::Framework::Graphics::SpriteEffects;

    MenuScreen::MenuScreen(std::string menuTitle) : menuTitle_(std::move(menuTitle))
    {
        setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(0.5));
        setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(0.5));
    }

    void MenuScreen::HandleInput(InputState& input)
    {
        if (input.IsMenuUp(getControllingPlayerProperty()))
        {
            --selectedEntry_;
            if (selectedEntry_ < 0)
                selectedEntry_ = static_cast<int>(menuEntries_.size()) - 1;
        }
        if (input.IsMenuDown(getControllingPlayerProperty()))
        {
            ++selectedEntry_;
            if (selectedEntry_ >= static_cast<int>(menuEntries_.size()))
                selectedEntry_ = 0;
        }

        PlayerIndex playerIndex;
        if (input.IsMenuSelect(getControllingPlayerProperty(), playerIndex))
            OnSelectEntry(selectedEntry_, playerIndex);
        else if (input.IsMenuCancel(getControllingPlayerProperty(), playerIndex))
            OnCancel(playerIndex);
    }

    void MenuScreen::Update(GameTime& gameTime, bool otherScreenHasFocus,
                            bool coveredByOtherScreen)
    {
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
        for (std::size_t i = 0; i < menuEntries_.size(); ++i)
            menuEntries_[i]->Update(*this,
                getIsActiveProperty() && static_cast<int>(i) == selectedEntry_, gameTime);
    }

    void MenuScreen::Draw(const GameTime& gameTime)
    {
        UpdateMenuEntryLocations();
        auto& manager = getScreenManagerProperty();
        auto& graphics = manager.getGraphicsDeviceProperty();
        auto& spriteBatch = manager.getSpriteBatchProperty();
        auto& font = manager.getFontProperty();
        spriteBatch.Begin();
        for (std::size_t i = 0; i < menuEntries_.size(); ++i)
            menuEntries_[i]->Draw(*this,
                getIsActiveProperty() && static_cast<int>(i) == selectedEntry_, gameTime);

        const float transitionOffset = static_cast<float>(
            std::pow(getTransitionPositionProperty(), 2.0f));
        Vector2 titlePosition(graphics.getViewportProperty().getWidthProperty() / 2.0f, 80.0f);
        const Vector2 titleOrigin = font.MeasureString(menuTitle_) / 2.0f;
        const Color titleColor = Color(192, 192, 192) * getTransitionAlphaProperty();
        titlePosition.Y -= transitionOffset * 100.0f;
        spriteBatch.DrawString(font, menuTitle_, titlePosition, titleColor, 0.0f, titleOrigin,
                               1.25f, SpriteEffects::None, 0.0f);
        spriteBatch.End();
    }

    std::vector<std::shared_ptr<MenuEntry>>& MenuScreen::getMenuEntriesProperty()
    { return menuEntries_; }
    void MenuScreen::OnSelectEntry(int entryIndex, PlayerIndex playerIndex)
    { menuEntries_[entryIndex]->OnSelectEntry(playerIndex); }
    void MenuScreen::OnCancel(PlayerIndex) { ExitScreen(); }
    void MenuScreen::OnCancel(System::Object*, const PlayerIndexEventArgs& e)
    { OnCancel(e.getPlayerIndexProperty()); }

    void MenuScreen::UpdateMenuEntryLocations()
    {
        const float transitionOffset = static_cast<float>(
            std::pow(getTransitionPositionProperty(), 2.0f));
        Vector2 position(0.0f, 175.0f);
        for (const auto& entry : menuEntries_)
        {
            position.X = getScreenManagerProperty().getGraphicsDeviceProperty()
                .getViewportProperty().getWidthProperty() / 2.0f - entry->GetWidth(*this) / 2.0f;
            position.X += getScreenStateProperty() == ScreenState::TransitionOn
                ? -transitionOffset * 256.0f : transitionOffset * 512.0f;
            entry->setPositionProperty(position);
            position.Y += static_cast<float>(entry->GetHeight(*this));
        }
    }
}
