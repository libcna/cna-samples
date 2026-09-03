// SPDX-License-Identifier: MS-PL

#include "GameScreens/TrackSelection.hpp"

#include <algorithm>
#include <array>
#include <memory>

#include "GameLogic/Input.hpp"
#include "GameScreens/GameScreen.hpp"
#include "GameScreens/MainMenu.hpp"
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
        constexpr int NumberOfButtons = 3;
    }

    TrackSelection::TrackSelection(RacingGameManager& setGame) : game(setGame) {}
    void TrackSelection::Update(Microsoft::Xna::Framework::GameTime&) {}

    bool TrackSelection::Render()
    {
        const std::array<Rectangle, NumberOfButtons> buttonRectangles{
            UIRenderer::TrackButtonBeginnerGfxRect,
            UIRenderer::TrackButtonAdvancedGfxRect,
            UIRenderer::TrackButtonExpertGfxRect};
        const std::array<Rectangle, NumberOfButtons> textRectangles{
            UIRenderer::TrackTextBeginnerGfxRect,
            UIRenderer::TrackTextAdvancedGfxRect,
            UIRenderer::TrackTextExpertGfxRect};
        game.BeginMenuFrame(true);
        auto& ui = game.getUIProperty();
        const auto& input = game.getControlsProperty();
        ui.RenderBlackBar(220, 280);
        ui.RenderHeader(UIRenderer::HeaderSelectTrackGfxRect);
        if (input.hasMouseMoved || input.mouseLeftJustPressed)
            ignoreMouse = false;

        const Rectangle active = ui.CalcRectangleCenteredWithGivenHeight(
            0, 0, 132 * buttonRectangles[0].Height /
                buttonRectangles[0].Width, buttonRectangles[0]);
        const Rectangle inactive = ui.CalcRectangleCenteredWithGivenHeight(
            0, 0, 108 * buttonRectangles[0].Height /
                buttonRectangles[0].Width, buttonRectangles[0]);
        const int totalWidth = active.Width + 2 * inactive.Width +
                               2 * ui.XToRes(32);
        int x = ui.XToRes(512) - totalWidth / 2;
        const int y = ui.YToRes(258);
        int mouseOver = -1;
        for (int index = 0; index < NumberOfButtons; ++index)
        {
            const bool selected = index == game.getSelectedTrackNumberProperty();
            auto& size = currentButtonSizes[static_cast<std::size_t>(index)];
            size = std::clamp(size + (selected ? 1.0f : -1.0f) *
                game.getMoveFactorPerSecondProperty() * 2.0f, 0.0f, 1.0f);
            const Rectangle interpolated = MainMenu::InterpolateRect(
                active, inactive, size);
            const Rectangle destination(
                x, y - (interpolated.Height - inactive.Height) / 2,
                interpolated.Width, interpolated.Height);
            ui.RenderButton(destination, buttonRectangles[index],
                selected ? Color::White : Color(192, 192, 192, 192));
            if (selected)
                ui.RenderButton(destination,
                    UIRenderer::TrackButtonSelectionGfxRect, Color::White);
            const Rectangle textDestination(
                x, destination.getBottomProperty() + ui.YToRes(5),
                destination.Width,
                destination.Height * textRectangles[0].Height /
                    buttonRectangles[0].Height);
            if (selected)
                ui.RenderButton(textDestination, textRectangles[index],
                                Color::White);
            if (UIRenderer::MouseInBox(input, destination)) mouseOver = index;
            x += interpolated.Width + ui.XToRes(32);
        }
        if (!ignoreMouse && mouseOver >= 0)
            game.setSelectedTrackNumberProperty(mouseOver);
        if (input.leftJustPressed)
        {
            game.PlaySound(Sounds::SoundCue::ButtonClick);
            game.setSelectedTrackNumberProperty(
                (game.getSelectedTrackNumberProperty() + 2) % 3);
            ignoreMouse = true;
        }
        else if (input.rightJustPressed)
        {
            game.PlaySound(Sounds::SoundCue::ButtonClick);
            game.setSelectedTrackNumberProperty(
                (game.getSelectedTrackNumberProperty() + 1) % 3);
            ignoreMouse = true;
        }
        const bool mouseAccept = ui.RenderBottomButtons(input, false);
        if ((mouseOver >= 0 && input.mouseLeftJustPressed) || mouseAccept ||
            input.acceptJustPressed)
            game.AddGameScreen(std::make_unique<GameScreen>(game));
        const bool exit = input.backJustPressed || input.cancelJustPressed ||
                          ui.getBackButtonPressedProperty();
        game.EndMenuFrame();
        return exit;
    }

    ScreenKind TrackSelection::getKindProperty() const
    {
        return ScreenKind::TrackSelection;
    }
}
