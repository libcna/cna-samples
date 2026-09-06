// SPDX-License-Identifier: MS-PL
#pragma once

// BlurManager.hpp — C++ port of ShipGame/Graphics/BlurManager.cs (XNA 4.0 Ship
// Game Starter Kit). The screen-aligned quad the glow pipeline blurs through,
// driven by Blur.fx.

#include <memory>
#include <optional>
#include <vector>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionTexture.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/IDisposable.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Vector4;
using Microsoft::Xna::Framework::Graphics::BufferUsage;
using Microsoft::Xna::Framework::Graphics::Effect;
using Microsoft::Xna::Framework::Graphics::EffectParameter;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::VertexBuffer;
using Microsoft::Xna::Framework::Graphics::VertexDeclaration;
using Microsoft::Xna::Framework::Graphics::VertexPositionTexture;

// supported render techniques. Port of the BlurTechnique enum in BlurManager.cs.
enum class BlurTechnique {
    Color = 0,           // plain color
    ColorTexture,        // plain texture mapping
    BlurHorizontal,      // horizontal blur
    BlurVertical,        // vertical blur
    BlurHorizontalSplit, // horizontal split screen blur
};

// Port of ShipGame/Graphics/BlurManager.cs.
class BlurManager : public System::IDisposable {
public:
    // Create a new blur manager
    BlurManager(GraphicsDevice* gd, std::shared_ptr<Effect> effect, int sizex, int sizey) {
        if (gd == nullptr) {
            throw System::ArgumentNullException("gd");
        }
        if (effect == nullptr) {
            throw System::ArgumentNullException("effect");
        }

        blurEffect_ = std::move(effect); // save effect
        // The two are swapped in the original: sizeX takes the vertical argument and sizeY the
        // horizontal one. Reproduced -- every caller passes a square buffer, so it never shows.
        sizeX_ = sizey; // save horizontal buffer size
        sizeY_ = sizex; // save verical buffer size

        // get effect parameters
        paramWorldViewProjection_ = blurEffect_->getParametersProperty()["g_WorldViewProj"];
        paramColorMap_ = blurEffect_->getParametersProperty()["g_ColorMap"];
        paramColor_ = blurEffect_->getParametersProperty()["g_Color"];
        paramPixelSize_ = blurEffect_->getParametersProperty()["g_PixelSize"];

        pixelSize_ = Vector2(1.0f / sizeX_, 1.0f / sizeY_);
        viewProjection_ = Matrix::CreateOrthographicOffCenter(0, (float)sizeX_, 0, (float)sizeY_,
                                                              -1, 1);

        // create vertex buffer
        vertexBuffer_.emplace(*gd, VertexPositionTexture::getVertexDeclarationStatic(), 6,
                              BufferUsage::WriteOnly);

        // create vertex declaration
        vertexDeclaration_.emplace(
            VertexPositionTexture::getVertexDeclarationStatic().GetVertexElements());

        // create vertex data
        SetVertexData();
    }

    ~BlurManager() override { Dispose(true); }

    // Set vertex data with textureCube vertex normals (used for cubemap blur option only)
    void SetVertexData() {
        std::vector<VertexPositionTexture> data(6);

        data[0] = VertexPositionTexture(Vector3(0, 0, 0), Vector2(0, 1));
        data[1] = VertexPositionTexture(Vector3((float)sizeX_, (float)sizeY_, 0), Vector2(1, 0));
        data[2] = VertexPositionTexture(Vector3((float)sizeX_, 0, 0), Vector2(1, 1));
        data[3] = VertexPositionTexture(Vector3(0, 0, 0), Vector2(0, 1));
        data[4] = VertexPositionTexture(Vector3(0, (float)sizeY_, 0), Vector2(0, 0));
        data[5] = VertexPositionTexture(Vector3((float)sizeX_, (float)sizeY_, 0), Vector2(1, 0));

        vertexBuffer_->SetData(data.data(), 6);
    }

    // Render a screen aligned quad used to process
    // the horizontal and vertical blur operations
    void RenderScreenQuad(GraphicsDevice* gd, BlurTechnique technique, Texture2D* texture,
                          Vector4 color) {
        if (gd == nullptr) {
            throw System::ArgumentNullException("gd");
        }

        gd->SetVertexBuffer(&vertexBuffer_.value());

        blurEffect_->setCurrentTechniqueProperty(
            &blurEffect_->getTechniquesProperty()[(int)technique]);

        paramWorldViewProjection_->SetValue(viewProjection_);
        paramPixelSize_->SetValue(pixelSize_);
        paramColorMap_->SetValue(texture);
        paramColor_->SetValue(color);

        blurEffect_->getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();
        gd->DrawPrimitives(PrimitiveType::TriangleList, 0, 2);

        gd->SetVertexBuffer(nullptr);
    }

    // Render a screen aligned quad used to process
    // the horizontal and vertical blur operations
    void RenderScreenQuad(GraphicsDevice* gd, BlurTechnique technique, Texture2D* texture,
                          Vector4 color, float scale) {
        if (gd == nullptr) {
            throw System::ArgumentNullException("gd");
        }

        gd->SetVertexBuffer(&vertexBuffer_.value());

        blurEffect_->setCurrentTechniqueProperty(
            &blurEffect_->getTechniquesProperty()[(int)technique]);

        Matrix m = Matrix::CreateTranslation((float)(-sizeX_ / 2), (float)(-sizeY_ / 2), 0) *
                   Matrix::CreateScale(scale, scale, 1) *
                   Matrix::CreateTranslation((float)(sizeX_ / 2), (float)(sizeY_ / 2), 0);

        paramWorldViewProjection_->SetValue(m * viewProjection_);
        paramPixelSize_->SetValue(pixelSize_);
        paramColorMap_->SetValue(texture);
        paramColor_->SetValue(color);

        blurEffect_->getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();
        gd->DrawPrimitives(PrimitiveType::TriangleList, 0, 2);

        gd->SetVertexBuffer(nullptr);
    }

    // ---- IDisposable Members ----

    bool IsDisposed() const { return isDisposed_; }

    void Dispose() override { Dispose(true); }

private:
    void Dispose(bool disposing) {
        if (disposing && !isDisposed_) {
            vertexBuffer_.reset();
            vertexDeclaration_.reset();
        }
    }

    bool isDisposed_ = false;

    // blur effect
    std::shared_ptr<Effect> blurEffect_;

    // screen quad vertex declaration and buffer
    std::optional<VertexDeclaration> vertexDeclaration_;
    std::optional<VertexBuffer> vertexBuffer_;

    // render target resolution
    int sizeX_ = 0;
    int sizeY_ = 0;

    // normalized pixel size (1.0/size)
    Vector2 pixelSize_;

    // 2D ortho view projection matrix
    Matrix viewProjection_;

    // parameters
    EffectParameter* paramWorldViewProjection_ = nullptr; // world * view * proj matrix
    EffectParameter* paramColorMap_ = nullptr;            // color texture
    EffectParameter* paramColor_ = nullptr;               // color
    EffectParameter* paramPixelSize_ = nullptr;           // pixel size
};

} // namespace ShipGame
