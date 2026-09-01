// SPDX-License-Identifier: MS-PL
#pragma once

#include <cmath>
#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Quaternion.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "System/TimeSpan.hpp"

#include "AccelerometerHelper.hpp"
#include "BallSimulation.hpp"
#include "DebugDraw.hpp"

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;
using namespace Microsoft::Xna::Framework::Input;

namespace TiltPerspectiveSample {

class ParallaxSample : public Game {
public:
    ParallaxSample() {
        getContentProperty().setRootDirectoryProperty("Content");
        graphics_ = std::make_unique<GraphicsDeviceManager>(this);

        graphics_->setPreferredBackBufferWidthProperty(480);
        graphics_->setPreferredBackBufferHeightProperty(800);

        graphics_->setIsFullScreenProperty(true);
        Microsoft::Xna::Framework::GamerServices::Guide::setIsScreenSaverEnabledProperty(false);

        setIsFixedTimeStepProperty(true);
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));

        accelerometer_ = std::make_unique<AccelerometerHelper>(*this);
        getComponentsProperty().Add(accelerometer_.get());

        ballSimulation_ = std::make_unique<BallSimulation>(*this);
        ballSimulation_->AddWalls(worldBox_);
        ballSimulation_->AddBalls(25, 25.0f, 75.0f, worldBox_);
        getComponentsProperty().Add(ballSimulation_.get());
    }

    const std::string& GetTypeName() const override {
        static const std::string name = "TiltPerspectiveSample.ParallaxSample";
        return name;
    }

protected:
    void Initialize() override {
        Game::Initialize();
    }

    void LoadContent() override {
        boxTexture_.emplace(getContentProperty().Load<Texture2D>("stone4"));

        worldGeometry_.emplace(DebugDraw::CreateBoxInterior(getGraphicsDeviceProperty(), worldBox_));
    }

    void Update(GameTime& gameTime) override {
        accelerometer_->Update(gameTime);

        GamePadState gamePadState = GamePad::GetState(PlayerIndex::One);
        if (gamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
            Exit();

        if (Microsoft::Xna::Framework::Input::Touch::TouchPanel::GetState().getCountProperty() > 0) {
            referenceDown_ = Vector3::Normalize(accelerometer_->getSmoothAccelerationProperty());
        }

        Game::Update(gameTime);
    }

    void Draw(const GameTime& gameTime) override {
        constexpr float zThreshold = 0.4f;

        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

        // Make the light always come from the actual ceiling.
        Vector3 lightDirection = Vector3::Normalize(accelerometer_->getSmoothAccelerationProperty());
        Vector3 eyeDirection = ComputeEyeVector();

        if (eyeDirection.Z < zThreshold) {
            // Limit how far we distort the perspective.
            eyeDirection.Z = zThreshold;
            eyeDirection.Normalize();
        }
        Vector3 eyePosition = eyeDirection * eyeDistance_;

        Matrix world = Matrix::getIdentityProperty();

        Matrix view = Matrix::CreateLookAt(Vector3(eyePosition.X, eyePosition.Y, eyePosition.Z),
                                            Vector3(eyePosition.X, eyePosition.Y, 0.0f), Vector3(0.0f, 1.0f, 0.0f));

        auto& vp = getGraphicsDeviceProperty().getViewportProperty();
        Matrix projection = Matrix::CreatePerspectiveOffCenter(
            (-eyePosition.X - vp.getWidthProperty() * 0.5f) * nearPlane_,
            (-eyePosition.X + vp.getWidthProperty() * 0.5f) * nearPlane_,
            (-eyePosition.Y - vp.getHeightProperty() * 0.5f) * nearPlane_,
            (-eyePosition.Y + vp.getHeightProperty() * 0.5f) * nearPlane_, eyePosition.Z * nearPlane_,
            farPlaneDistance_);

        worldGeometry_->getBasicEffectProperty().getDirectionalLight0Property().setDirectionProperty(lightDirection);
        worldGeometry_->Draw(world, view, projection, *boxTexture_);

        ballSimulation_->Draw(view, projection, lightDirection);

        Game::Draw(gameTime);
    }

private:
    // distance from eye to screen, in pixel units.
    static constexpr float eyeDistance_ = 2000.0f;

    // distance from eye to near clip plane, as a fraction of the Z distance
    // to the screen. We do it this way because we can end up with some
    // highly skewed projection matrices if the device is tilted enough.
    static constexpr float nearPlane_ = 0.5f;

    // distance from the eye to the far clip plane, in pixel units.
    static constexpr float farPlaneDistance_ = 4000.0f;

    // size of the box that everything takes place in, measured in pixel units.
    BoundingBox worldBox_{Vector3(-400.0f, -400.0f, -400.0f), Vector3(400.0f, 400.0f, 0.0f)};

    // "down" direction (smoothed accelerometer reading) to use as our
    // reference position. The user can reset this by touching the screen.
    Vector3 referenceDown_ = -Vector3::UnitZ;

    std::unique_ptr<GraphicsDeviceManager> graphics_;
    std::unique_ptr<AccelerometerHelper> accelerometer_;
    std::unique_ptr<BallSimulation> ballSimulation_;
    std::optional<DebugDraw> worldGeometry_;

    std::optional<Texture2D> boxTexture_;

    // Compute (guess) the user's eye direction, given a reference 'down'
    // direction and the current 'down' direction.
    //
    // 'world' vectors in this function refer to the actual real-world coords
    // from the (estimated) user's perspective. We don't really know where the
    // user is relative to the screen, so we have to make an assumption about
    // how they hold and tilt the device.
    [[nodiscard]] Vector3 ComputeEyeVector() const {
        float referencePitch = std::asin(referenceDown_.Y);
        constexpr float rollEpsilon = 0.1f;

        Vector3 worldDown = -accelerometer_->getSmoothAccelerationProperty();
        worldDown.Normalize();

        Vector3 worldRight = Vector3::Cross(Vector3::UnitY, worldDown);

        if (worldRight.LengthSquared() < rollEpsilon) {
            // The device is held nearly vertically, so the worldRight vector
            // isn't well-defined (its length is close to zero). Just use our
            // local right vector as the world right vector, which means we
            // generate an orientation with no roll.
            worldRight = Vector3::Right;
        } else {
            // We have a good 'right' vector; normalize it for
            // CreateFromAxisAngle() below.
            worldRight.Normalize();
        }
        Quaternion rot = Quaternion::CreateFromAxisAngle(worldRight, -referencePitch);
        return Vector3::Transform(worldDown, rot);
    }

};

} // namespace TiltPerspectiveSample
