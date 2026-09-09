// SPDX-License-Identifier: MS-PL
#pragma once

// DebugManager.hpp — C++ port of GameDebugTools/DebugManager.cs (XNA 4.0
// PerformanceMeasuring sample). Holds shared graphics resources (SpriteBatch,
// white texture, debug font) used by the other GameDebugTools components.

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace PerformanceMeasuring::GameDebugTools {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::DrawableGameComponent;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::Texture2D;

/**
 * @brief Holds the graphics resources the other debug components draw with.
 *
 * Registers itself as a game service so `DebugCommandUI`, `FpsCounter` and `TimeRuler` can find
 * the one sprite batch, white texture and font rather than each creating its own.
 */
class DebugManager : public DrawableGameComponent {
public:
    /**
     * @brief Creates the manager and registers it as a service.
     *
     * @param game          Game the component belongs to.
     * @param debugFontName Content name of the font the debug components draw with.
     */
    DebugManager(Game& game, std::string debugFontName)
        : DrawableGameComponent(game), debugFontName_(std::move(debugFontName)) {
        game.getServicesProperty().AddService<DebugManager>(this);

        // This component doesn't need to be updated nor drawn.
        setEnabledProperty(false);
        setVisibleProperty(false);
    }

    /** @brief Gets the sprite batch shared by the debug components. @return The sprite batch. */
    SpriteBatch& getSpriteBatchProperty() { return *spriteBatch_; }

    /** @brief Gets a one-pixel white texture, used to fill rectangles. @return The texture. */
    Texture2D& getWhiteTextureProperty() { return *whiteTexture_; }

    /** @brief Gets the font the debug components draw text with. @return The font. */
    SpriteFont& getDebugFontProperty() { return *debugFont_; }

    /** @brief Creates the sprite batch and white texture and loads the debug font. */
    void LoadContent() override {
        spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());

        debugFont_.emplace(getGameProperty().getContentProperty().Load<SpriteFont>(debugFontName_));

        whiteTexture_ = std::make_unique<Texture2D>(getGraphicsDeviceProperty(), 1, 1);
        Color white = Color::White;
        whiteTexture_->SetData(&white, 1);

        DrawableGameComponent::LoadContent();
    }

private:
    std::string debugFontName_;

    std::unique_ptr<SpriteBatch> spriteBatch_;
    std::unique_ptr<Texture2D> whiteTexture_;
    std::optional<SpriteFont> debugFont_;
};

} // namespace PerformanceMeasuring::GameDebugTools
