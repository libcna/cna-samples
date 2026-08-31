// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ScreenManager/GameScreen.hpp"
#include "Screens/MenuEntry.hpp"

namespace GameStateManagement
{
    /** @brief Base screen for a vertical menu of selectable entries. */
    class MenuScreen : public GameScreen
    {
    public:
        /** @brief Constructs a menu. @param menuTitle Displayed title. */
        explicit MenuScreen(std::string menuTitle);
        /** @brief Handles menu navigation and selection. @param input Current input. */
        void HandleInput(InputState& input) override;
        /** @brief Updates transitions and entry selection fades. @param gameTime Timing. @param otherScreenHasFocus Focus state. @param coveredByOtherScreen Cover state. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime,
                    bool otherScreenHasFocus, bool coveredByOtherScreen) override;
        /** @brief Draws entries and title. @param gameTime Timing. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    protected:
        /** @brief Gets mutable menu entries. @return Entry list. */
        [[nodiscard]] std::vector<std::shared_ptr<MenuEntry>>& getMenuEntriesProperty();
        /** @brief Selects an entry. @param entryIndex Entry index. @param playerIndex Triggering player. */
        virtual void OnSelectEntry(int entryIndex,
                                   Microsoft::Xna::Framework::PlayerIndex playerIndex);
        /** @brief Cancels the menu. @param playerIndex Triggering player. */
        virtual void OnCancel(Microsoft::Xna::Framework::PlayerIndex playerIndex);
        /** @brief Event-handler overload for cancel. @param sender Event sender. @param e Event data. */
        void OnCancel(System::Object* sender, const PlayerIndexEventArgs& e);
        /** @brief Positions all entries for the current transition. */
        virtual void UpdateMenuEntryLocations();

    private:
        std::vector<std::shared_ptr<MenuEntry>> menuEntries_;
        int selectedEntry_ = 0;
        std::string menuTitle_;
    };
}
