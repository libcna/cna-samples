// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "System/InvalidCastException.hpp"

#include "Camera.hpp"
#include "../Misc/IntersectDetails.hpp"

namespace MarbleMazeGame
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    enum class Axis
    {
        X = 0x1,
        Y = 0x2,
        Z = 0x4,
    };

    constexpr Axis operator&(Axis left, Axis right)
    {
        return static_cast<Axis>(static_cast<int>(left) & static_cast<int>(right));
    }

    constexpr Axis operator|(Axis left, Axis right)
    {
        return static_cast<Axis>(static_cast<int>(left) | static_cast<int>(right));
    }

    class DrawableComponent3D : public DrawableGameComponent
    {
    public:
        static constexpr float gravity = 100.0f * 9.81f;
        static constexpr float wallFriction = 100.0f * 0.8f;

        std::optional<Microsoft::Xna::Framework::Graphics::Model> Model;
        MarbleMazeGame::Camera* Camera = nullptr;

        Vector3 Position = Vector3::Zero;
        Vector3 Rotation = Vector3::Zero;
        Vector3 Velocity = Vector3::Zero;
        Vector3 Acceleration = Vector3::Zero;

        std::vector<Matrix> AbsoluteBoneTransforms;
        Matrix FinalWorldTransforms{};
        Matrix OriginalWorldTransforms = Matrix::getIdentityProperty();

        DrawableComponent3D(Game& game, std::string modelName)
            : DrawableGameComponent(game), modelName_(std::move(modelName))
        {
        }

        void Update(GameTime& gameTime) override
        {
            CalcPhysics(gameTime);
            UpdateFinalWorldTransform();
            GameComponent::Update(gameTime);
        }

        void Draw(const GameTime& gameTime) override
        {
            for (ModelMesh* mesh : Model->getMeshesProperty())
            {
                for (Effect* meshEffect : mesh->getEffectsProperty())
                {
                    auto* effect = dynamic_cast<BasicEffect*>(meshEffect);
                    if (effect == nullptr)
                    {
                        throw System::InvalidCastException(
                            "MarbleMazeGame.DrawableComponent3D expected a BasicEffect.");
                    }

                    effect->EnableDefaultLighting();
                    effect->setPreferPerPixelLightingProperty(preferPerPixelLighting_);
                    effect->setProjectionProperty(Camera->Projection);
                    effect->setViewProperty(Camera->View);
                    effect->setWorldProperty(FinalWorldTransforms);
                }

                mesh->Draw();
            }

            DrawableGameComponent::Draw(gameTime);
        }

    protected:
        IntersectDetails intersectDetails_{};
        bool preferPerPixelLighting_ = false;
        float staticGroundFriction_ = 0.1f;

        void LoadContent() override
        {
            Model.emplace(
                getGameProperty().getContentProperty().Load<Microsoft::Xna::Framework::Graphics::Model>(
                    "Models/" + modelName_));
            AbsoluteBoneTransforms.resize(
                static_cast<std::size_t>(Model->getBonesProperty().getCountProperty()));
            Model->CopyAbsoluteBoneTransformsTo(AbsoluteBoneTransforms);
            DrawableGameComponent::LoadContent();
        }

        virtual void UpdateFinalWorldTransform()
        {
            FinalWorldTransforms = Matrix::getIdentityProperty() *
                Matrix::CreateFromYawPitchRoll(Rotation.Y, Rotation.X, Rotation.Z) *
                OriginalWorldTransforms * Matrix::CreateTranslation(Position);
        }

        virtual void CalcPhysics(GameTime& gameTime)
        {
            CalculateCollisions();
            CalculateAcceleration();
            CalculateFriction();
            CalculateVelocityAndPosition(gameTime);
        }

        virtual void CalculateFriction() = 0;
        virtual void CalculateAcceleration() = 0;
        virtual void CalculateVelocityAndPosition(GameTime& gameTime) = 0;
        virtual void CalculateCollisions() = 0;

    private:
        std::string modelName_;
    };
}
