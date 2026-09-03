// SPDX-License-Identifier: MS-PL

#include "Graphics/UIRenderer.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "GameLogic/CarPhysics.hpp"
#include "Graphics/ResolutionMapper.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/TextureFont.hpp"
#include "Graphics/TextureFontBigNumbers.hpp"
#include "Helpers/ColorHelper.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace RacingGame::Graphics
{
    using GameLogic::TimeFadeupMode;
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Point;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using Microsoft::Xna::Framework::Graphics::BlendState;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteSortMode;

    const Rectangle UIRenderer::LapsGfxRect(381, 132, 222, 160);
    const Rectangle UIRenderer::TachoGfxRect(0, 0, 343, 341);
    const Rectangle UIRenderer::TachoArrowGfxRect(347, 0, 28, 186);
    const Rectangle UIRenderer::TachoMphGfxRect(184, 256, 148, 72);
    const Rectangle UIRenderer::TachoGearGfxRect(286, 149, 52, 72);
    const Rectangle UIRenderer::CurrentAndBestGfxRect(381, 2, 342, 128);
    const Rectangle UIRenderer::TrackNameGfxRect(726, 2, 282, 62);
    const Rectangle UIRenderer::Best5GfxRect(726, 66, 282, 62);

    UIRenderer::UIRenderer(GraphicsDevice& device, ContentManager& content)
        : mapper(std::make_unique<ResolutionMapper>(device)),
          alphaSprite(std::make_unique<SpriteBatch>(device)),
          additiveSprite(std::make_unique<SpriteBatch>(device)),
          ingame(std::make_unique<Texture>(
              content, *mapper, *alphaSprite, *additiveSprite, "Ingame.png")),
          font(std::make_unique<TextureFont>(
              device, content, *mapper, *alphaSprite, *additiveSprite)),
          bigNumbers(std::make_unique<TextureFontBigNumbers>(*ingame, *mapper))
    {
    }

    UIRenderer::~UIRenderer() = default;

    std::string UIRenderer::FormatTime(const int timeMilliseconds)
    {
        std::ostringstream text;
        text << (timeMilliseconds / 1000) / 60 << ':'
             << std::setw(2) << std::setfill('0')
             << (timeMilliseconds / 1000) % 60 << '.'
             << std::setw(2) << std::setfill('0')
             << (timeMilliseconds / 10) % 100;
        return text.str();
    }

    void UIRenderer::AddTimeFadeupEffect(
        const int timeMilliseconds, const TimeFadeupMode mode)
    {
        std::string text = FormatTime(timeMilliseconds);
        Color color = Color::White;
        if (mode == TimeFadeupMode::Plus)
        {
            text = "+ " + text;
            color = Color::Red;
        }
        else if (mode == TimeFadeupMode::Minus)
        {
            text = "- " + text;
            color = Color::Green;
        }
        fadeupTexts.push_back({std::move(text), color});
    }

    void UIRenderer::WriteTextCentered(
        const int x, const int y, const std::string& text,
        const Color color, const float scale)
    {
        font->WriteTextCentered(x, y, text, color, scale);
    }

    void UIRenderer::RenderTimeFadeupEffects(const float elapsedMilliseconds)
    {
        for (auto iterator = fadeupTexts.begin();
             iterator != fadeupTexts.end();)
        {
            iterator->showTimeMilliseconds -= elapsedMilliseconds;
            if (iterator->showTimeMilliseconds < 0.0f)
            {
                iterator = fadeupTexts.erase(iterator);
                continue;
            }

            float alpha = 1.0f;
            if (iterator->showTimeMilliseconds < 1500.0f)
                alpha = iterator->showTimeMilliseconds / 1500.0f;
            const float moveUp =
                (TimeFadeupText::MaxShowTimeMilliseconds -
                 iterator->showTimeMilliseconds) /
                TimeFadeupText::MaxShowTimeMilliseconds;
            font->WriteTextCentered(
                mapper->getWidthProperty() / 2,
                mapper->getHeightProperty() / 3 -
                    static_cast<int>(moveUp * mapper->getHeightProperty() / 3),
                iterator->text,
                Helpers::ColorHelper::ApplyAlphaToColor(
                    iterator->color, alpha),
                2.25f);
            ++iterator;
        }
    }

    void UIRenderer::RenderGameUI(
        const int currentGameTime, const int bestLapTime,
        const int lapNumber, float speed, int gear, float acceleration,
        const std::string& trackName,
        const std::array<int, 10>& topLapTimes,
        const bool gameOver, const float elapsedMilliseconds)
    {
        if (gameOver)
        {
            speed = 0.0f;
            gear = 1;
            acceleration = 0.0f;
        }

        lastAtlasSpriteCount = 0;
        additiveSprite->Begin(SpriteSortMode::Deferred, BlendState::Additive);
        alphaSprite->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend);

        const Color baseUiColor = Color::White;
        const Rectangle lapsRectangle = mapper->CalcRectangle1600(
            10, 10, LapsGfxRect.Width, LapsGfxRect.Height);
        ingame->RenderOnScreen(lapsRectangle, LapsGfxRect, baseUiColor);
        ++lastAtlasSpriteCount;

        Rectangle timesRectangle = mapper->CalcRectangle1600(
            10, 10, CurrentAndBestGfxRect.Width,
            CurrentAndBestGfxRect.Height);
        timesRectangle.Y = mapper->getHeightProperty() -
                           timesRectangle.getBottomProperty();
        ingame->RenderOnScreen(
            timesRectangle, CurrentAndBestGfxRect, baseUiColor);
        ++lastAtlasSpriteCount;

        Rectangle trackNameRectangle = mapper->CalcRectangle1600(
            10, 10, TrackNameGfxRect.Width, TrackNameGfxRect.Height);
        trackNameRectangle.X = mapper->getWidthProperty() -
                               trackNameRectangle.getRightProperty();
        ingame->RenderOnScreen(
            trackNameRectangle, TrackNameGfxRect, baseUiColor);
        ++lastAtlasSpriteCount;

        std::array<Rectangle, 5> topRectangles;
        topRectangles[0] = mapper->CalcRectangle1600(
            10, 4, Best5GfxRect.Width, Best5GfxRect.Height);
        topRectangles[0].X = trackNameRectangle.X;
        const int topDistance = topRectangles[0].Y;
        topRectangles[0].Y += trackNameRectangle.getBottomProperty();
        for (std::size_t index = 1; index < topRectangles.size(); ++index)
        {
            const Rectangle& previous = topRectangles[index - 1];
            topRectangles[index] = Rectangle(
                topRectangles[0].X,
                previous.getBottomProperty() + topDistance,
                topRectangles[0].Width, topRectangles[0].Height);
        }
        for (const Rectangle& topRectangle : topRectangles)
        {
            ingame->RenderOnScreen(topRectangle, Best5GfxRect, baseUiColor);
            ++lastAtlasSpriteCount;
        }

        Rectangle tachoRectangle = mapper->CalcRectangle1600(
            10, 10, TachoGfxRect.Width, TachoGfxRect.Height);
        tachoRectangle.X = mapper->getWidthProperty() -
                           tachoRectangle.getRightProperty();
        tachoRectangle.Y = mapper->getHeightProperty() -
                           tachoRectangle.getBottomProperty();
        ingame->RenderOnScreen(tachoRectangle, TachoGfxRect, baseUiColor);
        ++lastAtlasSpriteCount;

        (void)bigNumbers->WriteNumber(
            lapsRectangle.X + mapper->XToRes1600(15),
            lapsRectangle.Y + mapper->YToRes1200(12), lapNumber);
        lastAtlasSpriteCount += bigNumbers->getLastDigitCountProperty();

        const Color highlightColor(255, 185, 80);
        const int blockHeight = mapper->YToRes1200(74);
        font->WriteGameTime(
            timesRectangle.X + mapper->XToRes1600(154),
            timesRectangle.Y + blockHeight / 2 -
                font->getHeightProperty() / 2,
            currentGameTime, highlightColor);
        font->WriteGameTime(
            timesRectangle.X + mapper->XToRes1600(154),
            timesRectangle.Y + timesRectangle.Height / 2 + blockHeight / 2 -
                font->getHeightProperty() / 2,
            bestLapTime, Color::White);
        font->WriteTextCentered(
            trackNameRectangle.X + trackNameRectangle.Width / 2,
            trackNameRectangle.Y + blockHeight / 2, trackName);

        for (std::size_t index = 0; index < topRectangles.size(); ++index)
        {
            const Color rankColor = bestLapTime == topLapTimes[index]
                ? highlightColor : Color::White;
            font->WriteTextCentered(
                topRectangles[index].X + mapper->XToRes(32) / 2,
                topRectangles[index].Y + blockHeight / 2,
                std::to_string(index + 1) + '.', rankColor, 1.0f);
            font->WriteGameTime(
                topRectangles[index].X + mapper->XToRes(50),
                topRectangles[index].Y + blockHeight / 2 -
                    font->getHeightProperty() / 2,
                topLapTimes[index], rankColor);
        }

        const Point tachoPoint(
            tachoRectangle.X + mapper->XToRes1600(194),
            tachoRectangle.Y + mapper->YToRes1200(194));
        acceleration = std::clamp(acceleration, 0.0f, 1.0f);
        const float rotation = -2.33f + acceleration * 2.5f;
        const Vector2 rotationPoint(
            TachoArrowGfxRect.Width / 2.0f,
            static_cast<float>(TachoArrowGfxRect.Height - 13));
        ingame->RenderOnScreenWithRotation(
            Rectangle(
                tachoPoint.X, tachoPoint.Y,
                mapper->XToRes1600(TachoArrowGfxRect.Width),
                mapper->YToRes1200(TachoArrowGfxRect.Height)),
            TachoArrowGfxRect, rotation, rotationPoint);
        ++lastAtlasSpriteCount;

        (void)bigNumbers->WriteNumber(
            tachoRectangle.X + mapper->XToRes1600(TachoMphGfxRect.X),
            tachoRectangle.Y + mapper->YToRes1200(TachoMphGfxRect.Y),
            TachoMphGfxRect.Height,
            static_cast<int>(std::nearbyint(speed)));
        lastAtlasSpriteCount += bigNumbers->getLastDigitCountProperty();
        (void)bigNumbers->WriteNumber(
            tachoRectangle.X + mapper->XToRes1600(TachoGearGfxRect.X),
            tachoRectangle.Y + mapper->YToRes1200(TachoGearGfxRect.Y),
            TachoGearGfxRect.Height, std::min(5, gear));
        lastAtlasSpriteCount += bigNumbers->getLastDigitCountProperty();

        additiveSprite->End();
        alphaSprite->End();

        RenderTimeFadeupEffects(elapsedMilliseconds);
        lastTextCount = font->getPendingTextCountProperty();
        font->WriteAll();
        lastGlyphCount = font->getLastGlyphCountProperty();
    }

    int UIRenderer::getLastAtlasSpriteCountProperty() const
    {
        return lastAtlasSpriteCount;
    }

    int UIRenderer::getLastGlyphCountProperty() const { return lastGlyphCount; }
    int UIRenderer::getLastTextCountProperty() const { return lastTextCount; }

    int UIRenderer::getFadeupCountProperty() const
    {
        return static_cast<int>(fadeupTexts.size());
    }

    int UIRenderer::getIngameTextureWidthProperty() const
    {
        return ingame->getWidthProperty();
    }

    int UIRenderer::getIngameTextureHeightProperty() const
    {
        return ingame->getHeightProperty();
    }

    Microsoft::Xna::Framework::Graphics::SurfaceFormat
    UIRenderer::getIngameTextureFormatProperty() const
    {
        return ingame->getXnaTextureProperty().getFormatProperty();
    }
}
