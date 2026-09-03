// SPDX-License-Identifier: MS-PL

#include "Graphics/TextureFont.hpp"

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>

#include "Graphics/ResolutionMapper.hpp"
#include "Graphics/Texture.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"

namespace RacingGame::Graphics
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using namespace Microsoft::Xna::Framework::Graphics;

    const std::array<Rectangle, 95> TextureFont::CharacterRectangles{{
        {0, 0, 1, 8}, {1, 0, 11, 10}, {12, 0, 14, 13},
        {26, 0, 20, 18}, {46, 0, 20, 18}, {66, 0, 24, 22},
        {90, 0, 25, 23}, {115, 0, 8, 7}, {124, 0, 10, 9},
        {136, 0, 10, 9}, {146, 0, 20, 18}, {166, 0, 20, 18},
        {186, 0, 10, 8}, {196, 0, 10, 9}, {207, 0, 10, 8},
        {217, 0, 18, 16}, {235, 0, 20, 19},

        {0, 36, 20, 18}, {20, 36, 20, 18}, {40, 36, 20, 18},
        {60, 36, 21, 19}, {81, 36, 20, 18}, {101, 36, 20, 18},
        {121, 36, 20, 18}, {141, 36, 20, 18}, {161, 36, 20, 18},
        {181, 36, 10, 8}, {191, 36, 10, 8}, {201, 36, 20, 18},
        {221, 36, 20, 18},

        {0, 72, 20, 18}, {20, 72, 19, 17}, {39, 72, 26, 24},
        {65, 72, 22, 20}, {87, 72, 22, 20}, {109, 72, 22, 20},
        {131, 72, 23, 21}, {154, 72, 20, 18}, {174, 72, 19, 17},
        {193, 72, 23, 21}, {216, 72, 23, 21}, {239, 72, 11, 10},

        {0, 108, 15, 13}, {15, 108, 22, 20}, {37, 108, 19, 17},
        {56, 108, 29, 26}, {85, 108, 23, 21}, {108, 108, 24, 22},
        {132, 108, 22, 20}, {154, 108, 24, 22}, {178, 108, 24, 22},
        {202, 108, 21, 19}, {223, 108, 17, 15},

        {0, 144, 22, 20}, {22, 144, 22, 20}, {44, 144, 30, 28},
        {74, 144, 22, 20}, {96, 144, 20, 18}, {116, 144, 20, 18},
        {136, 144, 10, 9}, {146, 144, 18, 16}, {167, 144, 10, 9},
        {177, 144, 17, 16}, {194, 144, 17, 16}, {211, 144, 17, 16},
        {228, 144, 20, 18},

        {0, 180, 20, 18}, {20, 180, 18, 16}, {38, 180, 20, 18},
        {58, 180, 20, 18}, {79, 180, 14, 12}, {93, 180, 20, 18},
        {114, 180, 19, 18}, {133, 180, 11, 10}, {145, 180, 11, 10},
        {156, 180, 20, 18}, {176, 180, 11, 9}, {187, 180, 29, 27},
        {216, 180, 20, 18}, {236, 180, 20, 19},

        {0, 216, 20, 18}, {20, 216, 20, 18}, {40, 216, 13, 12},
        {53, 216, 17, 16}, {70, 216, 14, 11}, {84, 216, 19, 18},
        {104, 216, 17, 16}, {122, 216, 25, 23}, {148, 216, 19, 17},
        {168, 216, 18, 16}, {186, 216, 16, 15}, {203, 216, 10, 9},
        {214, 216, 12, 11}, {227, 216, 10, 9}, {237, 216, 18, 17},
    }};

    TextureFont::TextureFont(
        GraphicsDevice& device, ContentManager& content,
        ResolutionMapper& setMapper, SpriteBatch& alphaSprite,
        SpriteBatch& additiveSprite)
        : mapper(setMapper),
          fontTexture(std::make_unique<Texture>(
              content, mapper, alphaSprite, additiveSprite, "GameFont.png")),
          fontSprite(device)
    {
    }

    TextureFont::~TextureFont() = default;

    int TextureFont::Round(const double value)
    {
        return static_cast<int>(std::nearbyint(value));
    }

    int TextureFont::getHeightProperty() const
    {
        return mapper.YToRes1050(FontHeight - SubRenderHeight);
    }

    int TextureFont::GetTextWidth(const std::string& text) const
    {
        int width = 0;
        for (const unsigned char character : text)
        {
            if (character >= 32 && character <= 126)
            {
                width += mapper.XToRes1400(
                    CharacterRectangles[character - 32].Height);
            }
        }
        return width;
    }

    void TextureFont::WriteText(
        const int x, const int y, const std::string& text, const Color color)
    {
        remainingTexts.push_back({x, y, text, color, 1.0f});
    }

    void TextureFont::WriteText(
        const int x, const int y, const std::string& text)
    {
        WriteText(x, y, text, Color::White);
    }

    void TextureFont::WriteTextCentered(
        const int x, const int y, const std::string& text)
    {
        WriteText(
            x - GetTextWidth(text) / 2,
            y - getHeightProperty() / 2, text);
    }

    void TextureFont::WriteTextCentered(
        const int x, const int y, const std::string& text,
        const Color color, const float scale)
    {
        const int width = GetTextWidth(text);
        remainingTexts.push_back({
            x - Round(width * scale / 2.0),
            y - Round(getHeightProperty() * scale / 2.0),
            text, color, scale});
    }

    void TextureFont::WriteGameTime(
        const int x, const int y, const int timeMilliseconds,
        const Color color)
    {
        const int absolute = std::abs(timeMilliseconds);
        std::ostringstream text;
        if (timeMilliseconds < 0)
            text << '-';
        text << (absolute / 1000) / 60 << ':'
             << std::setw(2) << std::setfill('0')
             << (absolute / 1000) % 60 << '.'
             << std::setw(2) << std::setfill('0')
             << (absolute / 10) % 100;
        WriteText(x, y, text.str(), color);
    }

    void TextureFont::WriteAll()
    {
        lastGlyphCount = 0;
        if (remainingTexts.empty())
            return;

        fontSprite.Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend);
        for (const FontToRender& fontText : remainingTexts)
        {
            int x = fontText.x;
            for (const unsigned char character : fontText.text)
            {
                if (character < 32 || character > 126)
                    continue;

                Rectangle source = CharacterRectangles[character - 32];
                source.Y += 1;
                source.Height = FontHeight;
                Rectangle destination(
                    x, fontText.y - mapper.YToRes1050(SubRenderHeight),
                    source.Width, source.Height);
                destination.Width = mapper.XToRes1400(
                    Round(destination.Width * fontText.scale));
                destination.Height = mapper.YToRes1050(
                    Round(destination.Height * fontText.scale));
                fontSprite.Draw(
                    fontTexture->getXnaTextureProperty(), destination, source,
                    fontText.color);
                ++lastGlyphCount;

                const int characterWidth =
                    CharacterRectangles[character - 32].Height;
                x += mapper.XToRes1400(
                    Round(characterWidth * fontText.scale));
            }
        }
        fontSprite.End();
        remainingTexts.clear();
    }

    int TextureFont::getPendingTextCountProperty() const
    {
        return static_cast<int>(remainingTexts.size());
    }

    int TextureFont::getLastGlyphCountProperty() const
    {
        return lastGlyphCount;
    }
}
