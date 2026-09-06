// SPDX-License-Identifier: MS-PL
#pragma once

// FontManager.hpp — C++ port of ShipGame/FontManager.cs (XNA 4.0 Ship Game
// Starter Kit). The one SpriteBatch the game draws all of its 2D through, plus
// the three Arial sizes it loads.

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/IDisposable.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::SpriteSortMode;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// supported font types and sizes
enum class FontType {
    ArialSmall = 0,
    ArialMedium,
    ArialLarge
};

// Port of ShipGame/FontManager.cs.
class FontManager : public System::IDisposable {
public:
    // Create a new font manager
    explicit FontManager(GraphicsDevice* gd) {
        if (gd == nullptr) {
            throw System::ArgumentNullException("gd");
        }

        graphics_ = gd;
        sprite_.emplace(*gd);
        textMode_ = false;
    }

    ~FontManager() override { Dispose(true); }

    // Load resources
    void LoadContent(ContentManager& content) {
        fonts_.push_back(content.Load<SpriteFont>("fonts/ArialS"));
        fonts_.push_back(content.Load<SpriteFont>("fonts/ArialM"));
        fonts_.push_back(content.Load<SpriteFont>("fonts/ArialL"));
    }

    // Free resources
    void UnloadContent() { fonts_.clear(); }

    // Get the current screen rectangle
    Rectangle getScreenRectangle() const {
        return Rectangle(graphics_->getViewportProperty().getXProperty(),
                         graphics_->getViewportProperty().getYProperty(),
                         graphics_->getViewportProperty().getWidthProperty(),
                         graphics_->getViewportProperty().getHeightProperty());
    }

    // Enter text mode
    void BeginText() {
        sprite_->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend);
        textMode_ = true;
    }

    // Drawn text using given font, position and color
    void DrawText(FontType font, const std::string& text, Vector2 position, Color color) {
        if (textMode_)
            sprite_->DrawString(fonts_[(int)font], text, position, color);
    }

    // End text mode
    void EndText() {
        sprite_->End();
        textMode_ = false;
    }

    // Draw a texture in screen
    void DrawTexture(Texture2D* texture, Rectangle rect, Color color, BlendState blend) {
        if (textMode_)
            sprite_->End();

        sprite_->Begin(SpriteSortMode::Immediate, blend);
        sprite_->Draw(*texture, rect, color);
        sprite_->End();

        if (textMode_)
            sprite_->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend);
    }

    // Draw a texture with rotation
    void DrawTexture(Texture2D* texture, Rectangle rect, float rotation, Color color,
                     BlendState blend) {
        if (textMode_)
            sprite_->End();

        rect.X += rect.Width / 2;
        rect.Y += rect.Height / 2;

        sprite_->Begin(SpriteSortMode::Immediate, blend);
        sprite_->Draw(*texture, rect, std::nullopt, color, rotation,
                      Vector2((float)(rect.Width / 2), (float)(rect.Height / 2)),
                      SpriteEffects::None, 0);
        sprite_->End();

        if (textMode_)
            sprite_->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend);
    }

    // Draw a texture with source and destination rectangles
    void DrawTexture(Texture2D* texture, Rectangle destinationRect, Rectangle sourceRect,
                     Color color, BlendState blend) {
        if (textMode_)
            sprite_->End();

        sprite_->Begin(SpriteSortMode::Immediate, blend);
        sprite_->Draw(*texture, destinationRect, sourceRect, color);
        sprite_->End();

        if (textMode_)
            sprite_->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend);
    }

    // ---- IDisposable Members ----

    bool IsDisposed() const { return isDisposed_; }

    void Dispose() override { Dispose(true); }

private:
    void Dispose(bool disposing) {
        if (disposing && !isDisposed_) {
            sprite_.reset();
        }
    }

    bool isDisposed_ = false;

    GraphicsDevice* graphics_ = nullptr; // graphics device
    std::optional<SpriteBatch> sprite_;  // sprite bacth
    std::vector<SpriteFont> fonts_;      // list of sprite fonts
    bool textMode_ = false;              // in text mode?
};

} // namespace ShipGame
