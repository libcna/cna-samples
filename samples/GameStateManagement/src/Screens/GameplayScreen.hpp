// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/Random.hpp"
#include "ScreenManager/GameScreen.hpp"

namespace GameStateManagement
{
    /** @brief Implements the placeholder gameplay used to demonstrate screen state changes. */
    class GameplayScreen final : public GameScreen
    {
    public:
        /** @brief Constructs the gameplay screen. */
        GameplayScreen();
        /** @brief Loads gameplay content and simulates a slow load. */
        void LoadContent() override;
        /** @brief Unloads gameplay content. */
        void UnloadContent() override;
        /** @brief Updates enemy motion and pause fade. @param gameTime Timing. @param otherScreenHasFocus Focus state. @param coveredByOtherScreen Cover state. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime,
                    bool otherScreenHasFocus, bool coveredByOtherScreen) override;
        /** @brief Handles movement and pause. @param input Current input. */
        void HandleInput(InputState& input) override;
        /** @brief Draws placeholder gameplay. @param gameTime Timing. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Gets the fully qualified logical type name. @return Type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        std::unique_ptr<Microsoft::Xna::Framework::Content::ContentManager> content_;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> gameFont_;
        Microsoft::Xna::Framework::Vector2 playerPosition_{100.0f, 100.0f};
        Microsoft::Xna::Framework::Vector2 enemyPosition_{100.0f, 100.0f};
        System::Random random_;
        float pauseAlpha_ = 0.0f;
    };
}
