// SPDX-License-Identifier: MS-PL
#include "OrientationSample.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/TimeSpan.hpp"

namespace OrientationSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;
    using namespace Microsoft::Xna::Framework::Input::Touch;

    OrientationSample::OrientationSample()
        : graphics_(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        // Frame rate is 30 fps by default for Windows Phone.
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));

        // Four different scenarios for initializing orientation support are
        // presented by the original source. The shipped program uses scenario
        // #1: SupportedOrientations remains unchanged, so only landscape is
        // supported.

        // Scenario #2: portrait only (uncomment both lines).
        // graphics_.setPreferredBackBufferWidthProperty(480);
        // graphics_.setPreferredBackBufferHeightProperty(800);

        // Scenario #3: landscape at half resolution, exercising the phone's
        // hardware scaler (uncomment both lines).
        // graphics_.setPreferredBackBufferWidthProperty(400);
        // graphics_.setPreferredBackBufferHeightProperty(240);

        // Scenario #4: all orientations with dynamic tap-to-lock behavior
        // (uncomment the complete block).
        // graphics_.setSupportedOrientationsProperty(
        //     DisplayOrientation::Portrait |
        //     DisplayOrientation::LandscapeLeft |
        //     DisplayOrientation::LandscapeRight);
        // enableOrientationLocking_ = true;

        graphics_.setIsFullScreenProperty(true);
    }

    const std::string& OrientationSample::GetTypeName() const
    {
        static const std::string name = "OrientationSample.OrientationSample";
        return name;
    }

    void OrientationSample::Initialize()
    {
        TouchPanel::setEnabledGesturesProperty(GestureType::Tap);
        Game::Initialize();
    }

    void OrientationSample::LoadContent()
    {
        spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        directions_.emplace(getContentProperty().Load<Texture2D>("directions"));
        font_.emplace(getContentProperty().Load<SpriteFont>("Font"));
    }

    void OrientationSample::UnloadContent()
    {
        // Nothing to unload in this sample.
    }

    void OrientationSample::Update(GameTime& gameTime)
    {
        if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBackProperty() ==
            ButtonState::Pressed)
        {
            Exit();
        }

        if (enableOrientationLocking_)
        {
            while (TouchPanel::getIsGestureAvailableProperty())
            {
                const GestureSample gesture = TouchPanel::ReadGesture();
                if (gesture.getGestureTypeProperty() == GestureType::Tap)
                {
                    orientationLocked_ = !orientationLocked_;

                    if (orientationLocked_)
                    {
                        graphics_.setSupportedOrientationsProperty(
                            getWindowProperty().getCurrentOrientationProperty());
                        const Viewport& viewport =
                            getGraphicsDeviceProperty().getViewportProperty();
                        graphics_.setPreferredBackBufferWidthProperty(viewport.getWidthProperty());
                        graphics_.setPreferredBackBufferHeightProperty(viewport.getHeightProperty());
                    }
                    else
                    {
                        graphics_.setSupportedOrientationsProperty(
                            DisplayOrientation::LandscapeLeft |
                            DisplayOrientation::LandscapeRight |
                            DisplayOrientation::Portrait);
                    }

                    graphics_.ApplyChanges();
                }
            }
        }

        Game::Update(gameTime);
    }

    void OrientationSample::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

        spriteBatch_->Begin();

        const Viewport& viewport = getGraphicsDeviceProperty().getViewportProperty();
        const Vector2 position(
            static_cast<float>(viewport.getWidthProperty() / 2 - directions_->getWidthProperty() / 2),
            static_cast<float>(viewport.getHeightProperty() / 2 - directions_->getHeightProperty() / 2));
        spriteBatch_->Draw(*directions_, position, Color::White);

        if (enableOrientationLocking_)
        {
            const std::string currentState =
                orientationLocked_ ? "Orientation: Locked" : "Orientation: Unlocked";
            const std::string instructions =
                orientationLocked_ ? "Tap to unlock orientation." : "Tap to lock orientation.";

            spriteBatch_->DrawString(*font_, currentState, Vector2(10.0f, 10.0f), Color::White);
            spriteBatch_->DrawString(*font_, instructions, Vector2(10.0f, 25.0f), Color::White);
        }

        spriteBatch_->End();
        Game::Draw(gameTime);
    }
}
