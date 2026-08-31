// SPDX-License-Identifier: MS-PL
#pragma once

#include <stdexcept>

#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::Point;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;

// A simple sprite-sheet frame animator. Advances exactly one frame per Update
// (matching the XNA original, whose "Speed" attribute is unused at runtime).
class Animation {
public:
    Animation(const Texture2D& frameSheet, Point size, Point frameSheetSize)
        : animatedCharacter_(&frameSheet), frameSize_(size), sheetSize_(frameSheetSize) {}

    Point getFrameSizeProperty() const { return frameSize_; }
    void setFrameSizeProperty(Point value) { frameSize_ = value; }

    int getFrameCountProperty() const { return sheetSize_.X * sheetSize_.Y; }

    Vector2 getOffsetProperty() const { return offset_; }
    void setOffsetProperty(Vector2 value) { offset_ = value; }

    int getFrameIndexProperty() const {
        return sheetSize_.X * currentFrame_.Y + currentFrame_.X;
    }
    void setFrameIndexProperty(int value) {
        if (value >= sheetSize_.X * sheetSize_.Y + 1) {
            throw std::runtime_error("Specified frame index exeeds available frames");
        }
        currentFrame_.Y = value / sheetSize_.X;
        currentFrame_.X = value % sheetSize_.X;
    }

    bool getIsActiveProperty() const { return isActive_; }

    void Update() {
        if (!isActive_)
            return;

        if (getFrameIndexProperty() >= getFrameCountProperty() - 1) {
            isActive_ = false;
            setFrameIndexProperty(getFrameCountProperty() - 1);
        } else {
            currentFrame_.X++;
            if (currentFrame_.X >= sheetSize_.X) {
                currentFrame_.X = 0;
                currentFrame_.Y++;
            }
            if (currentFrame_.Y >= sheetSize_.Y)
                currentFrame_.Y = 0;
        }
    }

    void Draw(SpriteBatch& spriteBatch, Vector2 position, float scale, SpriteEffects spriteEffect) {
        spriteBatch.Draw(*animatedCharacter_, position + offset_,
                         Rectangle(frameSize_.X * currentFrame_.X,
                                   frameSize_.Y * currentFrame_.Y,
                                   frameSize_.X, frameSize_.Y),
                         Color::White, 0.0f, Vector2::Zero, scale, spriteEffect, 0.0f);
    }

    void PlayFromFrameIndex(int frameIndex) {
        setFrameIndexProperty(frameIndex);
        isActive_ = true;
    }

private:
    const Texture2D* animatedCharacter_;
    Point sheetSize_;
    Point currentFrame_{0, 0};
    Point frameSize_{0, 0};
    Vector2 offset_ = Vector2::Zero;
    bool isActive_ = false;
};

} // namespace CatapultGame
