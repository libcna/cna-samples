// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game1.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Game1.hpp"

#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/IO/Path.hpp"
#include "System/Text/StringBuilder.hpp"
#include "System/TimeSpan.hpp"

namespace SampleGame
{
    using namespace Microsoft::Xna::Framework::Input;

    Game1::Game1()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        // Frame rate is 30 fps by default for Windows Phone.
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));

#if defined(WINDOWS_PHONE)
        graphics.setIsFullScreenProperty(true);

        // Pre-autoscale settings.
        graphics.setPreferredBackBufferWidthProperty(480);
        graphics.setPreferredBackBufferHeightProperty(800);
#endif
    }

    const std::string& Game1::GetTypeName() const
    {
        static const std::string typeName{"SampleGame.Game1"};
        return typeName;
    }

    void Game1::Initialize()
    {
        Game::Initialize();
    }

    void Game1::LoadContent()
    {
        spriteBatch.emplace(getGraphicsDeviceProperty());
        font.emplace(getContentProperty().Load<SpriteFont>("Font"));

        // Load our manifest so we know what files we have.
        const std::vector<std::string> contentFiles =
            getContentProperty().Load<std::vector<std::string>>("manifest");

        std::vector<std::string> content;
        std::vector<std::string> copiedFiles;
        for (const std::string& file : contentFiles)
        {
            if (System::IO::Path::HasExtension(file))
            {
                copiedFiles.push_back(file);
            }
            else
            {
                content.push_back(file);
            }
        }

        System::Text::StringBuilder builder;
        // For debugging, write out the two categories.
        builder.AppendLine("CONTENT:");
        for (const std::string& file : content)
        {
            builder.AppendLine("  " + file);
        }

        builder.AppendLine("COPIED FILES:");
        for (const std::string& file : copiedFiles)
        {
            builder.AppendLine("  " + file);
        }

        displayedText = builder.ToString();

        // Content.Load can now load built content, while TitleContainer.OpenStream can open
        // copied files without probing the file system first.
    }

    void Game1::UnloadContent()
    {
    }

    void Game1::Update(GameTime& gameTime)
    {
        if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBackProperty() ==
                ButtonState::Pressed ||
            Keyboard::GetState().IsKeyDown(Keys::Escape))
        {
            Exit();
        }

        Game::Update(gameTime);
    }

    void Game1::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

        spriteBatch->Begin();
        spriteBatch->DrawString(*font, displayedText, Vector2(10.0f), Color::White);
        spriteBatch->End();

        Game::Draw(gameTime);
    }
}
