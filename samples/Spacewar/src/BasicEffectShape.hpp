// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Shape.hpp"
#include "Enums.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Spacewar
{
    enum class BasicEffectShapes
    {
        Ship,
        Asteroid,
        Projectile,
        Weapon,
    };

    class BasicEffectShape final : public Shape
    {
    public:
        BasicEffectShape(Microsoft::Xna::Framework::Game* game, BasicEffectShapes shape,
                         Microsoft::Xna::Framework::PlayerIndex player, int shipNumber,
                         int skinNumber, LightingType scene);
        BasicEffectShape(Microsoft::Xna::Framework::Game* game, BasicEffectShapes shape,
                         Microsoft::Xna::Framework::PlayerIndex player, int shapeNumber,
                         LightingType scene);
        BasicEffectShape(Microsoft::Xna::Framework::Game* game, BasicEffectShapes shape,
                         int shapeNumber, LightingType scene);

        void Create() override;
        void OnCreateDevice() override;
        void Render() override;
        static void Preload();

    private:
        void CreateShip();
        void CreateShape();
        void SetupEffect();
        [[nodiscard]] std::string ShipMeshName() const;
        [[nodiscard]] std::string ShipDiffuseName() const;

        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> texture_;
        int skinNumber_ = 0;
        int shapeNumber_ = 0;
        BasicEffectShapes shapeType_ = BasicEffectShapes::Projectile;
        Microsoft::Xna::Framework::PlayerIndex player_ = Microsoft::Xna::Framework::PlayerIndex::One;
        std::optional<Microsoft::Xna::Framework::Graphics::Model> model_;
        bool deviceCreated_ = false;
        std::vector<std::string> modelNames_;
        std::vector<std::string> textureNames_;
        int scene_ = 0;
        std::vector<Microsoft::Xna::Framework::Color> material_;
    };
}
