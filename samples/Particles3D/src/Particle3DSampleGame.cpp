// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Particle3DSampleGame.hpp"

#include <cmath>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerStateCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/Random.hpp"

#include "ParticleSystems/ExplosionParticleSystem.hpp"
#include "ParticleSystems/ExplosionSmokeParticleSystem.hpp"
#include "ParticleSystems/FireParticleSystem.hpp"
#include "ParticleSystems/ProjectileTrailParticleSystem.hpp"
#include "ParticleSystems/SmokePlumeParticleSystem.hpp"

namespace Particle3DSample
{
    namespace
    {
        // Random number generator for the fire effect.
        System::Random& GameRandom()
        {
            static System::Random random;
            return random;
        }

        /// The C# enum's ToString(), which the overlay prints verbatim.
        const char* StateName(int state)
        {
            switch (state)
            {
                case 0: return "Explosions";
                case 1: return "SmokePlume";
                case 2: return "RingOfFire";
            }
            return "";
        }
    }

    Particle3DSampleGame::Particle3DSampleGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        // Construct our particle system components.
        explosionParticles =
            std::make_unique<ExplosionParticleSystem>(*this, getContentProperty());
        explosionSmokeParticles =
            std::make_unique<ExplosionSmokeParticleSystem>(*this, getContentProperty());
        projectileTrailParticles =
            std::make_unique<ProjectileTrailParticleSystem>(*this, getContentProperty());
        smokePlumeParticles =
            std::make_unique<SmokePlumeParticleSystem>(*this, getContentProperty());
        fireParticles = std::make_unique<FireParticleSystem>(*this, getContentProperty());

        // Set the draw order so the explosions and fire
        // will appear over the top of the smoke.
        smokePlumeParticles->setDrawOrderProperty(100);
        explosionSmokeParticles->setDrawOrderProperty(200);
        projectileTrailParticles->setDrawOrderProperty(300);
        explosionParticles->setDrawOrderProperty(400);
        fireParticles->setDrawOrderProperty(500);

        // Register the particle system components.
        getComponentsProperty().Add(explosionParticles.get());
        getComponentsProperty().Add(explosionSmokeParticles.get());
        getComponentsProperty().Add(projectileTrailParticles.get());
        getComponentsProperty().Add(smokePlumeParticles.get());
        getComponentsProperty().Add(fireParticles.get());
    }

    const std::string& Particle3DSampleGame::GetTypeName() const
    {
        static const std::string typeName{"Particle3DSample.Particle3DSampleGame"};
        return typeName;
    }

    void Particle3DSampleGame::LoadContent()
    {
        spriteBatch.emplace(*graphics.getGraphicsDeviceProperty());

        font.emplace(getContentProperty().Load<SpriteFont>("font"));
        grid.emplace(getContentProperty().Load<Model>("grid"));
    }

    void Particle3DSampleGame::Update(GameTime& gameTime)
    {
        HandleInput();

        UpdateCamera(gameTime);

        switch (currentState)
        {
            case ParticleState::Explosions:
                UpdateExplosions(gameTime);
                break;

            case ParticleState::SmokePlume:
                UpdateSmokePlume();
                break;

            case ParticleState::RingOfFire:
                UpdateFire();
                break;
        }

        UpdateProjectiles(gameTime);

        Game::Update(gameTime);
    }

    void Particle3DSampleGame::UpdateExplosions(const GameTime& gameTime)
    {
        timeToNextProjectile -= gameTime.getElapsedGameTimeProperty();

        if (timeToNextProjectile <= System::TimeSpan::Zero)
        {
            // Create a new projectile once per second. The real work of moving
            // and creating particles is handled inside the Projectile class.
            projectiles.emplace_back(*explosionParticles, *explosionSmokeParticles,
                                     *projectileTrailParticles);

            timeToNextProjectile += System::TimeSpan::FromSeconds(1);
        }
    }

    void Particle3DSampleGame::UpdateProjectiles(const GameTime& gameTime)
    {
        std::size_t i = 0;

        while (i < projectiles.size())
        {
            if (!projectiles[i].Update(gameTime))
            {
                // Remove projectiles at the end of their life.
                projectiles.erase(projectiles.begin() + (std::ptrdiff_t)i);
            }
            else
            {
                // Advance to the next projectile.
                i++;
            }
        }
    }

    void Particle3DSampleGame::UpdateSmokePlume()
    {
        // This is trivial: we just create one new smoke particle per frame.
        smokePlumeParticles->AddParticle(Vector3::Zero, Vector3::Zero);
    }

    void Particle3DSampleGame::UpdateFire()
    {
        constexpr int fireParticlesPerFrame = 20;

        // Create a number of fire particles, randomly positioned around a circle.
        for (int i = 0; i < fireParticlesPerFrame; i++)
        {
            fireParticles->AddParticle(RandomPointOnCircle(), Vector3::Zero);
        }

        // Create one smoke particle per frmae, too.
        smokePlumeParticles->AddParticle(RandomPointOnCircle(), Vector3::Zero);
    }

    Vector3 Particle3DSampleGame::RandomPointOnCircle()
    {
        constexpr float radius = 30;
        constexpr float height = 40;

        const double angle = GameRandom().NextDouble() * MathHelper::Pi * 2;

        const float x = (float)std::cos(angle);
        const float y = (float)std::sin(angle);

        return Vector3(x * radius, y * radius + height, 0);
    }

    void Particle3DSampleGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        device.Clear(Color::CornflowerBlue);

        // Compute camera matrices.
        const float aspectRatio = (float)device.getViewportProperty().getWidthProperty() /
                                  (float)device.getViewportProperty().getHeightProperty();

        const Matrix view = Matrix::CreateTranslation(0, -25, 0) *
                            Matrix::CreateRotationY(MathHelper::ToRadians(cameraRotation)) *
                            Matrix::CreateRotationX(MathHelper::ToRadians(cameraArc)) *
                            Matrix::CreateLookAt(Vector3(0, 0, -cameraDistance),
                                                 Vector3(0, 0, 0), Vector3::Up);

        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(MathHelper::PiOver4,
                                                                       aspectRatio,
                                                                       1, 10000);

        // Pass camera matrices through to the particle system components.
        explosionParticles->SetCamera(view, projection);
        explosionSmokeParticles->SetCamera(view, projection);
        projectileTrailParticles->SetCamera(view, projection);
        smokePlumeParticles->SetCamera(view, projection);
        fireParticles->SetCamera(view, projection);

        // Draw our background grid and message text.
        DrawGrid(view, projection);

        DrawMessage();

        // This will draw the particle system components.
        Game::Draw(gameTime);
    }

    void Particle3DSampleGame::DrawGrid(const Matrix& view, const Matrix& projection)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        device.setBlendStateProperty(BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.getSamplerStatesProperty()[0] = SamplerState::LinearWrap;

        grid->Draw(Matrix::getIdentityProperty(), view, projection);
    }

    void Particle3DSampleGame::DrawMessage()
    {
        const std::string message =
            std::string("Current effect: ") + StateName((int)currentState) + "!!!\n" +
            "Hit the A button or space bar to switch.";

        spriteBatch->Begin();
        spriteBatch->DrawString(*font, message, Vector2(50, 50), Color::White);
        spriteBatch->End();
    }

    void Particle3DSampleGame::HandleInput()
    {
        lastKeyboardState = currentKeyboardState;
        lastGamePadState = currentGamePadState;

        currentKeyboardState = Input::Keyboard::GetState();
        currentGamePadState = Input::GamePad::GetState(PlayerIndex::One);

        // Check for exit.
        if (currentKeyboardState.IsKeyDown(Input::Keys::Escape) ||
            currentGamePadState.getButtonsProperty().getBackProperty() ==
                Input::ButtonState::Pressed)
        {
            Exit();
        }

        // Check for changing the active particle effect.
        if (((currentKeyboardState.IsKeyDown(Input::Keys::Space) &&
             (lastKeyboardState.IsKeyUp(Input::Keys::Space))) ||
            ((currentGamePadState.getButtonsProperty().getAProperty() ==
                  Input::ButtonState::Pressed)) &&
             (lastGamePadState.getButtonsProperty().getAProperty() ==
                  Input::ButtonState::Released)))
        {
            currentState = (ParticleState)((int)currentState + 1);

            if (currentState > ParticleState::RingOfFire)
                currentState = (ParticleState)0;
        }
    }

    void Particle3DSampleGame::UpdateCamera(const GameTime& gameTime)
    {
        const float time =
            (float)gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty();

        // Check for input to rotate the camera up and down around the model.
        if (currentKeyboardState.IsKeyDown(Input::Keys::Up) ||
            currentKeyboardState.IsKeyDown(Input::Keys::W))
        {
            cameraArc += time * 0.025f;
        }

        if (currentKeyboardState.IsKeyDown(Input::Keys::Down) ||
            currentKeyboardState.IsKeyDown(Input::Keys::S))
        {
            cameraArc -= time * 0.025f;
        }

        cameraArc += currentGamePadState.getThumbSticksProperty().getRightProperty().Y *
                     time * 0.05f;

        // Limit the arc movement.
        if (cameraArc > 90.0f)
            cameraArc = 90.0f;
        else if (cameraArc < -90.0f)
            cameraArc = -90.0f;

        // Check for input to rotate the camera around the model.
        if (currentKeyboardState.IsKeyDown(Input::Keys::Right) ||
            currentKeyboardState.IsKeyDown(Input::Keys::D))
        {
            cameraRotation += time * 0.05f;
        }

        if (currentKeyboardState.IsKeyDown(Input::Keys::Left) ||
            currentKeyboardState.IsKeyDown(Input::Keys::A))
        {
            cameraRotation -= time * 0.05f;
        }

        cameraRotation += currentGamePadState.getThumbSticksProperty().getRightProperty().X *
                          time * 0.1f;

        // Check for input to zoom camera in and out.
        if (currentKeyboardState.IsKeyDown(Input::Keys::Z))
            cameraDistance += time * 0.25f;

        if (currentKeyboardState.IsKeyDown(Input::Keys::X))
            cameraDistance -= time * 0.25f;

        cameraDistance += currentGamePadState.getTriggersProperty().getLeftProperty() * time * 0.5f;
        cameraDistance -= currentGamePadState.getTriggersProperty().getRightProperty() * time * 0.5f;

        // Limit the camera distance.
        if (cameraDistance > 500)
            cameraDistance = 500;
        else if (cameraDistance < 10)
            cameraDistance = 10;

        if (currentGamePadState.getButtonsProperty().getRightStickProperty() ==
                Input::ButtonState::Pressed ||
            currentKeyboardState.IsKeyDown(Input::Keys::R))
        {
            cameraArc = -5;
            cameraRotation = 0;
            cameraDistance = 200;
        }
    }
}
