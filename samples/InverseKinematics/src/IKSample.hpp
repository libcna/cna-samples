// SPDX-License-Identifier: MS-PL
#pragma once

#include "CNA/CNAHelper.hpp"
#include "Cat.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarRenderer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesComponent.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace InverseKinematicsSample
{
    using SharpRuntime::Single;

    /**
     * @brief Demonstrates Cyclic Coordinate Descent inverse kinematics on a cylinder chain
     * and an avatar model.
     */
    class IKSample : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Creates the inverse-kinematics sample. */
        IKSample();

        /**
         * @brief Updates the world and local transforms for a sorted bone hierarchy.
         *
         * @param worldTransforms World transforms to update.
         * @param localTransforms Local transforms to update.
         * @param rootWorldTransform Root bone world transform.
         * @param bindPose Bone hierarchy bind pose.
         * @param animationTransforms Transform offsets from the bind pose.
         * @param parentBones Parent index for every bone.
         */
        static void UpdateTransforms(
            std::vector<Microsoft::Xna::Framework::Matrix>& worldTransforms,
            std::vector<Microsoft::Xna::Framework::Matrix>& localTransforms,
            const Microsoft::Xna::Framework::Matrix& rootWorldTransform,
            const std::vector<Microsoft::Xna::Framework::Matrix>& bindPose,
            const std::vector<Microsoft::Xna::Framework::Matrix>& animationTransforms,
            const std::vector<int>& parentBones);

        /**
         * @brief Tests whether a game-pad button was just pressed.
         *
         * @param button Button to test.
         * @return True when the button changed from up to down.
         */
        [[nodiscard]] bool IsTriggered(Microsoft::Xna::Framework::Input::Buttons button) const;

        /**
         * @brief Tests whether a keyboard key was just pressed.
         *
         * @param key Key to test.
         * @return True when the key changed from up to down.
         */
        [[nodiscard]] bool IsTriggered(Microsoft::Xna::Framework::Input::Keys key) const;

        /**
         * @brief Tests whether a game-pad button is down.
         *
         * @param button Button to test.
         * @return True when the button is down.
         */
        [[nodiscard]] bool IsDown(Microsoft::Xna::Framework::Input::Buttons button) const;

        /**
         * @brief Tests whether a keyboard key is down.
         *
         * @param key Key to test.
         * @return True when the key is down.
         */
        [[nodiscard]] bool IsDown(Microsoft::Xna::Framework::Input::Keys key) const;

        /**
         * @brief Returns the fully-qualified managed type name.
         *
         * @return Type name.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Creates the IK chains for the avatar and cylinder chain. */
        void LoadContent() override;

        /**
         * @brief Updates game logic.
         *
         * @param gameTime Snapshot of timing values.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Draws the game.
         *
         * @param gameTime Snapshot of timing values.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        static constexpr int CylinderCount = 20;

        std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager> graphics;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch;
        std::optional<Microsoft::Xna::Framework::Graphics::Model> cylinderModel;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font;
        std::unique_ptr<Microsoft::Xna::Framework::GamerServices::GamerServicesComponent>
            gamerServices;

        Microsoft::Xna::Framework::Input::KeyboardState currentKeyboardState;
        Microsoft::Xna::Framework::Input::GamePadState currentGamePadState;
        Microsoft::Xna::Framework::Input::GamePadState prevGamePadState;
        Microsoft::Xna::Framework::Input::KeyboardState prevKeyboardState;

        Single cameraRotX = 0.0f;
        Single cameraRotY = 0.0f;
        Single cameraRadius = 5.0f;
        Microsoft::Xna::Framework::Matrix view;
        Microsoft::Xna::Framework::Matrix projection;

        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> catTexture;
        std::optional<Cat> cat;
        bool runSimulation = true;
        bool singleStep = false;

        std::unique_ptr<Microsoft::Xna::Framework::GamerServices::AvatarRenderer>
            avatarRenderer;
        std::vector<int> avatarBoneChain;
        std::vector<Microsoft::Xna::Framework::Matrix> avatarBoneTransforms;
        std::vector<Microsoft::Xna::Framework::Matrix> avatarWorldTransforms;
        std::vector<Microsoft::Xna::Framework::Matrix> avatarLocalTransforms;
        int avatarChainIndex = 1;
        bool isAvatarInitialized = false;

        std::vector<int> cylinderChain;
        std::vector<Microsoft::Xna::Framework::Matrix> cylinderChainBindPose;
        std::vector<Microsoft::Xna::Framework::Matrix> cylinderChainTransforms;
        std::vector<int> cylinderChainParentBones;
        std::vector<Microsoft::Xna::Framework::Matrix> cylinderWorldTransforms;
        std::vector<Microsoft::Xna::Framework::Matrix> cylinderLocalTransforms;
        Microsoft::Xna::Framework::Matrix cylinderRootWorldTransform =
            Microsoft::Xna::Framework::Matrix::getIdentityProperty();
        int cylinderChainIndex = 1;

        void LoadCylinderModel();
        void LoadAvatar();
        void InitializeCylinderChain();
        void UpdateCamera();
        void UpdateAvatarIK();
        void UpdateAvatarWorldTransforms();
        void AvatarLookAt(const Microsoft::Xna::Framework::Vector3& position);
        void UpdateCylinderChainIK();
        static void UpdateBone(
            std::vector<Microsoft::Xna::Framework::Matrix>& transforms,
            int curBone,
            int endEffector,
            const Microsoft::Xna::Framework::Vector3& goal,
            const std::vector<Microsoft::Xna::Framework::Matrix>& worldTransforms,
            const std::vector<Microsoft::Xna::Framework::Matrix>& localTransforms);
        void DrawHUD();
        void DrawXboxSpecificHUD();
        void DrawWindowsSpecificHUD();
        void DrawCylinderChain();
        void DrawAvatar();
        void DrawBones(Microsoft::Xna::Framework::Graphics::Model& model,
                       const Microsoft::Xna::Framework::Matrix& modelTransform,
                       int boneToColor,
                       const std::vector<int>& boneChain,
                       const std::vector<Microsoft::Xna::Framework::Matrix>& worldTransforms);
        void HandleInput(const Microsoft::Xna::Framework::GameTime& gameTime);
        void Reset();
    };
}
