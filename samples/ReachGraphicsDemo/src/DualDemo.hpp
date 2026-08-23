#pragma once

// Ported from XnaGraphicsDemo.DualDemo (DualDemo.cs).

#include <optional>
#include <stdexcept>
#include <vector>

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DualTextureEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "DemoGame.hpp"
#include "MenuComponent.hpp"
#include "MenuEntry.hpp"

namespace ReachGraphicsDemoSample {

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

class DualDemo : public MenuComponent {
public:
    explicit DualDemo(DemoGame& game) : MenuComponent(game) {
        auto showTextureEntry = std::make_unique<BoolMenuEntry>("texture");
        showTexture_ = showTextureEntry.get();
        Entries.push_back(std::move(showTextureEntry));

        auto showLightmapEntry = std::make_unique<BoolMenuEntry>("light map");
        showLightmap_ = showLightmapEntry.get();
        Entries.push_back(std::move(showLightmapEntry));

        auto backEntry = std::make_unique<MenuEntry>();
        backEntry->SetText("back");
        backEntry->Clicked = [&game]() { game.SetActiveMenu(0); };
        Entries.push_back(std::move(backEntry));
    }

    const std::string& GetTypeName() const override {
        static const std::string name = "DualDemo";
        return name;
    }

    // Resets the menu state.
    void Reset() override {
        showTexture_->Value = true;
        showLightmap_->Value = true;

        cameraRotation_ = 124.0f;
        cameraArc_ = -12.0f;

        MenuComponent::Reset();
    }

    // Loads content for this demo.
    void LoadContent() override {
        model_ = GetGame().getContentProperty().Load<Model>("model");

        grey_.emplace(getGraphicsDeviceProperty(), 1, 1);
        Color grey(128, 128, 128, 255);
        grey_->SetData(&grey, 1);

    }

    // Draws the DualTextureEffect demo.
    void Draw(const GameTime& gameTime) override {
        DrawTitle("dual texture effect", Color(128, 160, 128, 255), Color(96, 128, 96, 255));

        // Compute camera matrices.
        Matrix rotation =
            Matrix::CreateRotationY(MathHelper::ToRadians(cameraRotation_)) * Matrix::CreateRotationZ(MathHelper::ToRadians(cameraArc_));

        Matrix view = Matrix::CreateLookAt(Vector3(35.0f, 13.0f, 0.0f), Vector3(0.0f, 3.0f, 0.0f), Vector3::Up);

        Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(), 2.0f, 100.0f);

        getGraphicsDeviceProperty().setBlendStateProperty(BlendState::Opaque);
        getGraphicsDeviceProperty().setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        getGraphicsDeviceProperty().setDepthStencilStateProperty(DepthStencilState::Default);
        getGraphicsDeviceProperty().getSamplerStatesProperty()[0] = SamplerState::LinearWrap;

        std::vector<Matrix> transforms(static_cast<std::size_t>(
            model_->getBonesProperty().getCountProperty()));
        model_->CopyAbsoluteBoneTransformsTo(transforms);

        for (ModelMesh* mesh : model_->getMeshesProperty()) {
            std::vector<Texture2D*> textures;

            for (Effect* effect : mesh->getEffectsPropertyMutable()) {
                auto* dualEffect = dynamic_cast<DualTextureEffect*>(effect);
                if (!dualEffect) {
                    throw std::runtime_error(
                        "Dual texture model contains a non-DualTextureEffect mesh.");
                }

                Matrix world = transforms[static_cast<std::size_t>(
                    mesh->getParentBoneProperty()->getIndexProperty())] * rotation;
                dualEffect->setWorldProperty(world);
                dualEffect->setViewProperty(view);
                dualEffect->setProjectionProperty(projection);
                dualEffect->setDiffuseColorProperty(Vector3(0.75f, 0.75f, 0.75f));

                textures.push_back(dualEffect->getTextureProperty());
                textures.push_back(dualEffect->getTexture2Property());

                if (!showTexture_->Value) {
                    dualEffect->setTextureProperty(&grey_.value());
                }
                if (!showLightmap_->Value) {
                    dualEffect->setTexture2Property(&grey_.value());
                }
            }

            mesh->Draw();

            std::size_t i = 0;
            for (Effect* effect : mesh->getEffectsPropertyMutable()) {
                auto* dualEffect = static_cast<DualTextureEffect*>(effect);
                dualEffect->setTextureProperty(textures[i++]);
                dualEffect->setTexture2Property(textures[i++]);
            }
        }

        MenuComponent::Draw(gameTime);
    }

protected:
    // Dragging on the menu background rotates the camera.
    void OnDrag(Vector2 delta) override {
        cameraRotation_ = MathHelper::Clamp(cameraRotation_ + delta.X / 8.0f, 0.0f, 180.0f);
        cameraArc_ = MathHelper::Clamp(cameraArc_ - delta.Y / 8.0f, -50.0f, 15.0f);
    }

private:
    std::optional<Model> model_;
    std::optional<Texture2D> grey_;

    BoolMenuEntry* showTexture_ = nullptr;
    BoolMenuEntry* showLightmap_ = nullptr;

    float cameraRotation_ = 0.0f;
    float cameraArc_ = 0.0f;
};

} // namespace ReachGraphicsDemoSample
