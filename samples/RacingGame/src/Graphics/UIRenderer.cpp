// SPDX-License-Identifier: MS-PL

#include "Graphics/UIRenderer.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "GameLogic/CarPhysics.hpp"
#include "GameLogic/Input.hpp"
#include "Graphics/ResolutionMapper.hpp"
#include "Graphics/LineManager2D.hpp"
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
    const Rectangle UIRenderer::BackgroundGfxRect(0, 0, 1024, 640);
    const Rectangle UIRenderer::RacingGameLogoGfxRect(0, 649, 1024, 374);
    const Rectangle UIRenderer::BlackBarGfxRect(99, 999, 1, 1);
    const Rectangle UIRenderer::PressStartGfxRect(2, 1, 631, 45);
    const Rectangle UIRenderer::HeaderChooseCarGfxRect(0, 212, 512, 100);
    const Rectangle UIRenderer::HeaderOptionsGfxRect(512, 212, 512, 100);
    const Rectangle UIRenderer::HeaderSelectTrackGfxRect(0, 312, 512, 100);
    const Rectangle UIRenderer::HeaderHelpGfxRect(512, 312, 512, 100);
    const Rectangle UIRenderer::HeaderHighscoresGfxRect(0, 412, 512, 100);
    const Rectangle UIRenderer::MenuButtonPlayGfxRect(0, 0, 212, 212);
    const Rectangle UIRenderer::MenuButtonHighscoresGfxRect(212, 0, 212, 212);
    const Rectangle UIRenderer::MenuButtonOptionsGfxRect(424, 0, 212, 212);
    const Rectangle UIRenderer::MenuButtonHelpGfxRect(636, 0, 212, 212);
    const Rectangle UIRenderer::MenuButtonQuitGfxRect(212, 240, 212, 212);
    const Rectangle UIRenderer::MenuButtonSelectionGfxRect(636, 240, 212, 212);
    const Rectangle UIRenderer::MenuTextPlayGfxRect(0, 214, 212, 24);
    const Rectangle UIRenderer::MenuTextHighscoresGfxRect(212, 214, 212, 24);
    const Rectangle UIRenderer::MenuTextOptionsGfxRect(424, 214, 212, 24);
    const Rectangle UIRenderer::MenuTextHelpGfxRect(636, 214, 212, 24);
    const Rectangle UIRenderer::MenuTextQuitGfxRect(212, 454, 212, 24);
    const Rectangle UIRenderer::TrackButtonBeginnerGfxRect(0, 480, 212, 352);
    const Rectangle UIRenderer::TrackButtonAdvancedGfxRect(212, 480, 212, 352);
    const Rectangle UIRenderer::TrackButtonExpertGfxRect(424, 480, 212, 352);
    const Rectangle UIRenderer::TrackButtonSelectionGfxRect(636, 480, 212, 352);
    const Rectangle UIRenderer::TrackTextBeginnerGfxRect(0, 834, 212, 24);
    const Rectangle UIRenderer::TrackTextAdvancedGfxRect(212, 834, 212, 24);
    const Rectangle UIRenderer::TrackTextExpertGfxRect(424, 834, 212, 24);
    const Rectangle UIRenderer::BottomButtonSelectionGfxRect(424, 240, 212, 92);
    const Rectangle UIRenderer::BottomButtonAButtonGfxRect(0, 872, 212, 92);
    const Rectangle UIRenderer::BottomButtonBButtonGfxRect(212, 872, 212, 92);
    const Rectangle UIRenderer::BigArrowGfxRect(867, 242, 127, 178);
    const Rectangle UIRenderer::SelectionArrowGfxRect(874, 426, 53, 39);
    const Rectangle UIRenderer::SelectionRadioButtonGfxRect(935, 427, 39, 39);

    UIRenderer::UIRenderer(GraphicsDevice& device, ContentManager& content)
        : mapper(std::make_unique<ResolutionMapper>(device)),
          alphaSprite(std::make_unique<SpriteBatch>(device)),
          additiveSprite(std::make_unique<SpriteBatch>(device)),
          ingame(std::make_unique<Texture>(
              content, *mapper, *alphaSprite, *additiveSprite, "Ingame.png")),
          background(std::make_unique<Texture>(
              content, *mapper, *alphaSprite, *additiveSprite, "background.png")),
          buttons(std::make_unique<Texture>(
              content, *mapper, *alphaSprite, *additiveSprite, "buttons.png")),
          headers(std::make_unique<Texture>(
              content, *mapper, *alphaSprite, *additiveSprite, "headers.png")),
          helpScreen(std::make_unique<Texture>(
              content, *mapper, *alphaSprite, *additiveSprite,
              "HelpScreenWindows.png")),
          optionsScreen(std::make_unique<Texture>(
              content, *mapper, *alphaSprite, *additiveSprite,
              "OptionsScreenWindows.png")),
          colorSelection(std::make_unique<Texture>(
              content, *mapper, *alphaSprite, *additiveSprite,
              "ColorSelection.png")),
          mouseCursor(std::make_unique<Texture>(
              content, *mapper, *alphaSprite, *additiveSprite,
              "MouseCursor.png")),
          trophies{
              std::make_unique<Texture>(
                  content, *mapper, *alphaSprite, *additiveSprite, "pokal1"),
              std::make_unique<Texture>(
                  content, *mapper, *alphaSprite, *additiveSprite, "pokal2"),
              std::make_unique<Texture>(
                  content, *mapper, *alphaSprite, *additiveSprite, "pokal3")},
          font(std::make_unique<TextureFont>(
              device, content, *mapper, *alphaSprite, *additiveSprite)),
          bigNumbers(std::make_unique<TextureFontBigNumbers>(*ingame, *mapper)),
          lineManager(std::make_unique<LineManager2D>(
              device, content, *mapper))
    {
    }

    UIRenderer::~UIRenderer() = default;

    void UIRenderer::BeginScreen()
    {
        lastAtlasSpriteCount = 0;
        lastMouseCursorCount = 0;
        lastTrophyCount = 0;
        additiveSprite->Begin(SpriteSortMode::Deferred, BlendState::Additive);
        alphaSprite->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend);
    }

    void UIRenderer::EndScreen()
    {
        additiveSprite->End();
        alphaSprite->End();
        lineManager->Render();
    }

    void UIRenderer::RenderTextsAndMouseCursor(
        const GameLogic::ControlFrame& controls, const bool showMouseCursor,
        const float elapsedMilliseconds)
    {
        RenderTimeFadeupEffects(elapsedMilliseconds);
        lastTextCount = font->getPendingTextCountProperty();
        font->WriteAll();
        lastGlyphCount = font->getLastGlyphCountProperty();

        if (!showMouseCursor) return;
        // The original intentionally draws the cursor through its alpha batch
        // while that batch uses additive blending.
        alphaSprite->Begin(SpriteSortMode::Deferred, BlendState::Additive);
        additiveSprite->Begin(
            SpriteSortMode::Deferred, BlendState::AlphaBlend);
        mouseCursor->RenderOnScreen(controls.mousePosition);
        ++lastAtlasSpriteCount;
        lastMouseCursorCount = 1;
        additiveSprite->End();
        alphaSprite->End();
    }

    void UIRenderer::RenderTrophy(const int rank)
    {
        const int trophy = std::clamp(rank, 0, 2);
        alphaSprite->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend);
        trophies[static_cast<std::size_t>(trophy)]->RenderOnScreen(Rectangle(
            mapper->getWidthProperty() / 2 - mapper->getWidthProperty() / 8,
            mapper->getHeightProperty() / 2 - mapper->YToRes(10),
            mapper->getWidthProperty() / 4,
            mapper->getHeightProperty() * 2 / 5));
        ++lastAtlasSpriteCount;
        lastTrophyCount = 1;
        alphaSprite->End();
    }

    void UIRenderer::RenderMenuOverlay(const float totalTimeSeconds)
    {
        background->RenderOnScreen(
            Rectangle(0, 0, mapper->getWidthProperty(), mapper->getHeightProperty()),
            BackgroundGfxRect,
            Helpers::ColorHelper::ApplyAlphaToColor(Color::White, 0.85f));
        ++lastAtlasSpriteCount;
        const float bounce = 1.005f + std::sin(totalTimeSeconds / 0.46f) *
            0.045f * std::cos(totalTimeSeconds / 0.285f);
        background->RenderOnScreen(
            mapper->CalcRectangleWithBounce(362, 36, 601, 218, bounce),
            RacingGameLogoGfxRect);
        ++lastAtlasSpriteCount;
    }

    void UIRenderer::RenderBlackBar(const int yPosition, const int height)
    {
        buttons->RenderOnScreen(
            mapper->CalcRectangle(0, yPosition, 1024, height),
            BlackBarGfxRect,
            Helpers::ColorHelper::ApplyAlphaToColor(Color::White, 0.85f));
        ++lastAtlasSpriteCount;
    }

    void UIRenderer::AddLine(
        const Point startPoint, const Point endPoint, const Color color)
    {
        lineManager->AddLine(startPoint, endPoint, color);
    }

    int UIRenderer::getLastLinePrimitiveCountProperty() const
    {
        return lineManager->getLastPrimitiveCountProperty();
    }

    void UIRenderer::RenderButton(
        const Rectangle& destination, const Rectangle& source, const Color color)
    {
        buttons->RenderOnScreen(destination, source, color);
        ++lastAtlasSpriteCount;
    }

    void UIRenderer::RenderHeader(const Rectangle& source)
    {
        headers->RenderOnScreenRelative1600(10, 18, source);
        ++lastAtlasSpriteCount;
    }

    void UIRenderer::RenderHelpScreen()
    {
        helpScreen->RenderOnScreenRelative4To3(
            0, 125, helpScreen->getGfxRectangleProperty());
        ++lastAtlasSpriteCount;
    }

    void UIRenderer::RenderOptionsScreen()
    {
        optionsScreen->RenderOnScreenRelative4To3(
            0, 125, optionsScreen->getGfxRectangleProperty());
        ++lastAtlasSpriteCount;
    }

    void UIRenderer::RenderOptionsRegion(
        const Rectangle& destination, const Rectangle& source,
        const Color color)
    {
        optionsScreen->RenderOnScreen(destination, source, color);
        ++lastAtlasSpriteCount;
    }

    void UIRenderer::RenderColorSelection(
        const Rectangle& destination, const Color color)
    {
        colorSelection->RenderOnScreen(
            destination, colorSelection->getGfxRectangleProperty(), color);
        ++lastAtlasSpriteCount;
    }

    bool UIRenderer::MouseInBox(
        const GameLogic::ControlFrame& controls, const Rectangle& rectangle)
    {
        return controls.mousePosition.X >= rectangle.X &&
               controls.mousePosition.Y >= rectangle.Y &&
               controls.mousePosition.X < rectangle.getRightProperty() &&
               controls.mousePosition.Y < rectangle.getBottomProperty();
    }

    bool UIRenderer::RenderBottomButtons(
        const GameLogic::ControlFrame& controls, const bool onlyBack)
    {
        Rectangle back = mapper->CalcRectangleCenteredWithGivenHeight(
            0, 587, 48, BottomButtonBButtonGfxRect);
        back.X = mapper->getWidthProperty() - back.Width - mapper->XToRes(50);
        const bool overBack = MouseInBox(controls, back);
        const int xAdd = mapper->XToRes(16);
        const int yAdd = mapper->YToRes(9);
        if (overBack)
            back = Rectangle(back.X - xAdd / 2, back.Y - yAdd / 2,
                             back.Width + xAdd, back.Height + yAdd);
        RenderButton(back, BottomButtonBButtonGfxRect, Color::White);
        if (overBack)
            RenderButton(back, BottomButtonSelectionGfxRect, Color::White);
        backButtonPressed = overBack && controls.mouseLeftJustPressed;
        if (onlyBack) return false;

        Rectangle accept = mapper->CalcRectangleCenteredWithGivenHeight(
            0, 587, 48, BottomButtonAButtonGfxRect);
        accept.X = mapper->getWidthProperty() -
                   accept.Width * 2 - mapper->XToRes(80);
        const bool overAccept = MouseInBox(controls, accept);
        if (overAccept)
            accept = Rectangle(accept.X - xAdd / 2, accept.Y - yAdd / 2,
                               accept.Width + xAdd, accept.Height + yAdd);
        RenderButton(accept, BottomButtonAButtonGfxRect, Color::White);
        if (overAccept)
            RenderButton(accept, BottomButtonSelectionGfxRect, Color::White);
        return overAccept && controls.mouseLeftJustPressed;
    }

    bool UIRenderer::getBackButtonPressedProperty() const
    {
        return backButtonPressed;
    }

    void UIRenderer::WriteText(
        const int x, const int y, const std::string& text, const Color color)
    {
        font->WriteText(x, y, text, color);
    }

    void UIRenderer::WriteText(
        const int x, const int y, const std::string& text)
    {
        font->WriteText(x, y, text);
    }

    void UIRenderer::WriteTextCentered(
        const int x, const int y, const std::string& text)
    {
        font->WriteTextCentered(x, y, text);
    }

    void UIRenderer::WriteGameTime(
        const int x, const int y, const int milliseconds, const Color color)
    {
        font->WriteGameTime(x, y, milliseconds, color);
    }

    int UIRenderer::GetTextWidth(const std::string& text) const
    {
        return font->GetTextWidth(text);
    }

    int UIRenderer::XToRes(const int value) const { return mapper->XToRes(value); }
    int UIRenderer::YToRes(const int value) const { return mapper->YToRes(value); }
    int UIRenderer::YToRes768(const int value) const
    {
        return mapper->YToRes768(value);
    }

    Rectangle UIRenderer::CalcRectangle(
        const int x, const int y, const int width, const int height) const
    {
        return mapper->CalcRectangle(x, y, width, height);
    }

    Rectangle UIRenderer::CalcRectangleKeep4To3(
        const Rectangle& rectangle) const
    {
        return mapper->CalcRectangleKeep4To3(rectangle);
    }

    Rectangle UIRenderer::CalcRectangleCenteredWithGivenHeight(
        const int x, const int y, const int height,
        const Rectangle& source) const
    {
        return mapper->CalcRectangleCenteredWithGivenHeight(
            x, y, height, source);
    }

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

        EndScreen();
    }

    int UIRenderer::getLastAtlasSpriteCountProperty() const
    {
        return lastAtlasSpriteCount;
    }

    int UIRenderer::getLastGlyphCountProperty() const { return lastGlyphCount; }
    int UIRenderer::getLastTextCountProperty() const { return lastTextCount; }

    int UIRenderer::getLastMouseCursorCountProperty() const
    {
        return lastMouseCursorCount;
    }

    int UIRenderer::getLastTrophyCountProperty() const
    {
        return lastTrophyCount;
    }

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
