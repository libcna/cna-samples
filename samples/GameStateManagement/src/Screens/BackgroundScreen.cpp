// SPDX-License-Identifier: MS-PL

#include "Screens/BackgroundScreen.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace GameStateManagement
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    BackgroundScreen::BackgroundScreen()
    {
        setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(0.5));
        setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(0.5));
    }

    void BackgroundScreen::LoadContent()
    {
        auto& game = getScreenManagerProperty().getGameProperty();
        if (!content_)
            content_ = std::make_unique<Content::ContentManager>(&game.getServicesProperty(), "Content");
        backgroundTexture_.emplace(content_->Load<Texture2D>("background"));
    }

    void BackgroundScreen::UnloadContent() { content_->Unload(); }

    void BackgroundScreen::Update(GameTime& gameTime, bool otherScreenHasFocus, bool)
    { GameScreen::Update(gameTime, otherScreenHasFocus, false); }

    void BackgroundScreen::Draw(const GameTime&)
    {
        auto& manager = getScreenManagerProperty();
        auto& viewport = manager.getGraphicsDeviceProperty().getViewportProperty();
        const Rectangle fullscreen(0, 0, viewport.getWidthProperty(), viewport.getHeightProperty());
        const float alpha = getTransitionAlphaProperty();
        auto& spriteBatch = manager.getSpriteBatchProperty();
        spriteBatch.Begin();
        spriteBatch.Draw(*backgroundTexture_, fullscreen, Color(alpha, alpha, alpha));
        spriteBatch.End();
    }

    const std::string& BackgroundScreen::GetTypeName() const
    {
        static const std::string name = "GameStateManagement.BackgroundScreen";
        return name;
    }
}
