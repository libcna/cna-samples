// SPDX-License-Identifier: MS-PL

#include "Game1.hpp"

#include "CNA/Platform/CannedGamepad.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

using Microsoft::Xna::Framework::Color;
using CNA::Platform::GamepadButton;
using CNA::Platform::GamepadSnapshot;
using CNA::Platform::Testing::CannedGamepadPlatform;
using CNA::Platform::Testing::ScopedCurrentPlatform;

namespace
{
    struct Bounds
    {
        int minX = 0;
        int minY = 0;
        int maxX = -1;
        int maxY = -1;
        int changed = 0;
    };

    class ProbeGame final : public TiledSprites::Game1
    {
    public:
        std::vector<Color> lastFrame;
        int frameWidth = 0;
        int frameHeight = 0;
        int frameCount = 0;

    protected:
        void EndDraw() override
        {
            auto& device = getGraphicsDeviceProperty();
            const auto& viewport = device.getViewportProperty();
            frameWidth = viewport.getWidthProperty();
            frameHeight = viewport.getHeightProperty();
            lastFrame.resize(static_cast<std::size_t>(frameWidth * frameHeight));
            device.GetBackBufferData(lastFrame.data(), frameWidth * frameHeight);
            ++frameCount;
            TiledSprites::Game1::EndDraw();
        }
    };

    Bounds Measure(const ProbeGame& game)
    {
        const int width = game.frameWidth;
        const int height = game.frameHeight;

        Bounds bounds;
        bounds.minX = width;
        bounds.minY = height;
        const Color clear = Color::CornflowerBlue;
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                if (game.lastFrame[static_cast<std::size_t>(y * width + x)] == clear)
                {
                    continue;
                }
                bounds.minX = std::min(bounds.minX, x);
                bounds.minY = std::min(bounds.minY, y);
                bounds.maxX = std::max(bounds.maxX, x);
                bounds.maxY = std::max(bounds.maxY, y);
                ++bounds.changed;
            }
        }
        return bounds;
    }

    void Publish(CannedGamepadPlatform& platform, const GamepadButton button, const bool pressed)
    {
        GamepadSnapshot snapshot;
        snapshot.connected = true;
        snapshot.buttons = pressed ? static_cast<std::uint32_t>(button) : 0u;
        platform.Canned().SetPendingSnapshot(0, snapshot);
        platform.Canned().Update();
    }

    Bounds Advance(ProbeGame& game, CannedGamepadPlatform& platform,
                   const GamepadButton button)
    {
        Publish(platform, button, true);
        platform.Delay(40);
        game.RunOneFrame();
        Publish(platform, button, false);
        platform.Delay(20);
        game.RunOneFrame();
        return Measure(game);
    }

    void Print(const char* label, const Bounds& bounds)
    {
        std::cout << label << '\t' << bounds.minX << '\t' << bounds.minY << '\t'
                  << bounds.maxX << '\t' << bounds.maxY << '\t' << bounds.changed << '\n';
    }
}

int main()
{
    CannedGamepadPlatform platform;
    Publish(platform, GamepadButton::A, false);

    ProbeGame game;
    ScopedCurrentPlatform installed(platform);
    platform.Delay(40);
    game.RunOneFrame();
    const Bounds initial = Measure(game);
    const Bounds afterB = Advance(game, platform, GamepadButton::B);
    const Bounds afterA = Advance(game, platform, GamepadButton::A);
    const Bounds afterX = Advance(game, platform, GamepadButton::X);
    const Bounds afterY = Advance(game, platform, GamepadButton::Y);
    const int framesBeforeBack = game.frameCount;
    Publish(platform, GamepadButton::Back, true);
    platform.Delay(20);
    game.RunOneFrame();
    const bool backExited = game.frameCount == framesBeforeBack;

    Print("initial-2x2", initial);
    Print("after-B", afterB);
    Print("after-A", afterA);
    Print("after-X", afterX);
    Print("after-Y", afterY);
    std::cout << "back-exit-suppressed-draw\t" << (backExited ? "true" : "false") << '\n';

    const bool valid = initial.changed > 0
        && afterB.maxX > initial.maxX
        && afterB.maxY == initial.maxY
        && afterA.maxX == afterB.maxX
        && afterA.maxY > afterB.maxY
        && afterX.maxX < afterA.maxX
        && afterX.maxY == afterA.maxY
        && afterY.maxX == afterX.maxX
        && afterY.maxY < afterX.maxY
        && backExited;
    if (!valid)
    {
        std::cerr << "TiledSprites gamepad/render behavior mismatch\n";
        return 1;
    }

    std::cout << "TiledSprites behavior probe passed\n";
    return 0;
}
