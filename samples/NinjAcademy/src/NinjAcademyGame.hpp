// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/EventArgs.hpp"
#include "System/Int32.hpp"
#include "System/IO/FileMode.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFile.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFileStream.hpp"
#include "System/IO/StreamReader.hpp"
#include "System/IO/StreamWriter.hpp"
#include "System/TimeSpan.hpp"

#include "AudioManager.hpp"
#include "ContentReaders.hpp"
#include "GameState.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/BackgroundScreen.hpp"
#include "Screens/GameplayScreen.hpp"
#include "Screens/HighScoreScreen.hpp"
#include "Screens/LoadingScreen.hpp"
#include "Screens/MainMenuScreen.hpp"

namespace NinjAcademy {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::DisplayOrientation;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GraphicsDeviceManager;
using Microsoft::Xna::Framework::GamerServices::Guide;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

class NinjAcademyGame final : public Game {
public:
    static constexpr const char* SaveFileName = NinjAcademy::SaveFileName;
    static constexpr const char* GameStateKey = NinjAcademy::GameStateKey;

    NinjAcademyGame() : graphics_(this) {
        NinjAcademyContentReaderRegistrationEXT::RegisterEXT();
        CNAEXT ScreenManager::RegisterScreenType<MainMenuScreen>();
        CNAEXT ScreenManager::RegisterScreenType<HighScoreScreen>();
        CNAEXT ScreenManager::RegisterScreenType<GameplayScreen>();

        getContentProperty().setRootDirectoryProperty("Content");
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));

        graphics_.setIsFullScreenProperty(true);
        graphics_.setSupportedOrientationsProperty(
            DisplayOrientation::LandscapeLeft | DisplayOrientation::LandscapeRight);

        // CNAEXT: this remains a touch-only game; desktop mouse events enter
        // the ordinary TouchPanel gesture path instead of changing InputState.
        CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true);

        screenManager_ = std::make_shared<ScreenManager>(*this);
        getComponentsProperty().Add(screenManager_.get());

        Activated += [this](System::Object*, const System::EventArgs&) {
            if (hasLaunched_)
                GameActivated();
            else {
                hasLaunched_ = true;
                GameLaunched();
            }
        };
        Deactivated += [this](System::Object*, const System::EventArgs&) { GameDeactivated(); };
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "NinjAcademy.NinjAcademyGame";
        return name;
    }

    static void CleanIsolatedStorage() { CleanSavedGameState(); }

protected:
    void Initialize() override {
        AudioManager::Initialize(*this);

        screenManager_->AddScreen(std::make_shared<BackgroundScreen>("titlescreenBG"), std::nullopt);
        screenManager_->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);

        Game::Initialize();
    }

    void LoadContent() override {
        Game::LoadContent();
        activatingTexture_.emplace(getContentProperty().Load<Microsoft::Xna::Framework::Graphics::Texture2D>(
            "Textures/Backgrounds/Activating"));
        loadingTexture_.emplace(getContentProperty().Load<Microsoft::Xna::Framework::Graphics::Texture2D>(
            "Textures/Backgrounds/loading"));
    }

    void Draw(const GameTime& gameTime) override {
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);
        Game::Draw(gameTime);

        auto& spriteBatch = screenManager_->getSpriteBatch();
        spriteBatch.Begin();
        CNAEXT Guide::RenderPendingKeyboardInputEXT(
            getGraphicsDeviceProperty(), spriteBatch, screenManager_->getFont(), screenManager_->getBlankTexture());
        CNAEXT Guide::RenderPendingMessageBoxEXT(
            getGraphicsDeviceProperty(), spriteBatch, screenManager_->getFont(), screenManager_->getBlankTexture());
        spriteBatch.End();
    }

private:
    void GameActivated() {
        LoadStateFromIsolatedStorage();

        if (!CurrentGameState().has_value()) {
            AudioManager::PlayMusic("Menu Music");
            return;
        }

        for (auto& screen : screenManager_->GetScreens())
            screen->ExitScreen();

        screenManager_->AddScreen(std::make_shared<BackgroundScreen>("Resuming"), std::nullopt);
        screenManager_->AddScreen(
            std::make_shared<LoadingScreen>(*activatingTexture_, *loadingTexture_), std::nullopt);
    }

    void GameDeactivated() { SaveStateToIsolatedStorage(); }

    void GameLaunched() {
        AudioManager::PlayMusic("Menu Music");
        LoadStateFromIsolatedStorage();
    }

    void SaveStateToIsolatedStorage() {
        GameplayScreen* gameplayScreen = GetGameplayScreen();
        if (gameplayScreen == nullptr) {
            CleanIsolatedStorage();
            return;
        }

        auto storage = System::IO::IsolatedStorage::IsolatedStorageFile::GetUserStoreForApplication();
        auto stream = storage.CreateFile(SaveFileName);
        System::IO::StreamWriter writer(&stream, true);
        writer.WriteLine(System::Int32::ToString(gameplayScreen->Score()));
        writer.WriteLine(System::Int32::ToString(gameplayScreen->HitPoints()));
        writer.WriteLine(System::Int32::ToString(gameplayScreen->GamePhasesPassed()));
        writer.WriteLine(gameplayScreen->ElapsedPhaseTime().ToString());
        writer.Flush();
    }

    void LoadStateFromIsolatedStorage() {
        auto storage = System::IO::IsolatedStorage::IsolatedStorageFile::GetUserStoreForApplication();
        if (!storage.FileExists(SaveFileName))
            return;

        try {
            auto stream = storage.OpenFile(SaveFileName, System::IO::FileMode::Open);
            System::IO::StreamReader reader(&stream, true);

            GameState result;
            result.Score = System::Int32::Parse(reader.ReadLine());
            result.HitPoints = System::Int32::Parse(reader.ReadLine());
            result.GamePhasesPassed = System::Int32::Parse(reader.ReadLine());
            result.ElapsedPhaseTime = System::TimeSpan::Parse(reader.ReadLine());
            CurrentGameState() = result;
        } catch (...) {
            CurrentGameState().reset();
        }
    }

    GameplayScreen* GetGameplayScreen() const {
        for (const auto& screen : screenManager_->GetScreens()) {
            if (auto* gameplay = dynamic_cast<GameplayScreen*>(screen.get()))
                return gameplay;
        }
        return nullptr;
    }

    GraphicsDeviceManager graphics_;
    std::shared_ptr<ScreenManager> screenManager_;
    std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> activatingTexture_;
    std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> loadingTexture_;
    bool hasLaunched_ = false;
};

} // namespace NinjAcademy
