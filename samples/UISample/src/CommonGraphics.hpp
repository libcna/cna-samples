// SPDX-License-Identifier: MS-PL
#pragma once

#include <cmath>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace UserInterfaceSample {

class CommonGraphics final {
public:
    static void DrawCenteredText(
        Microsoft::Xna::Framework::Graphics::SpriteBatch& batch,
        const Microsoft::Xna::Framework::Graphics::SpriteFont& font,
        const Microsoft::Xna::Framework::Rectangle& rectangle,
        const std::string& text,
        Microsoft::Xna::Framework::Color color) {
        if (!text.empty()) {
            const Microsoft::Xna::Framework::Vector2 size = font.MeasureString(text);
            const Microsoft::Xna::Framework::Vector2 topLeft(
                rectangle.getCenterProperty().X - size.X * 0.5f,
                rectangle.getCenterProperty().Y - size.Y * 0.5f);
            batch.DrawString(font, text, topLeft, color);
        }
    }

    static void DrawRectangle(
        Microsoft::Xna::Framework::Graphics::SpriteBatch& batch,
        Microsoft::Xna::Framework::Graphics::Texture2D& blankTexture,
        const Microsoft::Xna::Framework::Rectangle& rectangle,
        Microsoft::Xna::Framework::Color color) {
        (void)color;
        DrawSpriteLine(batch, blankTexture,
            Microsoft::Xna::Framework::Vector2(
                static_cast<float>(rectangle.getLeftProperty()),
                static_cast<float>(rectangle.getTopProperty())),
            Microsoft::Xna::Framework::Vector2(
                static_cast<float>(rectangle.getRightProperty()),
                static_cast<float>(rectangle.getTopProperty())));
        DrawSpriteLine(batch, blankTexture,
            Microsoft::Xna::Framework::Vector2(
                static_cast<float>(rectangle.getLeftProperty()),
                static_cast<float>(rectangle.getBottomProperty())),
            Microsoft::Xna::Framework::Vector2(
                static_cast<float>(rectangle.getRightProperty()),
                static_cast<float>(rectangle.getBottomProperty())));
        DrawSpriteLine(batch, blankTexture,
            Microsoft::Xna::Framework::Vector2(
                static_cast<float>(rectangle.getLeftProperty()),
                static_cast<float>(rectangle.getTopProperty())),
            Microsoft::Xna::Framework::Vector2(
                static_cast<float>(rectangle.getLeftProperty()),
                static_cast<float>(rectangle.getBottomProperty())));
        DrawSpriteLine(batch, blankTexture,
            Microsoft::Xna::Framework::Vector2(
                static_cast<float>(rectangle.getRightProperty()),
                static_cast<float>(rectangle.getTopProperty())),
            Microsoft::Xna::Framework::Vector2(
                static_cast<float>(rectangle.getRightProperty()),
                static_cast<float>(rectangle.getBottomProperty())));
    }

private:
    static void DrawSpriteLine(
        Microsoft::Xna::Framework::Graphics::SpriteBatch& batch,
        Microsoft::Xna::Framework::Graphics::Texture2D& blankTexture,
        Microsoft::Xna::Framework::Vector2 vector1,
        Microsoft::Xna::Framework::Vector2 vector2) {
        using Microsoft::Xna::Framework::Color;
        using Microsoft::Xna::Framework::Vector2;
        using Microsoft::Xna::Framework::Graphics::SpriteEffects;

        const float distance = Vector2::Distance(vector1, vector2);
        const float angle = std::atan2(vector2.Y - vector1.Y, vector2.X - vector1.X);
        batch.Draw(blankTexture, vector1, std::nullopt, Color::White, angle,
                   Vector2::Zero, Vector2(distance, 1.0f),
                   SpriteEffects::None, 0.0f);
    }
};

} // namespace UserInterfaceSample
