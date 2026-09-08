#pragma once

// NetworkPlayer.hpp -- C++ port of Yacht/Objects/NetworkPlayer.cs.

#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "Objects/YachtPlayer.hpp"

namespace Yacht {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;

class YachtGame;

/**
 * @brief Another person, playing through the server.
 *
 * The client never simulates them: their moves arrive as game states from the server, so all
 * this player does on its own turn is say who everyone is waiting for.
 */
class NetworkPlayer : public YachtPlayer {
public:
    /**
     * @brief Creates a network player.
     *
     * @param name         The player's name.
     * @param screenBounds The screen the waiting message is centred in.
     */
    NetworkPlayer(std::string name, const Rectangle& screenBounds);

    /**
     * @brief Draws the waiting message.
     *
     * @param spriteBatch The batch to draw into; must already be begun.
     */
    void Draw(SpriteBatch& spriteBatch) override;

private:
    static inline const std::string text_ = "Waiting for network player";

    Vector2 position_;
};

} // namespace Yacht
