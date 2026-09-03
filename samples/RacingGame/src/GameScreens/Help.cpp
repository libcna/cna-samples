// SPDX-License-Identifier: MS-PL

#include "GameScreens/Help.hpp"

#include "GameLogic/Input.hpp"
#include "Graphics/UIRenderer.hpp"
#include "RacingGameManager.hpp"

namespace RacingGame::GameScreens
{
    using RacingGame::Graphics::UIRenderer;

    Help::Help(RacingGameManager& setGame) : game(setGame) {}

    void Help::Update(Microsoft::Xna::Framework::GameTime&) {}

    bool Help::Render()
    {
        game.BeginMenuFrame(true);
        auto& ui = game.getUIProperty();
        ui.RenderHeader(UIRenderer::HeaderHelpGfxRect);
        ui.RenderHelpScreen();
        (void)ui.RenderBottomButtons(game.getControlsProperty(), true);
        game.EndMenuFrame();
        const auto& input = game.getControlsProperty();
        return input.backJustPressed || input.cancelJustPressed ||
               input.mouseLeftJustPressed;
    }

    ScreenKind Help::getKindProperty() const { return ScreenKind::Help; }
}
