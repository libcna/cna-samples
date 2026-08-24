// SPDX-License-Identifier: MS-PL

#include "BasicEffectShape.hpp"

#include <array>
#include <cassert>

#include "SpacewarGame.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DirectionalLight.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    namespace
    {
        const std::array<std::array<std::string, 3>, 2> ShipMeshesBasic{{
            {{"models/pencil_player1", "models/saucer_player1", "models/wedge_player1"}},
            {{"models/pencil_player2", "models/saucer_player2", "models/wedge_player2"}},
        }};
        const std::array<std::array<std::string, 3>, 2> ShipDiffuseBasic{{
            {{"textures/pencil_p1_diff_v", "textures/saucer_p1_diff_v", "textures/wedge_p1_diff_v"}},
            {{"textures/pencil_p2_diff_v", "textures/saucer_p2_diff_v", "textures/wedge_p2_diff_v"}},
        }};
        const std::array<std::string, 5> ProjectileMeshesBasic{
            "models/pea_proj", "models/mgun_proj", "models/mgun_proj", "models/p1_rocket_proj", "models/bfg_proj"};
        const std::array<std::string, 5> ProjectileDiffuseBasic{
            "textures/pea_proj", "textures/pea_proj", "textures/mgun_proj", "textures/rocket_proj", "textures/bfg_proj"};
        const std::array<std::string, 2> AsteroidMeshesBasic{"models/asteroid1", "models/asteroid2"};
        const std::array<std::string, 2> AsteroidDiffuseBasic{"textures/asteroid1", "textures/asteroid2"};
        const std::array<std::array<std::string, 5>, 2> WeaponMeshesBasic{{
            {{"models/p1_pea", "models/p1_mgun", "models/p1_dual", "models/p1_rocket", "models/p1_bfg"}},
            {{"models/p2_pea", "models/p2_mgun", "models/p2_dual", "models/p2_rocket", "models/p2_bfg"}},
        }};
        const std::array<std::array<std::vector<std::string>, 5>, 2> WeaponDiffuseBasic{{
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
        const Color Cockpit1Basic(static_cast<SharpRuntime::intcs>(0.529f * 255.0f), 255, 255, 0);
        const Color Cockpit2Basic(255, 255, static_cast<SharpRuntime::intcs>(0.373f * 255.0f), 0);
        const Color EnginesBasic(static_cast<SharpRuntime::intcs>(0.925f * 255.0f),
                                 static_cast<SharpRuntime::intcs>(0.529f * 255.0f), 255, 0);

        std::vector<Color> ShipMaterialsBasic(std::size_t player, std::size_t ship)
        {
            if (player == 0)
            {
                if (ship == 0) return {Color::White, EnginesBasic, Cockpit1Basic, Cockpit1Basic};
                return {Color::White, Cockpit1Basic, EnginesBasic};
            }
            if (ship == 0) return {Cockpit2Basic, Color::White, Color::White};
            if (ship == 1) return {Color::White, Color::White, Cockpit2Basic};
            return {Color::White, Cockpit2Basic, Color::White};
        }
    }

    BasicEffectShape::BasicEffectShape(Game* game, BasicEffectShapes shape, PlayerIndex player,
                                       int shipNumber, int skinNumber, LightingType scene)
        : Shape(game), skinNumber_(skinNumber), shapeNumber_(shipNumber), shapeType_(shape),
          player_(player), scene_(static_cast<int>(scene))
    {
        assert(shape == BasicEffectShapes::Ship);
        OnCreateDevice();
        CreateShip();
    }

    BasicEffectShape::BasicEffectShape(Game* game, BasicEffectShapes shape, PlayerIndex player,
                                       int shapeNumber, LightingType scene)
        : Shape(game), shapeNumber_(shapeNumber), shapeType_(shape), player_(player),
          scene_(static_cast<int>(scene))
    {
        assert(shape == BasicEffectShapes::Weapon);
        OnCreateDevice();
        CreateShape();
    }

    BasicEffectShape::BasicEffectShape(Game* game, BasicEffectShapes shape, int shapeNumber,
                                       LightingType scene)
        : Shape(game), shapeNumber_(shapeNumber), shapeType_(shape), scene_(static_cast<int>(scene))
    {
        OnCreateDevice();
        CreateShape();
    }

    void BasicEffectShape::Create() { OnCreateDevice(); }
    void BasicEffectShape::OnCreateDevice() { deviceCreated_ = true; }

    std::string BasicEffectShape::ShipMeshName() const
    {
        return ShipMeshesBasic[static_cast<std::size_t>(player_)][static_cast<std::size_t>(shapeNumber_)];
    }

    std::string BasicEffectShape::ShipDiffuseName() const
    {
        return ShipDiffuseBasic[static_cast<std::size_t>(player_)][static_cast<std::size_t>(shapeNumber_)] +
               std::to_string(skinNumber_ + 1);
    }

    void BasicEffectShape::CreateShip()
    {
        model_.emplace(SpacewarGame::getContentManagerProperty().Load<Model>(
            SpacewarGame::getSettingsProperty().MediaPath + ShipMeshName()));
        texture_.emplace(SpacewarGame::getContentManagerProperty().Load<Texture2D>(
            SpacewarGame::getSettingsProperty().MediaPath + ShipDiffuseName()));
        material_ = ShipMaterialsBasic(static_cast<std::size_t>(player_), static_cast<std::size_t>(shapeNumber_));
        SetupEffect();
    }

    void BasicEffectShape::CreateShape()
    {
        if (shapeType_ == BasicEffectShapes::Projectile)
        {
            modelNames_.assign(ProjectileMeshesBasic.begin(), ProjectileMeshesBasic.end());
            textureNames_.assign(ProjectileDiffuseBasic.begin(), ProjectileDiffuseBasic.end());
        }
        else if (shapeType_ == BasicEffectShapes::Asteroid)
        {
            modelNames_.assign(AsteroidMeshesBasic.begin(), AsteroidMeshesBasic.end());
            textureNames_.assign(AsteroidDiffuseBasic.begin(), AsteroidDiffuseBasic.end());
        }
        else if (shapeType_ == BasicEffectShapes::Weapon)
        {
            const std::size_t player = static_cast<std::size_t>(player_);
            modelNames_.assign(WeaponMeshesBasic[player].begin(), WeaponMeshesBasic[player].end());
            textureNames_ = WeaponDiffuseBasic[player][static_cast<std::size_t>(shapeNumber_)];
        }
        else
            assert(false);

        model_.emplace(SpacewarGame::getContentManagerProperty().Load<Model>(
            SpacewarGame::getSettingsProperty().MediaPath + modelNames_[static_cast<std::size_t>(shapeNumber_)]));
        if (shapeType_ == BasicEffectShapes::Asteroid || shapeType_ == BasicEffectShapes::Projectile)
        {
            texture_.emplace(SpacewarGame::getContentManagerProperty().Load<Texture2D>(
                SpacewarGame::getSettingsProperty().MediaPath + textureNames_[static_cast<std::size_t>(shapeNumber_)]));
        }
        SetupEffect();
    }

    void BasicEffectShape::SetupEffect()
    {
        int i = 0;
        for (ModelMesh* modelMesh : model_->getMeshesProperty())
        {
            for (Effect* genericEffect : modelMesh->getEffectsProperty())
            {
                auto* effect = dynamic_cast<BasicEffect*>(genericEffect);
                assert(effect != nullptr);
                effect->setAlphaProperty(1.0f);
                effect->setSpecularPowerProperty(200.0f);
                effect->setAmbientLightColorProperty(Vector3(0.15f, 0.15f, 0.15f));
                effect->setLightingEnabledProperty(true);

                auto& light0 = effect->getDirectionalLight0Property();
                light0.setEnabledProperty(true);
                const auto& light = SpacewarGame::getSettingsProperty().ShipLights[static_cast<std::size_t>(scene_)];
                light0.setDiffuseColorProperty(Vector3(light.DirectionalColor.X, light.DirectionalColor.Y,
                                                        light.DirectionalColor.Z));
                light0.setSpecularColorProperty(Vector3(0.1f, 0.1f, 0.1f));
                light0.setDirectionProperty(Vector3::Normalize(Vector3(
                    light.DirectionalDirection.X, light.DirectionalDirection.Y, light.DirectionalDirection.Z)));

                auto& light1 = effect->getDirectionalLight1Property();
                light1.setEnabledProperty(true);
                light1.setDiffuseColorProperty(Vector3(light.PointColor.X, light.PointColor.Y, light.PointColor.Z));
                light1.setSpecularColorProperty(Vector3(0.1f, 0.1f, 0.1f));

                if (shapeType_ == BasicEffectShapes::Weapon)
                {
                    if (textureNames_[static_cast<std::size_t>(i)].empty()) texture_.reset();
                    else texture_.emplace(SpacewarGame::getContentManagerProperty().Load<Texture2D>(
                        SpacewarGame::getSettingsProperty().MediaPath + textureNames_[static_cast<std::size_t>(i)]));
                }
                effect->setTextureProperty(texture_ ? &*texture_ : nullptr);
                if (shapeType_ == BasicEffectShapes::Ship)
                {
                    const Color color = material_[static_cast<std::size_t>(i)];
                    const Vector3 material(static_cast<float>(color.getRProperty()) / 255.0f,
                                           static_cast<float>(color.getGProperty()) / 255.0f,
                                           static_cast<float>(color.getBProperty()) / 255.0f);
                    effect->setSpecularColorProperty(material);
                    effect->setDiffuseColorProperty(material);
                    effect->setTextureEnabledProperty(color == Color::White);
                }
                else
                {
                    effect->setSpecularColorProperty(Vector3::One);
                    effect->setDiffuseColorProperty(Vector3::One);
                    effect->setTextureEnabledProperty(true);
                }
                ++i;
            }
        }
    }

    void BasicEffectShape::Render()
    {
        Shape::Render();
        auto& device = getGameInstanceProperty()->getGraphicsDeviceProperty();
        device.setBlendStateProperty(BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::Default);

        if (shapeType_ == BasicEffectShapes::Ship)
        {
            model_.emplace(SpacewarGame::getContentManagerProperty().Load<Model>(
                SpacewarGame::getSettingsProperty().MediaPath + ShipMeshName()));
            texture_.emplace(SpacewarGame::getContentManagerProperty().Load<Texture2D>(
                SpacewarGame::getSettingsProperty().MediaPath + ShipDiffuseName()));
        }
        else
        {
            model_.emplace(SpacewarGame::getContentManagerProperty().Load<Model>(
                SpacewarGame::getSettingsProperty().MediaPath + modelNames_[static_cast<std::size_t>(shapeNumber_)]));
            if (shapeType_ == BasicEffectShapes::Asteroid || shapeType_ == BasicEffectShapes::Projectile)
            {
                texture_.emplace(SpacewarGame::getContentManagerProperty().Load<Texture2D>(
                    SpacewarGame::getSettingsProperty().MediaPath + textureNames_[static_cast<std::size_t>(shapeNumber_)]));
            }
        }

        if (deviceCreated_)
        {
            SetupEffect();
            deviceCreated_ = false;
        }

        const Vector3 sunPosition(-0.5f, -0.5f, 100.0f);
        for (ModelMesh* modelMesh : model_->getMeshesProperty())
        {
            for (Effect* genericEffect : modelMesh->getEffectsProperty())
            {
                auto* effect = dynamic_cast<BasicEffect*>(genericEffect);
                assert(effect != nullptr);
                effect->setViewProperty(SpacewarGame::getCameraProperty().getViewProperty());
                effect->setProjectionProperty(SpacewarGame::getCameraProperty().getProjectionProperty());
                effect->setWorldProperty(world_);
                effect->setTextureProperty(texture_ ? &*texture_ : nullptr);
                Vector3 direction = position_ - sunPosition;
                direction.Normalize();
                effect->getDirectionalLight1Property().setDirectionProperty(direction);
            }
            modelMesh->Draw();
        }
    }

    void BasicEffectShape::Preload()
    {
    }
}
