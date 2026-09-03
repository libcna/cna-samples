// SPDX-License-Identifier: MS-PL

#include "GameScreens/SplashScreen.hpp"

#include "GameLogic/Input.hpp"
#include "Graphics/UIRenderer.hpp"
#include "RacingGameManager.hpp"

namespace RacingGame::GameScreens
{
    using RacingGame::Graphics::UIRenderer;

    SplashScreen::SplashScreen(RacingGameManager& setGame) : game(setGame) {}

    void SplashScreen::Update(Microsoft::Xna::Framework::GameTime&) {}

    bool SplashScreen::Render()
    {
        game.BeginMenuFrame(false, true, false);
        auto& ui = game.getUIProperty();
        ui.RenderBlackBar(518, 61);
        if (static_cast<int>(game.getTotalTimeSecondsProperty() / 0.375f) % 3 != 0)
        {
            const auto destination = ui.CalcRectangleCenteredWithGivenHeight(
                512, 518 + 61 / 2, 26, UIRenderer::PressStartGfxRect);
            ui.RenderButton(destination, UIRenderer::PressStartGfxRect,
                            Microsoft::Xna::Framework::Color::White);
        }
        game.EndMenuFrame();

        const auto& input = game.getControlsProperty();
        return input.mouseLeftJustPressed || input.acceptJustPressed ||
               input.backJustPressed || input.startPressed;
    }

    ScreenKind SplashScreen::getKindProperty() const
    {
        return ScreenKind::Splash;
    }
}
