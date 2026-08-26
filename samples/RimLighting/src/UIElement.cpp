// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// UIElement.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "UIElement.hpp"

#include <vector>

namespace RimLighting
{
    namespace
    {
        const char kSplitTokens[] = {' ', '-'};
        const String kSpaceString = " ";
    }

    UIElement::UIElement()
        : isVisible(true)
    {
    }

    Vector2 UIElement::getPositionProperty() const { return position; }

    void UIElement::setPositionProperty(Vector2 value)
    {
        position = value;
        needsMeasure = true;
    }

    Vector2 UIElement::getSizeProperty() const { return size; }

    void UIElement::setSizeProperty(Vector2 value)
    {
        size = value;
        needsMeasure = true;
    }

    bool UIElement::getIsVisibleProperty() const { return isVisible; }

    void UIElement::setIsVisibleProperty(bool value) { isVisible = value; }

    void UIElement::Draw(SpriteBatch& spriteBatch)
    {
        (void)spriteBatch;
        if (needsMeasure)
        {
            Measure();
        }
    }

    void UIElement::HandleTouch(const Input::Touch::TouchLocation& loc)
    {
        (void)loc;
    }

    String UIElement::WordWrap(const String& input, int width, const SpriteFont& font)
    {
        String output;

        // C#'s string.Split(char[], StringSplitOptions.None) keeps empty entries, so a run of
        // separators yields empty words; this reproduces that rather than collapsing them.
        std::vector<String> wordArray;
        String current;
        for (char c : input)
        {
            bool isSplit = false;
            for (char token : kSplitTokens)
            {
                if (c == token) { isSplit = true; break; }
            }
            if (isSplit)
            {
                wordArray.push_back(current);
                current.clear();
            }
            else
            {
                current.push_back(c);
            }
        }
        wordArray.push_back(current);

        int space = (int)font.MeasureString(kSpaceString).X;

        int lineLength = 0;
        int wordLength = 0;
        int wordCount = 0;

        for (std::size_t i = 0; i < wordArray.size(); i++)
        {
            wordLength = (int)font.MeasureString(wordArray[i]).X;

            // don't overflow the desired width unless there are no other words on the line
            if (wordCount > 0 && wordLength + lineLength > width)
            {
                output += "\n";
                lineLength = 0;
                wordCount = 0;
            }

            output += wordArray[i];
            output += kSpaceString;
            lineLength += wordLength + space;
            wordCount++;
        }

        return output;
    }
}
