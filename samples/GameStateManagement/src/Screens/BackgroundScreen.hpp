// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "ScreenManager/GameScreen.hpp"

namespace GameStateManagement
{
    /** @brief Draws the fixed background behind menu screens. */
    class BackgroundScreen final : public GameScreen
    {
    public:
        /** @brief Constructs the background screen. */
        BackgroundScreen();
        /** @brief Loads the background with a screen-local content manager. */
        void LoadContent() override;
        /** @brief Unloads screen-local content. */
        void UnloadContent() override;
        /** @brief Updates without transitioning off when covered. @param gameTime Timing. @param otherScreenHasFocus Focus state. @param coveredByOtherScreen Ignored cover state. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime,
                    bool otherScreenHasFocus, bool coveredByOtherScreen) override;
        /** @brief Draws the background. @param gameTime Timing. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Gets the fully qualified logical type name. @return Type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        std::unique_ptr<Microsoft::Xna::Framework::Content::ContentManager> content_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> backgroundTexture_;
    };
}
