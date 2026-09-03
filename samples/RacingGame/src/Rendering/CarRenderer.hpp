// SPDX-License-Identifier: MS-PL

#pragma once

#include <vector>

#include "Graphics/CarModelHierarchy.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

namespace Microsoft::Xna::Framework::Content
{
    class ContentManager;
}

namespace Microsoft::Xna::Framework::Graphics
{
    class Effect;
    class EffectTechnique;
    class GraphicsDevice;
}

namespace RacingGame::Rendering
{
    /** @brief Draws the authentic XNA car model with its original wheel and glass passes. */
    class CarRenderer
    {
    public:
        /**
         * @brief Loads and initializes the original processed car model.
         * @param device Graphics device owning the model resources.
         * @param content Content manager rooted at the authentic XNA output.
         */
        CarRenderer(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
            Microsoft::Xna::Framework::Content::ContentManager& content);

        /**
         * @brief Draws solid car parts followed by reflection/glass parts.
         * @param wheelPosition Accumulated wheel rotation from car physics.
         * @param renderMatrix Car world transform.
         * @param view Camera view matrix.
         * @param projection Camera projection matrix.
         * @param carColor Selected car hue color; retained for the original API contract.
         * @return Number of submitted model mesh parts.
         */
        int Draw(
            float wheelPosition,
            Microsoft::Xna::Framework::Matrix renderMatrix,
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection,
            Microsoft::Xna::Framework::Color carColor);

        /** @brief Gets the loaded authentic car model. */
        [[nodiscard]] const Microsoft::Xna::Framework::Graphics::Model&
        getModelProperty() const;

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device;
        Microsoft::Xna::Framework::Graphics::Model model;
        Graphics::CarModelHierarchy hierarchy;
        Microsoft::Xna::Framework::Graphics::Texture2D carTexture;
        std::vector<bool> reflectionSpecularEffects;

        void InitializeEffects();
        [[nodiscard]] static
        Microsoft::Xna::Framework::Graphics::EffectTechnique*
        SelectOriginalTechnique(
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            const SharpRuntime::String& meshName, int meshPartNumber);
    };
}
