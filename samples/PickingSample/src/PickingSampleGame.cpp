// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "PickingSampleGame.hpp"

#include <algorithm>

#include "BoundingSphereRenderer.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/InvalidCastException.hpp"

namespace PickingSample
{
    using BoundingVolumeRendering::BoundingSphereRenderer;
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::BlendState;
    using Microsoft::Xna::Framework::Graphics::DepthStencilState;
    using Microsoft::Xna::Framework::Graphics::Effect;
    using Microsoft::Xna::Framework::Graphics::ModelMesh;
    using Microsoft::Xna::Framework::Graphics::SpriteEffects;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Keyboard;
    using Microsoft::Xna::Framework::Input::Keys;

    const std::array<std::string, 4> PickingSampleGame::ModelFilenames = {
        "Sphere",
        "Cats",
        "P2Wedge",
        "Cylinder",
    };

    PickingSampleGame::PickingSampleGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        // The original's WINDOWS_PHONE block -- a 30 fps TargetElapsedTime and IsFullScreen --
        // is compiled out of the Windows build this port is, so it is not translated here.
    }

    const std::string& PickingSampleGame::GetTypeName() const
    {
        static const std::string name = "PickingSample.PickingSampleGame";
        return name;
    }

    void PickingSampleGame::Initialize()
    {
        // Set up the world transforms that each model will use. They'll be
        // positioned in a line along the x axis.
        modelWorldTransforms[0] = Matrix::CreateTranslation(Vector3(-1.5f, 0, 0));
        modelWorldTransforms[1] = Matrix::CreateTranslation(Vector3(-0.5f, 0, 0));
        modelWorldTransforms[2] = Matrix::CreateTranslation(Vector3(0.5f, 0, 0));
        modelWorldTransforms[3] = Matrix::CreateTranslation(Vector3(1.5f, 0, 0));

        cursor = std::make_unique<Cursor>(*this);
        getComponentsProperty().Add(cursor.get());

        Game::Initialize();
    }

    void PickingSampleGame::LoadContent()
    {
        // load all of the models that will appear on the table:
        for (std::size_t i = 0; i < ModelFilenames.size(); i++)
        {
            // load the actual model, using ModelFilenames to determine what
            // file to load.
            models[i] = getContentProperty().Load<Model>(ModelFilenames[i]);

            // create an array of matrices to hold the absolute bone transforms,
            // calculate them, and copy them in.
            modelAbsoluteBoneTransforms[i].resize(
                static_cast<std::size_t>(models[i]->getBonesProperty().getCountProperty()));
            models[i]->CopyAbsoluteBoneTransformsTo(modelAbsoluteBoneTransforms[i]);
        }

        // now that we've loaded in the models that will sit on the table, go
        // through the same procedure for the table itself.
        table = getContentProperty().Load<Model>("Table");
        tableAbsoluteBoneTransforms.resize(
            static_cast<std::size_t>(table->getBonesProperty().getCountProperty()));
        table->CopyAbsoluteBoneTransformsTo(tableAbsoluteBoneTransforms);

        // create a spritebatch and load the font, which we'll use to draw the
        // models' names.
        spriteBatch.emplace(*graphics.getGraphicsDeviceProperty());
        spriteFont = getContentProperty().Load<SpriteFont>("hudFont");

        // calculate the projection matrix now that the graphics device is created.
        projectionMatrix = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(),
            0.01f, 1000);

        // Initialize the renderer for our bounding spheres
        BoundingSphereRenderer::Initialize(getGraphicsDeviceProperty(), 45);
    }

    void PickingSampleGame::Update(GameTime& gameTime)
    {
        // Check for exit.
        if (Keyboard::GetState().IsKeyDown(Keys::Escape)
            || GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBackProperty()
                   == ButtonState::Pressed)
        {
            Exit();
        }

        // we rotate our view around the models over time
        float time = static_cast<float>(
            gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty());
        cameraRotation += time * CameraRotateSpeed;
        Matrix unrotatedView = Matrix::CreateLookAt(
            Vector3(0, 0, -CameraDefaultDistance), Vector3::Zero, Vector3::Up);
        viewMatrix = Matrix::CreateRotationY(MathHelper::ToRadians(cameraRotation))
                   * Matrix::CreateRotationX(MathHelper::ToRadians(CameraDefaultArc))
                   * unrotatedView;

        // base.Update will update all of the components in the .Components
        // collection, including the cursor.
        Game::Update(gameTime);
    }

    void PickingSampleGame::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

        // drawing sprites changes some render states around, which don't play
        // nicely with 3d models. In particular, we want to enable the depth buffer and turn off
        // alpha blending.
        getGraphicsDeviceProperty().setDepthStencilStateProperty(DepthStencilState::Default);
        getGraphicsDeviceProperty().setBlendStateProperty(BlendState::Opaque);

        // draw the table. DrawModel is a function defined below draws a model using
        // a world matrix and the model's bone transforms.
        DrawModel(*table, Matrix::getIdentityProperty(), tableAbsoluteBoneTransforms, false);

        // use the same DrawModel function to draw all of the models on the table.
        for (std::size_t i = 0; i < models.size(); i++)
        {
            DrawModel(*models[i], modelWorldTransforms[i], modelAbsoluteBoneTransforms[i],
                      drawBoundingSphere);
        }

        // now we'll check to see if the cursor is over any of the models, and draw
        // their names if it is.
        DrawModelNames();

        Game::Draw(gameTime);
    }

    void PickingSampleGame::DrawModelNames()
    {
        // begin on the spritebatch, because we're going to be drawing some text.
        spriteBatch->Begin();

        // If the cursor is over a model, we'll draw its name. To figure out if
        // the cursor is over a model, we'll use cursor.CalculateCursorRay. That
        // function gives us a world space ray that starts at the "eye" of the
        // camera, and shoots out in the direction pointed to by the cursor.
        Ray cursorRay = cursor->CalculateCursorRay(projectionMatrix, viewMatrix);

        // go through all of the models...
        for (std::size_t i = 0; i < models.size(); i++)
        {
            // check to see if the cursorRay intersects the model....
            if (RayIntersectsModel(cursorRay, *models[i], modelWorldTransforms[i],
                                   modelAbsoluteBoneTransforms[i]))
            {
                // now we know that we want to draw the model's name. We want to
                // draw the name a little bit above the model: but where's that?
                // SpriteBatch.DrawString takes screen space coordinates, but the
                // model's position is stored in world space.

                // we'll use Viewport.Project, which will project a world space
                // point into screen space. We'll project the vector (0,0,0) using
                // the model's world matrix, and the view and projection matrices.
                // that will tell us where the model's origin is on the screen.
                Vector3 screenSpace = graphics.getGraphicsDeviceProperty()->getViewportProperty().Project(
                    Vector3::Zero, projectionMatrix, viewMatrix, modelWorldTransforms[i]);

                // we want to draw the text a little bit above that, so we'll use
                // the screen space position - 60 to move up a little bit. A better
                // approach would be to calculate where the top of the model is, and
                // draw there. It's not that much harder to do, but to keep the
                // sample easy, we'll take the easy way out.
                Vector2 textPosition(screenSpace.X, screenSpace.Y - 60);

                // we want to draw the text centered around textPosition, so we'll
                // calculate the center of the string, and use that as the origin
                // argument to spriteBatch.DrawString. DrawString automatically
                // centers text around the vector specified by the origin argument.
                Vector2 stringCenter = spriteFont->MeasureString(ModelFilenames[i]) / 2;

                // to make the text readable, we'll draw the same thing twice, once
                // white and once black, with a little offset to get a drop shadow
                // effect.

                // first we'll draw the shadow...
                Vector2 shadowOffset(1, 1);
                spriteBatch->DrawString(*spriteFont, ModelFilenames[i],
                                        textPosition + shadowOffset, Color::Black, 0.0f,
                                        stringCenter, 1.0f, SpriteEffects::None, 0.0f);

                // ...and then the real text on top.
                spriteBatch->DrawString(*spriteFont, ModelFilenames[i],
                                        textPosition, Color::White, 0.0f,
                                        stringCenter, 1.0f, SpriteEffects::None, 0.0f);
            }
        }

        spriteBatch->End();
    }

    void PickingSampleGame::DrawModel(Model& model, const Matrix& worldTransform,
                                      const std::vector<Matrix>& absoluteBoneTransforms,
                                      bool drawBoundingSphere)
    {
        // nothing tricky in here; this is the same model drawing code that we see
        // everywhere. we'll loop over all of the meshes in the model, set up their
        // effects, and then draw them.
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
                        "PickingSample: a mesh effect is not a BasicEffect.");
                }

                effect->EnableDefaultLighting();

                effect->setViewProperty(viewMatrix);
                effect->setProjectionProperty(projectionMatrix);
                effect->setWorldProperty(
                    absoluteBoneTransforms[static_cast<std::size_t>(
                        mesh->getParentBoneProperty()->getIndexProperty())]
                    * worldTransform);
            }
            mesh->Draw();

            if (drawBoundingSphere)
            {
                // the mesh's BoundingSphere is stored relative to the mesh itself.
                // (Mesh space). We want to get this BoundingSphere in terms of world
                // coordinates. To do this, we calculate a matrix that will transform
                // from coordinates from mesh space into world space....
                Matrix world = absoluteBoneTransforms[static_cast<std::size_t>(
                                   mesh->getParentBoneProperty()->getIndexProperty())]
                             * worldTransform;

                // ... and then transform the BoundingSphere using that matrix.
                BoundingSphere sphere = TransformBoundingSphere(mesh->getBoundingSphereProperty(),
                                                                world);

                // now draw the sphere with our renderer
                BoundingSphereRenderer::Draw(sphere, viewMatrix, projectionMatrix);
            }
        }
    }

    bool PickingSampleGame::RayIntersectsModel(const Ray& ray, Model& model,
                                               const Matrix& worldTransform,
                                               const std::vector<Matrix>& absoluteBoneTransforms)
    {
        // Each ModelMesh in a Model has a bounding sphere, so to check for an
        // intersection in the Model, we have to check every mesh.
        for (ModelMesh* mesh : model.getMeshesProperty())
        {
            // the mesh's BoundingSphere is stored relative to the mesh itself.
            // (Mesh space). We want to get this BoundingSphere in terms of world
            // coordinates. To do this, we calculate a matrix that will transform
            // from coordinates from mesh space into world space....
            Matrix world = absoluteBoneTransforms[static_cast<std::size_t>(
                               mesh->getParentBoneProperty()->getIndexProperty())]
                         * worldTransform;

            // ... and then transform the BoundingSphere using that matrix.
            BoundingSphere sphere = TransformBoundingSphere(mesh->getBoundingSphereProperty(), world);

            // now that the we have a sphere in world coordinates, we can just use
            // the BoundingSphere class's Intersects function. Intersects returns a
            // nullable float (float?). This value is the distance at which the ray
            // intersects the BoundingSphere, or null if there is no intersection.
            // so, if the value is not null, we have a collision.
            if (sphere.Intersects(ray).has_value())
            {
                return true;
            }
        }

        // if we've gotten this far, we've made it through every BoundingSphere, and
        // none of them intersected the ray. This means that there was no collision,
        // and we should return false.
        return false;
    }

    BoundingSphere PickingSampleGame::TransformBoundingSphere(const BoundingSphere& sphere,
                                                              const Matrix& transform)
    {
        BoundingSphere transformedSphere;

        // the transform can contain different scales on the x, y, and z components.
        // this has the effect of stretching and squishing our bounding sphere along
        // different axes. Obviously, this is no good: a bounding sphere has to be a
        // SPHERE. so, the transformed sphere's radius must be the maximum of the
        // scaled x, y, and z radii.

        // to calculate how the transform matrix will affect the x, y, and z
        // components of the sphere, we'll create a vector3 with x y and z equal
        // to the sphere's radius...
        Vector3 scale3(sphere.Radius, sphere.Radius, sphere.Radius);

        // then transform that vector using the transform matrix. we use
        // TransformNormal because we don't want to take translation into account.
        scale3 = Vector3::TransformNormal(scale3, transform);

        // scale3 contains the x, y, and z radii of a squished and stretched sphere.
        // we'll set the finished sphere's radius to the maximum of the x y and z
        // radii, creating a sphere that is large enough to contain the original
        // squished sphere.
        transformedSphere.Radius = std::max(scale3.X, std::max(scale3.Y, scale3.Z));

        // transforming the center of the sphere is much easier. we can just use
        // Vector3.Transform to transform the center vector. notice that we're using
        // Transform instead of TransformNormal because in this case we DO want to
        // take translation into account.
        transformedSphere.Center = Vector3::Transform(sphere.Center, transform);

        return transformedSphere;
    }
}
