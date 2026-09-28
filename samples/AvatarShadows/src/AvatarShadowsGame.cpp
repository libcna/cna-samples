// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AvatarShadowGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "AvatarShadowsGame.hpp"

#include <array>
#include <cmath>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/GameComponentCollection.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesComponent.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPassCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PresentationParameters.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerStateCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionTexture.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Plane.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/Random.hpp"
#include "System/Type.hpp"

namespace AvatarShadows
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::GamerServices;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    AvatarShadowsGame::AvatarShadowsGame()
        : lightRotation(MathHelper::PiOver4)
    {
        graphics = std::make_unique<GraphicsDeviceManager>(this);
        graphics->setPreferredBackBufferWidthProperty(1280);
        graphics->setPreferredBackBufferHeightProperty(720);
        graphics->setPreferMultiSamplingProperty(true);
        getContentProperty().setRootDirectoryProperty("Content");

        // Add the GamerServicesComponent so we can use avatars
        gamerServices = std::make_unique<GamerServicesComponent>(*this);
        getComponentsProperty().Add(gamerServices.get());
    }

    AvatarShadowsGame::~AvatarShadowsGame() = default;

    const std::string& AvatarShadowsGame::GetTypeName() const
    {
        static const std::string name = "AvatarShadows.AvatarShadowsGame";
        return name;
    }

    void AvatarShadowsGame::LoadContent()
    {
        // Create a new SpriteBatch, which can be used to draw textures.
        spriteBatch.emplace(getGraphicsDeviceProperty());
        font.emplace(getContentProperty().Load<SpriteFont>("Font"));

        // Create our ground vertex buffer
        groundVertices = std::make_unique<VertexBuffer>(
            getGraphicsDeviceProperty(), System::Type::From<VertexPositionTexture>(), 4, BufferUsage::WriteOnly);
        const std::array<VertexPositionTexture, 4> ground{
            VertexPositionTexture(Vector3(-15.0f, 0.0f, -15.0f), Vector2(0.0f, 0.0f)),
            VertexPositionTexture(Vector3(15.0f, 0.0f, -15.0f), Vector2(8.0f, 0.0f)),
            VertexPositionTexture(Vector3(-15.0f, 0.0f, 15.0f), Vector2(0.0f, 8.0f)),
            VertexPositionTexture(Vector3(15.0f, 0.0f, 15.0f), Vector2(8.0f, 8.0f)),
        };
        groundVertices->SetData(ground.data(), static_cast<int>(ground.size()));

        // Load our ground effect and texture
        groundEffect = std::make_unique<GroundEffect>(getContentProperty().Load<std::shared_ptr<Effect>>("GroundEffect"));
        groundTexture.emplace(getContentProperty().Load<Texture2D>("ground"));
        groundEffect->setTextureProperty(&*groundTexture);

        // Create our avatars in a box centered at the origin
        const int numRows = 4;
        const float spacing = 1.35f;
        const float origin = -(numRows - 1) / 2.0f * spacing;
        System::Random random;
        for (int x = 0; x < numRows; x++)
        {
            for (int z = 0; z < numRows; z++)
            {
                // Create a random rotation value for the avatar
                const float rotation = static_cast<float>(random.NextDouble()) * MathHelper::TwoPi;

                // Position the avatar based on our loops
                const Vector3 position(origin + x * spacing, 0.0f, origin + z * spacing);

                // Create the avatar, set its world matrix, and add it to our list
                auto avatar = std::make_unique<Avatar>();
                avatar->setWorldProperty(Matrix::CreateRotationY(rotation) * Matrix::CreateTranslation(position));
                avatars.push_back(std::move(avatar));
            }
        }

        // Create our shadow render target. We use the Alpha8 format because we only care about the alpha
        // channel of the avatar rendering. Our ground effect simply checks the alpha being greater than
        // zero to indicate an area that is shadowed. By using Alpha8, we cut our memory usage by 25% over
        // using SurfaceFormat.Color.
        const int width = getGraphicsDeviceProperty().getPresentationParametersProperty().getBackBufferWidthProperty();
        const int height = getGraphicsDeviceProperty().getPresentationParametersProperty().getBackBufferHeightProperty();
        shadowTarget = std::make_unique<RenderTarget2D>(
            getGraphicsDeviceProperty(), width, height, false, SurfaceFormat::Alpha8, DepthFormat::None);
    }

    void AvatarShadowsGame::Update(GameTime& gameTime)
    {
        // Update input
        gamePadPrev = gamePad;
        gamePad = GamePad::GetState(PlayerIndex::One);

        // Allows the game to exit
        if (gamePad.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }

        // Rotate the camera with the left thumbstick X axis
        cameraRotation += gamePad.getThumbSticksProperty().getLeftProperty().X * 0.03f;

        // Rotate the light with the right thumbstick X axis
        lightRotation += gamePad.getThumbSticksProperty().getRightProperty().X * 0.03f;

        // Update the animations
        for (const auto& avatar : avatars)
        {
            avatar->getAnimationProperty().Update(gameTime.getElapsedGameTimeProperty(), true);
        }

        Game::Update(gameTime);
    }

    void AvatarShadowsGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = getGraphicsDeviceProperty();

        // Reset some states that may have changed when drawing shadows and our instruction text
        device.setBlendStateProperty(BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.getSamplerStatesProperty()[0] = SamplerState::LinearWrap;

        // Calculate the view matrix from our camera rotation
        const Vector3 cameraPos =
            Vector3(std::sin(cameraRotation), 0.5f, std::cos(cameraRotation)) * 8.0f;
        const Matrix view = Matrix::CreateLookAt(cameraPos, Vector3(0.0f, 1.0f, 0.0f), Vector3::Up);

        // Create our projection matrix
        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, device.getViewportProperty().getAspectRatioProperty(), 1.0f, 1000.0f);

        // Calculate the light direction from our light rotation
        Vector3 lightDirection(std::cos(lightRotation), -1.0f, std::sin(lightRotation));
        lightDirection.Normalize();

        // Draw our shadows to our render target first
        DrawAvatarShadows(view, projection, lightDirection);

        // Clear the screen
        device.Clear(Color::CornflowerBlue);

        // Draw the ground
        DrawGround(view, projection);

        // Draw all of our avatars in our scene
        for (const auto& avatar : avatars)
        {
            AvatarRenderer& renderer = avatar->getRendererProperty();
            renderer.setLightDirectionProperty(lightDirection);
            renderer.setWorldProperty(avatar->getWorldProperty());
            renderer.setViewProperty(view);
            renderer.setProjectionProperty(projection);
            renderer.Draw(&avatar->getAnimationProperty());
        }

        // Draw instructions over the scene
        DrawInstructions();

        Game::Draw(gameTime);
    }

    void AvatarShadowsGame::DrawAvatarShadows(const Matrix& view, const Matrix& projection, const Vector3& lightDirection)
    {
        GraphicsDevice& device = getGraphicsDeviceProperty();

        // First we draw any avatar shadows to our render target
        device.SetRenderTarget(shadowTarget.get());
        device.Clear(Color::Transparent);

        // We generate a shadow matrix with Matrix.CreateShadow. This matrix is used to flatten an object
        // down to a plane based on a light direction. The light direction required for CreateShadow is
        // the direction TO the light, not from, so we must reverse our vector. We also create our plane
        // slightly offset from the origin to keep our shadow from having Z-fighting issues with the ground.
        const Matrix shadowMatrix = Matrix::CreateShadow(-lightDirection, Plane(Vector3::Up, -0.001f));

        // Draw all of our avatars to the shadow render target
        for (const auto& avatar : avatars)
        {
            AvatarRenderer& renderer = avatar->getRendererProperty();
            renderer.setWorldProperty(avatar->getWorldProperty() * shadowMatrix);
            renderer.setViewProperty(view);
            renderer.setProjectionProperty(projection);
            renderer.Draw(&avatar->getAnimationProperty());
        }

        // Unset our render target to draw to our back buffer
        device.SetRenderTarget(nullptr);
    }

    void AvatarShadowsGame::DrawGround(const Matrix& view, const Matrix& projection)
    {
        GraphicsDevice& device = getGraphicsDeviceProperty();

        // Set our matrices
        groundEffect->setWorldProperty(Matrix::getIdentityProperty());
        groundEffect->setViewProperty(view);
        groundEffect->setProjectionProperty(projection);

        // Assign our shadow render target to the effect
        groundEffect->setShadowProperty(shadowTarget.get());

        // Set our vertex buffer
        device.SetVertexBuffer(groundVertices.get());

        // Apply the effect and draw the primitives
        groundEffect->getBaseEffectProperty().getCurrentTechniqueProperty()->getPassesProperty()[0]->Apply();
        device.DrawPrimitives(PrimitiveType::TriangleStrip, 0, 2);
    }

    void AvatarShadowsGame::DrawInstructions()
    {
        // Create our position from the TitleSafeArea
        const Rectangle safeArea = getGraphicsDeviceProperty().getViewportProperty().getTitleSafeAreaProperty();
        const Vector2 position(static_cast<float>(safeArea.X), static_cast<float>(safeArea.Y));

        // Draw our instructions with SpriteBatch
        const std::string instructions = "Left thumbstick  - Rotate camera\nRight thumbstick - Rotate light";
        spriteBatch->Begin();
        spriteBatch->DrawString(*font, instructions, position, Color::White);
        spriteBatch->End();
    }
}
