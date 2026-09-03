// SPDX-License-Identifier: MS-PL

#include <cstdio>
#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "GameLogic/Input.hpp"
#include "Graphics/Model.hpp"
#include "Helpers/Log.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageContainer.hpp"
#include "Microsoft/Xna/Framework/Storage/StorageDevice.hpp"
#include "RacingGameManager.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/IO/Stream.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFile.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFileStream.hpp"

namespace
{
    class DesktopControls final : public RacingGame::GameLogic::ControlSource
    {
    public:
        RacingGame::GameLogic::ControlFrame Capture(
            bool, bool, int, int) override
        {
            RacingGame::GameLogic::ControlFrame result;
            if (frame == 8) result.printScreenJustPressed = true;
            if (frame == 10) result.f1JustPressed = true;
            if (frame == 11)
            {
                observedF1Toggle = game && !game->getShowFpsProperty();
                result.gamePadLeftShoulderPressed = true;
                result.gamePadYJustPressed = true;
            }
            if (frame == 12)
                observedGamePadToggle = game && game->getShowFpsProperty();
            ++frame;
            return result;
        }

        void SetGame(RacingGame::RacingGameManager& value)
        {
            game = &value;
        }

        [[nodiscard]] bool ObservedF1Toggle() const
        {
            return observedF1Toggle;
        }

        [[nodiscard]] bool ObservedGamePadToggle() const
        {
            return observedGamePadToggle;
        }

    private:
        RacingGame::RacingGameManager* game = nullptr;
        int frame = 0;
        bool observedF1Toggle = false;
        bool observedGamePadToggle = false;
    };

    bool Check(const bool condition, const char* label)
    {
        std::printf("[%s] %s\n", condition ? "PASS" : "FAIL", label);
        return condition;
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::fprintf(stderr,
                     "usage: RacingGameDesktopRuntimeProbe CONTENT_ROOT\n");
        return 2;
    }
    try
    {
        constexpr const char* StorageApp =
            "RacingGameDesktopRuntimeQualificationV1";
        using Microsoft::Xna::Framework::PlayerIndex;
        using namespace Microsoft::Xna::Framework::Storage;
        StorageDevice::SetAppNameEXT(StorageApp);
        auto logStore = System::IO::IsolatedStorage::IsolatedStorageFile::
            GetUserStoreForDomain();
        if (logStore.FileExists("Log.txt"))
            logStore.DeleteFile("Log.txt");
        auto selection = StorageDevice::BeginShowSelector(
            PlayerIndex::One, nullptr, nullptr);
        auto device = StorageDevice::EndShowSelector(selection.get());
        device->DeleteContainer("RacingGame");

        auto controls = std::make_unique<DesktopControls>();
        DesktopControls* observedControls = controls.get();
        RacingGame::RacingRunConfiguration configuration;
        configuration.contentRoot = argv[1];
        configuration.frameLimit = 30;
        configuration.elapsedMillisecondsOverride = 1100.0f;
        configuration.storageAppName = StorageApp;
        configuration.honorDisplaySettings = false;
        configuration.loadingReadyDelayMilliseconds = 1000000000.0f;
        RacingGame::RacingGameManager game(
            std::move(controls), std::move(configuration));
        observedControls->SetGame(game);
        game.Run();

        bool passed = true;
#ifndef NDEBUG
        passed = Check(observedControls->ObservedF1Toggle(),
                       "F1 toggled the original debug FPS overlay off") && passed;
        passed = Check(observedControls->ObservedGamePadToggle(),
                       "LeftShoulder plus Y toggled the FPS overlay on") && passed;
#else
        passed = Check(!game.getShowFpsProperty(),
                       "Release keeps the debug-only FPS overlay disabled") && passed;
#endif
        passed = Check(game.getFpsProperty() == 0 &&
                           game.getFpsInterpolatedProperty() < 5.0f &&
                           game.getTotalFrameCountProperty() == 30,
                       "original one-second FPS counters reached the low-FPS tier") && passed;
        passed = Check(
            RacingGame::Graphics::Model::getMaxViewDistanceProperty() == 50,
            "low FPS reduced the original model view distance to 50") && passed;
        RacingGame::Graphics::Model::setMaxViewDistanceProperty(175);
        passed = Check(
            RacingGame::Graphics::Model::getMaxViewDistanceProperty() == 50,
            "adaptive model view distance never increases again") && passed;
        passed = Check(game.getScreenshotNumberProperty() == 0,
                       "PrintScreen allocated screenshot 0000") && passed;
        RacingGame::Helpers::Log::Write("desktop runtime probe");
        passed = Check(logStore.FileExists("Log.txt"),
                       "original isolated-storage Log.txt was created") && passed;
        if (logStore.FileExists("Log.txt"))
        {
            auto stream = logStore.OpenFile(
                "Log.txt", System::IO::FileMode::Open);
            std::vector<SharpRuntime::bytecs> bytes(
                static_cast<std::size_t>(stream.getLengthProperty()));
            const int read = stream.Read(
                bytes.data(), 0, static_cast<int>(bytes.size()));
            stream.Close();
            const std::string text(bytes.begin(), bytes.end());
            passed = Check(
                read == static_cast<int>(bytes.size()) &&
                    text.find("/// Session started at: ") != std::string::npos &&
                    text.find("/// RacingGame") != std::string::npos &&
                    text.find("desktop runtime probe") != std::string::npos,
                "isolated log preserves the original session and entry shape") && passed;
        }

        auto open = device->BeginOpenContainer("RacingGame", nullptr, nullptr);
        auto container = device->EndOpenContainer(open.get());
        constexpr const char* Screenshot =
            "Screenshots/Racing Game Screenshot 0000.jpg";
        passed = Check(container->FileExists(Screenshot),
                       "PrintScreen wrote the JPEG through XNA storage") && passed;
        if (container->FileExists(Screenshot))
        {
            auto stream = container->OpenFile(
                Screenshot, System::IO::FileMode::Open,
                System::IO::FileAccess::Read);
            SharpRuntime::bytecs signature[2]{};
            const int read = stream->Read(signature, 0, 2);
            passed = Check(read == 2 && signature[0] == 0xff &&
                               signature[1] == 0xd8 &&
                               stream->getLengthProperty() > 1024,
                           "stored screenshot has a non-empty JPEG payload") && passed;
            stream->Close();
        }
        container->Dispose();
        game.Dispose();
        std::printf("=== Racing Desktop Runtime: %s ===\n",
                    passed ? "PASS" : "FAIL");
        return passed ? 0 : 1;
    }
    catch (const std::exception& exception)
    {
        std::fprintf(stderr, "[FAIL] desktop runtime: %s\n",
                     exception.what());
        return 1;
    }
}
