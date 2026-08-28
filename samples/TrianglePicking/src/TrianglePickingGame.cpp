// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "TrianglePickingGame.hpp"

#include <limits>

#include "CNA/Content/ObjectDictionaryEXT.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/CullMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/FillMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/InvalidCastException.hpp"
#include "System/InvalidOperationException.hpp"
#include "System/String.hpp"

namespace TrianglePicking
{
    using Microsoft::Xna::Framework::BoundingSphere;
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Graphics::BlendState;
    using Microsoft::Xna::Framework::Graphics::CullMode;
    using Microsoft::Xna::Framework::Graphics::DepthStencilState;
    using Microsoft::Xna::Framework::Graphics::Effect;
    using Microsoft::Xna::Framework::Graphics::FillMode;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::ModelMesh;
    using Microsoft::Xna::Framework::Graphics::PrimitiveType;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Keyboard;
    using Microsoft::Xna::Framework::Input::Keys;

    const std::array<std::string, 4> TrianglePickingGame::ModelFilenames = {
        "Sphere",
        "Cats",
        "P2Wedge",
        "Cylinder",
    };

    namespace
    {
        RasterizerState MakeWireFrame()
        {
            RasterizerState state;
            state.setFillModeProperty(FillMode::WireFrame);
            state.setCullModeProperty(CullMode::None);
            return state;
        }
    }

    const RasterizerState TrianglePickingGame::WireFrame = MakeWireFrame();

    TrianglePickingGame::TrianglePickingGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        for (std::size_t i = 0; i < modelWorldTransforms.size(); i++)
        {
            const float x = static_cast<float>(i)
                          - static_cast<float>(modelWorldTransforms.size() / 2);
            modelWorldTransforms[i] = Matrix::CreateTranslation(Vector3(x, 0, 0));
        }

        pickedTriangle = {
            VertexPositionColor(Vector3::Zero, Color::Magenta),
            VertexPositionColor(Vector3::Zero, Color::Magenta),
            VertexPositionColor(Vector3::Zero, Color::Magenta),
        };

        cursor = std::make_unique<Cursor>(*this, getContentProperty());
        getComponentsProperty().Add(cursor.get());
    }

    const std::string& TrianglePickingGame::GetTypeName() const
    {
        static const std::string name = "TrianglePicking.TrianglePickingGame";
        return name;
    }

    void TrianglePickingGame::Initialize()
    {
        projectionMatrix = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::ToRadians(45.0f),
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(),
            0.01f, 1000);

        Game::Initialize();
    }

    void TrianglePickingGame::LoadContent()
    {
        for (std::size_t i = 0; i < ModelFilenames.size(); i++)
        {
            models[i] = getContentProperty().Load<Model>(ModelFilenames[i]);
            modelAbsoluteBoneTransforms[i].resize(
                static_cast<std::size_t>(models[i]->getBonesProperty().getCountProperty()));
            models[i]->CopyAbsoluteBoneTransformsTo(modelAbsoluteBoneTransforms[i]);
        }

        table = getContentProperty().Load<Model>("Table");
        tableAbsoluteBoneTransforms.resize(
            static_cast<std::size_t>(table->getBonesProperty().getCountProperty()));
        table->CopyAbsoluteBoneTransformsTo(tableAbsoluteBoneTransforms);

        spriteBatch.emplace(*graphics.getGraphicsDeviceProperty());
        spriteFont = getContentProperty().Load<SpriteFont>("hudFont");

        lineEffect.emplace(*graphics.getGraphicsDeviceProperty());
        lineEffect->setVertexColorEnabledProperty(true);
    }

    void TrianglePickingGame::Update(GameTime& gameTime)
    {
        HandleInput();
        UpdateCamera(gameTime);
        UpdatePicking();

        Game::Update(gameTime);
    }

    void TrianglePickingGame::UpdatePicking()
    {
        // create a ray going in to the scene from the cursor's position
        Ray cursorRay = cursor->CalculateCursorRay(projectionMatrix, viewMatrix);

        // keep track of the closest object we have seen so far, so we can
        // choose the closest one of all the objects that intersect the ray.
        insideBoundingSpheres.clear();
        pickedModelName.reset();

        float closestIntersection = std::numeric_limits<float>::max();

        for (std::size_t i = 0; i < models.size(); i++)
        {
            bool insideBoundingSphere = false;
            Vector3 vertex1, vertex2, vertex3;

            const std::optional<float> intersection =
                RayIntersectsModel(cursorRay, *models[i], modelWorldTransforms[i],
                                   insideBoundingSphere, vertex1, vertex2, vertex3);

            if (insideBoundingSphere)
            {
                insideBoundingSpheres.push_back(ModelFilenames[i]);
            }

            if (intersection.has_value())
            {
                if (*intersection < closestIntersection)
                {
                    closestIntersection = *intersection;

                    pickedModelName = ModelFilenames[i];

                    pickedTriangle[0].Position = vertex1;
                    pickedTriangle[1].Position = vertex2;
                    pickedTriangle[2].Position = vertex3;
                }
            }
        }
    }

    std::optional<float> TrianglePickingGame::RayIntersectsModel(
        Ray ray, Model& model, const Matrix& modelTransform,
        bool& insideBoundingSphere, Vector3& vertex1, Vector3& vertex2, Vector3& vertex3)
    {
        vertex1 = vertex2 = vertex3 = Vector3::Zero;

        // The input ray is in world space, but our model data is stored in object
        // space. We would normally have to transform all the model data by the
        // modelTransform matrix, moving it into world space before we test it
        // against the ray. That transform can be slow if there are a lot of
        // triangles in the model, however, so instead we do the opposite.
        // Transforming our ray by the inverse modelTransform moves it into object
        // space, where we can test it directly against our model data.
        const Matrix inverseTransform = Matrix::Invert(modelTransform);

        ray.Position = Vector3::Transform(ray.Position, inverseTransform);
        ray.Direction = Vector3::TransformNormal(ray.Direction, inverseTransform);

        // The custom TrianglePickingProcessor attached a Dictionary<string, object> to Model.Tag.
        // C++ has no `object`, so the C# cast becomes a dynamic_cast to CNA's carrier for that
        // dictionary; each entry keeps the type its own content type reader produced.
        auto* tagData = dynamic_cast<CNA::Content::ObjectDictionaryEXT*>(model.getTagProperty());
        if (tagData == nullptr)
        {
            throw System::InvalidOperationException(
                "Model.Tag is not set correctly. Make sure your model "
                "was built using the custom TrianglePickingProcessor.");
        }

        const BoundingSphere& boundingSphere = tagData->Get<BoundingSphere>("BoundingSphere");

        if (!boundingSphere.Intersects(ray).has_value())
        {
            insideBoundingSphere = false;
            return std::nullopt;
        }

        // The bounding sphere test passed, so we need to do a full
        // triangle picking test.
        insideBoundingSphere = true;

        // Keep track of the closest triangle we found so far,
        // so we can always return the closest one.
        std::optional<float> closestIntersection;

        // Look up our custom collision data from the Tag property of the model.
        const std::vector<Vector3>& vertices = tagData->Get<std::vector<Vector3>>("Vertices");

        // Loop over the vertex data, 3 at a time (3 vertices = 1 triangle).
        for (std::size_t i = 0; i + 2 < vertices.size(); i += 3)
        {
            // Perform a ray to triangle intersection test.
            std::optional<float> intersection;
            RayIntersectsTriangle(ray, vertices[i], vertices[i + 1], vertices[i + 2], intersection);

            // Does the ray intersect this triangle?
            if (intersection.has_value())
            {
                // If so, is it closer than any other previous triangle?
                if (!closestIntersection.has_value() || (*intersection < *closestIntersection))
                {
                    // Store the distance to this triangle.
                    closestIntersection = intersection;

                    // Transform the three vertex positions into world space
                    vertex1 = Vector3::Transform(vertices[i], modelTransform);
                    vertex2 = Vector3::Transform(vertices[i + 1], modelTransform);
                    vertex3 = Vector3::Transform(vertices[i + 2], modelTransform);
                }
            }
        }

        return closestIntersection;
    }

    void TrianglePickingGame::RayIntersectsTriangle(const Ray& ray, const Vector3& vertex1,
                                                    const Vector3& vertex2, const Vector3& vertex3,
                                                    std::optional<float>& result)
    {
        // Compute vectors along two edges of the triangle.
        const Vector3 edge1 = Vector3::Subtract(vertex2, vertex1);
        const Vector3 edge2 = Vector3::Subtract(vertex3, vertex1);

        // Compute the determinant.
        const Vector3 directionCrossEdge2 = Vector3::Cross(ray.Direction, edge2);
        const float determinant = Vector3::Dot(edge1, directionCrossEdge2);

        // If the ray is parallel to the triangle plane, there is no collision.
        if (determinant > -std::numeric_limits<float>::epsilon()
            && determinant < std::numeric_limits<float>::epsilon())
        {
            result.reset();
            return;
        }

        const float inverseDeterminant = 1.0f / determinant;

        // Calculate the U parameter of the intersection point.
        const Vector3 distanceVector = Vector3::Subtract(ray.Position, vertex1);
        float triangleU = Vector3::Dot(distanceVector, directionCrossEdge2);
        triangleU *= inverseDeterminant;

        // Make sure it is inside the triangle.
        if (triangleU < 0 || triangleU > 1)
        {
            result.reset();
            return;
        }

        // Calculate the V parameter of the intersection point.
        const Vector3 distanceCrossEdge1 = Vector3::Cross(distanceVector, edge1);
        float triangleV = Vector3::Dot(ray.Direction, distanceCrossEdge1);
        triangleV *= inverseDeterminant;

        // Make sure it is inside the triangle.
        if (triangleV < 0 || triangleU + triangleV > 1)
        {
            result.reset();
            return;
        }

        // Compute the distance along the ray to the triangle.
        float rayDistance = Vector3::Dot(edge2, distanceCrossEdge1);
        rayDistance *= inverseDeterminant;

        // Is the triangle behind the ray origin?
        if (rayDistance < 0)
        {
            result.reset();
            return;
        }

        result = rayDistance;
    }

    void TrianglePickingGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        device.Clear(Color::CornflowerBlue);

        device.setBlendStateProperty(BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::Default);

        DrawModel(*table, Matrix::getIdentityProperty(), tableAbsoluteBoneTransforms);

        for (std::size_t i = 0; i < models.size(); i++)
        {
            DrawModel(*models[i], modelWorldTransforms[i], modelAbsoluteBoneTransforms[i]);
        }

        DrawPickedTriangle();
        DrawText();

        Game::Draw(gameTime);
    }

    void TrianglePickingGame::DrawPickedTriangle()
    {
        if (pickedModelName.has_value())
        {
            GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

            // Draw the triangle in wireframe, and turn off the depth buffer so it
            // shows up even when it is inside the model.
            device.setRasterizerStateProperty(WireFrame);
            device.setDepthStencilStateProperty(DepthStencilState::None);

            lineEffect->setProjectionProperty(projectionMatrix);
            lineEffect->setViewProperty(viewMatrix);
            lineEffect->getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();

            device.DrawUserPrimitives(PrimitiveType::TriangleList, pickedTriangle.data(), 0, 1,
                                      VertexPositionColor::getVertexDeclarationStatic());

            device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
            device.setDepthStencilStateProperty(DepthStencilState::Default);
        }
    }

    void TrianglePickingGame::DrawText()
    {
        const Vector2 shadowOffset(1, 1);

        spriteBatch->Begin();

        if (!insideBoundingSpheres.empty())
        {
            const std::string text = "Inside bounding sphere: "
                                   + System::String::Join(", ", insideBoundingSpheres);
            const Vector2 position(50, 50);
            spriteBatch->DrawString(*spriteFont, text, position + shadowOffset, Color::Black);
            spriteBatch->DrawString(*spriteFont, text, position, Color::White);
        }

        if (pickedModelName.has_value())
        {
            Vector2 position = cursor->getPositionProperty();
            position.Y += 32;
            position -= spriteFont->MeasureString(*pickedModelName) / 2;
            spriteBatch->DrawString(*spriteFont, *pickedModelName,
                                    position + shadowOffset, Color::Black);
            spriteBatch->DrawString(*spriteFont, *pickedModelName, position, Color::White);
        }

        spriteBatch->End();
    }

    void TrianglePickingGame::DrawModel(Model& model, const Matrix& worldTransform,
                                        const std::vector<Matrix>& absoluteBoneTransforms)
    {
        for (ModelMesh* mesh : model.getMeshesProperty())
        {
            for (Effect* meshEffect : mesh->getEffectsProperty())
            {
                // The original's foreach is typed `BasicEffect`, and a C# cast-per-element loop
                // throws on a mismatch, so this does too.
                auto* effect = dynamic_cast<BasicEffect*>(meshEffect);
                if (effect == nullptr)
                {
                    throw System::InvalidCastException(
                        "TrianglePicking: a mesh effect is not a BasicEffect.");
                }

                effect->EnableDefaultLighting();
                effect->setPreferPerPixelLightingProperty(true);

                effect->setViewProperty(viewMatrix);
                effect->setProjectionProperty(projectionMatrix);
                effect->setWorldProperty(
                    absoluteBoneTransforms[static_cast<std::size_t>(
                        mesh->getParentBoneProperty()->getIndexProperty())]
                    * worldTransform);
            }
            mesh->Draw();
        }
    }

    void TrianglePickingGame::HandleInput()
    {
        currentKeyboardState = Keyboard::GetState();
        currentGamePadState = GamePad::GetState(PlayerIndex::One);

        // Check for exit.
        if (currentKeyboardState.IsKeyDown(Keys::Escape)
            || currentGamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }
    }

    void TrianglePickingGame::UpdateCamera(const GameTime& gameTime)
    {
        const float time = static_cast<float>(
            gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty());

        // Check for input to reset the camera.
        if (currentKeyboardState.IsKeyDown(Keys::R)
            || currentGamePadState.getButtonsProperty().getRightStickProperty()
                   == ButtonState::Pressed)
        {
            cameraArc = CameraDefaultArc;
            cameraDistance = CameraDefaultDistance;
            cameraRotation = CameraDefaultRotation;
        }

        // Check for input to rotate the camera up and down around the model.
        if (currentKeyboardState.IsKeyDown(Keys::Up) || currentKeyboardState.IsKeyDown(Keys::W))
        {
            cameraArc += time * CameraRotateSpeed;
        }

        if (currentKeyboardState.IsKeyDown(Keys::Down) || currentKeyboardState.IsKeyDown(Keys::S))
        {
            cameraArc -= time * CameraRotateSpeed;
        }

        cameraArc += currentGamePadState.getThumbSticksProperty().getRightProperty().Y * time
                   * CameraRotateSpeed;

        // Limit the arc movement.
        cameraArc = MathHelper::Clamp(cameraArc, -90.0f, 90.0f);

        // Check for input to rotate the camera around the model.
        if (currentKeyboardState.IsKeyDown(Keys::Right) || currentKeyboardState.IsKeyDown(Keys::D))
        {
            cameraRotation += time * CameraRotateSpeed;
        }

        if (currentKeyboardState.IsKeyDown(Keys::Left) || currentKeyboardState.IsKeyDown(Keys::A))
        {
            cameraRotation -= time * CameraRotateSpeed;
        }

        cameraRotation += currentGamePadState.getThumbSticksProperty().getRightProperty().X * time
                        * CameraRotateSpeed;

        // Check for input to zoom camera in and out.
        if (currentKeyboardState.IsKeyDown(Keys::Z)) cameraDistance += time * CameraZoomSpeed;
        if (currentKeyboardState.IsKeyDown(Keys::X)) cameraDistance -= time * CameraZoomSpeed;

        cameraDistance += currentGamePadState.getTriggersProperty().getLeftProperty() * time
                        * CameraZoomSpeed;
        cameraDistance -= currentGamePadState.getTriggersProperty().getRightProperty() * time
                        * CameraZoomSpeed;

        // Limit the camera distance.
        cameraDistance = MathHelper::Clamp(cameraDistance, CameraMinDistance, CameraMaxDistance);

        const Matrix unrotatedView = Matrix::CreateLookAt(
            Vector3(0, 0, -cameraDistance), Vector3::Zero, Vector3::Up);

        viewMatrix = Matrix::CreateRotationY(MathHelper::ToRadians(cameraRotation))
                   * Matrix::CreateRotationX(MathHelper::ToRadians(cameraArc))
                   * unrotatedView;
    }
}

/**
 * @brief The main entry point for the application.
 *
 * The original declares this as a `static class Program` at the bottom of Game.cs, with no
 * platform guard around it.
 *
 * @return The process exit code.
 */
int main()
{
    TrianglePicking::TrianglePickingGame game;
    game.Run();
    return 0;
}
