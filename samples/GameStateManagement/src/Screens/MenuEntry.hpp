// SPDX-License-Identifier: MS-PL
#pragma once

#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/EventHandler.hpp"
#include "System/Object.hpp"
#include "Screens/PlayerIndexEventArgs.hpp"

namespace GameStateManagement
{
    class MenuScreen;

    /** @brief Represents one selectable text entry in a menu screen. */
    class MenuEntry : public System::Object
    {
    public:
        /** @brief Raised when the entry is selected. */
        System::EventHandler<PlayerIndexEventArgs> Selected;

        /** @brief Constructs an entry. @param text Initial text. */
        explicit MenuEntry(std::string text);
        /** @brief Destroys the entry. */
        ~MenuEntry() override = default;
        /** @brief Gets entry text. @return Text. */
        [[nodiscard]] const std::string& getTextProperty() const;
        /** @brief Sets entry text. @param value New text. */
        void setTextProperty(std::string value);
        /** @brief Gets draw position. @return Position. */
        [[nodiscard]] Microsoft::Xna::Framework::Vector2 getPositionProperty() const;
        /** @brief Sets draw position. @param value New position. */
        void setPositionProperty(Microsoft::Xna::Framework::Vector2 value);
        /** @brief Raises Selected. @param playerIndex Triggering player. */
        virtual void OnSelectEntry(Microsoft::Xna::Framework::PlayerIndex playerIndex);
        /** @brief Updates selection fade. @param screen Owning menu. @param isSelected Selection state. @param gameTime Timing. */
        virtual void Update(MenuScreen& screen, bool isSelected,
                            Microsoft::Xna::Framework::GameTime& gameTime);
        /** @brief Draws the entry. @param screen Owning menu. @param isSelected Selection state. @param gameTime Timing. */
        virtual void Draw(MenuScreen& screen, bool isSelected,
                          const Microsoft::Xna::Framework::GameTime& gameTime);
        /** @brief Gets required height. @param screen Owning menu. @return Height. */
        [[nodiscard]] virtual int GetHeight(MenuScreen& screen) const;
        /** @brief Gets required width. @param screen Owning menu. @return Width. */
        [[nodiscard]] virtual int GetWidth(MenuScreen& screen) const;

        /** @brief Gets the fully qualified logical type name. @return Type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        std::string text_;
        float selectionFade_ = 0.0f;
        Microsoft::Xna::Framework::Vector2 position_;
    };
}
