// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CNAEXT -- the C++ counterpart of the reflection XNA performs for this game. Not upstream.
//-----------------------------------------------------------------------------

#include "CustomModelContentReaders.hpp"

#include <memory>
#include <string>

#include "CustomModel.hpp"

#include "CNA/Internal/Xnb/CollectionContentTypeReaders.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"
#include "Microsoft/Xna/Framework/Content/ReflectiveTypeReader.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"

namespace CustomModelSample
{
    using CNA::Internal::Xnb::ListReader;
    using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;
    using Microsoft::Xna::Framework::Content::ReflectiveTypeReader;
    using Microsoft::Xna::Framework::Content::ReflectiveTypeReaderBuilder;

    namespace
    {
        const std::string ModelTypeName = "CustomModelSample.CustomModel";
        const std::string ModelPartTypeName = "CustomModelSample.CustomModel+ModelPart";
    }

    void CustomModelContentReaderRegistrationEXT::RegisterEXT()
    {
        using ModelPart = CustomModel::ModelPart;

        ReflectiveTypeReaderBuilder<ModelPart>(ModelPartTypeName)
            .Field(&ModelPart::TriangleCount)
            .Field(&ModelPart::VertexCount)
            .Field(&ModelPart::VertexBuffer)
            .Field(&ModelPart::IndexBuffer)
            .SharedResourceField(&ModelPart::Effect)
            .RegisterShared();

        ContentTypeReaderManager::AddTypeCreator(
            "Microsoft.Xna.Framework.Content.ListReader`1[[" + ModelPartTypeName + "]]",
            [] {
                return std::make_unique<ListReader<std::shared_ptr<ModelPart>>>(
                    "System.Collections.Generic.List`1[[" + ModelPartTypeName + "]]",
                    ReflectiveTypeReader<ModelPart>::CanonicalReaderName(ModelPartTypeName));
            });

        ReflectiveTypeReaderBuilder<CustomModel>(ModelTypeName)
            .Field(&CustomModel::modelParts)
            .RegisterShared();
    }
}
