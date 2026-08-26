// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleSampleGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "ParticleSampleGame.hpp"

#include "ExplosionParticleSystem.hpp"
#include "ExplosionSmokeParticleSystem.hpp"
#include "SmokePlumeParticleSystem.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/Int32.hpp"

namespace ParticleSample
{
    using namespace Microsoft::Xna::Framework::Input::Touch;

    System::Random ParticleSampleGame::random;

    ParticleSampleGame::ParticleSampleGame()
        : graphics(this)
    {
#if defined(WINDOWS_PHONE)
        graphics.setIsFullScreenProperty(true);

        // Frame rate is 30 fps by default for Windows Phone.
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
#endif

        getContentProperty().setRootDirectoryProperty("Content");

        // create the particle systems and add them to the components list.
        // we should never see more than one explosion at once
        explosion = std::make_unique<ExplosionParticleSystem>(*this, 1);
        getComponentsProperty().Add(explosion.get());

        // but the smoke from the explosion lingers a while.
        smoke = std::make_unique<ExplosionSmokeParticleSystem>(*this, 2);
        getComponentsProperty().Add(smoke.get());

        // we'll see lots of these effects at once; this is ok
        // because they have a fairly small number of particles per effect.
        smokePlume = std::make_unique<SmokePlumeParticleSystem>(*this, 9);
        getComponentsProperty().Add(smokePlume.get());

        // enable the tap gesture for changing particle effects
        TouchPanel::setEnabledGesturesProperty(GestureType::Tap);
    }

    ParticleSampleGame::~ParticleSampleGame() = default;

    const std::string& ParticleSampleGame::GetTypeName() const
    {
        static const std::string name = "ParticleSample.ParticleSampleGame";
        return name;
    }

    void ParticleSampleGame::LoadContent()
    {
        spriteBatch = std::make_unique<SpriteBatch>(*graphics.getGraphicsDeviceProperty());
        font = getContentProperty().Load<SpriteFont>("font");
    }

    void ParticleSampleGame::Update(GameTime& gameTime)
    {
        // check the input devices to see if someone has decided they want to see
        // the other effect, if they want to quit.
        HandleInput();

        const float dt =
            (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
        switch (currentState)
        {
            // if we should be demoing the explosions effect, check to see if it's
            // time for a new explosion.
            case State::Explosions:
                UpdateExplosions(dt);
                break;
            // if we're showing off the smoke plume, check to see if it's time for a
            // new puff of smoke.
            case State::SmokePlume:
                UpdateSmokePlume(dt);
                break;
        }

        // the base update will handle updating the particle systems themselves,
        // because we added them to the components collection.
        Game::Update(gameTime);
    }

    void ParticleSampleGame::UpdateSmokePlume(float dt)
    {
        timeTillPuff -= dt;
        if (timeTillPuff < 0)
        {
            Vector2 where = Vector2::Zero;
            // add more particles at the bottom of the screen, halfway across.
            where.X = (float)(graphics.getGraphicsDeviceProperty()
                                  ->getViewportProperty().getWidthProperty() / 2);
            where.Y = (float)graphics.getGraphicsDeviceProperty()
                                  ->getViewportProperty().getHeightProperty();
            smokePlume->AddParticles(where);

            // and then reset the timer.
            timeTillPuff = TimeBetweenSmokePlumePuffs;
        }
    }

    void ParticleSampleGame::UpdateExplosions(float dt)
    {
        timeTillExplosion -= dt;
        if (timeTillExplosion < 0)
        {
            Vector2 where = Vector2::Zero;
            // create the explosion at some random point on the screen.
            where.X = RandomBetween(0, (float)graphics.getGraphicsDeviceProperty()
                                           ->getViewportProperty().getWidthProperty());
            where.Y = RandomBetween(0, (float)graphics.getGraphicsDeviceProperty()
                                           ->getViewportProperty().getHeightProperty());

            // the overall explosion effect is actually comprised of two particle
            // systems: the fiery bit, and the smoke behind it. add particles to
            // both of those systems.
            explosion->AddParticles(where);
            smoke->AddParticles(where);

            // reset the timer.
            timeTillExplosion = TimeBetweenExplosions;
        }
    }

    void ParticleSampleGame::Draw(const GameTime& gameTime)
    {
        graphics.getGraphicsDeviceProperty()->Clear(Color::Black);

        spriteBatch->Begin();

        // draw some instructions on the screen
        const String message =
            "Current effect: " + ToString(currentState) + "!\n"
            "Hit the A button or space bar, or tap the screen, to switch.\n\n"
            "Free particles:\n"
            "    ExplosionParticleSystem:      " +
                System::Int32::ToString(explosion->getFreeParticleCountProperty()) + "\n"
            "    ExplosionSmokeParticleSystem: " +
                System::Int32::ToString(smoke->getFreeParticleCountProperty()) + "\n"
            "    SmokePlumeParticleSystem:     " +
                System::Int32::ToString(smokePlume->getFreeParticleCountProperty());
        spriteBatch->DrawString(*font, message, Vector2(50, 50), Color::White);

        spriteBatch->End();

        Game::Draw(gameTime);
    }

    void ParticleSampleGame::HandleInput()
    {
        KeyboardState currentKeyboardState = Keyboard::GetState();
        GamePadState currentGamePadState = GamePad::GetState(PlayerIndex::One);

        // Allows the game to exit
        if (currentGamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed ||
            currentKeyboardState.IsKeyDown(Keys::Escape))
            this->Exit();

        // check to see if someone has just released the space bar.
        const bool keyboardSpace =
            currentKeyboardState.IsKeyUp(Keys::Space) &&
            lastKeyboardState.IsKeyDown(Keys::Space);

        // check the gamepad to see if someone has just released the A button.
        const bool gamepadA =
            currentGamePadState.getButtonsProperty().getAProperty() == ButtonState::Pressed &&
            lastGamepadState.getButtonsProperty().getAProperty() == ButtonState::Released;

        // check our gestures to see if someone has tapped the screen. we want
        // to read all available gestures even if a tap occurred so we clear
        // the queue.
        bool tapGesture = false;
        while (TouchPanel::getIsGestureAvailableProperty())
        {
            const GestureSample sample = TouchPanel::ReadGesture();
            if (sample.getGestureTypeProperty() == GestureType::Tap)
            {
                tapGesture = true;
            }
        }

        // if either the A button or the space bar was just released, or the screen
        // was tapped, move to the next state. Doing modulus by the number of
        // states lets us wrap back around to the first state.
        if (keyboardSpace || gamepadA || tapGesture)
        {
            currentState = (State)(((int)currentState + 1) % NumStates);
        }

        lastKeyboardState = currentKeyboardState;
        lastGamepadState = currentGamePadState;
    }

    String ParticleSampleGame::ToString(State state)
    {
        // C#'s string.Format("{0}", currentState) prints the enum member's own name;
        // C++ has no such reflection, so the two names are spelled out here.
        switch (state)
        {
            case State::Explosions: return "Explosions";
            case State::SmokePlume: return "SmokePlume";
        }
        return "";
    }

    float ParticleSampleGame::RandomBetween(float min, float max)
    {
        return min + (float)random.NextDouble() * (max - min);
    }
}
