// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Checkbox.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Checkbox.hpp"

#include "../Graphics3DSampleGame.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"

namespace Graphics3DSample
{
    using Microsoft::Xna::Framework::Color;

    Checkbox::Checkbox(Graphics3DSampleGame& game, const std::string& textureName,
                       const Rectangle& targetRectangle, bool isChecked)
        : Clickable(game, targetRectangle), asset(textureName), isChecked(isChecked)
    {
    }

    const std::string& Checkbox::GetTypeName() const
    {
        static const std::string name = "Graphics3DSample.Checkbox";
        return name;
    }

    void Checkbox::LoadContent()
    {
        textureOn = getGameProperty().getContentProperty().Load<Texture2D>(asset);
        Clickable::LoadContent();
    }

    void Checkbox::Update(GameTime& gameTime)
    {
        HandleInput();
        isChecked = getIsClickedProperty() ? !isChecked : isChecked;
        Clickable::Update(gameTime);
    }

    void Checkbox::Draw(const GameTime& gameTime)
    {
        getGameProperty().getSpriteBatchProperty().Begin();
        getGameProperty().getSpriteBatchProperty().Draw(
            *textureOn, getRectangleProperty(),
            getIsCheckedProperty() ? Color::Yellow : Color::White);
        getGameProperty().getSpriteBatchProperty().End();
        Clickable::Draw(gameTime);
    }
}
