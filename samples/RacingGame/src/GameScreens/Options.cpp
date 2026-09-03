// SPDX-License-Identifier: MS-PL

#include "GameScreens/Options.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "GameLogic/Input.hpp"
#include "Graphics/UIRenderer.hpp"
#include "Properties/GameSettings.hpp"
#include "RacingGameManager.hpp"
#include "Sounds/Sound.hpp"

namespace RacingGame::GameScreens
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Rectangle;
    using RacingGame::Graphics::UIRenderer;

    namespace
    {
        const std::array<Rectangle, 3> SliderSources{
            Rectangle(384, 281, 448, 39),
            Rectangle(384, 354, 448, 39),
            Rectangle(384, 428, 448, 39)};
        const std::array<Rectangle, 3> ArrowSources{
            Rectangle(154, 284, 62, 39),
            Rectangle(160, 354, 62, 39),
            Rectangle(72, 437, 62, 39)};
        const std::array<Rectangle, 5> ResolutionSources{
            Rectangle(339, 112, 98, 32),
            Rectangle(454, 112, 98, 32),
            Rectangle(575, 112, 108, 32),
            Rectangle(704, 112, 116, 32),
            Rectangle(838, 112, 69, 32)};
        const std::array<Rectangle, 4> GraphicsOptionSources{
            Rectangle(339, 182, 105, 36),
            Rectangle(339, 226, 206, 36),
            Rectangle(616, 226, 90, 36),
            Rectangle(784, 226, 120, 36)};
        const Color SelectionColor(255, 156, 0, 160);
    }

    Options::Options(RacingGameManager& setGame) : game(setGame)
    {
        auto& settings = game.getSettingsProperty();
        currentPlayerName = settings.getPlayerNameProperty();
        const int width = game.getDisplayWidthProperty();
        const int height = game.getDisplayHeightProperty();
        if (width == 640 && height == 480) currentResolution = 0;
        else if (width == 800 && height == 600) currentResolution = 1;
        else if (width == 1024 && height == 768) currentResolution = 2;
        else if (width == 1280 && height == 1024) currentResolution = 3;
        fullscreen = game.getFullscreenProperty();
        usePostScreenShaders = settings.getPostScreenEffectsProperty();
        useShadowMapping = settings.getShadowMappingProperty();
        useHighDetail = settings.getHighDetailProperty();
        currentMusicVolume = settings.getMusicVolumeProperty();
        currentSoundVolume = settings.getSoundVolumeProperty();
        currentSensitivity = settings.getControllerSensitivityProperty();
    }

    void Options::Update(Microsoft::Xna::Framework::GameTime&) {}

    bool Options::Render()
    {
        game.BeginMenuFrame(true);
        auto& ui = game.getUIProperty();
        const auto& input = game.getControlsProperty();
        ui.RenderHeader(UIRenderer::HeaderOptionsGfxRect);
        ui.RenderOptionsScreen();

        ui.WriteText(
            ui.XToRes(352), ui.YToRes768(170),
            currentPlayerName +
                (static_cast<int>(game.getTotalTimeSecondsProperty() / 0.35f) %
                         2 == 0
                     ? "|" : ""));
        if (currentPlayerName.size() < 32)
        {
            const std::size_t remaining = 32 - currentPlayerName.size();
            currentPlayerName += input.typedText.substr(0, remaining);
        }
        if (input.backspaceJustPressed && !currentPlayerName.empty())
            currentPlayerName.pop_back();

        for (int index = 0; index < 5; ++index)
        {
            Rectangle destination = ui.CalcRectangleKeep4To3(
                ResolutionSources[static_cast<std::size_t>(index)]);
            destination.Y += ui.YToRes768(125);
            if (currentResolution == index)
                ui.RenderOptionsRegion(
                    destination,
                    ResolutionSources[static_cast<std::size_t>(index)],
                    SelectionColor);
            if (UIRenderer::MouseInBox(input, destination) &&
                input.mouseLeftJustPressed)
            {
                game.PlaySound(Sounds::SoundCue::ButtonClick);
                currentResolution = index;
            }
        }

        std::array<bool*, 4> graphicsValues{
            &fullscreen, &usePostScreenShaders, &useShadowMapping,
            &useHighDetail};
        for (int index = 0; index < 4; ++index)
        {
            Rectangle destination = ui.CalcRectangleKeep4To3(
                GraphicsOptionSources[static_cast<std::size_t>(index)]);
            destination.Y += ui.YToRes768(125);
            if (*graphicsValues[static_cast<std::size_t>(index)])
                ui.RenderOptionsRegion(
                    destination,
                    GraphicsOptionSources[static_cast<std::size_t>(index)],
                    SelectionColor);
            if (UIRenderer::MouseInBox(input, destination) &&
                input.mouseLeftJustPressed)
            {
                game.PlaySound(Sounds::SoundCue::ButtonClick);
                *graphicsValues[static_cast<std::size_t>(index)] =
                    !*graphicsValues[static_cast<std::size_t>(index)];
            }
        }

        std::array<float*, 3> values{
            &currentSoundVolume, &currentMusicVolume, &currentSensitivity};
        for (int index = 0; index < 3; ++index)
        {
            Rectangle slider = ui.CalcRectangleKeep4To3(
                SliderSources[static_cast<std::size_t>(index)]);
            slider.Y += ui.YToRes768(125);
            if (UIRenderer::MouseInBox(input, slider) &&
                input.mouseLeftJustPressed)
            {
                *values[static_cast<std::size_t>(index)] = std::clamp(
                    (input.mousePosition.X - slider.X) /
                        static_cast<float>(slider.Width),
                    0.0f, 1.0f);
                game.PlaySound(Sounds::SoundCue::Highlight);
            }
            if (currentOption == index)
            {
                if (input.leftJustPressed)
                {
                    *values[static_cast<std::size_t>(index)] -= 0.1f;
                    game.PlaySound(Sounds::SoundCue::Highlight);
                }
                if (input.rightJustPressed)
                {
                    *values[static_cast<std::size_t>(index)] += 0.1f;
                    game.PlaySound(Sounds::SoundCue::Highlight);
                }
                *values[static_cast<std::size_t>(index)] = std::clamp(
                    *values[static_cast<std::size_t>(index)], 0.0f, 1.0f);
            }
            const Rectangle radio = UIRenderer::SelectionRadioButtonGfxRect;
            ui.RenderButton(
                Rectangle(
                    slider.X + static_cast<int>(
                        slider.Width * *values[static_cast<std::size_t>(index)]) -
                        ui.XToRes(radio.Width) / 2,
                    slider.Y, ui.XToRes(radio.Width),
                    ui.YToRes768(radio.Height)),
                radio, Color::White);
            Rectangle arrow = ui.CalcRectangleKeep4To3(
                ArrowSources[static_cast<std::size_t>(index)]);
            arrow.Y += ui.YToRes768(125);
            arrow.X -= ui.XToRes(8 + static_cast<int>(std::nearbyint(
                8.0 * std::sin(game.getTotalTimeSecondsProperty() /
                               0.21212f))));
            if (currentOption == index)
                ui.RenderButton(arrow, UIRenderer::SelectionArrowGfxRect,
                                Color::White);
        }
        game.SetSoundVolumes(currentSoundVolume, currentMusicVolume);

        if (input.upJustPressed)
        {
            game.PlaySound(Sounds::SoundCue::Highlight);
            currentOption = (currentOption + 2) % 3;
        }
        else if (input.downJustPressed)
        {
            game.PlaySound(Sounds::SoundCue::Highlight);
            currentOption = (currentOption + 1) % 3;
        }
        (void)ui.RenderBottomButtons(input, true);
        const bool exit = input.backJustPressed || input.cancelJustPressed ||
                          ui.getBackButtonPressedProperty();
        if (exit)
        {
            auto& settings = game.getSettingsProperty();
            static constexpr std::array<int, 5> Widths{
                640, 800, 1024, 1280, 0};
            static constexpr std::array<int, 5> Heights{
                480, 600, 768, 1024, 0};
            settings.setPlayerNameProperty(currentPlayerName);
            settings.setResolutionWidthProperty(
                Widths[static_cast<std::size_t>(currentResolution)]);
            settings.setResolutionHeightProperty(
                Heights[static_cast<std::size_t>(currentResolution)]);
            settings.setFullscreenProperty(fullscreen);
            settings.setPostScreenEffectsProperty(usePostScreenShaders);
            settings.setShadowMappingProperty(useShadowMapping);
            settings.setHighDetailProperty(useHighDetail);
            settings.setMusicVolumeProperty(currentMusicVolume);
            settings.setSoundVolumeProperty(currentSoundVolume);
            settings.setControllerSensitivityProperty(currentSensitivity);
            settings.Save();
        }
        game.EndMenuFrame();
        return exit;
    }

    ScreenKind Options::getKindProperty() const
    {
        return ScreenKind::Options;
    }
}
