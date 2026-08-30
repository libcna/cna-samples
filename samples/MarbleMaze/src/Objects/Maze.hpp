// SPDX-License-Identifier: MS-PL
#pragma once

#include <vector>

#include "CNA/CNAHelper.hpp"
#include "CNA/Content/ObjectDictionaryEXT.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "System/Collections/Generic/LinkedList.hpp"
#include "System/InvalidCastException.hpp"

#include "DrawableComponent3D.hpp"
#include "../Misc/TriangleSphereCollisionDetection.hpp"

namespace MarbleMazeGame
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using System::Collections::Generic::LinkedList;

    class Maze : public DrawableComponent3D
    {
    public:
        std::vector<Vector3> Ground;
        std::vector<Vector3> Walls;
        std::vector<Vector3> FloorSides;
        LinkedList<Vector3> Checkpoints;
        Vector3 StartPoistion{};
        Vector3 End{};

        explicit Maze(Game& game)
            : DrawableComponent3D(game, "maze1")
        {
            preferPerPixelLighting_ = false;
        }

        void Draw(const GameTime&) override
        {
            auto& device = getGraphicsDeviceProperty();
            const SamplerState originalSamplerState = device.getSamplerStatesProperty()[0];
            device.getSamplerStatesProperty()[0] = SamplerState::LinearWrap;

            for (ModelMesh* mesh : Model->getMeshesProperty())
            {
                for (Effect* meshEffect : mesh->getEffectsProperty())
                {
                    auto* effect = dynamic_cast<BasicEffect*>(meshEffect);
                    if (effect == nullptr)
                    {
                        throw System::InvalidCastException(
                            "MarbleMazeGame.Maze expected a BasicEffect.");
                    }

                    effect->EnableDefaultLighting();
                    effect->setPreferPerPixelLightingProperty(preferPerPixelLighting_);
                    effect->setProjectionProperty(Camera->Projection);
                    effect->setViewProperty(Camera->View);
                    effect->setWorldProperty(
                        AbsoluteBoneTransforms[static_cast<std::size_t>(
                            mesh->getParentBoneProperty()->getIndexProperty())] *
                        FinalWorldTransforms);
                }

                mesh->Draw();
            }

            device.getSamplerStatesProperty()[0] = originalSamplerState;
        }

        void GetCollisionDetails(
            const BoundingSphere& boundingSphere,
            IntersectDetails& intersectDetails,
            bool light)
        {
            intersectDetails.IntersectWithGround =
                TriangleSphereCollisionDetection::IsSphereCollideWithTringles(
                    Ground, boundingSphere, intersectDetails.IntersectedGroundTriangle, true);
            intersectDetails.IntersectWithWalls =
                TriangleSphereCollisionDetection::IsSphereCollideWithTringles(
                    Walls, boundingSphere, intersectDetails.IntersectedWallTriangle, light);
            intersectDetails.IntersectWithFloorSides =
                TriangleSphereCollisionDetection::IsSphereCollideWithTringles(
                    FloorSides, boundingSphere, intersectDetails.IntersectedFloorSidesTriangle, true);
        }

    protected:
        void LoadContent() override
        {
            DrawableComponent3D::LoadContent();

            StartPoistion = Model->getBonesProperty()["Start"]->getTransformProperty().getTranslationProperty();
            End = Model->getBonesProperty()["Finish"]->getTransformProperty().getTranslationProperty();

            CNAEXT auto* tagData =
                dynamic_cast<CNA::Content::ObjectDictionaryEXT*>(Model->getTagProperty());
            if (tagData == nullptr)
            {
                throw System::InvalidCastException(
                    "MarbleMazeGame.Maze expected Model.Tag to contain "
                    "Dictionary<string, List<Vector3>>.");
            }

            Ground = tagData->Get<std::vector<Vector3>>("Floor");
            FloorSides = tagData->Get<std::vector<Vector3>>("floorSides");
            Walls = tagData->Get<std::vector<Vector3>>("walls");

            Checkpoints.AddFirst(StartPoistion);
            for (ModelBone* bone : Model->getBonesProperty())
            {
                if (bone->getNameProperty().find("spawn") != std::string::npos)
                {
                    Checkpoints.AddLast(bone->getTransformProperty().getTranslationProperty());
                }
            }
        }

        void CalculateCollisions() override {}
        void CalculateVelocityAndPosition(GameTime&) override {}
        void CalculateFriction() override {}
        void CalculateAcceleration() override {}
    };
}
