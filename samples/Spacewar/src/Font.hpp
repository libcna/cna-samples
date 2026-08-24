// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Vector4.hpp"

namespace Microsoft::Xna::Framework
{
    class Game;
    namespace Graphics
    {
        class SpriteBatch;
    }
}

namespace Spacewar
{
    enum class FontStyle
    {
        Score,
        WeaponLarge,
        WeaponSmall,
        GameCountDown,
        GamePlayerNames,
        ScoreButtons,
        WeaponIcons,
        HealthBar,
        ShipNames,
    };

    class Font
    {
    public:
        static void Init(Microsoft::Xna::Framework::Game* game);
        static void Dispose();
        static void Begin();
        static void End();
        static void Draw(FontStyle fontStyle, int x, int y, int number,
                         Microsoft::Xna::Framework::Vector4 color);
        static void Draw(FontStyle fontStyle, int x, int y, int number);
        static void Draw(FontStyle fontStyle, int x, int y, const std::string& digits);
        static void Draw(FontStyle fontStyle, int x, int y, const std::string& digits,
                         Microsoft::Xna::Framework::Vector4 color);

    private:
        struct FontInfo
        {
            std::string Filename;
            std::string Characters;
            int StartOffset;
            int CharacterSpacing;
            int CharacterWidth;
            int CharacterHeight;
        };

        static std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> batch_;
        static const std::array<FontInfo, 9> fontInfo_;
    };
}
