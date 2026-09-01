// SPDX-License-Identifier: MS-PL
#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Blend.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendFunction.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/CompareFunction.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexElementSize.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "System/ArgumentOutOfRangeException.hpp"
#include "System/IDisposable.hpp"

#include "VertexPositionNormal.hpp"

namespace TiltPerspectiveSample {

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

class GeometricPrimitive : public System::IDisposable {
public:
    ~GeometricPrimitive() override {
        Dispose(false);
    }

    void Dispose() override {
        Dispose(true);
    }

    // Matches the original's `LightDirection` setter -- overrides
    // basicEffect's DirectionalLight0.Direction. Called once per frame by
    // BallSimulation::Draw() before drawing any balls.
    void setLightDirectionProperty(Vector3 value) {
        basicEffect_->getDirectionalLight0Property().setDirectionProperty(Vector3::Normalize(value));
    }

    // Draws the primitive using a caller-supplied effect, with no render
    // state changes (matches `Draw(BasicEffect effect)`).
    void Draw(BasicEffect& effect) {
        GraphicsDevice& device = effect.getGraphicsDeviceInternal();

        device.SetVertexBuffer(vertexBuffer_.get());
        device.setIndicesProperty(indexBuffer_.get());

        int primitiveCount = indexBuffer_->getIndexCountProperty() / 3;

        for (auto& pass : effect.getCurrentTechniqueProperty()->getPassesProperty()) {
            pass.Apply();
            device.DrawIndexedPrimitives(PrimitiveType::TriangleList, 0, 0,
                                          vertexBuffer_->getVertexCountProperty(), 0, primitiveCount);
        }
    }

    // Draws the primitive with a full BasicEffect setup (world/view/projection,
    // diffuse color/alpha, and the depth/blend states needed for opaque vs.
    // translucent geometry). Matches `Draw(Matrix, Matrix, Matrix, Color, bool)`.
    void Draw(const Matrix& world, const Matrix& view, const Matrix& projection, Color color, bool drawShadow) {
        BasicEffect& drawBasicEffect = drawShadow ? *basicEffectShadow_ : *basicEffect_;

        drawBasicEffect.World = world;
        drawBasicEffect.View = view;
        drawBasicEffect.Projection = projection;
        drawBasicEffect.setDiffuseColorProperty(color.ToVector3());
        drawBasicEffect.setAlphaProperty(color.getAProperty() / 255.0f);

        GraphicsDevice& device = basicEffect_->getGraphicsDeviceInternal();

        BlendState blendState;
        DepthStencilState depthState;

        depthState.setDepthBufferEnableProperty(true);
        depthState.setDepthBufferFunctionProperty(CompareFunction::LessEqual);

        if (color.getAProperty() < 255) {
            blendState.setAlphaBlendFunctionProperty(BlendFunction::Add);
            blendState.setAlphaSourceBlendProperty(Blend::SourceAlpha);
            blendState.setAlphaDestinationBlendProperty(Blend::InverseSourceAlpha);
            depthState.setDepthBufferWriteEnableProperty(false);
        } else {
            depthState.setDepthBufferWriteEnableProperty(true);
        }

        device.setDepthStencilStateProperty(depthState);
        device.setBlendStateProperty(blendState);

        Draw(drawBasicEffect);
    }

protected:
    void AddVertex(const Vector3& position, const Vector3& normal) {
        vertices_.emplace_back(position, normal);
    }

    void AddIndex(int index) {
        if (index < 0 || index > UINT16_MAX)
            throw System::ArgumentOutOfRangeException("index");
        indices_.push_back(static_cast<std::uint16_t>(index));
    }

    [[nodiscard]] int CurrentVertex() const { return static_cast<int>(vertices_.size()); }

    void InitializePrimitive(GraphicsDevice& device) {
        vertexBuffer_ = std::make_unique<VertexBuffer>(
            device, VertexPositionNormal::VertexDeclaration,
            static_cast<int>(vertices_.size()), BufferUsage::None);
        vertexBuffer_->SetData(vertices_.data(), static_cast<int>(vertices_.size()));

        indexBuffer_ = std::make_unique<IndexBuffer>(
            device, IndexElementSize::SixteenBits,
            static_cast<int>(indices_.size()), BufferUsage::None);
        indexBuffer_->SetData(indices_.data(), static_cast<int>(indices_.size()));

        basicEffect_ = std::make_unique<BasicEffect>(device);
        basicEffect_->setLightingEnabledProperty(true);

        basicEffect_->getDirectionalLight0Property().setEnabledProperty(true);
        Vector3 initialLightDirection(0.25f, -1.0f, -1.0f);
        initialLightDirection.Normalize();
        basicEffect_->getDirectionalLight0Property().setDirectionProperty(initialLightDirection);

        basicEffect_->setSpecularColorProperty(Vector3(1.0f, 1.0f, 1.0f));
        basicEffect_->getDirectionalLight0Property().setSpecularColorProperty(Vector3(1.0f, 1.0f, 1.0f));
        basicEffect_->setSpecularPowerProperty(32.0f);
        basicEffect_->setPreferPerPixelLightingProperty(false);

        basicEffectShadow_ = std::make_unique<BasicEffect>(device);
        basicEffectShadow_->setLightingEnabledProperty(false);
        basicEffectShadow_->setPreferPerPixelLightingProperty(false);

    }

    void Dispose(bool disposing) {
        if (isDisposed_)
            return;

        if (disposing) {
            if (vertexBuffer_)
                vertexBuffer_->Dispose();
            if (indexBuffer_)
                indexBuffer_->Dispose();
            if (basicEffect_)
                basicEffect_->Dispose();
            if (basicEffectShadow_)
                basicEffectShadow_->Dispose();
        }

        isDisposed_ = true;
    }

private:
    std::vector<VertexPositionNormal> vertices_;
    std::vector<std::uint16_t> indices_;

    std::unique_ptr<VertexBuffer> vertexBuffer_;
    std::unique_ptr<IndexBuffer> indexBuffer_;
    std::unique_ptr<BasicEffect> basicEffect_;
    std::unique_ptr<BasicEffect> basicEffectShadow_;
    bool isDisposed_ = false;
};

} // namespace TiltPerspectiveSample
