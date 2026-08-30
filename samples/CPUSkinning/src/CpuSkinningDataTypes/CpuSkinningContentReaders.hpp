// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CpuSkinnedModelReader.cs and CNAEXT reader registration
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"

namespace Microsoft::Xna::Framework::Content
{
    class ContentReader;
}

namespace CpuSkinningDataTypes
{
    class CpuSkinnedModel;
    class CpuSkinnedModelPart;

    /** @brief Reads a complete CpuSkinnedModel written by the sample content processor. */
    class CpuSkinnedModelReader
        : public Microsoft::Xna::Framework::Content::ContentTypeReader<
              std::shared_ptr<CpuSkinnedModel>>
    {
    public:
        /** @brief Constructs the reader with the CpuSkinnedModel target type. */
        CpuSkinnedModelReader();

    protected:
        /**
         * @brief Reads the model-part list followed by its skinning data.
         * @param input Reader positioned at the model payload.
         * @param existingInstance Existing model instance, unused by this reader.
         * @return The reconstructed CPU-skinned model.
         */
        std::shared_ptr<CpuSkinnedModel> Read(
            Microsoft::Xna::Framework::Content::ContentReader& input,
            std::optional<std::shared_ptr<CpuSkinnedModel>> existingInstance) override;
    };

    /** @brief Reads one CpuSkinnedModelPart written by the sample content processor. */
    class CpuSkinnedModelPartReader
        : public Microsoft::Xna::Framework::Content::ContentTypeReader<
              std::shared_ptr<CpuSkinnedModelPart>>
    {
    public:
        /** @brief Constructs the reader with the CpuSkinnedModelPart target type. */
        CpuSkinnedModelPartReader();

    protected:
        /**
         * @brief Reads geometry and queues the shared BasicEffect fixup.
         * @param input Reader positioned at the model-part payload.
         * @param existingInstance Existing part instance, unused by this reader.
         * @return The reconstructed model part.
         */
        std::shared_ptr<CpuSkinnedModelPart> Read(
            Microsoft::Xna::Framework::Content::ContentReader& input,
            std::optional<std::shared_ptr<CpuSkinnedModelPart>> existingInstance) override;
    };

    /** @brief Registers the AOT equivalents of XNA's sample-owned XNB readers. */
    struct CpuSkinningContentReaderRegistrationEXT
    {
        /** @brief Registers the model, vertex and animation reader graph. */
        CNAEXT static void RegisterEXT();
    };
}
