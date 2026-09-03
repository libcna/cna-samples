// SPDX-License-Identifier: MS-PL

#pragma once

#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Microsoft::Xna::Framework::Content
{
    class ContentManager;
}

namespace Microsoft::Xna::Framework::Graphics
{
    class BlendState;
    class SpriteBatch;
}

namespace RacingGame::Graphics
{
    class ResolutionMapper;

    /** @brief Loads and draws one of the original Racing texture assets. */
    class Texture
    {
    public:
        /**
         * @brief Loads a texture from the authentic `Textures` content directory.
         * @param content Content manager used to load the XNB asset.
         * @param mapper Original Racing screen-coordinate mapper.
         * @param alphaSprite Shared alpha sprite batch.
         * @param additiveSprite Shared additive sprite batch.
         * @param filename Source filename or extensionless content name.
         */
        Texture(Microsoft::Xna::Framework::Content::ContentManager& content,
                ResolutionMapper& mapper,
                Microsoft::Xna::Framework::Graphics::SpriteBatch& alphaSprite,
                Microsoft::Xna::Framework::Graphics::SpriteBatch& additiveSprite,
                const std::string& filename);
        /**
         * @brief Wraps an existing texture value.
         * @param texture Texture value to wrap.
         * @param mapper Original Racing screen-coordinate mapper.
         * @param alphaSprite Shared alpha sprite batch.
         * @param additiveSprite Shared additive sprite batch.
         */
        Texture(Microsoft::Xna::Framework::Graphics::Texture2D texture,
                ResolutionMapper& mapper,
                Microsoft::Xna::Framework::Graphics::SpriteBatch& alphaSprite,
                Microsoft::Xna::Framework::Graphics::SpriteBatch& additiveSprite);

        /** @brief Gets the extensionless texture filename. */
        [[nodiscard]] const std::string& getFilenameProperty() const;
        /** @brief Gets texture width in pixels. */
        [[nodiscard]] int getWidthProperty() const;
        /** @brief Gets texture height in pixels. */
        [[nodiscard]] int getHeightProperty() const;
        /** @brief Gets the complete texture source rectangle. */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle
        getGfxRectangleProperty() const;
        /** @brief Gets half a texel in normalized texture coordinates. */
        [[nodiscard]] Microsoft::Xna::Framework::Vector2
        getHalfPixelSizeProperty() const;
        /** @brief Gets the wrapped XNA texture. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::Texture2D&
        getXnaTextureProperty();
        /** @brief Gets the wrapped XNA texture. */
        [[nodiscard]] const Microsoft::Xna::Framework::Graphics::Texture2D&
        getXnaTextureProperty() const;
        /** @brief Gets whether loading succeeded and the texture remains available. */
        [[nodiscard]] bool getValidProperty() const;
        /** @brief Gets whether the original format carries DXT alpha. */
        [[nodiscard]] bool getHasAlphaPixelsProperty() const;
        /** @brief Releases the wrapped texture reference. */
        void Dispose();

        /** @brief Draws a source rectangle into a screen rectangle. */
        void RenderOnScreen(const Microsoft::Xna::Framework::Rectangle& rectangle,
                            const Microsoft::Xna::Framework::Rectangle& pixelRectangle);
        /** @brief Draws an explicitly specified source region. */
        void RenderOnScreen(const Microsoft::Xna::Framework::Rectangle& rectangle,
                            int pixelX, int pixelY, int pixelWidth, int pixelHeight);
        /** @brief Draws the texture at its natural size. */
        void RenderOnScreen(Microsoft::Xna::Framework::Point position);
        /** @brief Draws the complete texture into a screen rectangle. */
        void RenderOnScreen(const Microsoft::Xna::Framework::Rectangle& rectangle);
        /** @brief Draws a source rectangle in the original 1024x640 coordinate space. */
        void RenderOnScreenRelative16To9(
            int relativeX, int relativeY,
            const Microsoft::Xna::Framework::Rectangle& pixelRectangle);
        /** @brief Draws a source rectangle in the original 1024x768 coordinate space. */
        void RenderOnScreenRelative4To3(
            int relativeX, int relativeY,
            const Microsoft::Xna::Framework::Rectangle& pixelRectangle);
        /** @brief Draws a source rectangle in the original 1600x1200 coordinate space. */
        void RenderOnScreenRelative1600(
            int relativeX, int relativeY,
            const Microsoft::Xna::Framework::Rectangle& pixelRectangle);
        /** @brief Draws a tinted source rectangle. */
        void RenderOnScreen(
            const Microsoft::Xna::Framework::Rectangle& rectangle,
            const Microsoft::Xna::Framework::Rectangle& pixelRectangle,
            Microsoft::Xna::Framework::Color color);
        /** @brief Draws a tinted source rectangle through the requested shared batch. */
        void RenderOnScreen(
            const Microsoft::Xna::Framework::Rectangle& rectangle,
            const Microsoft::Xna::Framework::Rectangle& pixelRectangle,
            Microsoft::Xna::Framework::Color color,
            const Microsoft::Xna::Framework::Graphics::BlendState& blendState);
        /** @brief Draws a source rectangle around a texture-space rotation point. */
        void RenderOnScreenWithRotation(
            const Microsoft::Xna::Framework::Rectangle& rectangle,
            const Microsoft::Xna::Framework::Rectangle& pixelRectangle,
            float rotation, Microsoft::Xna::Framework::Vector2 rotationPoint);
        /** @brief Describes the texture and its load state. */
        [[nodiscard]] std::string ToString() const;

    private:
        void InitializeSize();
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::Texture2D& RequireTexture();

        ResolutionMapper& mapper;
        Microsoft::Xna::Framework::Graphics::SpriteBatch& alphaSprite;
        Microsoft::Xna::Framework::Graphics::SpriteBatch& additiveSprite;
        std::string textureFilename;
        int textureWidth = 0;
        int textureHeight = 0;
        Microsoft::Xna::Framework::Vector2 halfPixelSize =
            Microsoft::Xna::Framework::Vector2::Zero;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> texture;
        bool loaded = false;
        bool hasAlpha = false;
    };
}
