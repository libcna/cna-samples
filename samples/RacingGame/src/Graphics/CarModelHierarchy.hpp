// SPDX-License-Identifier: MS-PL

#pragma once

#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Model.hpp"

namespace RacingGame::Graphics
{
    /** @brief One car mesh and the world transform produced for its draw. */
    struct CarMeshPose
    {
        /** @brief Mesh owned by the authentic XNA model. */
        Microsoft::Xna::Framework::Graphics::ModelMesh* mesh = nullptr;
        /** @brief Final mesh world transform. */
        Microsoft::Xna::Framework::Matrix world;
        /** @brief Original one-based wheel sequence, or zero for a non-wheel mesh. */
        int wheelNumber = 0;
    };

    /** @brief Applies the original Racing wheel animation to an authentic XNA Model hierarchy. */
    class CarModelHierarchy
    {
    public:
        /**
         * @brief Caches absolute transforms from the authentic car model.
         * @param model Loaded `Models/Car` XNA model retained by the caller.
         */
        explicit CarModelHierarchy(
            Microsoft::Xna::Framework::Graphics::Model& model);

        /**
         * @brief Builds final mesh transforms for one car draw.
         * @param wheelPosition Accumulated wheel rotation from CarPhysics.
         * @param renderMatrix Car world transform.
         * @return Meshes in original model order with exact wheel numbers and transforms.
         */
        [[nodiscard]] std::vector<CarMeshPose> BuildMeshPoses(
            float wheelPosition,
            Microsoft::Xna::Framework::Matrix renderMatrix) const;

        /** @brief Gets the original fixed conversion from 3ds Max model space. */
        [[nodiscard]] static Microsoft::Xna::Framework::Matrix
        GetObjectMatrix();

    private:
        Microsoft::Xna::Framework::Graphics::Model& model;
        std::vector<Microsoft::Xna::Framework::Matrix> transforms;
    };
}
