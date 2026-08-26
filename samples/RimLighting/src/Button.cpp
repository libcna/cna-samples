// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Button.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Button.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPassCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocationState.hpp"

namespace RimLighting
{
    using Microsoft::Xna::Framework::Input::Touch::TouchLocationState;

    Button::Button(GraphicsDevice& device, const SpriteFont& font, const String& text)
    {
        buttonFont = &font;
        setTextProperty(text);

        setSizeProperty(buttonFont->MeasureString(buttonText));

        effect = std::make_unique<BasicEffect>(device);
        effect->setProjectionProperty(Matrix::CreateOrthographicOffCenter(
            0, (float)device.getViewportProperty().getWidthProperty(),
            (float)device.getViewportProperty().getHeightProperty(), 0, 1, 1000));
        effect->setViewProperty(Matrix::CreateLookAt(Vector3(0, 0, 5), Vector3::Zero, Vector3::Up));
        effect->VertexColorEnabled = true;

        for (std::size_t i = 0; i < verts.size(); i++)
        {
            verts[i].Color = Color::White;
        }
    }

    String Button::getTextProperty() const { return buttonText; }

    void Button::setTextProperty(const String& value)
    {
        buttonText = value;
        needsMeasure = true;
    }

    void Button::HandleTouch(const Input::Touch::TouchLocation& loc)
    {
        if (loc.getStateProperty() == TouchLocationState::Pressed)
        {
            if (pressId == 0 && HitTest(loc.getPositionProperty()))
            {
                if (getIsVisibleProperty())
                {
                    pressId = loc.getIdProperty();
                }
            }
        }
        else if (loc.getStateProperty() == TouchLocationState::Released)
        {
            if (pressId == loc.getIdProperty())
            {
                pressId = 0;

                if (HitTest(loc.getPositionProperty()))
                {
                    OnClick.Raise(nullptr, System::EventArgs::Empty);
                }
            }
        }
    }

    bool Button::HitTest(Vector2 point) const
    {
        return (point.X >= getPositionProperty().X &&
                point.X < getPositionProperty().X + getSizeProperty().X &&
                point.Y >= getPositionProperty().Y &&
                point.Y < getPositionProperty().Y + getSizeProperty().Y);
    }

    void Button::Measure()
    {
        if (buttonFont != nullptr && !buttonText.empty())
        {
            textOffset = (getSizeProperty() - buttonFont->MeasureString(buttonText)) / 2;
        }

        // this array will be used to draw either a line strip or a triangle list
        const Vector2 p = getPositionProperty();
        const Vector2 s = getSizeProperty();
        verts[0].Position = Vector3(p.X, p.Y, 0);
        verts[1].Position = Vector3(p.X + s.X, p.Y, 0);
        verts[2].Position = Vector3(p.X + s.X, p.Y + s.Y, 0);
        verts[3].Position = Vector3(p.X, p.Y + s.Y, 0);
        verts[4].Position = verts[0].Position;
        verts[5].Position = verts[2].Position;
    }

    void Button::Draw(SpriteBatch& spriteBatch)
    {
        if (!getIsVisibleProperty())
        {
            return;
        }

        UIElement::Draw(spriteBatch);

        if (needsMeasure)
        {
            Measure();
        }

        // draw a box
        EffectPassCollection& passes = effect->getCurrentTechniqueProperty()->getPassesProperty();
        for (int i = 0; i < passes.getCountProperty(); i++)
        {
            passes[i].Apply();

            if (pressId == 0)
            {
                effect->getGraphicsDeviceProperty()->DrawUserPrimitives(
                    PrimitiveType::LineStrip, verts.data(), 0, 4);
            }
            else
            {
                effect->getGraphicsDeviceProperty()->DrawUserPrimitives(
                    PrimitiveType::TriangleList, verts.data(), 0, 2);
            }
        }

        // draw the text
        spriteBatch.Begin();
        spriteBatch.DrawString(*buttonFont, buttonText, getPositionProperty() + textOffset,
                               pressId == 0 ? Color::White : Color::Black);
        spriteBatch.End();
    }
}
