// SPDX-License-Identifier: MS-PL

#include "Font.hpp"

#include "SpacewarGame.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/Int32.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    std::unique_ptr<SpriteBatch> Font::batch_;
    const std::array<Font::FontInfo, 9> Font::fontInfo_{
        FontInfo{"fonts/in-game_score", "0123456789", 0, 60, 58, 100},
        FontInfo{"fonts/weapon_large_font", "0123456789$,ptsx=", 0, 20, 18, 35},
        FontInfo{"fonts/weapon_small_font", "0123456789$\x2c", 0, 15, 13, 30},
        FontInfo{"fonts/ingame_counter", "0123456789:", 0, 30, 24, 70},
        FontInfo{"fonts/in-game_player_text", "12", 0, 120, 120, 30},
        FontInfo{"fonts/hud_round_button", "01", 0, 28, 28, 22},
        FontInfo{"fonts/hud_weapon_icons", "0123456789", 0, 150, 150, 150},
        FontInfo{"fonts/health", "54321", 0, 50, 50, 70},
        FontInfo{"fonts/Ship_names", "012", 0, 200, 200, 30},
    };

    void Font::Init(Game* game)
    {
        if (game && !batch_)
            batch_ = std::make_unique<SpriteBatch>(game->getGraphicsDeviceProperty());
    }

    void Font::Dispose()
    {
        if (batch_)
            batch_->Dispose();
        batch_.reset();
    }

    void Font::Begin() { batch_->Begin(SpriteSortMode::Texture, BlendState::AlphaBlend); }
    void Font::End() { batch_->End(); }

    void Font::Draw(FontStyle fontStyle, int x, int y, int number, Vector4 color)
    {
        Draw(fontStyle, x, y, System::Int32::ToString(number), color);
    }

    void Font::Draw(FontStyle fontStyle, int x, int y, int number)
    {
        Draw(fontStyle, x, y, System::Int32::ToString(number), Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    void Font::Draw(FontStyle fontStyle, int x, int y, const std::string& digits)
    {
        Draw(fontStyle, x, y, digits, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    void Font::Draw(FontStyle fontStyle, int x, int y, const std::string& digits, Vector4 color)
    {
        float xPosition = static_cast<float>(x);
        const FontInfo& fontInfo = fontInfo_[static_cast<std::size_t>(fontStyle)];
        for (char digit : digits)
        {
            if (digit != ' ')
            {
                const std::size_t character = fontInfo.Characters.find(digit);
                auto texture = SpacewarGame::getContentManagerProperty().Load<Texture2D>(
                    SpacewarGame::getSettingsProperty().MediaPath + fontInfo.Filename);
                batch_->Draw(texture, Vector2(xPosition, static_cast<float>(y)),
                             Rectangle(static_cast<int>(character) * fontInfo.CharacterSpacing + fontInfo.StartOffset,
                                       0, fontInfo.CharacterWidth, fontInfo.CharacterHeight),
                             Color(color));
            }
            xPosition += static_cast<float>((digit == ',' || digit == ':')
                                                ? fontInfo.CharacterWidth / 2
                                                : fontInfo.CharacterWidth);
        }
    }
}
