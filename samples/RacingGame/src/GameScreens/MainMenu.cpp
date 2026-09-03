// SPDX-License-Identifier: MS-PL

#include "GameScreens/MainMenu.hpp"

#include <algorithm>
#include <cmath>
#include <memory>

#include "GameLogic/Input.hpp"
#include "GameScreens/CarSelection.hpp"
#include "GameScreens/Help.hpp"
#include "GameScreens/Highscores.hpp"
#include "GameScreens/Options.hpp"
#include "GameScreens/SplashScreen.hpp"
#include "Graphics/UIRenderer.hpp"
#include "RacingGameManager.hpp"
#include "Sounds/Sound.hpp"

namespace RacingGame::GameScreens
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Rectangle;
    using RacingGame::Graphics::UIRenderer;

    namespace
    {
        constexpr int NumberOfButtons = 5;
        constexpr int ActiveButtonWidth = 132;
        constexpr int InactiveButtonWidth = 108;
        constexpr int DistanceBetweenButtons = 14;
        constexpr float TimeOutMenu = 60000.0f;

    }

    MainMenu::MainMenu(RacingGameManager& setGame) : game(setGame) {}

    void MainMenu::Update(Microsoft::Xna::Framework::GameTime&)
    {
        if (!musicHasStarted)
        {
            game.PlayMenuMusic();
            musicHasStarted = true;
        }
    }

    Rectangle MainMenu::InterpolateRect(
        const Rectangle first, const Rectangle second,
        const float interpolation)
    {
        return Rectangle(
            static_cast<int>(std::nearbyint(
                first.X * interpolation + second.X * (1.0f - interpolation))),
            static_cast<int>(std::nearbyint(
                first.Y * interpolation + second.Y * (1.0f - interpolation))),
            static_cast<int>(std::nearbyint(
                first.Width * interpolation +
                second.Width * (1.0f - interpolation))),
            static_cast<int>(std::nearbyint(
                first.Height * interpolation +
                second.Height * (1.0f - interpolation))));
    }

    void MainMenu::SetSelectedButton(const int value)
    {
        selectedButton = value;
        idleTime = 0.0f;
    }

    bool MainMenu::Render()
    {
        const std::array<Rectangle, NumberOfButtons> buttonRectangles{
            UIRenderer::MenuButtonPlayGfxRect,
            UIRenderer::MenuButtonHighscoresGfxRect,
            UIRenderer::MenuButtonOptionsGfxRect,
            UIRenderer::MenuButtonHelpGfxRect,
            UIRenderer::MenuButtonQuitGfxRect};
        const std::array<Rectangle, NumberOfButtons> textRectangles{
            UIRenderer::MenuTextPlayGfxRect,
            UIRenderer::MenuTextHighscoresGfxRect,
            UIRenderer::MenuTextOptionsGfxRect,
            UIRenderer::MenuTextHelpGfxRect,
            UIRenderer::MenuTextQuitGfxRect};
        game.BeginMenuFrame(true);
        auto& ui = game.getUIProperty();
        const auto& input = game.getControlsProperty();
        ui.RenderBlackBar(280, 192);

        if (input.hasMouseMoved || input.mouseLeftJustPressed)
            ignoreMouse = false;

        const Rectangle active = ui.CalcRectangleCenteredWithGivenHeight(
            0, 0, ActiveButtonWidth, buttonRectangles[0]);
        const Rectangle inactive = ui.CalcRectangleCenteredWithGivenHeight(
            0, 0, InactiveButtonWidth, buttonRectangles[0]);
        const int totalWidth = active.Width +
            (NumberOfButtons - 1) * inactive.Width +
            (NumberOfButtons - 1) * ui.XToRes(DistanceBetweenButtons);
        int x = ui.XToRes(512) - totalWidth / 2;
        const int y = ui.YToRes(316);
        int mouseOverButton = -1;
        for (int index = 0; index < NumberOfButtons; ++index)
        {
            const bool selected = index == selectedButton;
            currentButtonSizes[static_cast<std::size_t>(index)] = std::clamp(
                currentButtonSizes[static_cast<std::size_t>(index)] +
                    (selected ? 1.0f : -1.0f) *
                        game.getMoveFactorPerSecondProperty() * 2.0f,
                0.0f, 1.0f);
            const Rectangle interpolated = InterpolateRect(
                active, inactive,
                currentButtonSizes[static_cast<std::size_t>(index)]);
            const Rectangle destination(
                x, y - (interpolated.Height - inactive.Height) / 2,
                interpolated.Width, interpolated.Height);
            ui.RenderButton(destination,
                buttonRectangles[static_cast<std::size_t>(index)],
                selected ? Color::White : Color(192, 192, 192, 192));
            if (selected)
                ui.RenderButton(destination,
                    UIRenderer::MenuButtonSelectionGfxRect, Color::White);
            const Rectangle textDestination(
                x, destination.getBottomProperty() + ui.YToRes(5),
                destination.Width,
                destination.Height * textRectangles[0].Height /
                    buttonRectangles[0].Height);
            if (selected)
                ui.RenderButton(textDestination,
                    textRectangles[static_cast<std::size_t>(index)],
                    Color::White);
            if (UIRenderer::MouseInBox(input, destination))
                mouseOverButton = index;
            x += interpolated.Width + ui.XToRes(DistanceBetweenButtons);
        }

        if (!ignoreMouse && mouseOverButton >= 0)
            SetSelectedButton(mouseOverButton);

        if (input.leftPressed)
            pressedLeftMilliseconds += game.getMoveFactorPerSecondProperty() * 1000.0f;
        else
            pressedLeftMilliseconds = 0.0f;
        if (input.rightPressed)
            pressedRightMilliseconds += game.getMoveFactorPerSecondProperty() * 1000.0f;
        else
            pressedRightMilliseconds = 0.0f;

        if (input.leftJustPressed ||
            (pressedLeftMilliseconds > 250.0f && input.leftPressed))
        {
            pressedLeftMilliseconds -= 250.0f;
            game.PlaySound(Sounds::SoundCue::Highlight);
            SetSelectedButton((selectedButton + NumberOfButtons - 1) %
                              NumberOfButtons);
            ignoreMouse = true;
        }
        else if (input.rightJustPressed ||
                 (pressedRightMilliseconds > 250.0f && input.rightPressed))
        {
            pressedRightMilliseconds -= 250.0f;
            game.PlaySound(Sounds::SoundCue::Highlight);
            SetSelectedButton((selectedButton + 1) % NumberOfButtons);
            ignoreMouse = true;
        }

        bool exit = false;
        if ((mouseOverButton >= 0 && input.mouseLeftJustPressed) ||
            input.acceptJustPressed)
        {
            idleTime = 0.0f;
            switch (selectedButton)
            {
            case 0:
                game.AddGameScreen(std::make_unique<CarSelection>(game));
                break;
            case 1:
                game.AddGameScreen(std::make_unique<Highscores>(game));
                break;
            case 2:
                game.AddGameScreen(std::make_unique<Options>(game));
                break;
            case 3:
                game.AddGameScreen(std::make_unique<Help>(game));
                break;
            case 4:
                exit = true;
                break;
            default:
                break;
            }
        }

        if (input.backJustPressed)
            exit = true;
        idleTime += game.getMoveFactorPerSecondProperty() * 1000.0f;
        if (idleTime > TimeOutMenu)
        {
            idleTime = 0.0f;
            game.AddGameScreen(std::make_unique<SplashScreen>(game));
        }
        game.EndMenuFrame();
        return exit;
    }

    ScreenKind MainMenu::getKindProperty() const
    {
        return ScreenKind::MainMenu;
    }
}
