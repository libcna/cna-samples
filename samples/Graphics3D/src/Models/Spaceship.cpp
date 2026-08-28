// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Spaceship.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Spaceship.hpp"

#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "System/InvalidCastException.hpp"

namespace Graphics3DSample
{
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::Effect;
    using Microsoft::Xna::Framework::Graphics::ModelMesh;

    void Spaceship::Load(ContentManager& content)
    {
        // Load the spaceship model from the ContentManager.
        spaceshipModel = content.Load<Model>("Models/spaceship");

        // Allocate the transform matrix array.
        boneTransforms.resize(static_cast<std::size_t>(spaceshipModel->getBonesProperty().getCountProperty()));
    }

    void Spaceship::Draw()
    {
        // Set the world matrix as the root transform of the model.
        spaceshipModel->getRootProperty()->setTransformProperty(getRotationProperty());

        // Look up combined bone matrices for the entire model.
        spaceshipModel->CopyAbsoluteBoneTransformsTo(boneTransforms);

        // Draw the model.
        for (ModelMesh* mesh : spaceshipModel->getMeshesProperty())
        {
            for (Effect* meshEffect : mesh->getEffectsProperty())
            {
                // The original's foreach is typed `BasicEffect`, and a C# cast-per-element loop
                // throws on a mismatch, so this does too.
                auto* effect = dynamic_cast<BasicEffect*>(meshEffect);
                if (effect == nullptr)
                {
                    throw System::InvalidCastException(
                        "Spaceship: a mesh effect is not a BasicEffect.");
                }

                effect->setWorldProperty(
                    boneTransforms[static_cast<std::size_t>(mesh->getParentBoneProperty()->getIndexProperty())]);
                effect->setViewProperty(getViewProperty());
                effect->setProjectionProperty(getProjectionProperty());

                SetEffectLights(*effect, getLightsProperty());
                SetEffectPerPixelLightingEnabled(*effect);

                effect->setTextureEnabledProperty(getIsTextureEnabledProperty());
            }

            mesh->Draw();
        }
    }

    void Spaceship::SetEffectPerPixelLightingEnabled(BasicEffect& effect)
    {
        effect.setPreferPerPixelLightingProperty(isPerPixelLightingEnabled);
    }

    void Spaceship::SetEffectLights(BasicEffect& effect, const std::vector<bool>& lights)
    {
        effect.setAlphaProperty(1.0f);
        effect.setDiffuseColorProperty(Vector3(0.75f, 0.75f, 0.75f));
        effect.setSpecularColorProperty(Vector3(0.25f, 0.25f, 0.25f));
        effect.setSpecularPowerProperty(5.0f);
        effect.setAmbientLightColorProperty(Vector3(0.75f, 0.75f, 0.75f));

        effect.getDirectionalLight0Property().setEnabledProperty(lights[0]);
        effect.getDirectionalLight0Property().setDiffuseColorProperty(Vector3::One);
        effect.getDirectionalLight0Property().setDirectionProperty(Vector3::Normalize(Vector3(1, -1, 0)));
        effect.getDirectionalLight0Property().setSpecularColorProperty(Vector3::One);

        effect.getDirectionalLight1Property().setEnabledProperty(lights[1]);
        effect.getDirectionalLight1Property().setDiffuseColorProperty(Vector3(0.5f, 0.5f, 0.5f));
        effect.getDirectionalLight1Property().setDirectionProperty(Vector3::Normalize(Vector3(-1, -1, 0)));
        effect.getDirectionalLight1Property().setSpecularColorProperty(Vector3(1.0f, 1.0f, 1.0f));

        effect.getDirectionalLight2Property().setEnabledProperty(lights[2]);
        effect.getDirectionalLight2Property().setDiffuseColorProperty(Vector3(0.3f, 0.3f, 0.3f));
        effect.getDirectionalLight2Property().setDirectionProperty(Vector3::Normalize(Vector3(-1, -1, -1)));
        effect.getDirectionalLight2Property().setSpecularColorProperty(Vector3(0.3f, 0.3f, 0.3f));

        effect.setLightingEnabledProperty(true);
    }
}
