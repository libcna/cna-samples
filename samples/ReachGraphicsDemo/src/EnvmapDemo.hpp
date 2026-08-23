#pragma once

// Ported from XnaGraphicsDemo.EnvmapDemo (EnvmapDemo.cs).

#include <optional>
#include <stdexcept>
#include <vector>

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/EnvironmentMapEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "DemoGame.hpp"
#include "MenuComponent.hpp"
#include "MenuEntry.hpp"

namespace ReachGraphicsDemoSample {

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

class EnvmapDemo : public MenuComponent {
public:
    explicit EnvmapDemo(DemoGame& game) : MenuComponent(game) {
        auto amountEntry = std::make_unique<FloatMenuEntry>();
        amountEntry->SetText("envmap");
        amount_ = amountEntry.get();
        Entries.push_back(std::move(amountEntry));

        auto fresnelEntry = std::make_unique<FloatMenuEntry>();
        fresnelEntry->SetText("fresnel");
        fresnel_ = fresnelEntry.get();
        Entries.push_back(std::move(fresnelEntry));

        auto specularEntry = std::make_unique<FloatMenuEntry>();
        specularEntry->SetText("specular");
        specular_ = specularEntry.get();
        Entries.push_back(std::move(specularEntry));

        auto backEntry = std::make_unique<MenuEntry>();
        backEntry->SetText("back");
        backEntry->Clicked = [&game]() { game.SetActiveMenu(0); };
        Entries.push_back(std::move(backEntry));
    }

    const std::string& GetTypeName() const override {
        static const std::string name = "EnvmapDemo";
        return name;
    }

    void Reset() override {
        amount_->Value = 1.0f;
        fresnel_->Value = 0.25f;
        specular_->Value = 0.5f;

        MenuComponent::Reset();
    }

    void LoadContent() override {
        background_ = GetGame().getContentProperty().Load<Texture2D>("background");
        model_ = GetGame().getContentProperty().Load<Model>("saucer");
    }

    void Draw(const GameTime& gameTime) override {
        getGraphicsDeviceProperty().Clear(Color::Black);

        GetSpriteBatch().Begin(SpriteSortMode::Deferred, BlendState::Opaque);
        GetSpriteBatch().Draw(
            background_.value(), Rectangle(0, 0, 480, 800), Color::White);
        GetSpriteBatch().End();

        DrawTitle("environment map effect", std::optional<Color>(),
                  Color(93, 142, 196, 255));

        float time = static_cast<float>(
            gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());

        Matrix rotation = Matrix::CreateRotationX(time * 0.3f) *
                          Matrix::CreateRotationY(time);
        Matrix view = Matrix::CreateLookAt(
            Vector3(4500.0f, -400.0f, 0.0f),
            Vector3(0.0f, -400.0f, 0.0f), Vector3::Up);
        Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(),
            10.0f, 10000.0f);

        std::vector<Matrix> transforms(static_cast<std::size_t>(
            model_->getBonesProperty().getCountProperty()));
        model_->CopyAbsoluteBoneTransformsTo(transforms);

        getGraphicsDeviceProperty().setBlendStateProperty(BlendState::Opaque);
        getGraphicsDeviceProperty().setRasterizerStateProperty(
            RasterizerState::CullCounterClockwise);
        getGraphicsDeviceProperty().setDepthStencilStateProperty(
            DepthStencilState::Default);
        getGraphicsDeviceProperty().getSamplerStatesProperty()[0] =
            SamplerState::LinearWrap;

        for (ModelMesh* mesh : model_->getMeshesProperty()) {
            for (Effect* effect : mesh->getEffectsPropertyMutable()) {
                auto* environmentMapEffect =
                    dynamic_cast<EnvironmentMapEffect*>(effect);
                if (!environmentMapEffect) {
                    throw std::runtime_error(
                        "Saucer model contains a non-EnvironmentMapEffect mesh.");
                }

                Matrix world = transforms[static_cast<std::size_t>(
                    mesh->getParentBoneProperty()->getIndexProperty())] * rotation;
                environmentMapEffect->setWorldProperty(world);
                environmentMapEffect->setViewProperty(view);
                environmentMapEffect->setProjectionProperty(projection);
                environmentMapEffect->EnableDefaultLighting();
                environmentMapEffect->setEnvironmentMapAmountProperty(amount_->Value);
                environmentMapEffect->setFresnelFactorProperty(fresnel_->Value * 2.0f);
                environmentMapEffect->setEnvironmentMapSpecularProperty(
                    Vector3(1.0f, 1.0f, 0.5f) * specular_->Value);
            }

            mesh->Draw();
        }

        MenuComponent::Draw(gameTime);
    }

private:
    std::optional<Model> model_;
    std::optional<Texture2D> background_;
    FloatMenuEntry* amount_ = nullptr;
    FloatMenuEntry* fresnel_ = nullptr;
    FloatMenuEntry* specular_ = nullptr;
};

} // namespace ReachGraphicsDemoSample
