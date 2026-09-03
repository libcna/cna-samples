// SPDX-License-Identifier: MS-PL

#include "Shaders/RenderToTexture.hpp"

#include <vector>

#include "Microsoft/Xna/Framework/Graphics/ClearOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsAdapter.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PresentationParameters.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTargetBinding.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTargetUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "System/InvalidOperationException.hpp"

namespace RacingGame::Shaders
{
    using Microsoft::Xna::Framework::Color;
    using namespace Microsoft::Xna::Framework::Graphics;

    RenderToTexture::RenderToTexture(
        GraphicsDevice& setDevice, const SizeType setSizeType)
        : device(setDevice), sizeType(setSizeType)
    {
        Create();
    }

    RenderTarget2D& RenderToTexture::getRenderTargetProperty() const
    {
        return *renderTarget;
    }

    Texture2D& RenderToTexture::getXnaTextureProperty() const
    {
        return *renderTarget;
    }

    bool RenderToTexture::getUsesHighPrecisionFormatProperty() const
    {
        return usesHighPrecisionFormat;
    }

    int RenderToTexture::getWidthProperty() const
    {
        return renderTarget->getWidthProperty();
    }

    int RenderToTexture::getHeightProperty() const
    {
        return renderTarget->getHeightProperty();
    }

    void RenderToTexture::HandleDeviceReset()
    {
        renderTarget.reset();
        Create();
    }

    void RenderToTexture::Clear(const Color clearColor)
    {
        device.Clear(ClearOptions::Target | ClearOptions::DepthBuffer,
                     clearColor, 1.0f, 0);
    }

    bool RenderToTexture::SetRenderTarget()
    {
        device.SetRenderTarget(renderTarget.get());
        return true;
    }

    void RenderToTexture::Resolve()
    {
        const std::vector<RenderTargetBinding> bindings =
            device.GetRenderTargets();
        if (bindings.size() != 1 ||
            bindings.front().getRenderTargetProperty() != renderTarget.get())
        {
            throw System::InvalidOperationException(
                "You can't call Resolve without first setting the render target!");
        }
        device.SetRenderTarget(static_cast<RenderTarget2D*>(nullptr));
    }

    void RenderToTexture::Create()
    {
        int width = device.getViewportProperty().getWidthProperty();
        int height = device.getViewportProperty().getHeightProperty();
        switch (sizeType)
        {
        case SizeType::FullScreen:
            break;
        case SizeType::HalfScreen:
            width /= 2;
            height /= 2;
            break;
        case SizeType::QuarterScreen:
            width /= 4;
            height /= 4;
            break;
        case SizeType::ShadowMap:
            width = 2048;
            height = 2048;
            break;
        }

        SurfaceFormat selectedFormat = SurfaceFormat::Color;
        DepthFormat selectedDepth = DepthFormat::None;
        int selectedMultiSampleCount = 0;
        const DepthFormat preferredDepth =
            device.getPresentationParametersProperty()
                .getDepthStencilFormatProperty();
        [[maybe_unused]] const bool exactFormat =
            device.getAdapterProperty().QueryRenderTargetFormat(
                device.getGraphicsProfileProperty(), SurfaceFormat::Rgba64,
                preferredDepth, 0, selectedFormat, selectedDepth,
                selectedMultiSampleCount);
        renderTarget = std::make_unique<RenderTarget2D>(
            device, width, height, false, selectedFormat, selectedDepth,
            sizeType == SizeType::ShadowMap ? 0 : selectedMultiSampleCount,
            RenderTargetUsage::DiscardContents);
        usesHighPrecisionFormat = selectedFormat != SurfaceFormat::Color;
    }
}
