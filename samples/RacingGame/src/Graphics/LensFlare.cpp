// SPDX-License-Identifier: MS-PL

#include "Graphics/LensFlare.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"

namespace RacingGame::Graphics
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Vector4;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using namespace Microsoft::Xna::Framework::Graphics;

    namespace
    {
        constexpr int SunFlareType = 0;
        constexpr int GlowFlareType = 1;
        constexpr int LensFlareType = 2;
        constexpr int StreaksType = 3;
        constexpr int RingType = 4;
        constexpr int HaloType = 5;
        constexpr int CircleType = 6;

        struct FlareData
        {
            int type;
            float position;
            float scale;
            Color color;
        };

        const std::array<FlareData, 17>& GetFlareTypes()
        {
            static const std::array<FlareData, 17> flareTypes{{
                {CircleType, 1.2f, 0.55f, Color(175, 175, 255, 20)},
                {SunFlareType, 1.0f, 0.9f, Color(255, 255, 255, 255)},
                {StreaksType, 1.0f, 1.8f, Color(255, 255, 255, 128)},
                {GlowFlareType, 1.0f, 2.6f, Color(255, 255, 200, 100)},
                {CircleType, 0.5f, 0.12f, Color(60, 60, 180, 35)},
                {CircleType, 0.45f, 0.46f, Color(100, 100, 200, 60)},
                {CircleType, 0.4f, 0.17f, Color(120, 120, 220, 40)},
                {RingType, 0.15f, 0.2f, Color(60, 60, 255, 100)},
                {LensFlareType, -0.5f, 0.2f, Color(255, 60, 60, 130)},
                {LensFlareType, -0.15f, 0.15f, Color(255, 60, 60, 90)},
                {HaloType, -0.3f, 0.6f, Color(60, 60, 255, 180)},
                {HaloType, -0.4f, 0.2f, Color(220, 80, 80, 98)},
                {CircleType, -0.5f, 0.1f, Color(220, 80, 80, 85)},
                {HaloType, -0.6f, 0.5f, Color(60, 60, 255, 80)},
                {RingType, -0.8f, 0.3f, Color(90, 60, 255, 110)},
                {HaloType, -0.95f, 0.5f, Color(60, 60, 255, 120)},
                {CircleType, -1.0f, 0.15f, Color(60, 60, 255, 85)},
            }};
            return flareTypes;
        }

        Color MultiplyColors(const Color color1, const Color color2)
        {
            if (color1 == Color::White) return color2;
            if (color2 == Color::White) return color1;
            const auto multiply = [](const std::uint8_t left,
                                     const std::uint8_t right)
            {
                const float value =
                    (static_cast<float>(left) / 255.0f) *
                    (static_cast<float>(right) / 255.0f);
                return static_cast<std::uint8_t>(
                    std::clamp(value, 0.0f, 1.0f) * 255.0f);
            };
            return Color(
                multiply(color1.getRProperty(), color2.getRProperty()),
                multiply(color1.getGProperty(), color2.getGProperty()),
                multiply(color1.getBProperty(), color2.getBProperty()),
                multiply(color1.getAProperty(), color2.getAProperty()));
        }

        Color ApplyAlphaToColor(const Color color, float alpha)
        {
            alpha = std::clamp(alpha, 0.0f, 1.0f);
            return Color(
                color.getRProperty(), color.getGProperty(),
                color.getBProperty(),
                static_cast<std::uint8_t>(alpha * 255.0f));
        }
    }

    Vector3 LensFlare::DefaultSunPos(2500.0f, -22500.0f, 15000.0f);
    Vector3 LensFlare::DefaultLightPos(8500.0f, -7250.0f, 15000.0f);
    Vector3 LensFlare::lensOrigin3D = Vector3::Zero;

    Vector3 LensFlare::RotateSun(const float rotation)
    {
        const Vector3 sunPos = DefaultSunPos;
        const Vector2 right(std::cos(rotation), std::sin(rotation));
        const Vector2 up(std::sin(rotation), -std::cos(rotation));
        return Vector3(
            -right.X * sunPos.X - up.X * sunPos.Z,
            sunPos.Y,
            -right.Y * sunPos.X - up.Y * sunPos.Z);
    }

    LensFlare::LensFlare(
        GraphicsDevice& device, ContentManager& content,
        const Vector3 lensOrigin)
        : device(device), spriteBatch(device)
    {
        lensOrigin3D = lensOrigin;
        static constexpr std::array<std::string_view, NumberOfFlareTypes>
            names{{"Sun", "Glow", "Lens", "Streaks", "Ring", "Halo",
                   "Circle"}};
        for (std::size_t index = 0; index < names.size(); ++index)
        {
            flareTextures[index].emplace(content.Load<Texture2D>(
                "Textures/" + std::string(names[index])));
        }
    }

    LensFlare::~LensFlare()
    {
        Dispose();
    }

    void LensFlare::Dispose()
    {
        if (isDisposed) return;
        for (std::optional<Texture2D>& texture : flareTextures)
        {
            if (texture) texture->Dispose();
            texture.reset();
        }
        isDisposed = true;
    }

    void LensFlare::setOrigin3DProperty(const Vector3 value)
    {
        lensOrigin3D = value;
    }

    Vector3 LensFlare::getOrigin3DProperty()
    {
        return lensOrigin3D;
    }

    int LensFlare::Render(
        const Color sunColor, const Matrix& view, const Matrix& projection,
        const bool enabled)
    {
        if (isDisposed)
            throw std::runtime_error("LensFlare used after disposal");
        lastSubmissionCount = 0;
        if (!enabled) return 0;

        const Viewport& viewport = device.getViewportProperty();
        const int width = viewport.getWidthProperty();
        const int height = viewport.getHeightProperty();
        const int screenFlareSize = 250 * width / 1024;
        const Vector3 cameraPosition =
            Matrix::Invert(view).getTranslationProperty();
        const Vector3 relativeLensPos = lensOrigin3D + cameraPosition;
        const Matrix viewProjection = view * projection;
        Vector4 projected = Vector4::Transform(
            Vector4(relativeLensPos.X, relativeLensPos.Y,
                    relativeLensPos.Z, 1.0f),
            viewProjection);
        if (!(projected.Z > projected.W - 0.5f)) return 0;
        if (projected.W == 0.0f) projected.W = 0.000001f;

        const int lensX = static_cast<int>(std::nearbyint(
            static_cast<double>(projected.X / projected.W) * (width / 2))) +
            width / 2;
        const int lensY = static_cast<int>(std::nearbyint(
            -static_cast<double>(projected.Y / projected.W) * (height / 2))) +
            height / 2;

        constexpr float thisSunIntensity = 0.75f;
        sunIntensity = thisSunIntensity * 0.1f + sunIntensity * 0.9f;
        if (sunIntensity < 0.01f) return 0;

        const int centerX = width / 2;
        const int centerY = height / 2;
        const int relativeX = centerX - lensX;
        const int relativeY = centerY - lensY;
        float alpha = 1.0f;
        float distance = static_cast<float>(
            std::abs(std::max(relativeX, relativeY)));
        const float borderDistance = static_cast<float>(height) / 1.75f;
        if (distance > borderDistance)
        {
            distance -= borderDistance;
            if (distance > borderDistance) return 0;
            alpha = 1.0f - distance / borderDistance;
            if (alpha > 1.0f) alpha = 1.0f;
        }
        alpha *= sunIntensity * sunIntensity;

        spriteBatch.Begin(SpriteSortMode::Deferred, BlendState::Additive);
        for (const FlareData& data : GetFlareTypes())
        {
            const int size = static_cast<int>(
                static_cast<float>(screenFlareSize) * data.scale);
            const int x = static_cast<int>(
                static_cast<float>(centerX) -
                static_cast<float>(relativeX) * data.position - size / 2);
            const int y = static_cast<int>(
                static_cast<float>(centerY) -
                static_cast<float>(relativeY) * data.position - size / 2);
            const float typeIntensity =
                data.type == SunFlareType || data.type == GlowFlareType
                    ? sunIntensity
                    : alpha;
            const Color tint = ApplyAlphaToColor(
                MultiplyColors(sunColor, data.color),
                (static_cast<float>(data.color.getAProperty()) / 255.0f) *
                    typeIntensity);
            const Texture2D& texture = *flareTextures[
                static_cast<std::size_t>(data.type)];
            spriteBatch.Draw(
                texture, Rectangle(x, y, size, size),
                Rectangle(0, 0, texture.getWidthProperty(),
                          texture.getHeightProperty()),
                tint);
            ++lastSubmissionCount;
        }
        spriteBatch.End();
        return lastSubmissionCount;
    }

    int LensFlare::getLastSubmissionCountProperty() const
    {
        return lastSubmissionCount;
    }

    float LensFlare::getSunIntensityProperty() const
    {
        return sunIntensity;
    }
}
