// SPDX-License-Identifier: MS-PL

#include "Graphics/CarModelHierarchy.hpp"

#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"

namespace RacingGame::Graphics
{
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Graphics::Model;
    using Microsoft::Xna::Framework::Graphics::ModelMesh;

    CarModelHierarchy::CarModelHierarchy(Model& setModel)
        : model(setModel), transforms(static_cast<std::size_t>(
              setModel.getBonesProperty().getCountProperty()))
    {
        model.CopyAbsoluteBoneTransformsTo(transforms);
    }

    Matrix CarModelHierarchy::GetObjectMatrix()
    {
        return Matrix::CreateRotationX(MathHelper::Pi / 2.0f);
    }

    std::vector<CarMeshPose> CarModelHierarchy::BuildMeshPoses(
        const float wheelPosition, Matrix renderMatrix) const
    {
        renderMatrix = GetObjectMatrix() * renderMatrix;
        std::vector<CarMeshPose> result;
        result.reserve(static_cast<std::size_t>(
            model.getMeshesProperty().getCountProperty()));

        int wheelNumber = 0;
        for (ModelMesh* mesh : model.getMeshesProperty())
        {
            Matrix meshMatrix = transforms[static_cast<std::size_t>(
                mesh->getParentBoneProperty()->getIndexProperty())];
            int thisWheelNumber = 0;
            if (mesh->getMeshPartsProperty().getCountProperty() == 2)
            {
                thisWheelNumber = ++wheelNumber;
                const float direction =
                    wheelNumber == 2 || wheelNumber == 4 ? 1.0f : -1.0f;
                meshMatrix = Matrix::CreateRotationX(
                    direction * wheelPosition) * meshMatrix;
            }
            result.push_back({mesh, meshMatrix * renderMatrix,
                              thisWheelNumber});
        }
        return result;
    }
}
