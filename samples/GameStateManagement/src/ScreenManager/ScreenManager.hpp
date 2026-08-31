// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "ScreenManager/GameScreen.hpp"
#include "ScreenManager/InputState.hpp"

namespace GameStateManagement
{
    /** @brief Manages the stack of game screens and routes update, draw and input. */
    class ScreenManager final : public Microsoft::Xna::Framework::DrawableGameComponent
    {
    public:
        /** @brief Constructs a manager for a game. @param game Owning game. */
        explicit ScreenManager(Microsoft::Xna::Framework::Game& game);
        /** @brief Gets the shared sprite batch. @return Shared sprite batch. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::SpriteBatch& getSpriteBatchProperty();
        /** @brief Gets the shared menu font. @return Shared font. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::SpriteFont& getFontProperty();
        /** @brief Gets debug screen tracing. @return true when enabled. */
        [[nodiscard]] bool getTraceEnabledProperty() const;
        /** @brief Sets debug screen tracing. @param value New trace state. */
        void setTraceEnabledProperty(bool value);
        /** @brief Initializes the component. */
        void Initialize() override;
        /** @brief Loads manager and existing-screen content. */
        void LoadContent() override;
        /** @brief Unloads all screen-owned content. */
        void UnloadContent() override;
        /** @brief Updates every screen and routes input. @param gameTime Timing. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Draws every non-hidden screen. @param gameTime Timing. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Adds a screen. @param screen Screen to add. @param controllingPlayer Player or any. */
        void AddScreen(std::shared_ptr<GameScreen> screen,
                       std::optional<Microsoft::Xna::Framework::PlayerIndex> controllingPlayer);
        /** @brief Removes a screen immediately. @param screen Screen to remove. */
        void RemoveScreen(GameScreen* screen);
        /** @brief Returns a copy of the current screen list. @return Current screens. */
        [[nodiscard]] std::vector<std::shared_ptr<GameScreen>> GetScreens() const;
        /** @brief Draws a translucent black fullscreen overlay. @param alpha Overlay alpha. */
        void FadeBackBufferToBlack(float alpha);

        /** @brief Gets the fully qualified logical type name. @return Type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        void TraceScreens() const;
        static void EraseByPointer(std::vector<std::shared_ptr<GameScreen>>& screens,
                                   GameScreen* screen);

        std::vector<std::shared_ptr<GameScreen>> screens_;
        std::vector<std::shared_ptr<GameScreen>> screensToUpdate_;
        InputState input_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch_;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> blankTexture_;
        bool isInitialized_ = false;
        bool traceEnabled_ = false;
    };
}
