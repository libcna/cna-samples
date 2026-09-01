// SPDX-License-Identifier: MS-PL
#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/Diagnostics/Debug.hpp"
#include "System/IO/BinaryReader.hpp"
#include "System/IO/BinaryWriter.hpp"
#include "System/IO/FileMode.hpp"
#include "System/IO/Path.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFile.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFileStream.hpp"

#include "ScreenManager/GameScreen.hpp"
#include "ScreenManager/InputState.hpp"

namespace UserInterfaceSample {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::DrawableGameComponent;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::PlayerIndex;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::SurfaceFormat;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Input::Touch::GestureType;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

class ScreenManager final : public DrawableGameComponent {
public:
    explicit ScreenManager(Game& game) : DrawableGameComponent(game) {
        TouchPanel::setEnabledGesturesProperty(GestureType::None);
    }

    SpriteBatch& getSpriteBatchProperty() { return *spriteBatch_; }
    SpriteFont& getFontProperty() { return *font_; }
    Texture2D& getBlankTextureProperty() { return *blankTexture_; }
    bool getTraceEnabledProperty() const { return traceEnabled_; }
    void setTraceEnabledProperty(bool value) { traceEnabled_ = value; }

    template <typename T>
    CNAEXT static void RegisterScreenType() {
        ScreenFactories()[T::StaticAssemblyQualifiedName()] = [] {
            return std::make_shared<T>();
        };
    }

    void Initialize() override {
        DrawableGameComponent::Initialize();
        isInitialized_ = true;
    }

    void LoadContent() override {
        auto& content = getGameProperty().getContentProperty();
        spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        font_.emplace(content.Load<SpriteFont>("Font\\MenuTitle"));
        blankTexture_.emplace(getGraphicsDeviceProperty(), 1, 1, false, SurfaceFormat::Color);
        const Color white = Color::White;
        blankTexture_->SetData(&white, 1);

        for (const auto& screen : screens_)
            screen->LoadContent();
    }

    void UnloadContent() override {
        for (const auto& screen : screens_)
            screen->UnloadContent();
    }

    void Update(GameTime& gameTime) override {
        input_.Update();
        screensToUpdate_ = screens_;

        bool otherScreenHasFocus = !getGameProperty().getIsActiveProperty();
        bool coveredByOtherScreen = false;

        while (!screensToUpdate_.empty()) {
            const std::shared_ptr<GameScreen> screen = screensToUpdate_.back();
            screensToUpdate_.pop_back();
            screen->Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);

            if (screen->GetScreenState() == ScreenState::TransitionOn ||
                screen->GetScreenState() == ScreenState::Active) {
                if (!otherScreenHasFocus) {
                    screen->HandleInput(input_);
                    otherScreenHasFocus = true;
                }
                if (!screen->IsPopup())
                    coveredByOtherScreen = true;
            }
        }

        if (traceEnabled_)
            TraceScreens();
    }

    void Draw(const GameTime& gameTime) override {
        for (const auto& screen : screens_) {
            if (screen->GetScreenState() != ScreenState::Hidden)
                screen->Draw(gameTime);
        }
    }

    void AddScreen(std::shared_ptr<GameScreen> screen,
                   std::optional<PlayerIndex> controllingPlayer) {
        screen->setControllingPlayer(controllingPlayer);
        screen->setScreenManager(this);
        screen->setIsExiting(false);

        if (isInitialized_)
            screen->LoadContent();

        screens_.push_back(std::move(screen));
        TouchPanel::setEnabledGesturesProperty(screens_.back()->EnabledGestures());
    }

    void RemoveScreen(GameScreen* screen) {
        if (isInitialized_)
            screen->UnloadContent();

        EraseByPointer(screens_, screen);
        EraseByPointer(screensToUpdate_, screen);

        if (!screens_.empty())
            TouchPanel::setEnabledGesturesProperty(screens_.back()->EnabledGestures());
    }

    std::vector<std::shared_ptr<GameScreen>> GetScreens() const { return screens_; }

    void FadeBackBufferToBlack(float alpha) {
        const auto viewport = getGraphicsDeviceProperty().getViewportProperty();
        spriteBatch_->Begin();
        spriteBatch_->Draw(*blankTexture_,
            Rectangle(0, 0, viewport.getWidthProperty(), viewport.getHeightProperty()),
            Color::Black * alpha);
        spriteBatch_->End();
    }

    void SerializeState() {
        auto storage = System::IO::IsolatedStorage::IsolatedStorageFile::GetUserStoreForApplication();
        if (storage.DirectoryExists("ScreenManager"))
            DeleteState(storage);
        else
            storage.CreateDirectory("ScreenManager");

        {
            auto stream = storage.CreateFile("ScreenManager\\ScreenList.dat");
            System::IO::BinaryWriter writer(&stream, true);
            for (const auto& screen : screens_) {
                if (screen->IsSerializable())
                    writer.Write(screen->GetAssemblyQualifiedName());
            }
            writer.Flush();
        }

        int screenIndex = 0;
        for (const auto& screen : screens_) {
            if (!screen->IsSerializable())
                continue;
            const std::string fileName = "ScreenManager\\Screen" +
                std::to_string(screenIndex) + ".dat";
            auto stream = storage.CreateFile(fileName);
            screen->Serialize(stream);
            ++screenIndex;
        }
    }

    bool DeserializeState() {
        auto storage = System::IO::IsolatedStorage::IsolatedStorageFile::GetUserStoreForApplication();
        if (!storage.DirectoryExists("ScreenManager"))
            return false;

        try {
            if (storage.FileExists("ScreenManager\\ScreenList.dat")) {
                auto stream = storage.OpenFile("ScreenManager\\ScreenList.dat", System::IO::FileMode::Open);
                System::IO::BinaryReader reader(&stream, true);
                while (stream.getPositionProperty() < stream.getLengthProperty()) {
                    const std::string typeName = reader.ReadString();
                    if (typeName.empty())
                        continue;
                    const auto factory = ScreenFactories().find(typeName);
                    if (factory == ScreenFactories().end())
                        throw std::runtime_error("No registered screen factory for " + typeName);
                    AddScreen(factory->second(), PlayerIndex::One);
                }
            }

            for (std::size_t index = 0; index < screens_.size(); ++index) {
                const std::string fileName = "ScreenManager\\Screen" +
                    std::to_string(index) + ".dat";
                auto stream = storage.OpenFile(fileName, System::IO::FileMode::Open);
                screens_[index]->Deserialize(stream);
            }
            return true;
        } catch (const std::exception&) {
            DeleteState(storage);
        }
        return false;
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "UserInterfaceSample.ScreenManager";
        return name;
    }

private:
    using ScreenFactory = std::function<std::shared_ptr<GameScreen>()>;

    static std::unordered_map<std::string, ScreenFactory>& ScreenFactories() {
        static std::unordered_map<std::string, ScreenFactory> factories;
        return factories;
    }

    static void DeleteState(System::IO::IsolatedStorage::IsolatedStorageFile& storage) {
        for (const std::string& file : storage.GetFileNames("ScreenManager\\*"))
            storage.DeleteFile(System::IO::Path::Combine("ScreenManager", file));
    }

    void TraceScreens() const {
        std::string names;
        for (const auto& screen : screens_) {
            if (!names.empty())
                names += ", ";
            const std::string& fullName = screen->GetTypeName();
            const std::size_t separator = fullName.find_last_of('.');
            names += separator == std::string::npos ? fullName : fullName.substr(separator + 1);
        }
        System::Diagnostics::Debug::WriteLine(names);
    }

    static void EraseByPointer(std::vector<std::shared_ptr<GameScreen>>& screens,
                               GameScreen* screen) {
        screens.erase(std::remove_if(screens.begin(), screens.end(),
            [screen](const std::shared_ptr<GameScreen>& item) {
                return item.get() == screen;
            }), screens.end());
    }

    std::vector<std::shared_ptr<GameScreen>> screens_;
    std::vector<std::shared_ptr<GameScreen>> screensToUpdate_;
    InputState input_;
    std::unique_ptr<SpriteBatch> spriteBatch_;
    std::optional<SpriteFont> font_;
    std::optional<Texture2D> blankTexture_;
    bool isInitialized_ = false;
    bool traceEnabled_ = false;
};

inline void GameScreen::Update(GameTime& gameTime, bool otherScreenHasFocus,
                               bool coveredByOtherScreen) {
    otherScreenHasFocus_ = otherScreenHasFocus;

    if (isExiting_) {
        screenState_ = ScreenState::TransitionOff;
        if (!UpdateTransition(gameTime, transitionOffTime_, 1))
            screenManager_->RemoveScreen(this);
    } else if (coveredByOtherScreen) {
        screenState_ = UpdateTransition(gameTime, transitionOffTime_, 1)
            ? ScreenState::TransitionOff
            : ScreenState::Hidden;
    } else {
        screenState_ = UpdateTransition(gameTime, transitionOnTime_, -1)
            ? ScreenState::TransitionOn
            : ScreenState::Active;
    }
}

inline void GameScreen::ExitScreen() {
    if (transitionOffTime_ == System::TimeSpan::Zero)
        screenManager_->RemoveScreen(this);
    else
        isExiting_ = true;
}

} // namespace UserInterfaceSample
