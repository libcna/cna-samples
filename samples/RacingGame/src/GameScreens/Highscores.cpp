// SPDX-License-Identifier: MS-PL

#include "GameScreens/Highscores.hpp"

#include <string>

#include "GameLogic/Input.hpp"
#include "Graphics/UIRenderer.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "RacingGameManager.hpp"
#include "Sounds/Sound.hpp"

namespace RacingGame::GameScreens
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Point;
    using RacingGame::Graphics::UIRenderer;

    Highscores::Highscores(RacingGameManager& setGame) : game(setGame) {}
    void Highscores::Update(Microsoft::Xna::Framework::GameTime&) {}

    bool Highscores::Render()
    {
        game.BeginMenuFrame(true);
        auto& ui = game.getUIProperty();
        const auto& input = game.getControlsProperty();
        ui.RenderBlackBar(160, 338);
        ui.RenderHeader(UIRenderer::HeaderHighscoresGfxRect);

        int x = ui.XToRes(512 - 160 * 3 / 2 + 25);
        const int trackY = ui.YToRes(182);
        const int lineHeight = ui.YToRes(27);
        constexpr const char* trackNames[]{"Beginner", "Advanced", "Expert"};
        for (int level = 0; level < 3; ++level)
        {
            const Rectangle hit(x, trackY, ui.XToRes(125), lineHeight);
            const bool over = UIRenderer::MouseInBox(input, hit);
            ui.WriteText(x, trackY, trackNames[level],
                selectedLevel == level ? Color::Yellow :
                over ? Color::White : Color::LightGray);
            if (over && input.mouseLeftJustPressed)
            {
                game.PlaySound(Sounds::SoundCue::ButtonClick);
                selectedLevel = level;
            }
            x += ui.XToRes(level == 1 ? 182 : 168);
        }
        if (input.leftJustPressed)
        {
            game.PlaySound(Sounds::SoundCue::ButtonClick);
            selectedLevel = (selectedLevel + 2) % 3;
        }
        else if (input.rightJustPressed)
        {
            game.PlaySound(Sounds::SoundCue::ButtonClick);
            selectedLevel = (selectedLevel + 1) % 3;
        }

        const int lineStart = ui.XToRes(300);
        const int lineEnd = ui.XToRes(640) + ui.GetTextWidth("5:67:89");
        const int separatorY = ui.YToRes(208);
        ui.AddLine(Point(lineStart, separatorY),
                   Point(lineEnd, separatorY), Color::Gray);
        ui.AddLine(Point(lineStart, separatorY + 2),
                   Point(lineEnd, separatorY + 2), Color::Gray);

        const int rankX = ui.XToRes(300);
        const int nameX = ui.XToRes(350);
        const int timeX = ui.XToRes(640);
        int y = ui.YToRes(220);
        const auto& times = game.getHighscoreTimesProperty(selectedLevel);
        for (int rank = 0; rank < 10; ++rank)
        {
            const Rectangle line(0, y, ui.XToRes(1024), lineHeight);
            const Color color = UIRenderer::MouseInBox(input, line)
                ? Color::White : Color(200, 200, 200);
            ui.WriteText(rankX, y, std::to_string(rank + 1) + '.', color);
            ui.WriteText(nameX, y,
                         game.getHighscoreNameProperty(selectedLevel, rank),
                         color);
            ui.WriteGameTime(timeX, y, times[static_cast<std::size_t>(rank)],
                             Color::Yellow);
            y += lineHeight;
        }
        (void)ui.RenderBottomButtons(input, true);
        const bool exit = input.backJustPressed || input.cancelJustPressed ||
            (input.mouseLeftJustPressed && input.mousePosition.Y > y);
        game.EndMenuFrame();
        return exit;
    }

    ScreenKind Highscores::getKindProperty() const
    {
        return ScreenKind::Highscores;
    }
}
