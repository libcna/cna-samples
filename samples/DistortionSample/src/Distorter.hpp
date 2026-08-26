// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Distorter.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>

#include "DistortionComponent.hpp"

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace DistortionSample
{
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Graphics::Model;
    using SharpRuntime::String;

    /**
     * @brief A combination of model and distortion technique.
     */
    class Distorter
    {
    public:
        /** @brief The content name of the model, shown in the overlay. */
        String ModelName;

        // XNA's Model is a reference type and this field starts null; CNA loads a Model by
        // value, so the empty optional is the null.
        /** @brief The model whose geometry is rendered into the distortion map. */
        std::optional<Model> Model_;

        /** @brief How far this distorter displaces the image. */
        float DistortionScale = 0.005f;

        /** @brief Where the model sits in the world. */
        Matrix World = Matrix::getIdentityProperty();

        /** @brief Which `Distorters.fx` technique renders it. */
        DistortionComponent::DistortionTechnique Technique =
            DistortionComponent::DistortionTechnique::ZeroDisplacement;

        /** @brief True to blur the distorted result. */
        bool DistortionBlur = false;

        /**
         * @brief Describes this distorter for the overlay.
         * @return The friendly technique name, the model name, and whether it is blurred.
         */
        [[nodiscard]] String ToString() const;
    };
}
