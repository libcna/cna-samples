// SPDX-License-Identifier: MS-PL
#pragma once

#include <any>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"

#include "IControl.hpp"

namespace DynamicMenu::Transitions { class Transition; }

namespace DynamicMenu::Controls
{
    /** @brief Base class for all dynamic-menu controls. */
    class Control : public IControl
    {
    public:
        /** @brief Constructs a visible, white control. */
        Control();
        /** @brief Destroys the abstract base class. */
        ~Control() override = 0;

        /** @brief Gets the left coordinate. @return Coordinate in parent space. */
        [[nodiscard]] int getLeftProperty() const override;
        /** @brief Sets the left coordinate. @param value Coordinate in parent space. */
        void setLeftProperty(int value) override;
        /** @brief Gets the top coordinate. @return Coordinate in parent space. */
        [[nodiscard]] int getTopProperty() const override;
        /** @brief Sets the top coordinate. @param value Coordinate in parent space. */
        void setTopProperty(int value) override;
        /** @brief Gets the width. @return Width in pixels. */
        [[nodiscard]] int getWidthProperty() const override;
        /** @brief Sets the width. @param value Width in pixels. */
        void setWidthProperty(int value) override;
        /** @brief Gets the height. @return Height in pixels. */
        [[nodiscard]] int getHeightProperty() const override;
        /** @brief Sets the height. @param value Height in pixels. */
        void setHeightProperty(int value) override;
        /** @brief Gets the bottom coordinate. @return Coordinate in parent space. */
        [[nodiscard]] int getBottomProperty() const override;
        /** @brief Gets the right coordinate. @return Coordinate in parent space. */
        [[nodiscard]] int getRightProperty() const override;
        /** @brief Gets the control name. @return Name used for lookup. */
        [[nodiscard]] const std::string& getNameProperty() const override;
        /** @brief Sets the control name. @param value Name used for lookup. */
        void setNameProperty(std::string value) override;
        /** @brief Gets the background texture asset name. @return Asset name. */
        [[nodiscard]] const std::string& getBackTextureNameProperty() const;
        /** @brief Sets the background texture asset name. @param value Asset name. */
        void setBackTextureNameProperty(std::string value);
        /** @brief Gets the loaded background texture. @return Texture or null. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::Texture2D* getBackTextureProperty();
        /** @brief Gets whether the control is visible. @return Visibility. */
        [[nodiscard]] bool getVisibleProperty() const;
        /** @brief Sets whether the control is visible. @param value Visibility. */
        void setVisibleProperty(bool value);
        /** @brief Gets the control tint. @return Tint color. */
        [[nodiscard]] Microsoft::Xna::Framework::Color getHueProperty() const override;
        /** @brief Sets the control tint. @param value Tint color. */
        void setHueProperty(Microsoft::Xna::Framework::Color value) override;
        /** @brief Gets the parent control. @return Non-owning parent pointer. */
        [[nodiscard]] IControl* getParentProperty() const override;
        /** @brief Sets the parent control. @param value Non-owning parent pointer. */
        void setParentProperty(IControl* value) override;
        /** @brief Gets arbitrary caller data. @return Tag value. */
        [[nodiscard]] std::any& getTagProperty();
        /** @brief Gets arbitrary caller data. @return Tag value. */
        [[nodiscard]] const std::any& getTagProperty() const;
        /** @brief Sets arbitrary caller data. @param value Tag value. */
        void setTagProperty(std::any value);

        /** @brief Initializes the control. */
        virtual void Initialize();
        /** @brief Loads content. @param graphics Graphics device. @param content Content manager. */
        virtual void LoadContent(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& graphics,
            Microsoft::Xna::Framework::Content::ContentManager& content);
        /** @brief Advances transitions. @param gameTime Current game time. @param gestures Gestures for the frame. */
        void Update(
            const Microsoft::Xna::Framework::GameTime& gameTime,
            const std::vector<Microsoft::Xna::Framework::Input::Touch::GestureSample>& gestures) override;
        /** @brief Draws the control. @param gameTime Current time. @param spriteBatch Sprite batch. */
        virtual void Draw(
            const Microsoft::Xna::Framework::GameTime& gameTime,
            Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch);
        /** @brief Gets the current background texture. @return Texture or null. */
        [[nodiscard]] virtual Microsoft::Xna::Framework::Graphics::Texture2D* GetCurrTexture();
        /** @brief Gets the absolute upper-left coordinate. @return Screen-space coordinate. */
        [[nodiscard]] Microsoft::Xna::Framework::Point GetAbsoluteTopLeft() const override;
        /** @brief Gets the absolute rectangle. @return Screen-space rectangle. */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle GetAbsoluteRect() const;
        /** @brief Starts a transition. @param transition Transition to apply. */
        void ApplyTransition(std::shared_ptr<DynamicMenu::Transitions::Transition> transition);

    protected:
        /** @brief Draws centered text. @param spriteBatch Sprite batch. @param font Font. @param rect Bounds. @param text Text. @param color Color. */
        void DrawCenteredText(
            Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch,
            Microsoft::Xna::Framework::Graphics::SpriteFont* font,
            Microsoft::Xna::Framework::Rectangle rect,
            const std::string& text,
            Microsoft::Xna::Framework::Color color);
        /** @brief Tests a screen position. @param pos Position. @return True when contained. */
        [[nodiscard]] bool ContainsPos(Microsoft::Xna::Framework::Vector2 pos) const;

    private:
        std::vector<std::shared_ptr<DynamicMenu::Transitions::Transition>> activeTransitions_;
        int left_ = 0;
        int top_ = 0;
        int width_ = 0;
        int height_ = 0;
        std::string name_;
        std::string backTextureName_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> backTexture_;
        bool visible_ = true;
        Microsoft::Xna::Framework::Color hue_ = Microsoft::Xna::Framework::Color::White;
        IControl* parent_ = nullptr;
        std::any tag_;
    };
}
