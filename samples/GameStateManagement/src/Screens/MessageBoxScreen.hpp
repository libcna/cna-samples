// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/EventHandler.hpp"
#include "ScreenManager/GameScreen.hpp"
#include "Screens/PlayerIndexEventArgs.hpp"

namespace GameStateManagement
{
    /** @brief Popup confirmation message box. */
    class MessageBoxScreen final : public GameScreen
    {
    public:
        /** @brief Raised when accepted. */
        System::EventHandler<PlayerIndexEventArgs> Accepted;
        /** @brief Raised when cancelled. */
        System::EventHandler<PlayerIndexEventArgs> Cancelled;

        /** @brief Constructs a box with usage text. @param message Message. */
        explicit MessageBoxScreen(const std::string& message);
        /** @brief Constructs a box. @param message Message. @param includeUsageText Whether to append controls. */
        MessageBoxScreen(const std::string& message, bool includeUsageText);
        /** @brief Loads the gradient texture. */
        void LoadContent() override;
        /** @brief Handles accept or cancel. @param input Current input. */
        void HandleInput(InputState& input) override;
        /** @brief Draws the popup. @param gameTime Timing. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Gets the fully qualified logical type name. @return Type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        std::string message_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> gradientTexture_;
    };
}
