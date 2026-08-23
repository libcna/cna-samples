#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <Microsoft/Xna/Framework/Color.hpp>
#include <Microsoft/Xna/Framework/Game.hpp>
#include <Microsoft/Xna/Framework/GameTime.hpp>
#include <Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp>
#include <Microsoft/Xna/Framework/Graphics/Effect.hpp>
#include <Microsoft/Xna/Framework/Graphics/EffectParameter.hpp>
#include <Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp>
#include <Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp>
#include <Microsoft/Xna/Framework/Graphics/Model.hpp>
#include <Microsoft/Xna/Framework/Graphics/ModelMesh.hpp>
#include <Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp>
#include <Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp>
#include <Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp>
#include <Microsoft/Xna/Framework/Graphics/SpriteFont.hpp>
#include <Microsoft/Xna/Framework/Graphics/Texture2D.hpp>
#include <Microsoft/Xna/Framework/GraphicsDeviceManager.hpp>
#include <Microsoft/Xna/Framework/Input/GamePad.hpp>
#include <Microsoft/Xna/Framework/Input/GamePadState.hpp>
#include <Microsoft/Xna/Framework/Input/Keyboard.hpp>
#include <Microsoft/Xna/Framework/Input/KeyboardState.hpp>
#include <Microsoft/Xna/Framework/Input/Keys.hpp>
#include <Microsoft/Xna/Framework/MathHelper.hpp>
#include <Microsoft/Xna/Framework/Matrix.hpp>
#include <Microsoft/Xna/Framework/Vector2.hpp>
#include <Microsoft/Xna/Framework/Vector3.hpp>
#include <Microsoft/Xna/Framework/Vector4.hpp>

#include "SampleCamera.hpp"
#include "SampleGrid.hpp"

namespace TexturesAndColorsSample {

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;
using namespace Microsoft::Xna::Framework::Input;

class TexturesAndColors : public Game {
public:
  TexturesAndColors()
      : graphics_(this), camera_(SampleArcBallCameraMode::RollConstrained) {
    getContentProperty().setRootDirectoryProperty("Content");
  }

  [[nodiscard]] const std::string &GetTypeName() const override {
    static const std::string name = "TexturesAndColorsSample.TexturesAndColors";
    return name;
  }

  void DrawSampleMesh(Model &sampleMesh) {
    // The C++ reference cannot be null, so the C# null guard has no equivalent
    // here.
    ModelMesh *mesh = sampleMesh.getMeshesProperty()[0];
    ModelMeshPart *meshPart = mesh->getMeshPartsProperty()[0];

    graphics_.getGraphicsDeviceProperty()->SetVertexBuffer(
        meshPart->getVertexBufferProperty(),
        meshPart->getVertexOffsetProperty());
    graphics_.getGraphicsDeviceProperty()->setIndicesProperty(
        meshPart->getIndexBufferProperty());

    effect_->setCurrentTechniqueProperty(
        &effect_->getTechniquesProperty()[activeTechnique_]);

    for (auto &pass :
         effect_->getCurrentTechniqueProperty()->getPassesProperty()) {
      pass.Apply();
      graphics_.getGraphicsDeviceProperty()->DrawIndexedPrimitives(
          PrimitiveType::TriangleList, 0, 0, meshPart->getNumVerticesProperty(),
          meshPart->getStartIndexProperty(),
          meshPart->getPrimitiveCountProperty());
    }
  }

protected:
  void LoadContent() override {
    GraphicsDevice &device = *graphics_.getGraphicsDeviceProperty();

    grid_.GridColor = Color::LimeGreen;
    grid_.GridScale = 1.0f;
    grid_.GridSize = 32;
    grid_.LoadGraphicsContent(device);

    camera_.setDistanceProperty(3.0f);
    camera_.OrbitRight(MathHelper::Pi);
    camera_.OrbitUp(0.2f);

    sampleMeshes_.reserve(5);
    sampleMeshes_.push_back(getContentProperty().Load<Model>("Cube"));
    sampleMeshes_.push_back(getContentProperty().Load<Model>("SphereHighPoly"));
    sampleMeshes_.push_back(getContentProperty().Load<Model>("SphereLowPoly"));
    sampleMeshes_.push_back(getContentProperty().Load<Model>("Cylinder"));
    sampleMeshes_.push_back(getContentProperty().Load<Model>("Cone"));

    modelTexture_.emplace(getContentProperty().Load<Texture2D>("Clouds"));

    effect_ =
        getContentProperty().Load<std::shared_ptr<Effect>>("TexturesAndColors");

    worldParameter_ = effect_->getParametersProperty()["world"];
    viewParameter_ = effect_->getParametersProperty()["view"];
    projectionParameter_ = effect_->getParametersProperty()["projection"];
    lightColorParameter_ = effect_->getParametersProperty()["lightColor"];
    lightDirectionParameter_ =
        effect_->getParametersProperty()["lightDirection"];
    ambientColorParameter_ = effect_->getParametersProperty()["ambientColor"];
    modelTextureParameter_ = effect_->getParametersProperty()["modelTexture"];

    spriteBatch_.emplace(device);
    debugTextFont_.emplace(getContentProperty().Load<SpriteFont>("DebugText"));

    const float aspectRatio =
        static_cast<float>(device.getViewportProperty().getWidthProperty()) /
        static_cast<float>(device.getViewportProperty().getHeightProperty());
    const float fieldOfView = MathHelper::PiOver4 * aspectRatio * 3.0f / 4.0f;
    projection_ = Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio,
                                                       0.1f, 1000.0f);

    world_ = Matrix::getIdentityProperty();
    grid_.ProjectionMatrix = projection_;
    grid_.WorldMatrix = Matrix::getIdentityProperty();

    safeBounds_ = Vector2(
        static_cast<float>(device.getViewportProperty().getXProperty()) +
            static_cast<float>(
                device.getViewportProperty().getWidthProperty()) *
                0.1f,
        static_cast<float>(device.getViewportProperty().getYProperty()) +
            static_cast<float>(
                device.getViewportProperty().getHeightProperty()) *
                0.1f);
  }

  void Update(GameTime &gameTime) override {
    const GamePadState gamePadState = GamePad::GetState(PlayerIndex::One);
    const KeyboardState keyboardState = Keyboard::GetState();

    if (gamePadState.IsButtonDown(Buttons::Back) ||
        keyboardState.IsKeyDown(Keys::Escape)) {
      Exit();
    }

    camera_.HandleDefaultGamepadControls(gamePadState, gameTime);
    camera_.HandleDefaultKeyboardControls(keyboardState, gameTime);
    HandleInput(gameTime, gamePadState, keyboardState);

    diffuseLightDirection_ = Vector3(-1.0f, -1.0f, -1.0f);
    diffuseLightDirection_.Normalize();

    diffuseLightColor_ = Color::CornflowerBlue.ToVector4();
    ambientLightColor_ = Color::DarkSlateGray.ToVector4();

    view_ = camera_.getViewMatrixProperty();
    grid_.ViewMatrix = camera_.getViewMatrixProperty();

    lastGamePadState_ = gamePadState;
    lastKeyboardState_ = keyboardState;
    Game::Update(gameTime);
  }

  void Draw(const GameTime &gameTime) override {
    GraphicsDevice &device = *graphics_.getGraphicsDeviceProperty();
    device.Clear(Color::Black);
    device.setDepthStencilStateProperty(DepthStencilState::Default);

    grid_.Draw();

    projectionParameter_->SetValue(projection_);
    viewParameter_->SetValue(view_);
    worldParameter_->SetValue(world_);
    ambientColorParameter_->SetValue(ambientLightColor_);
    lightColorParameter_->SetValue(diffuseLightColor_);
    lightDirectionParameter_->SetValue(diffuseLightDirection_);
    modelTextureParameter_->SetValue(&modelTexture_.value());

    DrawSampleMesh(sampleMeshes_[static_cast<std::size_t>(activeMesh_)]);

    spriteBatch_->Begin();
    spriteBatch_->DrawString(
        debugTextFont_.value(),
        effect_->getCurrentTechniqueProperty()->getNameProperty(), safeBounds_,
        Color::White);
    spriteBatch_->End();

    Game::Draw(gameTime);
  }

private:
  void HandleInput(const GameTime &gameTime, const GamePadState &gamePadState,
                   const KeyboardState &keyboardState) {
    const float elapsedTime = static_cast<float>(
        gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());

    if ((gamePadState.IsButtonDown(Buttons::X) &&
         lastGamePadState_.IsButtonUp(Buttons::X)) ||
        (keyboardState.IsKeyDown(Keys::Tab) &&
         lastKeyboardState_.IsKeyUp(Keys::Tab))) {
      activeMesh_ = (activeMesh_ + 1) % static_cast<int>(sampleMeshes_.size());
    }

    if ((gamePadState.IsButtonDown(Buttons::Y) &&
         lastGamePadState_.IsButtonUp(Buttons::Y)) ||
        (keyboardState.IsKeyDown(Keys::Space) &&
         lastKeyboardState_.IsKeyUp(Keys::Space))) {
      activeTechnique_ = (activeTechnique_ + 1) %
                         effect_->getTechniquesProperty().getCountProperty();
    }

    const float dx = SampleArcBallCamera::ReadKeyboardAxis(
                         keyboardState, Keys::Left, Keys::Right) +
                     gamePadState.getThumbSticksProperty().getLeftProperty().X;
    const float dy = SampleArcBallCamera::ReadKeyboardAxis(
                         keyboardState, Keys::Down, Keys::Up) +
                     gamePadState.getThumbSticksProperty().getLeftProperty().Y;

    if (dx != 0.0f) {
      world_ = world_ * Matrix::CreateFromAxisAngle(camera_.getUpProperty(),
                                                    elapsedTime * dx);
    }
    if (dy != 0.0f) {
      world_ = world_ * Matrix::CreateFromAxisAngle(camera_.getRightProperty(),
                                                    elapsedTime * -dy);
    }
  }

  GraphicsDeviceManager graphics_;
  Vector2 safeBounds_;
  std::optional<SpriteBatch> spriteBatch_;
  std::optional<SpriteFont> debugTextFont_;
  SampleArcBallCamera camera_;
  std::vector<Model> sampleMeshes_;
  std::optional<Texture2D> modelTexture_;
  SampleGrid grid_;
  int activeMesh_ = 0;
  int activeTechnique_ = 0;
  GamePadState lastGamePadState_;
  KeyboardState lastKeyboardState_;

  std::shared_ptr<Effect> effect_;
  EffectParameter *projectionParameter_ = nullptr;
  EffectParameter *viewParameter_ = nullptr;
  EffectParameter *worldParameter_ = nullptr;
  EffectParameter *lightColorParameter_ = nullptr;
  EffectParameter *lightDirectionParameter_ = nullptr;
  EffectParameter *ambientColorParameter_ = nullptr;
  EffectParameter *modelTextureParameter_ = nullptr;

  Matrix world_ = Matrix::getIdentityProperty();
  Matrix view_ = Matrix::getIdentityProperty();
  Matrix projection_ = Matrix::getIdentityProperty();
  Vector3 diffuseLightDirection_;
  Vector4 diffuseLightColor_;
  Vector4 ambientLightColor_;
};

} // namespace TexturesAndColorsSample
