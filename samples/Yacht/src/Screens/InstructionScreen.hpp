#pragma once

// InstructionScreen.hpp -- C++ port of Yacht/Screens/InstructionScreen.cs.

#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/TimeSpan.hpp"

#include "../Misc/StringUtility.hpp"
#include "../ScreenManager/ScreenBodies.hpp"

namespace Yacht {

using Microsoft::Xna::Framework::GamerServices::Guide;

/**
 * @brief The rules, shown before a game starts.
 *
 * It is also where a new game asks the player's name, which is why it takes a flag rather than
 * being a plain "tap to continue" page: the same screen serves a fresh game and a loaded one.
 */
class InstructionScreen : public GameScreen {
public:
    /** @brief The type's name. @return "InstructionScreen". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "InstructionScreen";
        return name;
    }

    /**
     * @brief Creates the screen.
     *
     * @param askName True to ask the player's name before starting.
     */
    explicit InstructionScreen(bool askName) : askName_(askName)
    {
        setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(0.0));
        setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(0.5));
        setEnabledGesturesProperty(GestureType::Tap);
    }

    /** @brief Load graphics content for the screen. */
    void LoadContent() override
    {
        background_.emplace(Load<Texture2D>("Images/instruction"));
        font_.emplace(Load<SpriteFont>("Fonts/MenuFont"));
    }

    /**
     * @brief Handles the player's input.
     *
     * @param input This frame's input.
     */
    void HandleInput(InputState& input) override;

    /**
     * @brief Runs the screen's logic.
     *
     * @param gameTime             The elapsed time.
     * @param otherScreenHasFocus  Whether another screen has focus.
     * @param coveredByOtherScreen Whether another screen covers this one.
     */
    void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen) override;

    /**
     * @brief Draws the rules, and "Loading..." once the game is on its way.
     *
     * @param gameTime The elapsed time.
     */
    void Draw(const GameTime& gameTime) override;

private:
    void EnterNameDialogEnded(System::IAsyncResult& result);

    std::optional<Texture2D> background_;
    std::optional<SpriteFont> font_;
    bool isExit_ = false;
    bool screenExited_ = false;
    std::string name_;
    bool askName_ = false;
    bool isInvalidName_ = false;
    std::string invalidName_;
};

} // namespace Yacht
