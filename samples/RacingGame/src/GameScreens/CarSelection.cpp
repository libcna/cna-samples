// SPDX-License-Identifier: MS-PL

#include "GameScreens/CarSelection.hpp"

#include <array>
#include <cmath>
#include <memory>
#include <string>

#include "GameLogic/CarPhysics.hpp"
#include "GameLogic/Input.hpp"
#include "GameScreens/TrackSelection.hpp"
#include "Graphics/UIRenderer.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "RacingGameManager.hpp"
#include "Sounds/Sound.hpp"

namespace RacingGame::GameScreens
{
    using GameLogic::CarPhysics;
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Rectangle;
    using RacingGame::Graphics::UIRenderer;

    namespace
    {
        const std::array<float, 3> CarTypeMaxSpeed{
            CarPhysics::DefaultMaxSpeed * 1.05f,
            CarPhysics::DefaultMaxSpeed,
            CarPhysics::DefaultMaxSpeed * 0.88f};
        const std::array<float, 3> CarTypeMass{
            CarPhysics::DefaultCarMass * 1.015f,
            CarPhysics::DefaultCarMass * 1.175f,
            CarPhysics::DefaultCarMass * 0.875f};
        const std::array<float, 3> CarTypeMaxAcceleration{
            CarPhysics::DefaultMaxAccelerationPerSec * 0.85f,
            CarPhysics::DefaultMaxAccelerationPerSec * 1.2f,
            CarPhysics::DefaultMaxAccelerationPerSec};
        const Rectangle PropertyBarSource(372, 297, 472, 6);
    }

    CarSelection::CarSelection(RacingGameManager& setGame) : game(setGame) {}
    void CarSelection::Update(Microsoft::Xna::Framework::GameTime&) {}

    void CarSelection::AdjustRotRange(
        float& desiredRotation, const float sourceRotation)
    {
        if (desiredRotation >= sourceRotation + MathHelper::Pi)
            desiredRotation -= MathHelper::TwoPi;
        if (desiredRotation < sourceRotation - MathHelper::Pi)
            desiredRotation += MathHelper::TwoPi;
    }

    void CarSelection::AdjustRotToPIRange(float& rotation)
    {
        if (rotation <= -MathHelper::Pi) rotation += MathHelper::TwoPi;
        if (rotation > MathHelper::Pi) rotation -= MathHelper::TwoPi;
    }

    float CarSelection::InterpolateRotation(
        float rotation, float targetRotation, const float nearlyEqualRotation)
    {
        AdjustRotRange(targetRotation, rotation);
        if (rotation > targetRotation)
            rotation = std::abs(rotation - targetRotation) < nearlyEqualRotation
                ? targetRotation : rotation - nearlyEqualRotation;
        else if (rotation < targetRotation)
            rotation = std::abs(rotation - targetRotation) < nearlyEqualRotation
                ? targetRotation : rotation + nearlyEqualRotation;
        AdjustRotToPIRange(rotation);
        return rotation;
    }

    bool CarSelection::Render()
    {
        auto& ui = game.getUIProperty();
        const auto& input = game.getControlsProperty();
        const float perCarRotation = MathHelper::TwoPi / 3.0f;
        carSelectionRotationZ = InterpolateRotation(
            carSelectionRotationZ,
            game.getCurrentCarNumberProperty() * perCarRotation,
            game.getMoveFactorPerSecondProperty() * 5.0f);

        game.PrepareCarSelectionShadows(carSelectionRotationZ);
        game.BeginMenuFrame(true, false);
        ui.RenderBlackBar(170, 390);
        ui.RenderHeader(UIRenderer::HeaderChooseCarGfxRect);
        ui.WriteText(ui.XToRes(85), ui.YToRes(512), "Car Color: ");

        for (int index = 0; index < game.getCarColorCountProperty(); ++index)
        {
            const bool selected = game.getCurrentCarColorProperty() == index;
            Rectangle destination = selected
                ? ui.CalcRectangle(250 + index * 50 - 6, 494, 58, 58)
                : ui.CalcRectangle(250 + index * 50, 500, 46, 46);
            const Color color = game.getCarColorProperty(index);
            ui.RenderColorSelection(destination, color);
            if (UIRenderer::MouseInBox(input, destination) &&
                input.mouseLeftPressed)
            {
                if (!selected)
                    game.PlaySound(Sounds::SoundCue::Highlight);
                game.setCurrentCarColorProperty(index);
            }
        }

        int car = game.getCurrentCarNumberProperty();
        CarPhysics::SetCarVariablesForCarType(
            CarTypeMaxSpeed[car], CarTypeMass[car],
            CarTypeMaxAcceleration[car]);
        const float maxSpeed = -1.5f + 2.45f *
            (CarTypeMaxSpeed[car] / CarPhysics::DefaultMaxSpeed);
        const float acceleration = -1.25f + 1.85f *
            (CarTypeMaxAcceleration[car] /
             CarPhysics::DefaultMaxAccelerationPerSec);
        const float mass = -0.65f + 1.5f *
            (CarTypeMass[car] / CarPhysics::DefaultCarMass);
        const float braking = -0.2f + acceleration - mass + maxSpeed;
        const float friction = -1.0f + (1.0f / mass + maxSpeed / 5.0f);
        const float engine = std::min(
            0.95f, -0.2f + 0.5f *
                (maxSpeed / mass + acceleration - maxSpeed * 5.0f + 5.0f));

        const auto showProperty = [&](const int y, const std::string& label,
                                      const float value)
        {
            const int x = ui.XToRes(1024 - 258);
            ui.WriteText(x, ui.YToRes(y), label);
            ui.RenderOptionsRegion(
                Rectangle(x, ui.YToRes(y) + ui.YToRes(29),
                          ui.XToRes(static_cast<int>(192.0f * value)),
                          ui.YToRes(6)),
                PropertyBarSource, Color::White);
        };
        showProperty(190, "Max Speed: " + std::to_string(static_cast<int>(
            CarTypeMaxSpeed[car] / CarPhysics::MphToMeterPerSec)) + "mph",
            maxSpeed);
        showProperty(235, "Acceleration:", acceleration);
        showProperty(280, "Car Mass:", mass);
        showProperty(335, "Braking:", braking);
        showProperty(390, "Friction:", friction);
        showProperty(445, "Engine:", engine);

        const float arrowWave = std::sin(
            game.getTotalTimeSecondsProperty() / 0.46f) *
            std::cos(game.getTotalTimeSecondsProperty() / 0.285f);
        const Rectangle arrowSource = UIRenderer::SelectionArrowGfxRect;
        Rectangle left = ui.CalcRectangle(
            35, 250, arrowSource.Width, arrowSource.Height);
        left.Y = ui.YToRes(360) + ui.YToRes(120) / 3;
        left.X += static_cast<int>(std::nearbyint(ui.XToRes(12) * arrowWave));
        ui.RenderButton(left,
            Rectangle(arrowSource.X + arrowSource.Width, arrowSource.Y,
                      -arrowSource.Width, arrowSource.Height), Color::White);
        Rectangle right = ui.CalcRectangle(
            1024 - 335 - arrowSource.Width, 250,
            arrowSource.Width, arrowSource.Height);
        right.Y = left.Y;
        right.X -= static_cast<int>(std::nearbyint(ui.XToRes(12) * arrowWave));
        ui.RenderButton(right, arrowSource, Color::White);

        const bool mouseSelectNext = input.mouseLeftJustPressed &&
            UIRenderer::MouseInBox(
                input, ui.CalcRectangle(562, 170, 362, 135));
        const bool mouseSelectPrevious = input.mouseLeftJustPressed &&
            UIRenderer::MouseInBox(
                input, ui.CalcRectangle(100, 170, 312, 135));
        if (input.leftJustPressed || mouseSelectNext)
        {
            game.PlaySound(Sounds::SoundCue::Highlight);
            game.setCurrentCarNumberProperty((car + 1) % 3);
        }
        else if (input.rightJustPressed || mouseSelectPrevious)
        {
            game.PlaySound(Sounds::SoundCue::Highlight);
            game.setCurrentCarNumberProperty((car + 2) % 3);
        }
        if (input.upJustPressed)
        {
            game.PlaySound(Sounds::SoundCue::Highlight);
            game.setCurrentCarColorProperty(
                (game.getCurrentCarColorProperty() +
                 game.getCarColorCountProperty() - 1) %
                game.getCarColorCountProperty());
        }
        else if (input.downJustPressed)
        {
            game.PlaySound(Sounds::SoundCue::Highlight);
            game.setCurrentCarColorProperty(
                (game.getCurrentCarColorProperty() + 1) %
                game.getCarColorCountProperty());
        }

        const bool mouseAccept = ui.RenderBottomButtons(input, false);
        if (input.acceptJustPressed || mouseAccept)
            game.AddGameScreen(std::make_unique<TrackSelection>(game));
        const bool exit = input.backJustPressed || input.cancelJustPressed ||
                          ui.getBackButtonPressedProperty();
        game.EndCarSelectionFrame(carSelectionRotationZ);
        return exit;
    }

    ScreenKind CarSelection::getKindProperty() const
    {
        return ScreenKind::CarSelection;
    }
}
