// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"

namespace Microsoft::Xna::Framework::Content
{
    class ContentManager;
}

namespace RacingGame::Graphics
{
    class ResolutionMapper;
    class Texture;

    /** @brief Renders the original Racing `GameFont` bitmap atlas. */
    class TextureFont
    {
    public:
        /**
         * @brief Loads the authentic bitmap font and creates its sprite batch.
         * @param device Graphics device used for sprite rendering.
         * @param content Content manager used to load `GameFont.xnb`.
         * @param mapper Original Racing screen-coordinate mapper.
         * @param alphaSprite Shared alpha sprite batch used by the texture wrapper.
         * @param additiveSprite Shared additive sprite batch used by the texture wrapper.
         */
        TextureFont(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                    Microsoft::Xna::Framework::Content::ContentManager& content,
                    ResolutionMapper& mapper,
                    Microsoft::Xna::Framework::Graphics::SpriteBatch& alphaSprite,
                    Microsoft::Xna::Framework::Graphics::SpriteBatch& additiveSprite);
        /** @brief Destroys the owned original texture wrapper. */
        ~TextureFont();

        /** @brief Gets the scaled font height excluding its top correction. */
        [[nodiscard]] int getHeightProperty() const;
        /** @brief Measures text using the bitmap atlas's stored advances. */
        [[nodiscard]] int GetTextWidth(const std::string& text) const;
        /** @brief Queues tinted text at a top-left position. */
        void WriteText(int x, int y, const std::string& text,
                       Microsoft::Xna::Framework::Color color);
        /** @brief Queues white text at a top-left position. */
        void WriteText(int x, int y, const std::string& text);
        /** @brief Queues centered white text. */
        void WriteTextCentered(int x, int y, const std::string& text);
        /** @brief Queues centered tinted and scaled text. */
        void WriteTextCentered(int x, int y, const std::string& text,
                               Microsoft::Xna::Framework::Color color,
                               float scale);
        /** @brief Formats and queues a race time in minutes, seconds and hundredths. */
        void WriteGameTime(int x, int y, int timeMilliseconds,
                           Microsoft::Xna::Framework::Color color);
        /** @brief Draws and clears every queued text entry. */
        void WriteAll();
        /** @brief Gets the number of queued text entries. */
        [[nodiscard]] int getPendingTextCountProperty() const;
        /** @brief Gets glyph draws emitted by the most recent flush. */
        [[nodiscard]] int getLastGlyphCountProperty() const;

    private:
        struct FontToRender
        {
            int x;
            int y;
            std::string text;
            Microsoft::Xna::Framework::Color color;
            float scale;
        };

        static constexpr int FontHeight = 36;
        static constexpr int SubRenderHeight = 5;
        static const std::array<Microsoft::Xna::Framework::Rectangle, 95>
            CharacterRectangles;

        ResolutionMapper& mapper;
        std::unique_ptr<Texture> fontTexture;
        Microsoft::Xna::Framework::Graphics::SpriteBatch fontSprite;
        std::vector<FontToRender> remainingTexts;
        int lastGlyphCount = 0;

        [[nodiscard]] static int Round(double value);
    };
}
