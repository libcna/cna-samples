#pragma once

// Ported from XnaGraphicsDemo.SkinnedDemo (SkinnedDemo.cs).

#include <memory>
#include <optional>
#include <stdexcept>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SkinnedEffect.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "DemoGame.hpp"
#include "MenuComponent.hpp"
#include "MenuEntry.hpp"
#include "SkinningModel.hpp"
#include "Sky.hpp"

namespace ReachGraphicsDemoSample {

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

class SkinnedDemo : public MenuComponent {
public:
    explicit SkinnedDemo(DemoGame& game) : MenuComponent(game) {
        auto backEntry = std::make_unique<MenuEntry>();
        backEntry->SetText("back");
        backEntry->Clicked = [&game]() { game.SetActiveMenu(0); };
        Entries.push_back(std::move(backEntry));
    }

    const std::string& GetTypeName() const override {
        static const std::string name = "SkinnedDemo";
        return name;
    }

    void Reset() override {
        cameraRotation_ = 0.0f;
        cameraArc_ = 0.0f;

        MenuComponent::Reset();
    }

    void LoadContent() override {
        sky_ = GetGame().getContentProperty().Load<GeneratedGeometry::Sky>("sky");
        dude_ = GetGame().getContentProperty().Load<Model>("dude");

        auto* skinningData =
            dynamic_cast<SkinnedModel::SkinningData*>(dude_->getTagProperty());
        if (!skinningData) {
            throw std::runtime_error(
                "This model does not contain a SkinningData tag.");
        }

        animationPlayer_ =
            std::make_unique<SkinnedModel::AnimationPlayer>(*skinningData);

        auto clip = skinningData->AnimationClips.find("Take 001");
        if (clip == skinningData->AnimationClips.end() || !clip->second) {
            throw std::runtime_error("Animation clip 'Take 001' is missing.");
        }
        animationPlayer_->StartClip(*clip->second);
    }

    void Update(GameTime& gameTime) override {
        animationPlayer_->Update(gameTime.getElapsedGameTimeProperty(), true,
                                 Matrix::getIdentityProperty());

        MenuComponent::Update(gameTime);
    }

    void Draw(const GameTime& gameTime) override {
        constexpr float cameraDistance = 100.0f;

        Matrix view = Matrix::CreateTranslation(0.0f, -40.0f, 0.0f) *
                      Matrix::CreateRotationY(MathHelper::ToRadians(cameraRotation_)) *
                      Matrix::CreateRotationX(MathHelper::ToRadians(cameraArc_)) *
                      Matrix::CreateLookAt(
                          Vector3(0.0f, 0.0f, -cameraDistance),
                          Vector3::Zero, Vector3::Up);
        Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(),
            1.0f, 10000.0f);

        getGraphicsDeviceProperty().Clear(Color::Black);

        sky_->Draw(view, projection);

        DrawTitle("skinned effect", std::optional<Color>(),
                  Color(127, 112, 104, 255));

        getGraphicsDeviceProperty().setBlendStateProperty(BlendState::Opaque);
        getGraphicsDeviceProperty().setRasterizerStateProperty(
            RasterizerState::CullCounterClockwise);
        getGraphicsDeviceProperty().setDepthStencilStateProperty(
            DepthStencilState::Default);
        getGraphicsDeviceProperty().getSamplerStatesProperty()[0] =
            SamplerState::LinearWrap;

        const std::vector<Matrix>& bones = animationPlayer_->GetSkinTransforms();

        for (ModelMesh* mesh : dude_->getMeshesProperty()) {
            for (Effect* effect : mesh->getEffectsPropertyMutable()) {
                auto* skinnedEffect = dynamic_cast<SkinnedEffect*>(effect);
                if (!skinnedEffect) {
                    throw std::runtime_error(
                        "Dude model contains a non-SkinnedEffect mesh.");
                }

                skinnedEffect->SetBoneTransforms(bones);
                skinnedEffect->setViewProperty(view);
                skinnedEffect->setProjectionProperty(projection);
                skinnedEffect->EnableDefaultLighting();
                skinnedEffect->setSpecularColorProperty(Vector3::Zero);
            }

            mesh->Draw();
        }

        MenuComponent::Draw(gameTime);
    }

protected:
    void OnDrag(Vector2 delta) override {
        cameraRotation_ += delta.X / 4.0f;
        cameraArc_ = MathHelper::Clamp(cameraArc_ - delta.Y / 4.0f, -70.0f, 70.0f);
    }

private:
    std::optional<GeneratedGeometry::Sky> sky_;
    std::optional<Model> dude_;
    std::unique_ptr<SkinnedModel::AnimationPlayer> animationPlayer_;
    float cameraRotation_ = 0.0f;
    float cameraArc_ = 0.0f;
};

} // namespace ReachGraphicsDemoSample
