#pragma once

// ScreenManager.hpp -- C++ port of Yacht/ScreenManager/ScreenManager.cs.

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "CNA/Logger.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFile.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFileStream.hpp"

#include "GameScreen.hpp"
#include "InputState.hpp"

namespace GameStateManagement {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::DrawableGameComponent;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::Texture2D;

/**
 * @brief The screen manager is a component which manages one or more GameScreen instances.
 *
 * It maintains a stack of screens, calls their Update and Draw methods at the appropriate times,
 * and automatically routes input to the topmost active screen.
 *
 * @note **Screen serialization needs a registry where .NET needed none.** SerializeState writes
 * each screen's type name and DeserializeState brings them back with `Type.GetType` and
 * `Activator.CreateInstance`. C++ has no such lookup, so a game registers a factory per screen
 * name and the restore uses that; a name with no factory is skipped, which is what .NET does
 * when the type cannot be resolved. Nothing in Yacht calls either method -- the game persists
 * itself through YachtState instead -- but they are part of the class the sample ships.
 */
class ScreenManager : public DrawableGameComponent {
public:
    /** @brief Builds a screen of a given kind, for restoring a serialized screen list. */
    using ScreenFactory = std::function<std::shared_ptr<GameScreen>()>;

    /**
     * @brief Registers how to rebuild a screen of a given name.
     *
     * @param screenName The name SerializeState writes, which is the screen's own type name.
     * @param factory    Builds one.
     */
    static void RegisterScreenFactory(const std::string& screenName, ScreenFactory factory)
    {
        ScreenFactories()[screenName] = std::move(factory);
    }

    /** @brief This frame's input, read once and shared by every screen. */
    InputState input;

    /**
     * @brief A default SpriteBatch shared by all the screens.
     *
     * This saves each screen having to bother creating their own local instance.
     *
     * @return The batch.
     */
    [[nodiscard]] SpriteBatch& getSpriteBatchProperty() { return *spriteBatch_; }

    /**
     * @brief A blank white texture, used to tint arbitrary rectangles.
     *
     * @return The texture.
     */
    [[nodiscard]] Texture2D& getBlankTextureProperty() { return *blankTexture_; }

    /**
     * @brief A default font shared by all the screens.
     *
     * @return The font.
     */
    [[nodiscard]] SpriteFont& getFontProperty() { return *font_; }

    /**
     * @brief Whether the manager prints what screens it has after every update.
     *
     * @return True when tracing.
     */
    [[nodiscard]] bool getTraceEnabledProperty() const { return traceEnabled_; }

    /**
     * @brief Turns the screen trace on or off.
     *
     * @param value True to trace.
     */
    void setTraceEnabledProperty(bool value) { traceEnabled_ = value; }

    /**
     * @brief The area of the screen a title may safely draw in.
     *
     * @return The safe rectangle.
     */
    [[nodiscard]] Rectangle getSafeAreaProperty() const
    {
        return getGameProperty().getGraphicsDeviceProperty().getViewportProperty()
            .getTitleSafeAreaProperty();
    }

    /**
     * @brief Constructs a new screen manager component.
     *
     * @param game The game that owns it.
     */
    explicit ScreenManager(Game& game) : DrawableGameComponent(game)
    {
        Microsoft::Xna::Framework::Input::Touch::TouchPanel::setEnabledGesturesProperty(
            Microsoft::Xna::Framework::Input::Touch::GestureType::None);
    }

    /** @brief The type's name. @return "ScreenManager". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "ScreenManager";
        return name;
    }

    /** @brief Initializes the screen manager component. */
    void Initialize() override
    {
        DrawableGameComponent::Initialize();
        isInitialized_ = true;
    }

    /** @brief Load your graphics content. */
    void LoadContent() override
    {
        auto& content = getGameProperty().getContentProperty();

        spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        font_.emplace(content.template Load<SpriteFont>("Fonts/MenuFont"));
        blankTexture_.emplace(content.template Load<Texture2D>("Images/button"));

        // Tell each of the screens to load their content.
        for (const auto& screen : screens_) {
            screen->LoadContent();
        }
    }

    /** @brief Unload your graphics content. */
    void UnloadContent() override
    {
        // Tell each of the screens to unload their content.
        for (const auto& screen : screens_) {
            screen->UnloadContent();
        }
    }

    /**
     * @brief Allows each screen to run logic.
     *
     * @param gameTime The elapsed time.
     */
    void Update(GameTime& gameTime) override
    {
        // Read the keyboard and gamepad.
        input.Update();

        // Make a copy of the master screen list, to avoid confusion if the process of updating
        // one screen adds or removes others.
        screensToUpdate_.clear();
        for (const auto& screen : screens_) {
            screensToUpdate_.push_back(screen);
        }

        bool otherScreenHasFocus = !getGameProperty().getIsActiveProperty();
        bool coveredByOtherScreen = false;

        // Loop as long as there are screens waiting to be updated.
        while (!screensToUpdate_.empty()) {
            // Pop the topmost screen off the waiting list.
            const auto screen = screensToUpdate_.back();
            screensToUpdate_.pop_back();

            // Update the screen.
            screen->Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);

            if (screen->getScreenStateProperty() == ScreenState::TransitionOn ||
                screen->getScreenStateProperty() == ScreenState::Active) {
                // If this is the first active screen we came across, give it a chance to
                // handle input.
                if (!otherScreenHasFocus) {
                    screen->HandleInput(input);
                    otherScreenHasFocus = true;
                }

                // If this is an active non-popup, inform any subsequent screens that they are
                // covered by it.
                if (!screen->getIsPopupProperty()) {
                    coveredByOtherScreen = true;
                }
            }
        }

        // Print debug trace?
        if (traceEnabled_) {
            TraceScreens();
        }
    }

    /**
     * @brief Tells each screen to draw itself.
     *
     * @param gameTime The elapsed time.
     */
    void Draw(const GameTime& gameTime) override
    {
        for (const auto& screen : screens_) {
            if (screen->getScreenStateProperty() == ScreenState::Hidden) {
                continue;
            }

            screen->Draw(gameTime);
        }
    }

    /**
     * @brief Adds a new screen to the screen manager.
     *
     * @param screen            The screen to add.
     * @param controllingPlayer The player who owns it, or nothing for any player.
     */
    void AddScreen(const std::shared_ptr<GameScreen>& screen,
                   const std::optional<PlayerIndex>& controllingPlayer)
    {
        screen->setControllingPlayerProperty(controllingPlayer);
        screen->setScreenManagerProperty(this);
        screen->setIsExitingProperty(false);

        // If we have a graphics device, tell the screen to load content.
        if (isInitialized_) {
            screen->LoadContent();
        }

        screens_.push_back(screen);

        // Update the TouchPanel to respond to gestures this screen is interested in.
        Microsoft::Xna::Framework::Input::Touch::TouchPanel::setEnabledGesturesProperty(
            screen->getEnabledGesturesProperty());
    }

    /**
     * @brief Removes a screen from the screen manager.
     *
     * You should normally use GameScreen.ExitScreen instead of calling this directly, so the
     * screen can gradually transition off rather than just being instantly removed.
     *
     * @param screen The screen to remove.
     */
    void RemoveScreen(GameScreen* screen)
    {
        // If we have a graphics device, tell the screen to unload content.
        if (isInitialized_) {
            screen->UnloadContent();
        }

        EraseByPointer(screens_, screen);
        EraseByPointer(screensToUpdate_, screen);

        // If there is a screen still in the manager, update TouchPanel to respond to gestures
        // that screen is interested in.
        if (!screens_.empty()) {
            Microsoft::Xna::Framework::Input::Touch::TouchPanel::setEnabledGesturesProperty(
                screens_.back()->getEnabledGesturesProperty());
        }
    }

    /**
     * @brief Expose an array holding all the screens.
     *
     * We return a copy rather than the real master list, because screens should only ever be
     * added or removed using the AddScreen and RemoveScreen methods.
     *
     * @return The screens, bottom first.
     */
    [[nodiscard]] std::vector<std::shared_ptr<GameScreen>> GetScreens() const { return screens_; }

    /**
     * @brief Helper draws a translucent black fullscreen sprite, used for fading screens in and
     *        out, and for darkening the background behind popups.
     *
     * @param alpha How dark, from zero to one.
     */
    void FadeBackBufferToBlack(float alpha)
    {
        const auto& viewport = getGraphicsDeviceProperty().getViewportProperty();

        spriteBatch_->Begin();
        spriteBatch_->Draw(*blankTexture_,
                           Rectangle(0, 0, viewport.getWidthProperty(),
                                     viewport.getHeightProperty()),
                           Color::Black * alpha);
        spriteBatch_->End();
    }

    /** @brief Informs the screen manager to serialize its state to disk. */
    void SerializeState();

    /**
     * @brief Attempts to deserialize the screen manager from disk.
     *
     * @return True when a state was found and restored.
     */
    bool DeserializeState();

private:
    static std::map<std::string, ScreenFactory>& ScreenFactories()
    {
        static std::map<std::string, ScreenFactory> value;
        return value;
    }

    static void EraseByPointer(std::vector<std::shared_ptr<GameScreen>>& screens, GameScreen* screen)
    {
        screens.erase(std::remove_if(screens.begin(), screens.end(),
                                     [screen](const std::shared_ptr<GameScreen>& candidate) {
                                         return candidate.get() == screen;
                                     }),
                      screens.end());
    }

    // Prints a list of all the screens, for debugging.
    void TraceScreens() const
    {
        std::string names;
        for (const auto& screen : screens_) {
            if (!names.empty()) {
                names += ", ";
            }
            names += screen->GetTypeName();
        }
        CNA::Logger::Info(names);
    }

    void DeleteState(System::IO::IsolatedStorage::IsolatedStorageFile& storage) const;

    std::vector<std::shared_ptr<GameScreen>> screens_;
    std::vector<std::shared_ptr<GameScreen>> screensToUpdate_;

    std::unique_ptr<SpriteBatch> spriteBatch_;
    std::optional<SpriteFont> font_;
    std::optional<Texture2D> blankTexture_;

    bool isInitialized_ = false;
    bool traceEnabled_ = false;
};

} // namespace GameStateManagement

namespace Yacht {

using GameStateManagement::ScreenManager;

} // namespace Yacht
