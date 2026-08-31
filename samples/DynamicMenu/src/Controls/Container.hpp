// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ContentReaders.hpp"
#include "Control.hpp"

namespace DynamicMenu::Controls
{
    /** @brief Holds controls at positions relative to this container. */
    class Container : public Control
    {
        friend void RegisterDynamicMenuContentReaders();

    public:
        /** @brief Gets the child controls. @return Mutable child list. */
        [[nodiscard]] std::vector<std::shared_ptr<IControl>>& getControlsProperty();
        /** @brief Gets the child controls. @return Child list. */
        [[nodiscard]] const std::vector<std::shared_ptr<IControl>>& getControlsProperty() const;
        /** @brief Adds a child immediately. @param control Child control. */
        void AddControl(std::shared_ptr<IControl> control);
        /** @brief Removes a child immediately. @param control Child control. */
        void RemoveControl(const std::shared_ptr<IControl>& control);
        /** @brief Marks a child for addition during the next update. @param control Child control. */
        void MarkForAdd(std::shared_ptr<IControl> control);
        /** @brief Marks a child for removal during the next update. @param control Child control. */
        void MarkForRemove(std::shared_ptr<IControl> control);
        /** @brief Initializes this container and every child control. */
        void Initialize() override;
        /** @brief Loads this container and every child control. @param graphics Graphics device. @param content Content manager. */
        void LoadContent(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& graphics,
            Microsoft::Xna::Framework::Content::ContentManager& content) override;
        /** @brief Updates visible children and applies deferred additions/removals. @param gameTime Current game time. @param gestures Gestures for the frame. */
        void Update(
            const Microsoft::Xna::Framework::GameTime& gameTime,
            const std::vector<Microsoft::Xna::Framework::Input::Touch::GestureSample>& gestures) override;
        /** @brief Draws this container and its visible children. @param gameTime Current game time. @param spriteBatch Sprite batch. */
        void Draw(
            const Microsoft::Xna::Framework::GameTime& gameTime,
            Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch) override;
        /** @brief Finds a direct child by name. @param name Name to find. @return Child or null. */
        [[nodiscard]] std::shared_ptr<IControl> FindControlByName(const std::string& name) const;
        /** @brief Gets the fully-qualified logical type name. @return Type name. */
        [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        std::vector<std::shared_ptr<IControl>> controls_;
        std::vector<std::shared_ptr<IControl>> markedForRemove_;
        std::vector<std::shared_ptr<IControl>> markedForAdd_;
    };
}
