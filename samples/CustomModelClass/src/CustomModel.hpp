// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CustomModel.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/Object.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class Effect;
    class IndexBuffer;
    class VertexBuffer;
}

namespace CustomModelSample
{
    /** @brief Grants the reflective XNB registration access to deserializer-written fields. */
    struct CustomModelContentReaderRegistrationEXT;

    /**
     * @brief Custom class that can be used as a replacement for the built-in Model type.
     *
     * This provides functionality roughly similar to Model, but simplified as far as possible
     * while still being able to correctly render data from arbitrary X or FBX files.
     */
    class CustomModel : public System::Object
    {
    public:
        /**
         * @brief Constructs an empty custom model for the XNB deserializer to populate.
         *
         * The C# constructor is private and reflection invokes it. The C++ reflective reader
         * constructs the object through `std::make_shared`, so the equivalent constructor must
         * be accessible here.
         */
        CustomModel() = default;

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `CustomModelSample.CustomModel`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Draws the model using the specified camera matrices.
         *
         * @param world World transformation matrix.
         * @param view View transformation matrix.
         * @param projection Projection transformation matrix.
         */
        void Draw(const Microsoft::Xna::Framework::Matrix& world,
                  const Microsoft::Xna::Framework::Matrix& view,
                  const Microsoft::Xna::Framework::Matrix& projection);

    private:
        friend struct CustomModelContentReaderRegistrationEXT;

        /** @brief One piece of geometry that uses a single effect. */
        class ModelPart : public System::Object
        {
        public:
            /** @brief Constructs an empty model part for the XNB deserializer to populate. */
            ModelPart() = default;

            /**
             * @brief Returns the fully qualified logical type name.
             * @return `CustomModelSample.CustomModel+ModelPart`.
             */
            CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        private:
            friend struct CustomModelContentReaderRegistrationEXT;
            friend class CustomModel;

            int TriangleCount = 0;
            int VertexCount = 0;
            std::shared_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> VertexBuffer;
            std::shared_ptr<Microsoft::Xna::Framework::Graphics::IndexBuffer> IndexBuffer;
            std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> Effect;
        };

        std::vector<std::shared_ptr<ModelPart>> modelParts;
    };
}
