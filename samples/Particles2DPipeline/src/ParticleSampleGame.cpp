// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleSampleGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ParticleSampleGame.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"

#include "ParticleHelpers.hpp"
#include "ParticleSystemSettingsReader.hpp"
#include "System/Int32.hpp"

namespace Particles2DPipelineSample
{
    namespace
    {
        /// The C# enum's ToString(), which the overlay prints verbatim.
        const char* StateName(int state)
        {
            switch (state)
            {
                case 0: return "Explosions";
                case 1: return "SmokePlume";
                case 2: return "Emitter";
            }
            return "";
        }
    }

    ParticleSampleGame::ParticleSampleGame()
        : graphics(this)
    {
        // Not a line of the original: XNA's content pipeline reflects over ParticleSystemSettings
        // at load time and CNA has no such reflection, so the game registers the reader for the
        // type it owns. See ParticleSystemSettingsReader.hpp.
        RegisterParticleSystemSettingsReader();

        getContentProperty().setRootDirectoryProperty("Content");

        explosion = std::make_unique<ParticleSystem>(*this, "ExplosionSettings");
        explosion->setDrawOrderProperty(ParticleSystem::AdditiveDrawOrder);
        getComponentsProperty().Add(explosion.get());

        smoke = std::make_unique<ParticleSystem>(*this, "ExplosionSmokeSettings");
        smoke->setDrawOrderProperty(ParticleSystem::AlphaBlendDrawOrder);
        getComponentsProperty().Add(smoke.get());

        smokePlume = std::make_unique<ParticleSystem>(*this, "SmokePlumeSettings");
        smokePlume->setDrawOrderProperty(ParticleSystem::AlphaBlendDrawOrder);
        getComponentsProperty().Add(smokePlume.get());

        emitterSystem = std::make_unique<ParticleSystem>(*this, "EmitterSettings");
        emitterSystem->setDrawOrderProperty(ParticleSystem::AlphaBlendDrawOrder);
        getComponentsProperty().Add(emitterSystem.get());

        emitter = std::make_unique<ParticleEmitter>(*emitterSystem, 60, Vector2(400, 240));
    }

    const std::string& ParticleSampleGame::GetTypeName() const
    {
        static const std::string typeName{"Particles2DPipelineSample.ParticleSampleGame"};
        return typeName;
    }

    void ParticleSampleGame::LoadContent()
    {
        spriteBatch.emplace(*graphics.getGraphicsDeviceProperty());
        font.emplace(getContentProperty().Load<SpriteFont>("font"));
        emitterSprite.emplace(getContentProperty().Load<Texture2D>("BlockEmitter"));
    }

    void ParticleSampleGame::Update(GameTime& gameTime)
    {
        HandleInput();

        const float dt = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        switch (currentState)
        {
            case State::Explosions:
                UpdateExplosions(dt);
                break;

            case State::SmokePlume:
                UpdateSmokePlume(dt);
                break;

            case State::Emitter:
                UpdateEmitter(gameTime);
                break;
        }

        Game::Update(gameTime);
    }

    void ParticleSampleGame::UpdateEmitter(const GameTime& gameTime)
    {
        const Input::MouseState mouseState = Input::Mouse::GetState();
        const Vector2 newPosition((float)mouseState.getXProperty(),
                                  (float)mouseState.getYProperty());

        emitter->Update(gameTime, newPosition);
    }

    void ParticleSampleGame::UpdateSmokePlume(float dt)
    {
        timeTillPuff -= dt;
        if (timeTillPuff < 0)
        {
            Vector2 where = Vector2::Zero;
            where.X = (float)graphics.getGraphicsDeviceProperty()
                          ->getViewportProperty().getWidthProperty() / 2;
            where.Y = (float)graphics.getGraphicsDeviceProperty()
                          ->getViewportProperty().getHeightProperty();

            smokePlume->AddParticles(where, Vector2::Zero);
            timeTillPuff = TimeBetweenSmokePlumePuffs;
        }
    }

    void ParticleSampleGame::UpdateExplosions(float dt)
    {
        timeTillExplosion -= dt;
        if (timeTillExplosion < 0)
        {
            Vector2 where = Vector2::Zero;
            where.X = ParticleHelpers::RandomBetween(
                0, (float)graphics.getGraphicsDeviceProperty()
                       ->getViewportProperty().getWidthProperty());
            where.Y = ParticleHelpers::RandomBetween(
                0, (float)graphics.getGraphicsDeviceProperty()
                       ->getViewportProperty().getHeightProperty());

            explosion->AddParticles(where, Vector2::Zero);
            smoke->AddParticles(where, Vector2::Zero);

            timeTillExplosion = TimeBetweenExplosions;
        }
    }

    void ParticleSampleGame::Draw(const GameTime& gameTime)
    {
        graphics.getGraphicsDeviceProperty()->Clear(Color::Black);

        spriteBatch->Begin();

        const std::string message =
            std::string("Current effect: ") + StateName((int)currentState) + "!\n" +
            "Hit the A button or space bar, or tap the screen, to switch.\n\n" +
            "Free particles:\n" +
            "    ExplosionParticleSystem:      " +
                System::Int32::ToString(explosion->getFreeParticleCountProperty()) + "\n" +
            "    ExplosionSmokeParticleSystem: " +
                System::Int32::ToString(smoke->getFreeParticleCountProperty()) + "\n" +
            "    SmokePlumeParticleSystem:     " +
                System::Int32::ToString(smokePlume->getFreeParticleCountProperty()) + "\n" +
            "    EmitterParticleSystem:        " +
                System::Int32::ToString(emitterSystem->getFreeParticleCountProperty());

        spriteBatch->DrawString(*font, message, Vector2(50, 50), Color::White);

        if (currentState == State::Emitter)
        {
            spriteBatch->Draw(
                *emitterSprite,
                emitter->getPositionProperty() -
                    Vector2((float)(emitterSprite->getWidthProperty() / 2),
                            (float)(emitterSprite->getHeightProperty() / 2)),
                Color::White);
        }

        spriteBatch->End();

        Game::Draw(gameTime);
    }

    void ParticleSampleGame::HandleInput()
    {
        const Input::KeyboardState currentKeyboardState = Input::Keyboard::GetState();
        const Input::GamePadState currentGamePadState = Input::GamePad::GetState(PlayerIndex::One);

        if (currentGamePadState.getButtonsProperty().getBackProperty() ==
                Input::ButtonState::Pressed ||
            currentKeyboardState.IsKeyDown(Input::Keys::Escape))
            Exit();

        const bool keyboardSpace =
            currentKeyboardState.IsKeyUp(Input::Keys::Space) &&
            lastKeyboardState.IsKeyDown(Input::Keys::Space);

        const bool gamepadA =
            currentGamePadState.getButtonsProperty().getAProperty() ==
                Input::ButtonState::Pressed &&
            lastGamepadState.getButtonsProperty().getAProperty() ==
                Input::ButtonState::Released;

        if (keyboardSpace || gamepadA)
        {
            currentState = (State)(((int)currentState + 1) % NumStates);
        }

        lastKeyboardState = currentKeyboardState;
        lastGamepadState = currentGamePadState;
    }
}
