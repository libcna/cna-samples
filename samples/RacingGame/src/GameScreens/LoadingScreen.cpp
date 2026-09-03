// SPDX-License-Identifier: MS-PL

#include "GameScreens/LoadingScreen.hpp"

#include <cmath>

#include "Graphics/UIRenderer.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "RacingGameManager.hpp"

namespace RacingGame::GameScreens
{
    LoadingScreen::LoadingScreen(RacingGameManager& setGame) : game(setGame) {}

    void LoadingScreen::Update(Microsoft::Xna::Framework::GameTime&)
    {
        game.AdvanceLoading();
    }

    bool LoadingScreen::Render()
    {
        game.BeginMenuFrame(false, false, false);
        auto& ui = game.getUIProperty();
        constexpr const char* LoadingText = "Loading...";
        int x = game.getDisplayWidthProperty() / 2 - 50;
        const int y = game.getDisplayHeightProperty() / 2 - 20;
        for (int index = 0; LoadingText[index] != '\0'; ++index)
        {
            const std::string character(1, LoadingText[index]);
            const int characterY = y + static_cast<int>(7.0 * std::abs(
                std::sin(index / 4.0f -
                         game.getTotalTimeSecondsProperty() * 3.0f)));
            ui.WriteText(x, characterY, character,
                         Microsoft::Xna::Framework::Color::Red);
            x += ui.GetTextWidth(character);
        }
        ui.WriteTextCentered(
            game.getDisplayWidthProperty() / 2, y + 40,
            game.getLoadingStatusProperty());
        game.EndMenuFrame();
        return game.getContentLoadedProperty();
    }

    ScreenKind LoadingScreen::getKindProperty() const
    {
        return ScreenKind::Loading;
    }
}
