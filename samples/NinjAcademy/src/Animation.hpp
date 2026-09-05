// SPDX-License-Identifier: MS-PL
#pragma once

// Animation.hpp — C++ port of NinjAcademyCommonTypes/Animation.cs (XNA 4.0
// NinjAcademy sample). Supports animation playback; Update()/Draw() must be
// called explicitly, it is not an independent game component.
//
// Faithful-to-original quirk (see missing.md): Update() only advances the
// frame when frameChangeTimer >= frameChangeInterval, but frameChangeTimer is
// only ever reset to Zero -- it is never incremented by elapsed game time,
// in the original C# too. This is reproduced as-is rather than "fixed".

#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/TimeSpan.hpp"
#include "System/ArgumentException.hpp"
#include "System/InvalidOperationException.hpp"

namespace NinjAcademy {

struct NinjAcademyContentReaderRegistrationEXT;

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Point;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Port of NinjAcademyCommonTypes/Animation.cs.
class Animation {
public:
    Animation() = default;

    Animation(Point frameDimensions, Point rowAndColumnAmount, Vector2 visualCenter, bool isCyclic)
        : frameSize_(frameDimensions), rowAndColumnAmount_(rowAndColumnAmount),
          frameCount_(rowAndColumnAmount.X * rowAndColumnAmount.Y), visualCenter_(visualCenter),
          isCyclic_(isCyclic) {}

    // Creates a new animation, deferring sheet loading to LoadSheet().
    Animation(std::string animationSheetPath, Point frameDimensions, Point rowAndColumnAmount, Vector2 visualCenter,
              bool isCyclic)
        : Animation(frameDimensions, rowAndColumnAmount, visualCenter, isCyclic) {
        animationSheetPath_ = std::move(animationSheetPath);
    }

    // Creates a new animation with an already-loaded sheet.
    Animation(Texture2D animationSheet, Point frameDimensions, Point rowAndColumnAmount, Vector2 visualCenter,
              bool isCyclic)
        : Animation(frameDimensions, rowAndColumnAmount, visualCenter, isCyclic) {
        animationSheet_ = std::move(animationSheet);
        hasSheet_ = true;
    }

    // Copy constructor: shares the loaded sheet (or the path to load it from)
    // but resets playback state, matching the C# original.
    Animation(const Animation& source)
        : animationSheetPath_(source.hasSheet_ ? std::string() : source.animationSheetPath_),
          frameSize_(source.frameSize_), rowAndColumnAmount_(source.rowAndColumnAmount_),
          frameCount_(source.frameCount_), visualCenter_(source.visualCenter_), isCyclic_(source.isCyclic_),
          animationSheet_(source.animationSheet_), hasSheet_(source.hasSheet_) {}

    const Texture2D& getAnimationSheetProperty() const { return animationSheet_; }
    int getFrameCountProperty() const { return frameCount_; }
    int getFrameWidthProperty() const { return frameSize_.X; }
    int getFrameHeightProperty() const { return frameSize_.Y; }
    int getFrameIndexProperty() const { return rowAndColumnAmount_.X * currentFrame_.Y + currentFrame_.X; }
    Vector2 getVisualCenterProperty() const { return visualCenter_; }
    bool getIsActiveProperty() const { return isActive_; }
    void setIsActiveProperty(bool value) { isActive_ = value; }

    void setFrameIndexProperty(int value) {
        if (value >= rowAndColumnAmount_.X * rowAndColumnAmount_.Y + 1) {
            throw System::InvalidOperationException("Specified frame index exceeds available frames");
        }
        currentFrame_.Y = value / rowAndColumnAmount_.X;
        currentFrame_.X = value % rowAndColumnAmount_.X;
    }

    // Loads the animation's sheet using the specified content manager. Call
    // before rendering, unless the sheet was supplied directly.
    void LoadSheet(ContentManager& contentManager) {
        if (animationSheetPath_.empty()) {
            throw System::ArgumentException(
                "Cannot load the animation sheet from an empty path. Did you supply the animation sheet directly?");
        }
        animationSheet_ = contentManager.Load<Texture2D>(animationSheetPath_);
        hasSheet_ = true;
    }

    void Update(const GameTime& gameTime) {
        (void)gameTime;
        if (!isActive_)
            return;

        // See if it is time to advance to the next frame.
        if (frameChangeTimer_ >= frameChangeInterval_) {
            frameChangeTimer_ = System::TimeSpan::Zero;

            currentFrame_.X++;
            if (currentFrame_.X >= rowAndColumnAmount_.X) {
                currentFrame_.X = 0;
                currentFrame_.Y++;
            }

            if (getFrameIndexProperty() >= frameCount_) {
                if (isCyclic_) {
                    setFrameIndexProperty(0);
                } else {
                    isActive_ = false;
                    setFrameIndexProperty(frameCount_ - 1);
                }
            }
        }
    }

    void Draw(SpriteBatch& spriteBatch, Vector2 position) {
        Draw(spriteBatch, position, 0.0f, Vector2::Zero, 1.0f, SpriteEffects::None, 0.0f);
    }

    void Draw(SpriteBatch& spriteBatch, Vector2 position, float rotation, Vector2 origin, float scale,
              SpriteEffects spriteEffect, float layerDepth) {
        spriteBatch.Draw(animationSheet_, position,
                         Rectangle(frameSize_.X * currentFrame_.X, frameSize_.Y * currentFrame_.Y, frameSize_.X,
                                   frameSize_.Y),
                         Color::White, rotation, origin, scale, spriteEffect, layerDepth);
    }

    // Causes the animation to start playing from a specified frame index.
    void PlayFromFrameIndex(int frameIndex) {
        setFrameIndexProperty(frameIndex);
        isActive_ = true;
    }

    // Sets the interval between frames.
    void SetFrameInterval(System::TimeSpan interval) { frameChangeInterval_ = interval; }

private:
    friend struct NinjAcademyContentReaderRegistrationEXT;

    std::string animationSheetPath_;
    Point rowAndColumnAmount_;
    Point currentFrame_;
    Point frameSize_;

    System::TimeSpan frameChangeTimer_ = System::TimeSpan::Zero;
    System::TimeSpan frameChangeInterval_ = System::TimeSpan::Zero;

    bool isCyclic_ = false;

    int frameCount_ = 0;
    Vector2 visualCenter_;
    bool isActive_ = true;

    Texture2D animationSheet_;
    bool hasSheet_ = false;
};

} // namespace NinjAcademy
