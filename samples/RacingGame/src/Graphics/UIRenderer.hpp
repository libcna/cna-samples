// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
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
    struct ControlFrame;
    struct MobileControlState;
    enum class TimeFadeupMode;
}

namespace RacingGame::Graphics
{
    class ResolutionMapper;
    class LineManager2D;
    class Texture;
    class TextureFont;
    class TextureFontBigNumbers;

    /** @brief Renders the original Racing HUD and menu screens from authentic atlases. */
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

        /**
         * @brief Draws the safe-area-aware touch controls when requested by mobile input.
         * @param controls Current mapped touch geometry and action state.
         */
        void RenderMobileControls(
            const GameLogic::MobileControlState& controls);

        /** @brief Starts the shared sprite batches used by one menu screen. */
        void BeginScreen();
        /** @brief Flushes shared sprites for one menu screen. */
        void EndScreen();
        /** @brief Flushes deferred text and optionally draws the authentic mouse cursor. */
        void RenderTextsAndMouseCursor(
            const GameLogic::ControlFrame& controls, bool showMouseCursor,
            float elapsedMilliseconds, int fps, int displayWidth,
            int displayHeight);
        /** @brief Draws the authentic finish trophy for a race rank. */
        void RenderTrophy(int rank);
        /** @brief Draws the original translucent menu background and animated logo. */
        void RenderMenuOverlay(float totalTimeSeconds);
        /** @brief Draws the original translucent horizontal menu bar. */
        void RenderBlackBar(int yPosition, int height);
        /** @brief Adds one authentic screen-space line to the deferred UI pass. */
        void AddLine(Microsoft::Xna::Framework::Point startPoint,
                     Microsoft::Xna::Framework::Point endPoint,
                     Microsoft::Xna::Framework::Color color);
        /** @brief Gets line primitives emitted by the latest UI flush. */
        [[nodiscard]] int getLastLinePrimitiveCountProperty() const;
        /** @brief Draws a menu-atlas region. */
        void RenderButton(
            const Microsoft::Xna::Framework::Rectangle& destination,
            const Microsoft::Xna::Framework::Rectangle& source,
            Microsoft::Xna::Framework::Color color);
        /** @brief Draws a menu header in the original 1600-wide coordinate space. */
        void RenderHeader(const Microsoft::Xna::Framework::Rectangle& source);
        /**
         * @brief Draws a header-atlas region into a specified destination.
         *
         * @param destination Destination rectangle in screen coordinates.
         * @param source Source rectangle in the headers texture.
         */
        void RenderHeader(
            const Microsoft::Xna::Framework::Rectangle& destination,
            const Microsoft::Xna::Framework::Rectangle& source);
        /** @brief Draws the platform-appropriate authentic help panel. */
        void RenderHelpScreen();
        /** @brief Draws the authentic desktop options panel. */
        void RenderOptionsScreen();
        /** @brief Draws a tinted region from the authentic desktop options atlas. */
        void RenderOptionsRegion(
            const Microsoft::Xna::Framework::Rectangle& destination,
            const Microsoft::Xna::Framework::Rectangle& source,
            Microsoft::Xna::Framework::Color color);
        /** @brief Draws one hue from the authentic color-selection texture. */
        void RenderColorSelection(
            const Microsoft::Xna::Framework::Rectangle& destination,
            Microsoft::Xna::Framework::Color color);
        /** @brief Draws the original A/B bottom controls and evaluates mouse activation. */
        [[nodiscard]] bool RenderBottomButtons(
            const GameLogic::ControlFrame& controls, bool onlyBack);
        /** @brief Gets whether the latest bottom-button render clicked Back. */
        [[nodiscard]] bool getBackButtonPressedProperty() const;
        /** @brief Tests a captured mouse position against a screen rectangle. */
        [[nodiscard]] static bool MouseInBox(
            const GameLogic::ControlFrame& controls,
            const Microsoft::Xna::Framework::Rectangle& rectangle);
        /** @brief Queues bitmap-font text at a top-left screen position. */
        void WriteText(int x, int y, const std::string& text,
                       Microsoft::Xna::Framework::Color color);
        /** @brief Queues white bitmap-font text at a top-left screen position. */
        void WriteText(int x, int y, const std::string& text);
        /** @brief Queues white bitmap-font text centered on a screen position. */
        void WriteTextCentered(int x, int y, const std::string& text);
        /** @brief Queues a formatted game time. */
        void WriteGameTime(int x, int y, int milliseconds,
                           Microsoft::Xna::Framework::Color color);
        /** @brief Measures text using the authentic GameFont atlas. */
        [[nodiscard]] int GetTextWidth(const std::string& text) const;
        /** @brief Converts a 1024-wide source coordinate. */
        [[nodiscard]] int XToRes(int value) const;
        /** @brief Converts a 640-high source coordinate. */
        [[nodiscard]] int YToRes(int value) const;
        /** @brief Converts a 768-high source coordinate. */
        [[nodiscard]] int YToRes768(int value) const;
        /** @brief Converts a source rectangle from the 1024x640 menu space. */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle CalcRectangle(
            int x, int y, int width, int height) const;
        /** @brief Converts a source rectangle from the original 1024x768 space. */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle
        CalcRectangleKeep4To3(
            const Microsoft::Xna::Framework::Rectangle& rectangle) const;
        /** @brief Scales a source rectangle around its centre. */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle
        CalcRectangleCenteredWithGivenHeight(
            int x, int y, int height,
            const Microsoft::Xna::Framework::Rectangle& source) const;

        /** @brief Gets atlas sprites emitted by the latest HUD render. */
        [[nodiscard]] int getLastAtlasSpriteCountProperty() const;
        /** @brief Gets bitmap-font glyphs emitted by the latest HUD render. */
        [[nodiscard]] int getLastGlyphCountProperty() const;
        /** @brief Gets text entries queued for the latest HUD font flush. */
        [[nodiscard]] int getLastTextCountProperty() const;
        /** @brief Gets mouse cursors emitted by the latest final UI pass. */
        [[nodiscard]] int getLastMouseCursorCountProperty() const;
        /** @brief Gets whether the original FPS overlay is enabled. */
        [[nodiscard]] bool getShowFpsProperty() const;
        /** @brief Gets trophies emitted by the latest final UI pass. */
        [[nodiscard]] int getLastTrophyCountProperty() const;
        /** @brief Gets sprites emitted by the latest mobile-control overlay. */
        [[nodiscard]] int getLastMobileControlSpriteCountProperty() const;
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
        /** @brief Full-screen translucent menu background source. */
        static const Microsoft::Xna::Framework::Rectangle BackgroundGfxRect;
        /** @brief Racing logo source in the background atlas. */
        static const Microsoft::Xna::Framework::Rectangle RacingGameLogoGfxRect;
        /** @brief One-pixel black bar source. */
        static const Microsoft::Xna::Framework::Rectangle BlackBarGfxRect;
        /** @brief Press-start header source. */
        static const Microsoft::Xna::Framework::Rectangle PressStartGfxRect;
        /** @brief Choose-car header source. */
        static const Microsoft::Xna::Framework::Rectangle HeaderChooseCarGfxRect;
        /** @brief Options header source. */
        static const Microsoft::Xna::Framework::Rectangle HeaderOptionsGfxRect;
        /** @brief Track-selection header source. */
        static const Microsoft::Xna::Framework::Rectangle HeaderSelectTrackGfxRect;
        /** @brief Help header source. */
        static const Microsoft::Xna::Framework::Rectangle HeaderHelpGfxRect;
        /** @brief Highscores header source. */
        static const Microsoft::Xna::Framework::Rectangle HeaderHighscoresGfxRect;
        /** @brief Main-menu Play button source. */
        static const Microsoft::Xna::Framework::Rectangle MenuButtonPlayGfxRect;
        /** @brief Main-menu Highscores button source. */
        static const Microsoft::Xna::Framework::Rectangle MenuButtonHighscoresGfxRect;
        /** @brief Main-menu Options button source. */
        static const Microsoft::Xna::Framework::Rectangle MenuButtonOptionsGfxRect;
        /** @brief Main-menu Help button source. */
        static const Microsoft::Xna::Framework::Rectangle MenuButtonHelpGfxRect;
        /** @brief Main-menu Quit button source. */
        static const Microsoft::Xna::Framework::Rectangle MenuButtonQuitGfxRect;
        /** @brief Selected main-menu button border source. */
        static const Microsoft::Xna::Framework::Rectangle MenuButtonSelectionGfxRect;
        /** @brief Main-menu Play label source. */
        static const Microsoft::Xna::Framework::Rectangle MenuTextPlayGfxRect;
        /** @brief Main-menu Highscores label source. */
        static const Microsoft::Xna::Framework::Rectangle MenuTextHighscoresGfxRect;
        /** @brief Main-menu Options label source. */
        static const Microsoft::Xna::Framework::Rectangle MenuTextOptionsGfxRect;
        /** @brief Main-menu Help label source. */
        static const Microsoft::Xna::Framework::Rectangle MenuTextHelpGfxRect;
        /** @brief Main-menu Quit label source. */
        static const Microsoft::Xna::Framework::Rectangle MenuTextQuitGfxRect;
        /** @brief Beginner track selection source. */
        static const Microsoft::Xna::Framework::Rectangle TrackButtonBeginnerGfxRect;
        /** @brief Advanced track selection source. */
        static const Microsoft::Xna::Framework::Rectangle TrackButtonAdvancedGfxRect;
        /** @brief Expert track selection source. */
        static const Microsoft::Xna::Framework::Rectangle TrackButtonExpertGfxRect;
        /** @brief Selected track button border source. */
        static const Microsoft::Xna::Framework::Rectangle TrackButtonSelectionGfxRect;
        /** @brief Beginner track label source. */
        static const Microsoft::Xna::Framework::Rectangle TrackTextBeginnerGfxRect;
        /** @brief Advanced track label source. */
        static const Microsoft::Xna::Framework::Rectangle TrackTextAdvancedGfxRect;
        /** @brief Expert track label source. */
        static const Microsoft::Xna::Framework::Rectangle TrackTextExpertGfxRect;
        /** @brief Hover border for the bottom controls. */
        static const Microsoft::Xna::Framework::Rectangle BottomButtonSelectionGfxRect;
        /** @brief A-button bottom control source. */
        static const Microsoft::Xna::Framework::Rectangle BottomButtonAButtonGfxRect;
        /** @brief B-button bottom control source. */
        static const Microsoft::Xna::Framework::Rectangle BottomButtonBButtonGfxRect;
        /** @brief Large animated car-selection arrow source. */
        static const Microsoft::Xna::Framework::Rectangle BigArrowGfxRect;
        /** @brief Small selection arrow source. */
        static const Microsoft::Xna::Framework::Rectangle SelectionArrowGfxRect;
        /** @brief Slider/color radio-button source. */
        static const Microsoft::Xna::Framework::Rectangle SelectionRadioButtonGfxRect;

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
        std::unique_ptr<Texture> background;
        std::unique_ptr<Texture> buttons;
        std::unique_ptr<Texture> headers;
        std::unique_ptr<Texture> helpScreen;
        std::unique_ptr<Texture> optionsScreen;
        std::unique_ptr<Texture> colorSelection;
        std::unique_ptr<Texture> mouseCursor;
        std::unique_ptr<Texture> mobileControlPixel;
        std::array<std::unique_ptr<Texture>, 3> trophies;
        std::unique_ptr<TextureFont> font;
        std::unique_ptr<TextureFontBigNumbers> bigNumbers;
        std::unique_ptr<LineManager2D> lineManager;
        std::vector<TimeFadeupText> fadeupTexts;
        int lastAtlasSpriteCount = 0;
        int lastGlyphCount = 0;
        int lastTextCount = 0;
        int lastMouseCursorCount = 0;
        int lastTrophyCount = 0;
        int lastMobileControlSpriteCount = 0;
        bool backButtonPressed = false;
#ifndef NDEBUG
        bool showFps = true;
#else
        bool showFps = false;
#endif

        void RenderTimeFadeupEffects(float elapsedMilliseconds);
        [[nodiscard]] static std::string FormatTime(int timeMilliseconds);
    };
}
