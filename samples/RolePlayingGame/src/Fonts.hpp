#pragma once

// Fonts.hpp -- C++ port of Fonts.cs. Static storage of SpriteFont objects and
// colors for use throughout the game.

#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/ArgumentOutOfRangeException.hpp"
#include "System/Int32.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteFont;

class Fonts {
public:
    static SpriteFont& HeaderFont() { return *headerFont_; }
    static SpriteFont& PlayerNameFont() { return *playerNameFont_; }
    static SpriteFont& DebugFont() { return *debugFont_; }
    static SpriteFont& ButtonNamesFont() { return *buttonNamesFont_; }
    static SpriteFont& DescriptionFont() { return *descriptionFont_; }
    static SpriteFont& GearInfoFont() { return *gearInfoFont_; }
    static SpriteFont& DamageFont() { return *damageFont_; }
    static SpriteFont& PlayerStatisticsFont() { return *playerStatisticsFont_; }
    static SpriteFont& HudDetailFont() { return *hudDetailFont_; }
    static SpriteFont& CaptionFont() { return *captionFont_; }

    static inline const Color CountColor{79, 24, 44, 255};
    static inline const Color TitleColor{59, 18, 6, 255};
    static inline const Color CaptionColor{228, 168, 57, 255};
    static inline const Color HighlightColor{223, 206, 148, 255};
    static inline const Color DisplayColor{68, 32, 19, 255};
    static inline const Color DescriptionColor{0, 0, 0, 255};
    static inline const Color RestrictionColor{0, 0, 0, 255};
    static inline const Color ModifierColor{0, 0, 0, 255};
    static inline const Color MenuSelectedColor{248, 218, 127, 255};

    static void LoadContent(ContentManager& contentManager) {
        buttonNamesFont_ = contentManager.Load<SpriteFont>("Fonts/ButtonNamesFont");
        captionFont_ = contentManager.Load<SpriteFont>("Fonts/CaptionFont");
        damageFont_ = contentManager.Load<SpriteFont>("Fonts/DamageFont");
        debugFont_ = contentManager.Load<SpriteFont>("Fonts/DebugFont");
        descriptionFont_ = contentManager.Load<SpriteFont>("Fonts/DescriptionFont");
        gearInfoFont_ = contentManager.Load<SpriteFont>("Fonts/GearInfoFont");
        headerFont_ = contentManager.Load<SpriteFont>("Fonts/HeaderFont");
        hudDetailFont_ = contentManager.Load<SpriteFont>("Fonts/HudDetailFont");
        playerNameFont_ = contentManager.Load<SpriteFont>("Fonts/PlayerNameFont");
        playerStatisticsFont_ = contentManager.Load<SpriteFont>("Fonts/PlayerStatisticsFont");
    }

    static void UnloadContent() {
        buttonNamesFont_.reset();
        captionFont_.reset();
        damageFont_.reset();
        debugFont_.reset();
        descriptionFont_.reset();
        gearInfoFont_.reset();
        headerFont_.reset();
        hudDetailFont_.reset();
        playerNameFont_.reset();
        playerStatisticsFont_.reset();
    }

    static std::string BreakTextIntoLines(const std::string& text, int maximumCharactersPerLine, int maximumLines) {
        (void)maximumLines;
        if (text.empty() || (int)text.size() < maximumCharactersPerLine) return text;
        std::string result = text;
        int currentLine = 0;
        size_t newLineIndex = 0;
        while ((result.size() - newLineIndex) > (size_t)maximumCharactersPerLine && currentLine < maximumLines) {
            size_t nextIndex = newLineIndex;
            size_t candidate = newLineIndex;
            while (candidate != std::string::npos && candidate < (size_t)maximumCharactersPerLine) {
                nextIndex = candidate;
                candidate = result.find(' ', candidate + 1);
            }
            if (nextIndex < result.size()) result[nextIndex] = '\n';
            newLineIndex = nextIndex;
            currentLine++;
        }
        return result;
    }

    // The same line-breaking with no cap on the number of lines.
    static std::string BreakTextIntoLines(const std::string& text, int maximumCharactersPerLine) {
        // check the parameters
        if (maximumCharactersPerLine <= 0) {
            throw System::ArgumentOutOfRangeException("maximumCharactersPerLine");
        }
        // if the string is trivial, then this is really easy
        if (text.empty()) return std::string();
        // if the text is short enough to fit on one line, then this is still easy
        if ((int)text.size() < maximumCharactersPerLine) return text;

        std::string result = text;
        std::size_t newLineIndex = 0;
        while ((result.size() - newLineIndex) > (std::size_t)maximumCharactersPerLine) {
            std::size_t nextIndex = newLineIndex;
            std::size_t candidate = newLineIndex;
            while (candidate != std::string::npos &&
                   candidate < (std::size_t)maximumCharactersPerLine) {
                nextIndex = candidate;
                candidate = result.find(' ', candidate + 1);
            }
            if (nextIndex >= result.size()) break;
            result[nextIndex] = '\n';
            newLineIndex = nextIndex;
        }
        return result;
    }

    // C#'s String.Format("{0:n0}", gold) -- the invariant thousands-grouped form.
    static std::string GetGoldString(int gold) {
        std::string digits = System::Int32::ToString(gold < 0 ? -gold : gold);
        std::string grouped;
        int count = 0;
        for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
            if (count > 0 && count % 3 == 0) grouped.push_back(',');
            grouped.push_back(*it);
            count++;
        }
        if (gold < 0) grouped.push_back('-');
        return std::string(grouped.rbegin(), grouped.rend());
    }

    static std::vector<std::string> BreakTextIntoList(const std::string& text, SpriteFont& font, int rowWidth) {
        std::vector<std::string> lines;
        if (text.empty()) { lines.push_back(""); return lines; }
        if (font.MeasureString(text).X <= rowWidth) { lines.push_back(text); return lines; }

        std::vector<std::string> words;
        std::istringstream iss(text);
        std::string word;
        while (iss >> word) words.push_back(word);

        size_t currentWord = 0;
        while (currentWord < words.size()) {
            int wordsThisLine = 0;
            std::string line;
            while (currentWord < words.size()) {
                std::string testLine = line;
                if (testLine.empty()) {
                    testLine += words[currentWord];
                } else if (!testLine.empty() &&
                           (testLine.back() == '.' || testLine.back() == '?' || testLine.back() == '!')) {
                    testLine += "  " + words[currentWord];
                } else {
                    testLine += " " + words[currentWord];
                }
                if (wordsThisLine > 0 && font.MeasureString(testLine).X > rowWidth) break;
                line = testLine;
                wordsThisLine++;
                currentWord++;
            }
            lines.push_back(line);
        }
        return lines;
    }

    static void DrawCenteredText(SpriteBatch& spriteBatch, SpriteFont& font, const std::string& text, Vector2 position,
                                 Color color) {
        // check for trivial text
        if (text.empty()) return;
        // calculate the centered position
        Vector2 textSize = font.MeasureString(text);
        Vector2 centeredPosition((float)((int)position.X - (int)textSize.X / 2),
                                 (float)((int)position.Y - (int)textSize.Y / 2));
        // draw the string
        spriteBatch.DrawString(font, text, centeredPosition, color, 0.0f, Vector2::Zero, 1.0f,
                               Microsoft::Xna::Framework::Graphics::SpriteEffects::None,
                               1.0f - position.Y / 720.0f);
    }

private:
    static inline std::optional<SpriteFont> headerFont_;
    static inline std::optional<SpriteFont> playerNameFont_;
    static inline std::optional<SpriteFont> debugFont_;
    static inline std::optional<SpriteFont> buttonNamesFont_;
    static inline std::optional<SpriteFont> descriptionFont_;
    static inline std::optional<SpriteFont> gearInfoFont_;
    static inline std::optional<SpriteFont> damageFont_;
    static inline std::optional<SpriteFont> playerStatisticsFont_;
    static inline std::optional<SpriteFont> hudDetailFont_;
    static inline std::optional<SpriteFont> captionFont_;
};

} // namespace RolePlaying
