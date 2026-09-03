// SPDX-License-Identifier: MS-PL

#pragma once

#include <memory>
#include <array>
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
         * @param carNumber Selected authentic car texture number.
         * @param wheelPosition Accumulated wheel rotation from car physics.
         * @param renderMatrix Car world transform.
         * @param view Camera view matrix.
         * @param projection Camera projection matrix.
         * @param carColor Selected car hue color; retained for the original API contract.
         * @return Number of submitted model mesh parts.
         */
        int Draw(
            int carNumber, float wheelPosition,
            Microsoft::Xna::Framework::Matrix renderMatrix,
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection,
            Microsoft::Xna::Framework::Color carColor);

        /**
         * @brief Draws the best-replay car with the authentic ShadowCar technique.
         * @param wheelPosition Current player wheel rotation, as used by the original ghost.
         * @param renderMatrix Pre-oriented replay car transform.
         * @param view Camera view matrix.
         * @param projection Camera projection matrix.
         * @return Number of submitted ghost mesh parts.
         */
        int DrawGhost(
            float wheelPosition,
            Microsoft::Xna::Framework::Matrix renderMatrix,
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection);

        /**
         * @brief Draws the car into the original shadow-depth pass.
         * @param effect Authentic ShadowMap effect using GenerateShadowMap20.
         * @param renderMatrix Car world transform.
         * @param lightViewProjection Current light view-projection matrix.
         * @return Number of submitted car mesh parts.
         */
        int GenerateShadow(
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            Microsoft::Xna::Framework::Matrix renderMatrix,
            const Microsoft::Xna::Framework::Matrix& lightViewProjection);

        /**
         * @brief Draws the car into the original shadow receiver pass.
         * @param effect Authentic ShadowMap effect using UseShadowMap20.
         * @param renderMatrix Car world transform.
         * @param viewProjection Current camera view-projection matrix.
         * @param lightViewProjection Current light view-projection matrix.
         * @param textureScaleBias Light projection to shadow-texture transform.
         * @return Number of submitted car mesh parts.
         */
        int UseShadow(
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            Microsoft::Xna::Framework::Matrix renderMatrix,
            const Microsoft::Xna::Framework::Matrix& viewProjection,
            const Microsoft::Xna::Framework::Matrix& lightViewProjection,
            const Microsoft::Xna::Framework::Matrix& textureScaleBias);

        /**
         * @brief Draws the authentic selection plate beneath one menu car.
         * @param renderMatrix Plate world transform shared with its car.
         * @param view Selection camera view matrix.
         * @param projection Selection camera projection matrix.
         * @return Number of submitted plate mesh parts.
         */
        int DrawSelectionPlate(
            Microsoft::Xna::Framework::Matrix renderMatrix,
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection);

        /** @brief Draws a selection plate into the authentic shadow receiver pass. */
        int UseSelectionPlateShadow(
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            Microsoft::Xna::Framework::Matrix renderMatrix,
            const Microsoft::Xna::Framework::Matrix& viewProjection,
            const Microsoft::Xna::Framework::Matrix& lightViewProjection,
            const Microsoft::Xna::Framework::Matrix& textureScaleBias);

        /** @brief Gets the loaded authentic car model. */
        [[nodiscard]] const Microsoft::Xna::Framework::Graphics::Model&
        getModelProperty() const;

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device;
        Microsoft::Xna::Framework::Graphics::Model model;
        Microsoft::Xna::Framework::Graphics::Model selectionPlate;
        Graphics::CarModelHierarchy hierarchy;
        std::vector<Microsoft::Xna::Framework::Matrix>
            selectionPlateTransforms;
        std::vector<Microsoft::Xna::Framework::Graphics::Texture2D> carTextures;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect>
            ghostEffect;
        std::vector<bool> reflectionSpecularEffects;

        void InitializeEffects();
        void InitializeSelectionPlateEffects();
        [[nodiscard]] static
        Microsoft::Xna::Framework::Graphics::EffectTechnique*
        SelectOriginalTechnique(
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            const SharpRuntime::String& meshName, int meshPartNumber);
        int DrawShadowParts(
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            Microsoft::Xna::Framework::Matrix renderMatrix,
            const Microsoft::Xna::Framework::Matrix& viewProjection,
            const Microsoft::Xna::Framework::Matrix& lightViewProjection,
            const Microsoft::Xna::Framework::Matrix* textureScaleBias);
        int DrawSelectionPlateShadowParts(
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            Microsoft::Xna::Framework::Matrix renderMatrix,
            const Microsoft::Xna::Framework::Matrix& viewProjection,
            const Microsoft::Xna::Framework::Matrix& lightViewProjection,
            const Microsoft::Xna::Framework::Matrix* textureScaleBias);
    };
}
