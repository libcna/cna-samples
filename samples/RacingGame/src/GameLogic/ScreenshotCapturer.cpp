// SPDX-License-Identifier: MS-PL

#include "GameLogic/ScreenshotCapturer.hpp"

#include <cstdio>
#include <exception>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PresentationParameters.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageContainer.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageDevice.hpp"
#include "RacingGameManager.hpp"
#include "Helpers/Log.hpp"
#include "System/IO/Stream.hpp"

namespace RacingGame::GameLogic
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::GameComponent;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::Texture2D;
    using namespace Microsoft::Xna::Framework::Storage;

    namespace
    {
        std::unique_ptr<StorageContainer> OpenContainer()
        {
            auto selection = StorageDevice::BeginShowSelector(
                PlayerIndex::One, nullptr, nullptr);
            auto device = StorageDevice::EndShowSelector(selection.get());
            if (!device || !device->getIsConnectedProperty()) return nullptr;
            auto operation = device->BeginOpenContainer(
                "RacingGame", nullptr, nullptr);
            return device->EndOpenContainer(operation.get());
        }
    }

    ScreenshotCapturer::ScreenshotCapturer(RacingGameManager& setGame)
        : GameComponent(setGame), game(&setGame),
          screenshotNum(GetCurrentScreenshotNum())
    {
    }

    void ScreenshotCapturer::Update(GameTime& gameTime)
    {
        if (game->getControlsProperty().printScreenJustPressed)
            MakeScreenshot();
        GameComponent::Update(gameTime);
    }

    int ScreenshotCapturer::getScreenshotNumberProperty() const
    {
        return screenshotNum;
    }

    const std::string& ScreenshotCapturer::GetTypeName() const
    {
        static const std::string name =
            "RacingGame.GameLogic.ScreenshotCapturer";
        return name;
    }

    std::string ScreenshotCapturer::ScreenshotNameBuilder(
        const int number) const
    {
        char digits[16]{};
        std::snprintf(digits, sizeof(digits), "%04d", number);
        return "Screenshots/" + game->getWindowProperty().getTitleProperty() +
               " Screenshot " + digits + ".jpg";
    }

    int ScreenshotCapturer::GetCurrentScreenshotNum() const
    {
        try
        {
            auto container = OpenContainer();
            if (!container) return -1;
            int i = 0;
            int j = 0;
            int k = 0;
            int l = -1;
            if (container->FileExists(ScreenshotNameBuilder(0)))
            {
                for (i = 1; i < 10; ++i)
                    if (!container->FileExists(ScreenshotNameBuilder(i * 1000)))
                        break;
                --i;
                for (j = 1; j < 10; ++j)
                    if (!container->FileExists(
                            ScreenshotNameBuilder(i * 1000 + j * 100)))
                        break;
                --j;
                for (k = 1; k < 10; ++k)
                    if (!container->FileExists(ScreenshotNameBuilder(
                            i * 1000 + j * 100 + k * 10)))
                        break;
                --k;
                for (l = 1; l < 10; ++l)
                    if (!container->FileExists(ScreenshotNameBuilder(
                            i * 1000 + j * 100 + k * 10 + l)))
                        break;
                --l;
            }
            container->Dispose();
            return i * 1000 + j * 100 + k * 10 + l;
        }
        catch (...)
        {
            return -1;
        }
    }

    void ScreenshotCapturer::MakeScreenshot()
    {
        try
        {
            ++screenshotNum;
            auto container = OpenContainer();
            if (!container) return;
            if (!container->DirectoryExists("Screenshots"))
                container->CreateDirectory("Screenshots");

            GraphicsDevice& device = game->getGraphicsDeviceProperty();
            const auto& presentation =
                device.getPresentationParametersProperty();
            const int width = presentation.getBackBufferWidthProperty();
            const int height = presentation.getBackBufferHeightProperty();
            std::vector<Color> backbuffer(
                static_cast<std::size_t>(width * height));
            device.GetBackBufferData(
                backbuffer.data(), static_cast<int>(backbuffer.size()));
            Texture2D texture(
                device, width, height, false,
                presentation.getBackBufferFormatProperty());
            texture.SetData(
                backbuffer.data(), static_cast<int>(backbuffer.size()));
            auto stream = container->CreateFile(
                ScreenshotNameBuilder(screenshotNum));
            texture.SaveAsJpeg(stream.get(), width, height);
            stream->Close();
            container->Dispose();
        }
        catch (const std::exception& exception)
        {
            Helpers::Log::Write(
                std::string("Failed to save Screenshot: ") +
                exception.what());
        }
        catch (...)
        {
            Helpers::Log::Write("Failed to save Screenshot: unknown error");
        }
    }
}
