// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Slidebar.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Slidebar.hpp"

#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocationState.hpp"

namespace RimLighting
{
    using Microsoft::Xna::Framework::Input::Touch::TouchLocationState;

    std::optional<Texture2D>& Slidebar::BlankTexture()
    {
        static std::optional<Texture2D> texureBlank;
        return texureBlank;
    }

    Slidebar::Slidebar(Microsoft::Xna::Framework::Game& gameInstance, const SpriteFont& font,
                       float min, float max)
        : MinValue(min)
        , MaxValue(max)
    {
        this->game = &gameInstance;
        spriteFont = &font;
    }

    Vector2 Slidebar::getTextSizeProperty() const { return textSize; }

    String Slidebar::getTextProperty() const { return sliderText; }

    void Slidebar::setTextProperty(const String& value)
    {
        sliderText = value;
        textSize = spriteFont->MeasureString(value);
        needsMeasure = true;
    }

    float Slidebar::getValueProperty() const { return valueInt; }

    void Slidebar::setValueProperty(float value)
    {
        valueInt = value;
        currentLength = (valueInt - MinValue) / (MaxValue - MinValue) * sizeBar.X;

        OnValueChanged.Raise(nullptr, System::EventArgs::Empty);
    }

    void Slidebar::SetBarOffsetSize(float offsetX, float offsetY, float maxwidth, float height)
    {
        offsetBar = Vector2(offsetX, offsetY);
        sizeBar = Vector2(maxwidth, height);
        needsMeasure = true;
    }

    void Slidebar::SetRange(float min, float max)
    {
        MinValue = min;
        MaxValue = max;
    }

    void Slidebar::Measure()
    {
        setSizeProperty(textSize + offsetBar + sizeBar);
        currentLength = (valueInt - MinValue) / (MaxValue - MinValue) * sizeBar.X;
        needsMeasure = false;
    }

    void Slidebar::LoadContent()
    {
        if (!BlankTexture().has_value())
        {
            BlankTexture().emplace(
                game->getContentProperty().Load<Texture2D>("blankTex"));
        }
    }

    void Slidebar::HandleTouch(const Input::Touch::TouchLocation& loc)
    {
        if (loc.getStateProperty() == TouchLocationState::Pressed && !IsDragging)
        {
            if (loc.getPositionProperty().Y >= getPositionProperty().Y &&
                loc.getPositionProperty().Y <= (getPositionProperty().Y + offsetBar.Y + sizeBar.Y))
            {
                IsDragging = true;
                lastPressPosition = loc.getPositionProperty();
            }
        }
        else
        {
            if (loc.getStateProperty() == TouchLocationState::Released)
                IsDragging = false;
        }

        if (IsDragging)
        {
            Vector2 delta = loc.getPositionProperty() - lastPressPosition;
            lastPressPosition = loc.getPositionProperty();

            currentLength += delta.X;
            if (currentLength < 0) currentLength = 0;
            if (currentLength > sizeBar.X) currentLength = sizeBar.X;
            valueInt = currentLength / sizeBar.X * (MaxValue - MinValue) + MinValue;
            OnValueChanged.Raise(nullptr, System::EventArgs::Empty);
        }
    }

    void Slidebar::Draw(SpriteBatch& spriteBatch)
    {
        if (!getIsVisibleProperty())
        {
            return;
        }

        UIElement::Draw(spriteBatch);

        LoadContent();

        spriteBatch.Begin();
        Rectangle rectangle;
        rectangle.X = (int)(getPositionProperty().X + offsetBar.X);
        rectangle.Y = (int)(getPositionProperty().Y + offsetBar.Y);
        rectangle.Width = (int)currentLength;
        rectangle.Height = (int)sizeBar.Y;
        spriteBatch.Draw(*BlankTexture(), rectangle, Color);
        spriteBatch.DrawString(*spriteFont, getTextProperty(), getPositionProperty(), Color);
        spriteBatch.End();
    }
}
