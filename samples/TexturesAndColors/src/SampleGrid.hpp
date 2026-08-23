#pragma once

#include <memory>
#include <vector>

#include <Microsoft/Xna/Framework/Color.hpp>
#include <Microsoft/Xna/Framework/Graphics/BasicEffect.hpp>
#include <Microsoft/Xna/Framework/Graphics/BufferUsage.hpp>
#include <Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp>
#include <Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp>
#include <Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp>
#include <Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp>
#include <Microsoft/Xna/Framework/Matrix.hpp>
#include <Microsoft/Xna/Framework/Vector3.hpp>

namespace TexturesAndColorsSample {

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

class SampleGrid {
public:
  SampleGrid() = default;

  ~SampleGrid() { UnloadGraphicsContent(); }

  SampleGrid(const SampleGrid &) = delete;
  SampleGrid &operator=(const SampleGrid &) = delete;

  void UnloadGraphicsContent() {
    if (vertexBuffer_) {
      vertexBuffer_->Dispose();
      vertexBuffer_.reset();
    }
    if (effect_) {
      effect_->Dispose();
      effect_.reset();
    }
  }

  void LoadGraphicsContent(GraphicsDevice &graphicsDevice) {
    device_ = &graphicsDevice;
    effect_ = std::make_unique<BasicEffect>(*device_);

    const int gridSize1 = GridSize + 1;
    primitiveCount_ = gridSize1 * 2;
    vertexCount_ = primitiveCount_ * 2;

    std::vector<VertexPositionColor> vertices(
        static_cast<std::size_t>(vertexCount_));

    const float length = static_cast<float>(GridSize) * GridScale;
    const float halfLength = length * 0.5f;

    int index = 0;
    for (int i = 0; i < gridSize1; ++i) {
      vertices[static_cast<std::size_t>(index++)] = VertexPositionColor(
          Vector3(-halfLength, 0.0f, i * GridScale - halfLength), GridColor);
      vertices[static_cast<std::size_t>(index++)] = VertexPositionColor(
          Vector3(halfLength, 0.0f, i * GridScale - halfLength), GridColor);
      vertices[static_cast<std::size_t>(index++)] = VertexPositionColor(
          Vector3(i * GridScale - halfLength, 0.0f, -halfLength), GridColor);
      vertices[static_cast<std::size_t>(index++)] = VertexPositionColor(
          Vector3(i * GridScale - halfLength, 0.0f, halfLength), GridColor);
    }

    vertexBuffer_ = std::make_unique<VertexBuffer>(
        *device_, VertexPositionColor::getVertexDeclarationStatic(),
        vertexCount_, BufferUsage::WriteOnly);
    vertexBuffer_->SetData(vertices.data(), vertexCount_);
  }

  void Draw() {
    effect_->World = WorldMatrix;
    effect_->View = ViewMatrix;
    effect_->Projection = ProjectionMatrix;
    effect_->VertexColorEnabled = true;
    effect_->setLightingEnabledProperty(false);

    device_->SetVertexBuffer(vertexBuffer_.get());

    for (auto &pass :
         effect_->getCurrentTechniqueProperty()->getPassesProperty()) {
      pass.Apply();
      device_->DrawPrimitives(PrimitiveType::LineList, 0, primitiveCount_);
    }
  }

  Color GridColor = Color::White;
  int GridSize = 16;
  float GridScale = 32.0f;
  Matrix ProjectionMatrix = Matrix::getIdentityProperty();
  Matrix WorldMatrix = Matrix::getIdentityProperty();
  Matrix ViewMatrix = Matrix::getIdentityProperty();

private:
  bool isDisposed_ = false;
  std::unique_ptr<VertexBuffer> vertexBuffer_;
  int vertexCount_ = 0;
  int primitiveCount_ = 0;
  std::unique_ptr<BasicEffect> effect_;
  GraphicsDevice *device_ = nullptr;
};

} // namespace TexturesAndColorsSample
