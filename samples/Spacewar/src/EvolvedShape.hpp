// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Shape.hpp"
#include "Enums.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/TextureCube.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class Effect;
    class EffectParameter;
}

namespace Spacewar
{
    enum class EvolvedShapes
    {
        Ship,
        Asteroid,
        Projectile,
        Weapon,
    };

    class EvolvedShape final : public Shape
    {
    public:
        EvolvedShape(Microsoft::Xna::Framework::Game* game, EvolvedShapes shape,
                     Microsoft::Xna::Framework::PlayerIndex player, int shipNumber,
                     int skinNumber, LightingType scene);
        EvolvedShape(Microsoft::Xna::Framework::Game* game, EvolvedShapes shape,
                     Microsoft::Xna::Framework::PlayerIndex player, int shapeNumber,
                     LightingType scene);
        EvolvedShape(Microsoft::Xna::Framework::Game* game, EvolvedShapes shape,
                     int shapeNumber, LightingType scene);

        void Create() override;
        void OnCreateDevice() override;
        void Render() override;
        static void Preload();

    private:
        void SetupEffect();
        void CreateShip();
        void CreateShape();
        [[nodiscard]] std::string ShipMeshName() const;
        [[nodiscard]] std::string ShipDiffuseName() const;

        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> texture_;
        std::optional<Microsoft::Xna::Framework::Graphics::TextureCube> reflection1_;
        std::optional<Microsoft::Xna::Framework::Graphics::TextureCube> reflection2_;
        int skinNumber_ = 0;
        int shapeNumber_ = 0;
        EvolvedShapes shapeType_ = EvolvedShapes::Projectile;
        Microsoft::Xna::Framework::PlayerIndex player_ = Microsoft::Xna::Framework::PlayerIndex::One;
        std::optional<Microsoft::Xna::Framework::Graphics::Model> model_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> blackTexture_;
        std::vector<std::string> modelNames_;
        std::vector<std::string> textureNames_;
        int scene_ = 0;
        std::vector<Microsoft::Xna::Framework::Color> material_;
        std::vector<bool> useReflection2_;

        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> effect_;
        Microsoft::Xna::Framework::Graphics::EffectParameter* worldParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* inverseWorldParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* worldViewProjectionParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* viewPositionParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* skinTextureParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* normalTextureParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* reflectionTextureParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* ambientParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* directionalDirectionParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* directionalColorParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* pointPositionParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* pointColorParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* pointFactorParam_ = nullptr;
        Microsoft::Xna::Framework::Graphics::EffectParameter* materialParam_ = nullptr;
    };
}
