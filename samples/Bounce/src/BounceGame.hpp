// SPDX-License-Identifier: MS-PL
#pragma once

#include <algorithm>
#include <cmath>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "Accelerometer.hpp"
#include "Sphere.hpp"
#include "SpherePrimitive.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "System/Random.hpp"
#include "System/TimeSpan.hpp"

namespace Bounce
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    class Game1 : public Game
    {
        GraphicsDeviceManager graphics;

        KeyboardState currentKeyboardState;
        KeyboardState lastKeyboardState;
        GamePadState currentGamePadState;
        GamePadState lastGamePadState;

        std::unique_ptr<GeometricPrimitive> primitive;
        std::vector<Sphere> spheres;

        static constexpr float worldSize = 3.00f;
        static constexpr float floorPlaneHeight = -1.0f;
        static constexpr float numSpheres = 100.0f;
        static constexpr float collisionDamping = 0.75f;

        float accelhistory[2] = {0.0f, 0.0f};

        Color sphereColors[5] = {
            Color::Red,
            Color::Green,
            Color::Blue,
            Color::White,
            Color::Black,
        };

    public:
        Game1()
            : graphics(this)
        {
            getContentProperty().setRootDirectoryProperty("Content");
            graphics.setIsFullScreenProperty(true);
            setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
        }

        [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name = "Bounce.Game1";
            return name;
        }

    protected:
        void LoadContent() override
        {
            Accelerometer::Initialize();
            primitive = std::make_unique<SpherePrimitive>(getGraphicsDeviceProperty());

            System::Random random;
            const int numsphereColors = static_cast<int>(std::size(sphereColors));

            float xpos = -10.0f;
            float zpos = -2.0f;
            const float ypos = floorPlaneHeight;

            for (int i = 0; i < numSpheres; ++i)
            {
                Sphere newSphere;
                newSphere.Velocity.X = 0.2f * random.Next(-10, 10);
                newSphere.Velocity.Z = 0.2f * random.Next(-10, 10);
                newSphere.Velocity.Y = 0.2f * random.Next(-3, 3);
                newSphere.Color = sphereColors[i % numsphereColors];
                newSphere.Radius = 0.10f + (static_cast<float>(random.Next(100)) / 100.0f) * 0.15f;
                newSphere.Position.X = xpos;
                newSphere.Position.Y = ypos + newSphere.Radius * 6.0f;
                newSphere.Position.Z = zpos;
                newSphere.Mass = MathHelper::Pi *
                    (newSphere.Radius * newSphere.Radius * newSphere.Radius);
                spheres.push_back(newSphere);

                xpos += 1.5f;
                if (xpos > 20.0f)
                {
                    xpos = -10.0f;
                    zpos -= 1.5f;
                }
            }
        }

        void Update(GameTime& gameTime) override
        {
            HandleInput();
            UpdateSpheres(gameTime);
            Game::Update(gameTime);
        }

        void Draw(const GameTime& gameTime) override
        {
            getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

            const float time = static_cast<float>(
                gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());
            const float yaw = time * 0.4f;
            const float pitch = time * 0.7f;
            const float roll = time * 1.1f;
            const Vector3 cameraLookat(0.0f, 0.0f, 2.5f);
            const float aspect = getGraphicsDeviceProperty()
                .getViewportProperty().getAspectRatioProperty();

            Matrix world = Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);
            const Matrix view = Matrix::CreateLookAt(cameraLookat, Vector3::Zero, Vector3::Up);
            const Matrix projection = Matrix::CreatePerspectiveFieldOfView(1.0f, aspect, 1.0f, 100.0f);

            Matrix worldTranslation = Matrix::CreateTranslation(0.0f, 0.0f, -5.0f);
            world = world * worldTranslation;

            GeometricPrimitive* currentPrimitive = primitive.get();

            Matrix shadowMatrix = Matrix::getIdentityProperty();
            shadowMatrix.M12 = 0.0f;
            shadowMatrix.M22 = 0.0f;
            shadowMatrix.M23 = 0.0f;

            for (int i = 0; i < numSpheres; ++i)
            {
                const Matrix matScale = Matrix::CreateScale(spheres[i].Radius / 0.5f);
                Matrix worldX = world * matScale;
                worldTranslation = Matrix::CreateTranslation(spheres[i].Position);
                worldX = worldX * worldTranslation;

                currentPrimitive->Draw(
                    worldX, view, projection, spheres[i].Color, false);

                worldX = worldX * shadowMatrix;
                worldX.M42 = -1.0f;
                currentPrimitive->Draw(
                    worldX, view, projection, Color::Black, true);
            }

            Game::Draw(gameTime);
        }

    private:
        void SphereCollisionImplicit(Sphere& sphere1, Sphere& sphere2)
        {
            constexpr float K_ELASTIC = 0.75f;

            const Vector3 relativepos = sphere2.Position - sphere1.Position;
            const float distance2 = relativepos.LengthSquared();
            const float radii = sphere1.Radius + sphere2.Radius;
            if (distance2 >= radii * radii)
                return;

            const float distance = relativepos.Length();
            const Vector3 relativeUnit = relativepos * (1.0f / distance);
            const Vector3 penetration = relativeUnit * (radii - distance);

            const float mass1 = sphere1.Mass;
            const float mass2 = sphere2.Mass;
            const float m_inv = 1.0f / (mass1 + mass2);
            const float weight1 = mass1 * m_inv;
            const float weight2 = mass2 * m_inv;

            sphere1.Position = sphere1.Position - weight2 * penetration;
            sphere2.Position = sphere2.Position + weight1 * penetration;

            const Vector3 velocity1 = sphere1.Velocity;
            const Vector3 velocity2 = sphere2.Velocity;
            const Vector3 velocityTotal = velocity1 * weight1 + velocity2 * weight2;
            Vector3 i2 = (velocity2 - velocityTotal) * mass2;

            if (Vector3::Dot(i2, relativeUnit) < 0.0f)
            {
                const Vector3 di = Vector3::Dot(i2, relativeUnit) * relativeUnit;
                i2 = i2 - di * (K_ELASTIC + 1.0f);
                sphere1.Velocity = (-i2) / mass1 + velocityTotal;
                sphere2.Velocity = i2 / mass2 + velocityTotal;
            }
        }

    protected:
        void UpdateSpheres(GameTime& gameTime)
        {
            Vector3 gravity = Vector3::UnitY * -4.0f;
            float shakeForce = 1.0f;

            constexpr float limit = 0.85f;
            constexpr float tiltoffset = 0.76f;

            const float elapsedGameTime = static_cast<float>(
                gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());

            Vector3 currentAccelerometerReading =
                Accelerometer::GetState().getAccelerationProperty();

            if (getWindowProperty().getCurrentOrientationProperty() ==
                DisplayOrientation::LandscapeLeft)
            {
                currentAccelerometerReading.Y = -currentAccelerometerReading.Y;
            }

            const float newMag = currentAccelerometerReading.Length();
            if (accelhistory[1] > 1.3f &&
                accelhistory[0] < accelhistory[1] &&
                accelhistory[1] > newMag)
            {
                shakeForce += 10.0f * (accelhistory[1] - 1.3f) / 3.5f;
            }

            accelhistory[0] = accelhistory[1];
            accelhistory[1] = newMag;

            const double accelReadingZ = currentAccelerometerReading.Z;
            const double accelReadingY = currentAccelerometerReading.Y;

            const float rotateX = std::max(
                std::min(static_cast<float>(-(accelReadingZ + tiltoffset)), limit), -limit);
            const float rotateY = std::max(
                std::min(static_cast<float>(accelReadingY), limit), -limit);

            const Matrix rotationToBeDone =
                Matrix::CreateRotationX(rotateX * MathHelper::PiOver2);
            const Matrix rotationToBeDone2 =
                Matrix::CreateRotationZ(rotateY * MathHelper::PiOver2);
            gravity = Vector3::Transform(gravity, rotationToBeDone);
            gravity = Vector3::Transform(gravity, rotationToBeDone2);

            for (int i = 0; i < numSpheres; ++i)
            {
                Sphere& mySphere = spheres[i];
                mySphere.Position = mySphere.Position +
                    mySphere.Velocity * elapsedGameTime * 0.99f;
                mySphere.Velocity = mySphere.Velocity + gravity * elapsedGameTime;
            }

            for (int i = 0; i < numSpheres; ++i)
            {
                for (int j = i + 1; j < numSpheres; ++j)
                    SphereCollisionImplicit(spheres[i], spheres[j]);
            }

            for (int i = 0; i < numSpheres; ++i)
            {
                Sphere& mySphere = spheres[i];
                if (mySphere.Position.Y < floorPlaneHeight + mySphere.Radius)
                {
                    mySphere.Velocity = mySphere.Velocity - gravity * elapsedGameTime;

                    if (shakeForce > 1.0f)
                    {
                        mySphere.Velocity.Y += 0.1f;
                        const float speed = mySphere.Velocity.Length();
                        float speedadjust = speed;
                        speedadjust = std::min(speed, 4.0f);
                        speedadjust = std::max(speed, 2.0f);
                        mySphere.Velocity = mySphere.Velocity * (1.0f / speed);
                        mySphere.Velocity = mySphere.Velocity * speedadjust * shakeForce;
                    }

                    mySphere.Position.Y = floorPlaneHeight + mySphere.Radius;
                    if (mySphere.Velocity.Y < 0.0f)
                    {
                        if (mySphere.Velocity.Y > gravity.Y * elapsedGameTime * 2.0f &&
                            mySphere.Velocity.LengthSquared() < 0.5f * 0.5f)
                        {
                            mySphere.Velocity.Y = 0.0f;
                        }
                        else
                        {
                            mySphere.Velocity.Y =
                                -mySphere.Velocity.Y * collisionDamping;
                        }
                    }
                }

                if (mySphere.Position.X < -worldSize + mySphere.Radius)
                {
                    mySphere.Position.X = -worldSize + mySphere.Radius;
                    if (mySphere.Velocity.X < 0.0f)
                        mySphere.Velocity.X = -mySphere.Velocity.X * collisionDamping;
                }

                if (mySphere.Position.X > worldSize - mySphere.Radius)
                {
                    mySphere.Position.X = worldSize - mySphere.Radius;
                    if (mySphere.Velocity.X > 0.0f)
                        mySphere.Velocity.X = -mySphere.Velocity.X * collisionDamping;
                }

                if (mySphere.Position.Z < -worldSize + mySphere.Radius)
                {
                    mySphere.Position.Z = -worldSize + mySphere.Radius;
                    if (mySphere.Velocity.Z < 0.0f)
                        mySphere.Velocity.Z = -mySphere.Velocity.Z * collisionDamping;
                }

                if (mySphere.Position.Z > worldSize - mySphere.Radius)
                {
                    mySphere.Position.Z = worldSize - mySphere.Radius;
                    if (mySphere.Velocity.Z > 0.0f)
                        mySphere.Velocity.Z = -mySphere.Velocity.Z * collisionDamping;
                }
            }
        }

    private:
        void HandleInput()
        {
            lastKeyboardState = currentKeyboardState;
            lastGamePadState = currentGamePadState;
            currentKeyboardState = Keyboard::GetState();
            currentGamePadState = GamePad::GetState(PlayerIndex::One);

            if (IsPressed(Keys::Escape, Buttons::Back))
                Exit();
        }

        [[nodiscard]] bool IsPressed(Keys key, Buttons button) const
        {
            return (currentKeyboardState.IsKeyDown(key) &&
                    lastKeyboardState.IsKeyUp(key)) ||
                   (currentGamePadState.IsButtonDown(button) &&
                    lastGamePadState.IsButtonUp(button));
        }
    };
}
