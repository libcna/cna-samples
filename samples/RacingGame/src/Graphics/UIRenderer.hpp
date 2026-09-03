// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"

namespace Microsoft::Xna::Framework::Content
{
    class ContentManager;
}

namespace Microsoft::Xna::Framework::Graphics
{
    class GraphicsDevice;
    class SpriteBatch;
}

namespace RacingGame::GameLogic
{
    enum class TimeFadeupMode;
}

namespace RacingGame::Graphics
{
    class ResolutionMapper;
    class Texture;
    class TextureFont;
    class TextureFontBigNumbers;

    /** @brief Renders the original Racing in-race HUD from its authentic atlases. */
    class UIRenderer
    {
    public:
        /**
         * @brief Loads the authentic in-game and bitmap-font content.
         * @param device Graphics device receiving HUD sprites.
         * @param content Content manager used to load the XNB assets.
         */
        UIRenderer(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
            Microsoft::Xna::Framework::Content::ContentManager& content);
        /** @brief Destroys owned UI helpers after their concrete types are complete. */
        ~UIRenderer();

        /** @brief Adds a checkpoint or lap time that rises and fades out. */
        void AddTimeFadeupEffect(int timeMilliseconds,
                                 GameLogic::TimeFadeupMode mode);
        /** @brief Queues centered result text for the end-of-frame font flush. */
        void WriteTextCentered(
            int x, int y, const std::string& text,
            Microsoft::Xna::Framework::Color color, float scale);
        /** @brief Draws the complete original desktop in-race HUD. */
        void RenderGameUI(
            int currentGameTime, int bestLapTime, int lapNumber,
            float speed, int gear, float acceleration,
            const std::string& trackName,
            const std::array<int, 10>& topLapTimes,
            bool gameOver, float elapsedMilliseconds);

        /** @brief Gets atlas sprites emitted by the latest HUD render. */
        [[nodiscard]] int getLastAtlasSpriteCountProperty() const;
        /** @brief Gets bitmap-font glyphs emitted by the latest HUD render. */
        [[nodiscard]] int getLastGlyphCountProperty() const;
        /** @brief Gets text entries queued for the latest HUD font flush. */
        [[nodiscard]] int getLastTextCountProperty() const;
        /** @brief Gets active checkpoint/lap time fade-up entries. */
        [[nodiscard]] int getFadeupCountProperty() const;
        /** @brief Gets the authentic in-game atlas width. */
        [[nodiscard]] int getIngameTextureWidthProperty() const;
        /** @brief Gets the authentic in-game atlas height. */
        [[nodiscard]] int getIngameTextureHeightProperty() const;
        /** @brief Gets the surface format preserved from the authentic in-game XNB. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::SurfaceFormat
        getIngameTextureFormatProperty() const;

        /** @brief Source rectangle for the lap counter panel. */
        static const Microsoft::Xna::Framework::Rectangle LapsGfxRect;
        /** @brief Source rectangle for the tachometer panel. */
        static const Microsoft::Xna::Framework::Rectangle TachoGfxRect;
        /** @brief Source rectangle for the tachometer needle. */
        static const Microsoft::Xna::Framework::Rectangle TachoArrowGfxRect;
        /** @brief Source rectangle locating the mph digit region. */
        static const Microsoft::Xna::Framework::Rectangle TachoMphGfxRect;
        /** @brief Source rectangle locating the gear digit region. */
        static const Microsoft::Xna::Framework::Rectangle TachoGearGfxRect;
        /** @brief Source rectangle for current and best times. */
        static const Microsoft::Xna::Framework::Rectangle CurrentAndBestGfxRect;
        /** @brief Source rectangle for the track-name panel. */
        static const Microsoft::Xna::Framework::Rectangle TrackNameGfxRect;
        /** @brief Source rectangle for one top-five time panel. */
        static const Microsoft::Xna::Framework::Rectangle Best5GfxRect;

    private:
        struct TimeFadeupText
        {
            static constexpr float MaxShowTimeMilliseconds = 2250.0f;
            std::string text;
            Microsoft::Xna::Framework::Color color;
            float showTimeMilliseconds = MaxShowTimeMilliseconds;
        };

        std::unique_ptr<ResolutionMapper> mapper;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch>
            alphaSprite;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch>
            additiveSprite;
        std::unique_ptr<Texture> ingame;
        std::unique_ptr<TextureFont> font;
        std::unique_ptr<TextureFontBigNumbers> bigNumbers;
        std::vector<TimeFadeupText> fadeupTexts;
        int lastAtlasSpriteCount = 0;
        int lastGlyphCount = 0;
        int lastTextCount = 0;

        void RenderTimeFadeupEffects(float elapsedMilliseconds);
        [[nodiscard]] static std::string FormatTime(int timeMilliseconds);
    };
}
