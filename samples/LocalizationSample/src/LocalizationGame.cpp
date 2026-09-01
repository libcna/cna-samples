// SPDX-License-Identifier: MS-PL

#include "LocalizationGame.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Strings.hpp"
#include "System/String.hpp"

namespace Localization
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteFont;
    using Microsoft::Xna::Framework::Graphics::Texture2D;
    using Microsoft::Xna::Framework::Input::Buttons;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Keyboard;
    using Microsoft::Xna::Framework::Input::Keys;
    using System::Globalization::CultureInfo;

    LocalizationGame::LocalizationGame()
    {
        getContentProperty().setRootDirectoryProperty("Content");
        graphics_ = std::make_unique<GraphicsDeviceManager>(this);
        Strings::setCultureProperty(CultureInfo::getCurrentCultureProperty());
    }

    const std::string& LocalizationGame::GetTypeName() const
    {
        static const std::string name = "Localization.LocalizationGame";
        return name;
    }

    void LocalizationGame::LoadContent()
    {
        spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        font_.emplace(getContentProperty().Load<SpriteFont>("Font"));
        currentFlag_.emplace(LoadLocalizedAsset<Texture2D>("Flag"));
    }

    void LocalizationGame::Update(GameTime& gameTime)
    {
        HandleInput();
        Game::Update(gameTime);
    }

    void LocalizationGame::Draw(const GameTime& gameTime)
    {
        const CultureInfo& culture = CultureInfo::getCurrentCultureProperty();
        const std::string string1 = Strings::getWelcomeProperty();
        const std::string string2 = System::String::Format(
            Strings::getCurrentLocaleProperty(),
            culture.getEnglishNameProperty(),
            culture.ToString());
        const std::string string3 = Strings::getHowToChangeProperty();

        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

        spriteBatch_->Begin();
        spriteBatch_->DrawString(*font_, string1, Vector2(100.0f, 100.0f), Color::White);
        spriteBatch_->DrawString(*font_, string2, Vector2(100.0f, 130.0f), Color::White);
        spriteBatch_->DrawString(*font_, string3, Vector2(100.0f, 160.0f), Color::White);
        spriteBatch_->Draw(*currentFlag_, Vector2(100.0f, 250.0f), Color::White);
        spriteBatch_->End();

        Game::Draw(gameTime);
    }

    void LocalizationGame::HandleInput()
    {
        const auto currentKeyboardState = Keyboard::GetState();
        const auto currentGamePadState = GamePad::GetState(PlayerIndex::One);

        if (currentKeyboardState.IsKeyDown(Keys::Escape) ||
            currentGamePadState.IsButtonDown(Buttons::Back))
        {
            Exit();
        }
    }
}
