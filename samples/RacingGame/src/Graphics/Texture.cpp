// SPDX-License-Identifier: MS-PL

#include "Graphics/Texture.hpp"

#include <filesystem>
#include <stdexcept>

#include "Graphics/ResolutionMapper.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"

namespace RacingGame::Graphics
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Point;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using Microsoft::Xna::Framework::Graphics::BlendState;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteEffects;
    using Microsoft::Xna::Framework::Graphics::SurfaceFormat;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    Texture::Texture(
        ContentManager& content, ResolutionMapper& setMapper,
        SpriteBatch& setAlphaSprite, SpriteBatch& setAdditiveSprite,
        const std::string& filename)
        : mapper(setMapper), alphaSprite(setAlphaSprite),
          additiveSprite(setAdditiveSprite)
    {
        if (filename.empty())
            throw std::invalid_argument(
                "Unable to create texture without valid filename");
        textureFilename = std::filesystem::path(filename).stem().string();
        texture.emplace(content.Load<Texture2D>(
            "Textures/" + textureFilename));
        InitializeSize();
    }

    Texture::Texture(
        Texture2D setTexture, ResolutionMapper& setMapper,
        SpriteBatch& setAlphaSprite, SpriteBatch& setAdditiveSprite)
        : mapper(setMapper), alphaSprite(setAlphaSprite),
          additiveSprite(setAdditiveSprite), texture(std::move(setTexture))
    {
        InitializeSize();
    }

    void Texture::InitializeSize()
    {
        Texture2D& value = RequireTexture();
        textureWidth = value.getWidthProperty();
        textureHeight = value.getHeightProperty();
        const SurfaceFormat format = value.getFormatProperty();
        hasAlpha = format == SurfaceFormat::Dxt5 ||
                   format == SurfaceFormat::Dxt3;
        loaded = true;
        halfPixelSize = Vector2(
            (1.0f / static_cast<float>(textureWidth)) / 2.0f,
            (1.0f / static_cast<float>(textureHeight)) / 2.0f);
    }

    Texture2D& Texture::RequireTexture()
    {
        if (!texture)
            throw std::runtime_error("Racing texture has been disposed");
        return *texture;
    }

    const std::string& Texture::getFilenameProperty() const
    {
        return textureFilename;
    }

    int Texture::getWidthProperty() const { return textureWidth; }
    int Texture::getHeightProperty() const { return textureHeight; }

    Rectangle Texture::getGfxRectangleProperty() const
    {
        return Rectangle(0, 0, textureWidth, textureHeight);
    }

    Vector2 Texture::getHalfPixelSizeProperty() const { return halfPixelSize; }

    Texture2D& Texture::getXnaTextureProperty() { return RequireTexture(); }

    const Texture2D& Texture::getXnaTextureProperty() const
    {
        if (!texture)
            throw std::runtime_error("Racing texture has been disposed");
        return *texture;
    }

    bool Texture::getValidProperty() const { return loaded && texture.has_value(); }
    bool Texture::getHasAlphaPixelsProperty() const { return hasAlpha; }

    void Texture::Dispose()
    {
        texture.reset();
        loaded = false;
    }

    void Texture::RenderOnScreen(
        const Rectangle& rectangle, const Rectangle& pixelRectangle)
    {
        alphaSprite.Draw(RequireTexture(), rectangle, pixelRectangle, Color::White);
    }

    void Texture::RenderOnScreen(
        const Rectangle& rectangle, const int pixelX, const int pixelY,
        const int pixelWidth, const int pixelHeight)
    {
        RenderOnScreen(
            rectangle, Rectangle(pixelX, pixelY, pixelWidth, pixelHeight));
    }

    void Texture::RenderOnScreen(const Point position)
    {
        RenderOnScreen(
            Rectangle(position.X, position.Y,
                      textureWidth, textureHeight));
    }

    void Texture::RenderOnScreen(const Rectangle& rectangle)
    {
        RenderOnScreen(rectangle, getGfxRectangleProperty());
    }

    void Texture::RenderOnScreenRelative16To9(
        const int relativeX, const int relativeY,
        const Rectangle& pixelRectangle)
    {
        RenderOnScreen(
            mapper.CalcRectangle(
                relativeX, relativeY,
                pixelRectangle.Width,
                pixelRectangle.Height),
            pixelRectangle);
    }

    void Texture::RenderOnScreenRelative4To3(
        const int relativeX, const int relativeY,
        const Rectangle& pixelRectangle)
    {
        RenderOnScreen(
            mapper.CalcRectangleKeep4To3(
                relativeX, relativeY,
                pixelRectangle.Width,
                pixelRectangle.Height),
            pixelRectangle);
    }

    void Texture::RenderOnScreenRelative1600(
        const int relativeX, const int relativeY,
        const Rectangle& pixelRectangle)
    {
        RenderOnScreen(
            mapper.CalcRectangle1600(
                relativeX, relativeY,
                pixelRectangle.Width,
                pixelRectangle.Height),
            pixelRectangle);
    }

    void Texture::RenderOnScreen(
        const Rectangle& rectangle, const Rectangle& pixelRectangle,
        const Color color)
    {
        alphaSprite.Draw(RequireTexture(), rectangle, pixelRectangle, color);
    }

    void Texture::RenderOnScreen(
        const Rectangle& rectangle, const Rectangle& pixelRectangle,
        const Color color, const BlendState& blendState)
    {
        SpriteBatch& batch = &blendState == &BlendState::Additive
            ? additiveSprite : alphaSprite;
        batch.Draw(RequireTexture(), rectangle, pixelRectangle, color);
    }

    void Texture::RenderOnScreenWithRotation(
        const Rectangle& rectangle, const Rectangle& pixelRectangle,
        const float rotation, const Vector2 rotationPoint)
    {
        alphaSprite.Draw(
            RequireTexture(), rectangle, pixelRectangle, Color::White,
            rotation, rotationPoint, SpriteEffects::None, 0.0f);
    }

    std::string Texture::ToString() const
    {
        return "Texture(filename=" + textureFilename +
            ", width=" + std::to_string(textureWidth) +
            ", height=" + std::to_string(textureHeight) +
            ", xnaTexture=" + (texture ? "valid" : "null") + ")";
    }
}
