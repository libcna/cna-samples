// SPDX-License-Identifier: MS-PL

#include "ScreenManager/ScreenManager.hpp"

#include <algorithm>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/Diagnostics/Debug.hpp"

namespace GameStateManagement
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Input::Touch::GestureType;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

    ScreenManager::ScreenManager(Microsoft::Xna::Framework::Game& game)
        : DrawableGameComponent(game)
    {
        TouchPanel::setEnabledGesturesProperty(GestureType::None);
    }

    Microsoft::Xna::Framework::Graphics::SpriteBatch& ScreenManager::getSpriteBatchProperty()
    { return *spriteBatch_; }
    Microsoft::Xna::Framework::Graphics::SpriteFont& ScreenManager::getFontProperty()
    { return *font_; }
    bool ScreenManager::getTraceEnabledProperty() const { return traceEnabled_; }
    void ScreenManager::setTraceEnabledProperty(bool value) { traceEnabled_ = value; }

    void ScreenManager::Initialize()
    {
        DrawableGameComponent::Initialize();
        isInitialized_ = true;
    }

    void ScreenManager::LoadContent()
    {
        auto& content = getGameProperty().getContentProperty();
        spriteBatch_ = std::make_unique<Microsoft::Xna::Framework::Graphics::SpriteBatch>(
            getGraphicsDeviceProperty());
        font_.emplace(content.Load<Microsoft::Xna::Framework::Graphics::SpriteFont>("menufont"));
        blankTexture_.emplace(
            content.Load<Microsoft::Xna::Framework::Graphics::Texture2D>("blank"));
        for (const auto& screen : screens_)
            screen->LoadContent();
    }

    void ScreenManager::UnloadContent()
    {
        for (const auto& screen : screens_)
            screen->UnloadContent();
    }

    void ScreenManager::Update(Microsoft::Xna::Framework::GameTime& gameTime)
    {
        input_.Update();
        screensToUpdate_.clear();
        for (const auto& screen : screens_)
            screensToUpdate_.push_back(screen);

        bool otherScreenHasFocus = !getGameProperty().getIsActiveProperty();
        bool coveredByOtherScreen = false;
        while (!screensToUpdate_.empty())
        {
            const std::shared_ptr<GameScreen> screen = screensToUpdate_.back();
            screensToUpdate_.pop_back();
            screen->Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);

            if (screen->getScreenStateProperty() == ScreenState::TransitionOn ||
                screen->getScreenStateProperty() == ScreenState::Active)
            {
                if (!otherScreenHasFocus)
                {
                    screen->HandleInput(input_);
                    otherScreenHasFocus = true;
                }
                if (!screen->getIsPopupProperty())
                    coveredByOtherScreen = true;
            }
        }
        if (traceEnabled_)
            TraceScreens();
    }

    void ScreenManager::Draw(const Microsoft::Xna::Framework::GameTime& gameTime)
    {
        for (const auto& screen : screens_)
            if (screen->getScreenStateProperty() != ScreenState::Hidden)
                screen->Draw(gameTime);
    }

    void ScreenManager::AddScreen(
        std::shared_ptr<GameScreen> screen,
        std::optional<Microsoft::Xna::Framework::PlayerIndex> controllingPlayer)
    {
        screen->setControllingPlayerProperty(controllingPlayer);
        screen->setScreenManagerProperty(*this);
        screen->setIsExitingProperty(false);
        if (isInitialized_)
            screen->LoadContent();
        screens_.push_back(std::move(screen));
        TouchPanel::setEnabledGesturesProperty(screens_.back()->getEnabledGesturesProperty());
    }

    void ScreenManager::RemoveScreen(GameScreen* screen)
    {
        if (isInitialized_)
            screen->UnloadContent();
        EraseByPointer(screens_, screen);
        EraseByPointer(screensToUpdate_, screen);
        if (!screens_.empty())
            TouchPanel::setEnabledGesturesProperty(screens_.back()->getEnabledGesturesProperty());
    }

    std::vector<std::shared_ptr<GameScreen>> ScreenManager::GetScreens() const { return screens_; }

    void ScreenManager::FadeBackBufferToBlack(float alpha)
    {
        const auto viewport = getGraphicsDeviceProperty().getViewportProperty();
        spriteBatch_->Begin();
        spriteBatch_->Draw(*blankTexture_, Rectangle(0, 0, viewport.getWidthProperty(),
            viewport.getHeightProperty()), Color::Black * alpha);
        spriteBatch_->End();
    }

    void ScreenManager::TraceScreens() const
    {
        std::string names;
        for (const auto& screen : screens_)
        {
            if (!names.empty())
                names += ", ";
            const std::string& fullName = screen->GetTypeName();
            const auto separator = fullName.find_last_of('.');
            names += separator == std::string::npos ? fullName : fullName.substr(separator + 1);
        }
        System::Diagnostics::Debug::WriteLine(names);
    }

    void ScreenManager::EraseByPointer(std::vector<std::shared_ptr<GameScreen>>& screens,
                                       GameScreen* screen)
    {
        screens.erase(std::remove_if(screens.begin(), screens.end(),
            [screen](const std::shared_ptr<GameScreen>& item) { return item.get() == screen; }),
            screens.end());
    }

    const std::string& ScreenManager::GetTypeName() const
    {
        static const std::string name = "GameStateManagement.ScreenManager";
        return name;
    }
}
