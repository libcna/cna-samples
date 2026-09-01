// SPDX-License-Identifier: MS-PL
#pragma once

// FpsCounter.hpp — C++ port of GameDebugTools/FpsCounter.cs (XNA 4.0
// PerformanceMeasuring sample). Component for FPS measurement and display.

#include <algorithm>
#include <cctype>
#include <string>

#include "System/Diagnostics/Stopwatch.hpp"
#include "System/InvalidOperationException.hpp"
#include "System/Text/StringBuilder.hpp"
#include "System/TimeSpan.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"

#include "DebugManager.hpp"
#include "IDebugCommandHost.hpp"
#include "Layout.hpp"
#include "StringBuilderExtensions.hpp"

namespace PerformanceMeasuring::GameDebugTools {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::DrawableGameComponent;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteFont;

// Component for FPS measurement and drawing. Port of GameDebugTools/FpsCounter.cs.
class FpsCounter : public DrawableGameComponent {
public:
    explicit FpsCounter(Game& game) : DrawableGameComponent(game) {
        stringBuilder_.EnsureCapacity(16);
    }

    [[nodiscard]] float getFpsProperty() const { return fps_; }
    [[nodiscard]] System::TimeSpan getSampleSpanProperty() const { return sampleSpan_; }
    void setSampleSpanProperty(System::TimeSpan value) { sampleSpan_ = value; }

    void Initialize() override {
        debugManager_ = getGameProperty().getServicesProperty().GetService<DebugManager>();
        if (debugManager_ == nullptr)
            throw System::InvalidOperationException("DebugManaer is not registered.");

        // Register the 'fps' command if a debug command host is registered.
        auto* host = getGameProperty().getServicesProperty().GetService<IDebugCommandHost>();
        if (host != nullptr) {
            host->RegisterCommand("fps", "FPS Counter",
                [this](IDebugCommandHost&, const std::string&, const std::vector<std::string>& args) {
                    CommandExecute(args);
                });
            setVisibleProperty(true);
        }

        fps_ = 0.0f;
        sampleFrames_ = 0;
        stopwatch_ = System::Diagnostics::Stopwatch::StartNew();
        stringBuilder_.setLengthProperty(0);

        DrawableGameComponent::Initialize();
    }

    void Update(GameTime&) override {
        if (stopwatch_.getElapsedProperty() > sampleSpan_) {
            fps_ = (float)sampleFrames_ / (float)stopwatch_.getElapsedProperty().getTotalSecondsProperty();

            stopwatch_.Reset();
            stopwatch_.Start();
            sampleFrames_ = 0;

            stringBuilder_.setLengthProperty(0);
            stringBuilder_.Append("FPS: ");
            StringBuilderExtensions::AppendNumber(stringBuilder_, fps_);
        }
    }

    void Draw(const GameTime& gameTime) override {
        sampleFrames_++;

        SpriteBatch& spriteBatch = debugManager_->getSpriteBatchProperty();
        SpriteFont& font = debugManager_->getDebugFontProperty();

        Vector2 size = font.MeasureString("X");
        Rectangle rc(0, 0, (int)(size.X * 14.0f), (int)(size.Y * 1.3f));

        Layout layout(getGraphicsDeviceProperty().getViewportProperty());
        rc = layout.Place(rc, 0.01f, 0.01f, Alignment::TopLeft);

        size = font.MeasureString(stringBuilder_.ToString());
        layout.ClientArea = rc;
        Vector2 pos = layout.Place(size, 0.0f, 0.1f, Alignment::Center);

        spriteBatch.Begin();
        spriteBatch.Draw(debugManager_->getWhiteTextureProperty(), rc, Color(0, 0, 0, 128));
        spriteBatch.DrawString(font, stringBuilder_.ToString(), pos, Color::White);
        spriteBatch.End();

        DrawableGameComponent::Draw(gameTime);
    }

private:
    void CommandExecute(const std::vector<std::string>& args) {
        if (args.empty())
            setVisibleProperty(!getVisibleProperty());

        for (const std::string& source : args) {
            std::string arg = source;
            std::transform(arg.begin(), arg.end(), arg.begin(),
                           [](unsigned char value) { return static_cast<char>(std::tolower(value)); });
            if (arg == "on")
                setVisibleProperty(true);
            else if (arg == "off")
                setVisibleProperty(false);
        }
    }

    DebugManager* debugManager_ = nullptr;
    System::Diagnostics::Stopwatch stopwatch_;
    int sampleFrames_ = 0;
    System::Text::StringBuilder stringBuilder_;
    float fps_ = 0.0f;
    System::TimeSpan sampleSpan_ = System::TimeSpan::FromSeconds(1.0);
};

} // namespace PerformanceMeasuring::GameDebugTools
