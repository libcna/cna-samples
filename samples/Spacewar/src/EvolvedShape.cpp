// SPDX-License-Identifier: MS-PL

#include "EvolvedShape.hpp"

#include <array>
#include <cassert>

#include "SpacewarGame.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "System/Int32.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    namespace
    {
        const std::array<std::array<std::string, 3>, 2> ShipMeshes{{
            {{"models/p1_pencil", "models/p1_saucer", "models/p1_wedge"}},
            {{"models/p2_pencil", "models/p2_saucer", "models/p2_wedge"}},
        }};
        const std::array<std::array<std::string, 3>, 2> ShipDiffuse{{
            {{"textures/pencil_p1_diff_v", "textures/saucer_p1_diff_v", "textures/wedge_p1_diff_v"}},
            {{"textures/pencil_p2_diff_v", "textures/saucer_p2_diff_v", "textures/wedge_p2_diff_v"}},
        }};
        const std::array<std::array<std::string, 2>, 2> ShipReflection{{
            {{"textures/p1_reflection_cubemap", ""}},
            {{"textures/p2_reflection_cubemap1", "textures/p2_reflection_cubemap2"}},
        }};
        const std::array<std::string, 5> ProjectileMeshes{
            "models/pea_proj", "models/mgun_proj", "models/mgun_proj", "models/p1_rocket_proj", "models/bfg_proj"};
        const std::array<std::string, 5> ProjectileDiffuse{
            "textures/pea_proj", "textures/pea_proj", "textures/mgun_proj", "textures/rocket_proj", "textures/bfg_proj"};
        const std::array<std::string, 2> AsteroidMeshes{"models/asteroid1", "models/asteroid2"};
        const std::array<std::string, 2> AsteroidDiffuse{"textures/asteroid1", "textures/asteroid2"};
        const std::array<std::array<std::string, 5>, 2> WeaponMeshes{{
            {{"models/p1_pea", "models/p1_mgun", "models/p1_dual", "models/p1_rocket", "models/p1_bfg"}},
            {{"models/p2_pea", "models/p2_mgun", "models/p2_dual", "models/p2_rocket", "models/p2_bfg"}},
        }};
        const std::array<std::array<std::vector<std::string>, 5>, 2> WeaponDiffuse{{
            {{
                {"textures/p1_back"},
                {"textures/p1_back", "textures/p1_dual"},
                {"textures/p1_dual", "textures/p1_back"},
                {"textures/p1_back", "textures/p1_rocket"},
                {"textures/p1_bfg", "", "textures/p1_back"},
            }},
            {{
                {"textures/p2_back", "textures/p2_back"},
                {"textures/p2_back", "textures/p2_back", "textures/p2_dual"},
                {"textures/p2_back", "textures/p2_back", "textures/p2_dual"},
                {"textures/p2_rocket", "textures/p2_back", "textures/p2_back"},
                {"textures/p2_back", "textures/p2_back", "textures/p2_bfg", "textures/p2_back"},
            }},
        }};

        const Color Cockpit1(static_cast<SharpRuntime::intcs>(0.529f * 255.0f), 255, 255, 0);
        const Color Cockpit2(255, 255, static_cast<SharpRuntime::intcs>(0.373f * 255.0f), 0);
        const Color Engines(static_cast<SharpRuntime::intcs>(0.925f * 255.0f),
                            static_cast<SharpRuntime::intcs>(0.529f * 255.0f), 255, 0);

        std::vector<Color> ShipMaterials(std::size_t player, std::size_t ship)
        {
            if (player == 0)
            {
                if (ship == 0) return {Color::White, Engines, Cockpit1, Cockpit1};
                return {Color::White, Cockpit1, Engines};
            }
            if (ship == 0) return {Cockpit2, Color::White, Color::White};
            if (ship == 1) return {Color::White, Color::White, Cockpit2};
            return {Color::White, Cockpit2, Color::White};
        }

        std::vector<bool> ShipUsesReflection2(std::size_t player, std::size_t ship)
        {
            if (player == 0)
                return ship == 0 ? std::vector<bool>{false, false, false, false}
                                 : std::vector<bool>{false, false, false};
            if (ship == 0) return {true, true, false};
            if (ship == 1) return {true, false, true};
            return {true, true, false};
        }
    }

    EvolvedShape::EvolvedShape(Game* game, EvolvedShapes shape, PlayerIndex player,
                               int shipNumber, int skinNumber, LightingType scene)
        : Shape(game), skinNumber_(skinNumber), shapeNumber_(shipNumber), shapeType_(shape),
          player_(player), scene_(static_cast<int>(scene))
    {
        assert(shape == EvolvedShapes::Ship);
        OnCreateDevice();
        CreateShip();
    }

    EvolvedShape::EvolvedShape(Game* game, EvolvedShapes shape, PlayerIndex player,
                               int shapeNumber, LightingType scene)
        : Shape(game), shapeNumber_(shapeNumber), shapeType_(shape), player_(player),
          scene_(static_cast<int>(scene))
    {
        assert(shape == EvolvedShapes::Weapon);
        OnCreateDevice();
        CreateShape();
    }

    EvolvedShape::EvolvedShape(Game* game, EvolvedShapes shape, int shapeNumber, LightingType scene)
        : Shape(game), shapeNumber_(shapeNumber), shapeType_(shape), scene_(static_cast<int>(scene))
    {
        OnCreateDevice();
        CreateShape();
    }

    void EvolvedShape::Create() { OnCreateDevice(); }

    void EvolvedShape::OnCreateDevice()
    {
        effect_ = SpacewarGame::getContentManagerProperty().Load<std::shared_ptr<Effect>>(
            SpacewarGame::getSettingsProperty().MediaPath + "shaders/ship");
        auto& parameters = effect_->getParametersProperty();
        worldParam_ = parameters["world"];
        inverseWorldParam_ = parameters["inverseWorld"];
        worldViewProjectionParam_ = parameters["worldViewProjection"];
        viewPositionParam_ = parameters["viewPosition"];
        skinTextureParam_ = parameters["SkinTexture"];
        normalTextureParam_ = parameters["NormalMapTexture"];
        reflectionTextureParam_ = parameters["ReflectionTexture"];
        ambientParam_ = parameters["Ambient"];
        directionalDirectionParam_ = parameters["DirectionalDirection"];
        directionalColorParam_ = parameters["DirectionalColor"];
        pointPositionParam_ = parameters["PointPosition"];
        pointColorParam_ = parameters["PointColor"];
        pointFactorParam_ = parameters["PointFactor"];
        materialParam_ = parameters["Material"];
        blackTexture_.emplace(SpacewarGame::getContentManagerProperty().Load<Texture2D>(
            SpacewarGame::getSettingsProperty().MediaPath + "Textures/Black"));
        SetupEffect();
    }

    void EvolvedShape::SetupEffect()
    {
        const auto& light = SpacewarGame::getSettingsProperty().ShipLights[static_cast<std::size_t>(scene_)];
        ambientParam_->SetValue(light.Ambient);
        directionalDirectionParam_->SetValue(light.DirectionalDirection);
        directionalColorParam_->SetValue(light.DirectionalColor);
        pointPositionParam_->SetValue(light.PointPosition);
        pointColorParam_->SetValue(light.PointColor);
        pointFactorParam_->SetValue(light.PointFactor);
        normalTextureParam_->SetValue(static_cast<Texture2D*>(nullptr));
    }

    void EvolvedShape::CreateShip()
    {
        material_ = ShipMaterials(static_cast<std::size_t>(player_), static_cast<std::size_t>(shapeNumber_));
        useReflection2_ = ShipUsesReflection2(static_cast<std::size_t>(player_), static_cast<std::size_t>(shapeNumber_));
        SetupEffect();
    }

    void EvolvedShape::CreateShape()
    {
        if (shapeType_ == EvolvedShapes::Projectile)
        {
            modelNames_.assign(ProjectileMeshes.begin(), ProjectileMeshes.end());
            textureNames_.assign(ProjectileDiffuse.begin(), ProjectileDiffuse.end());
        }
        else if (shapeType_ == EvolvedShapes::Asteroid)
        {
            modelNames_.assign(AsteroidMeshes.begin(), AsteroidMeshes.end());
            textureNames_.assign(AsteroidDiffuse.begin(), AsteroidDiffuse.end());
        }
        else if (shapeType_ == EvolvedShapes::Weapon)
        {
            const std::size_t player = static_cast<std::size_t>(player_);
            modelNames_.assign(WeaponMeshes[player].begin(), WeaponMeshes[player].end());
            textureNames_ = WeaponDiffuse[player][static_cast<std::size_t>(shapeNumber_)];
        }
        else
            assert(false);
        SetupEffect();
    }

    std::string EvolvedShape::ShipMeshName() const
    {
        return ShipMeshes[static_cast<std::size_t>(player_)][static_cast<std::size_t>(shapeNumber_)];
    }

    std::string EvolvedShape::ShipDiffuseName() const
    {
        return ShipDiffuse[static_cast<std::size_t>(player_)][static_cast<std::size_t>(shapeNumber_)] +
               System::Int32::ToString(skinNumber_ + 1);
    }

    void EvolvedShape::Render()
    {
        Shape::Render();
        worldParam_->SetValue(world_);
        inverseWorldParam_->SetValue(Matrix::Invert(world_));
        worldViewProjectionParam_->SetValue(
            world_ * SpacewarGame::getCameraProperty().getViewProperty() *
            SpacewarGame::getCameraProperty().getProjectionProperty());
        viewPositionParam_->SetValue(Vector4(SpacewarGame::getCameraProperty().getViewPositionProperty(), 0.0f));

        if (shapeType_ == EvolvedShapes::Ship)
        {
            model_.emplace(SpacewarGame::getContentManagerProperty().Load<Model>(
                SpacewarGame::getSettingsProperty().MediaPath + ShipMeshName()));
            texture_.emplace(SpacewarGame::getContentManagerProperty().Load<Texture2D>(
                SpacewarGame::getSettingsProperty().MediaPath + ShipDiffuseName()));
            reflection1_.emplace(SpacewarGame::getContentManagerProperty().Load<TextureCube>(
                SpacewarGame::getSettingsProperty().MediaPath +
                ShipReflection[static_cast<std::size_t>(player_)][0]));
            if (player_ == PlayerIndex::Two)
            {
                reflection2_.emplace(SpacewarGame::getContentManagerProperty().Load<TextureCube>(
                    SpacewarGame::getSettingsProperty().MediaPath + ShipReflection[1][1]));
            }
        }
        else
        {
            model_.emplace(SpacewarGame::getContentManagerProperty().Load<Model>(
                SpacewarGame::getSettingsProperty().MediaPath + modelNames_[static_cast<std::size_t>(shapeNumber_)]));
            if (shapeType_ == EvolvedShapes::Asteroid || shapeType_ == EvolvedShapes::Projectile)
            {
                texture_.emplace(SpacewarGame::getContentManagerProperty().Load<Texture2D>(
                    SpacewarGame::getSettingsProperty().MediaPath + textureNames_[static_cast<std::size_t>(shapeNumber_)]));
            }
        }

        int i = 0;
        for (ModelMesh* modelMesh : model_->getMeshesProperty())
        {
            for (ModelMeshPart* meshPart : modelMesh->getMeshPartsProperty())
            {
                if (shapeType_ == EvolvedShapes::Weapon)
                {
                    if (textureNames_[static_cast<std::size_t>(i)].empty())
                        texture_.reset();
                    else
                        texture_.emplace(SpacewarGame::getContentManagerProperty().Load<Texture2D>(
                            SpacewarGame::getSettingsProperty().MediaPath + textureNames_[static_cast<std::size_t>(i)]));
                }
                skinTextureParam_->SetValue(texture_ ? &*texture_ : nullptr);
                if (shapeType_ == EvolvedShapes::Ship)
                {
                    materialParam_->SetValue(material_[static_cast<std::size_t>(i)].ToVector4());
                    if (material_[static_cast<std::size_t>(i)] != Color::White)
                        skinTextureParam_->SetValue(&*blackTexture_);
                    reflectionTextureParam_->SetValue(
                        useReflection2_[static_cast<std::size_t>(i)] ? &*reflection2_ : &*reflection1_);
                }
                else
                {
                    materialParam_->SetValue(Color::White.ToVector4());
                    reflectionTextureParam_->SetValue(static_cast<Texture2D*>(nullptr));
                }

                effect_->getTechniquesProperty()[0].getPassesProperty()[0].Apply();
                if (meshPart->getPrimitiveCountProperty() > 0)
                {
                    auto& device = getGameInstanceProperty()->getGraphicsDeviceProperty();
                    device.SetVertexBuffer(meshPart->getVertexBufferProperty());
                    device.SetIndexBuffer(meshPart->getIndexBufferProperty());
                    device.DrawIndexedPrimitives(PrimitiveType::TriangleList,
                        meshPart->getVertexOffsetProperty(), 0, meshPart->getNumVerticesProperty(),
                        meshPart->getStartIndexProperty(), meshPart->getPrimitiveCountProperty());
                }
                ++i;
            }
        }
    }

    void EvolvedShape::Preload()
    {
    }
}
