// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CpuSkinnedModel.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Animation/SkinningData.hpp"
#include "CpuSkinnedModelPart.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/Object.hpp"
#include "System/Collections/ObjectModel/ReadOnlyCollection.hpp"

namespace CpuSkinningDataTypes
{
    class CpuSkinnedModelReader;

    /** @brief Model whose bind-pose vertices are transformed on the CPU each frame. */
    class CpuSkinnedModel : public System::Object
    {
    public:
        /**
         * @brief Returns the fully qualified logical type name.
         * @return `CpuSkinningDataTypes.CpuSkinnedModel`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Gets the animation and skeleton data associated with this model.
         * @return The model's skinning data.
         */
        [[nodiscard]] SkinningData* getSkinningDataProperty() const
        {
            return skinningData.get();
        }

        /**
         * @brief Gets a read-only view of the model parts.
         * @return The model-part collection.
         */
        [[nodiscard]] const System::Collections::ObjectModel::ReadOnlyCollection<
            std::shared_ptr<CpuSkinnedModelPart>>& getPartsProperty() const
        {
            return parts;
        }

        /**
         * @brief Sets the bone transforms on every model part.
         * @param bones Current skin transforms for the skeleton.
         */
        void SetBones(const std::vector<Microsoft::Xna::Framework::Matrix>& bones);

        /**
         * @brief Draws every model part using the supplied camera matrices and default lighting.
         * @param world Model-to-world transform.
         * @param view World-to-view transform.
         * @param projection View-to-projection transform.
         */
        void Draw(const Microsoft::Xna::Framework::Matrix& world,
                  const Microsoft::Xna::Framework::Matrix& view,
                  const Microsoft::Xna::Framework::Matrix& projection);

    private:
        friend class CpuSkinnedModelReader;

        CpuSkinnedModel(
            std::vector<std::shared_ptr<CpuSkinnedModelPart>> modelParts,
            std::shared_ptr<SkinningData> skinningData);

        std::shared_ptr<std::vector<std::shared_ptr<CpuSkinnedModelPart>>> modelParts;
        std::shared_ptr<SkinningData> skinningData;
        System::Collections::ObjectModel::ReadOnlyCollection<
            std::shared_ptr<CpuSkinnedModelPart>> parts;
    };
}
