#pragma once

// Dice.hpp -- C++ port of Yacht/Objects/Dice.cs.

#include <array>
#include <atomic>
#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/EventArgs.hpp"
#include "System/EventHandler.hpp"
#include "System/IComparable.hpp"
#include "System/Object.hpp"
#include "System/Random.hpp"
#include "System/Threading/Timer.hpp"

namespace Yacht {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Input::Touch::GestureSample;
using Microsoft::Xna::Framework::Input::Touch::GestureType;

/** @brief Possible die values. */
enum class DiceValue : SharpRuntime::bytecs {
    /** @brief A single pip. */
    One = 1,
    /** @brief Two pips. */
    Two = 2,
    /** @brief Three pips. */
    Three = 3,
    /** @brief Four pips. */
    Four = 4,
    /** @brief Five pips. */
    Five = 5,
    /** @brief Six pips. */
    Six = 6
};

/**
 * @brief A six-sided die which can draw itself on screen.
 *
 * @note **The roll is driven by a timer on another thread, as the original's is.** Update()
 * does not animate anything; it arms a `System::Threading::Timer` that fires once, 300 to 600
 * milliseconds later, gives the die a new random face, and keeps it rolling with probability
 * four in five. Update() then arms the next one. That is where the tumbling look comes from,
 * and it is why a die stops after an unpredictable number of steps rather than a fixed one --
 * a frame-counted animation would roll for a constant time and look wrong.
 *
 * The face and the rolling flag are therefore written from the timer thread and read from the
 * game thread, so both are atomic here. The original simply races; C++ makes that undefined
 * rather than merely unpredictable, and the observable behaviour is identical either way.
 */
class Dice : public System::Object, public System::IComparable<Dice> {
public:
    /**
     * @brief Loads assets that will be used by all instances.
     *
     * @param content The content manager to load through.
     */
    static void LoadAssets(ContentManager& content)
    {
        diceStrip_.emplace(content.Load<Texture2D>("Images/dice"));

        // Create rectangles which designate the position of individual die faces in the dice
        // strip texture which contains all faces
        for (std::size_t i = 0; i < dice_.size(); i++) {
            dice_[i] = Rectangle(static_cast<int>(i) * diceStrip_->getWidthProperty() /
                                     static_cast<int>(dice_.size()),
                                 0,
                                 diceStrip_->getWidthProperty() / static_cast<int>(dice_.size()),
                                 diceStrip_->getHeightProperty());
        }
    }

    /** @brief Raised when the die is tapped. */
    System::EventHandler<System::EventArgs> Click;

    /** @brief Creates a die showing one. */
    Dice() = default;

    /** @brief Stops the die's timer and destroys it. */
    ~Dice() override
    {
        timer_.reset();
    }

    Dice(const Dice&) = delete;
    Dice& operator=(const Dice&) = delete;

    /** @brief The type's name. @return "Dice". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "Dice";
        return name;
    }

    /**
     * @brief The die's face.
     *
     * @return The value showing.
     */
    [[nodiscard]] DiceValue getValueProperty() const
    {
        return value_.load(std::memory_order_relaxed);
    }

    /**
     * @brief Sets the die's face.
     *
     * @param value The value to show.
     */
    void setValueProperty(DiceValue value)
    {
        value_.store(value, std::memory_order_relaxed);
    }

    /**
     * @brief Where on screen the die is drawn.
     *
     * @return The position.
     */
    [[nodiscard]] Vector2 getPositionProperty() const { return position_; }

    /**
     * @brief Sets where on screen the die is drawn.
     *
     * @param value The position.
     */
    void setPositionProperty(const Vector2& value) { position_ = value; }

    /**
     * @brief Whether the die is still tumbling.
     *
     * @return True while the die is rolling.
     */
    [[nodiscard]] bool getIsRollingProperty() const
    {
        return isRolling_.load(std::memory_order_relaxed);
    }

    /** @brief Arms the next step of the roll, if one is due. */
    void Update()
    {
        // Check if need to roll and skip some calls to make a delay effect
        // of the rolling.
        if (getIsRollingProperty() && timer_ == nullptr) {
            timer_ = std::make_unique<System::Threading::Timer>(
                [](void* state) { static_cast<Dice*>(state)->RandomizeDiceMotion(); }, this,
                random_.Next(300, 600), -1);
        }
    }

    /**
     * @brief Raises Click when the gesture taps this die.
     *
     * @param sample The gesture to handle.
     */
    void HandleInput(const GestureSample& sample)
    {
        if (sample.getGestureTypeProperty() == GestureType::Tap) {
            // Create the touch rectangle
            const Rectangle touchRect(static_cast<int>(sample.getPositionProperty().X) - 1,
                                      static_cast<int>(sample.getPositionProperty().Y) - 1, 2, 2);

            // Create the die bounds rectangle
            Rectangle bounds = dice_[0];
            bounds.X += static_cast<int>(position_.X);
            bounds.Y += static_cast<int>(position_.Y);

            // Check for intersection between the rectangles
            if (bounds.Intersects(touchRect)) {
                System::EventArgs empty;
                Click.Raise(this, empty);
            }
        }
    }

    /**
     * @brief Draws the die.
     *
     * @param spriteBatch The batch to draw into; must already be begun.
     */
    void Draw(SpriteBatch& spriteBatch) const
    {
        // Create the destination rectangle according to the die's position
        Rectangle bounds = dice_[0];
        bounds.X += static_cast<int>(position_.X);
        bounds.Y += static_cast<int>(position_.Y);

        // Draw the die on the screen
        spriteBatch.Draw(*diceStrip_, bounds,
                         dice_[static_cast<std::size_t>(getValueProperty()) - 1], Color::White);
    }

    /** @brief Starts the die rolling. */
    void Roll() { isRolling_.store(true, std::memory_order_relaxed); }

    /**
     * @brief Orders this die against another by face value.
     *
     * @param other The die to compare against.
     * @return 1 when this die shows more, -1 when it shows less, 0 when they match.
     */
    [[nodiscard]] int CompareTo(const Dice& other) const override
    {
        if (static_cast<int>(getValueProperty()) > static_cast<int>(other.getValueProperty())) {
            return 1;
        }
        if (static_cast<int>(getValueProperty()) < static_cast<int>(other.getValueProperty())) {
            return -1;
        }
        return 0;
    }

    /**
     * @brief Whether a rectangle overlaps the die.
     *
     * @param rect The rectangle to test.
     * @return True when they intersect.
     */
    [[nodiscard]] bool Intersects(const Rectangle& rect) const
    {
        // Get the die's bounds
        Rectangle bounds = dice_[0];
        bounds.X += static_cast<int>(position_.X);
        bounds.Y += static_cast<int>(position_.Y);

        // Return the intersection result
        return bounds.Intersects(rect);
    }

private:
    void RandomizeDiceMotion()
    {
        timer_.reset();
        setValueProperty(static_cast<DiceValue>(random_.Next(1, 7)));
        isRolling_.store(random_.Next(0, 5) != 1, std::memory_order_relaxed);
    }

    // One generator shared by every die, as the original's static Random is.
    static inline System::Random random_;
    static inline std::optional<Texture2D> diceStrip_;
    static inline std::array<Rectangle, 6> dice_{};

    std::unique_ptr<System::Threading::Timer> timer_;
    std::atomic<DiceValue> value_{DiceValue::One};
    std::atomic<bool> isRolling_{false};
    Vector2 position_;
};

} // namespace Yacht
