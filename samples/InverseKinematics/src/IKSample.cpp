// SPDX-License-Identifier: MS-PL

#include "IKSample.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarBone.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarDescription.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarExpression.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarRendererState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include <cmath>
#include <cstddef>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::GamerServices;
using namespace Microsoft::Xna::Framework::Graphics;
using namespace Microsoft::Xna::Framework::Input;

namespace InverseKinematicsSample
{
    IKSample::IKSample()
    {
        graphics = std::make_unique<GraphicsDeviceManager>(this);
        graphics->setPreferredBackBufferWidthProperty(853);
        graphics->setPreferredBackBufferHeightProperty(480);
        graphics->setPreferMultiSamplingProperty(true);

        gamerServices = std::make_unique<GamerServicesComponent>(*this);
        getComponentsProperty().Add(gamerServices.get());

        getContentProperty().setRootDirectoryProperty("Content");
    }

    const std::string& IKSample::GetTypeName() const
    {
        static const std::string name = "InverseKinematicsSample.IKSample";
        return name;
    }

    void IKSample::LoadContent()
    {
        spriteBatch.emplace(getGraphicsDeviceProperty());
        font.emplace(getContentProperty().Load<SpriteFont>("font"));

        LoadCylinderModel();

        catTexture.emplace(getContentProperty().Load<Texture2D>("cat"));
        cat.emplace(getGraphicsDeviceProperty());
        cat->setScaleProperty(0.3f);
        cat->setPositionProperty(Vector3(-1.0f, 0.25f, -2.0f));
        cat->setTextureProperty(&catTexture.value());

        LoadAvatar();
        InitializeCylinderChain();
    }

    void IKSample::LoadCylinderModel()
    {
        cylinderModel.emplace(getContentProperty().Load<Model>("cylinder"));

        for (ModelMesh* mesh : cylinderModel->getMeshesProperty())
        {
            for (Effect* effect : mesh->getEffectsProperty())
            {
                if (auto* basicEffect = dynamic_cast<BasicEffect*>(effect))
                {
                    basicEffect->EnableDefaultLighting();
                    basicEffect->setPreferPerPixelLightingProperty(true);
                }
            }
        }
    }

    void IKSample::LoadAvatar()
    {
        AvatarDescription description = AvatarDescription::CreateRandom();
        avatarRenderer = std::make_unique<AvatarRenderer>(&description, true);

        avatarBoneChain.clear();
        avatarBoneChain.push_back(static_cast<int>(AvatarBone::FingerMiddle3Left));
        avatarBoneChain.push_back(static_cast<int>(AvatarBone::WristLeft));
        avatarBoneChain.push_back(static_cast<int>(AvatarBone::ElbowLeft));
        avatarBoneChain.push_back(static_cast<int>(AvatarBone::ShoulderLeft));

        const int boneCount = AvatarRenderer::BoneCount;
        avatarBoneTransforms.assign(boneCount, Matrix::getIdentityProperty());
        avatarWorldTransforms = avatarBoneTransforms;
        avatarLocalTransforms = avatarBoneTransforms;

        avatarBoneTransforms[static_cast<int>(AvatarBone::ShoulderRight)] =
            Matrix::CreateRotationZ(MathHelper::ToRadians(80.0f));
        avatarRenderer->setWorldProperty(Matrix::CreateTranslation(1.0f, 0.0f, 0.0f));
    }

    void IKSample::InitializeCylinderChain()
    {
        const Matrix translation = Matrix::CreateTranslation(0.0f, 0.1f, 0.0f);
        cylinderChainBindPose.assign(CylinderCount, translation);

        cylinderChainTransforms.assign(CylinderCount, Matrix::getIdentityProperty());
        cylinderWorldTransforms = cylinderChainTransforms;
        cylinderLocalTransforms = cylinderChainTransforms;

        cylinderChainParentBones.clear();
        cylinderChainParentBones.reserve(CylinderCount);
        for (int i = 0; i < CylinderCount; ++i)
        {
            cylinderChainParentBones.push_back(i - 1);
        }

        cylinderChain.clear();
        cylinderChain.reserve(CylinderCount);
        for (int i = CylinderCount - 1; i >= 0; --i)
        {
            cylinderChain.push_back(i);
        }

        UpdateTransforms(cylinderWorldTransforms, cylinderLocalTransforms,
                         cylinderRootWorldTransform, cylinderChainBindPose,
                         cylinderChainTransforms, cylinderChainParentBones);
    }

    void IKSample::Update(GameTime& gameTime)
    {
        HandleInput(gameTime);
        UpdateCamera();

        if (runSimulation || IsTriggered(Buttons::B) || IsTriggered(Keys::Space))
        {
            UpdateAvatarIK();
            UpdateCylinderChainIK();
        }

        Game::Update(gameTime);
    }

    void IKSample::UpdateCamera()
    {
        view = Matrix::CreateRotationY(MathHelper::ToRadians(cameraRotY))
             * Matrix::CreateRotationX(MathHelper::ToRadians(cameraRotX))
             * Matrix::CreateLookAt(
                   Vector3(0.0f, 0.0f, -cameraRadius), Vector3::Zero, Vector3::Up);

        const Single aspectRatio =
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty();
        projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::ToRadians(45.0f), aspectRatio, 1.0f, 1000.0f);
    }

    void IKSample::UpdateAvatarIK()
    {
        if (avatarRenderer->getStateProperty() != AvatarRendererState::Ready)
        {
            isAvatarInitialized = false;
        }
        else
        {
            if (!isAvatarInitialized)
            {
                UpdateAvatarWorldTransforms();
                isAvatarInitialized = true;
            }

            AvatarLookAt(cat->getPositionProperty());

            while (avatarChainIndex < static_cast<int>(avatarBoneChain.size()))
            {
                UpdateBone(avatarBoneTransforms, avatarBoneChain[avatarChainIndex],
                           avatarBoneChain[0], cat->getPositionProperty(),
                           avatarWorldTransforms, avatarLocalTransforms);
                UpdateAvatarWorldTransforms();
                ++avatarChainIndex;
                if (singleStep)
                {
                    break;
                }
            }

            if (avatarChainIndex >= static_cast<int>(avatarBoneChain.size()))
            {
                avatarChainIndex = 1;
            }
        }
    }

    void IKSample::UpdateAvatarWorldTransforms()
    {
        const auto bindPoseCollection = avatarRenderer->getBindPoseProperty();
        const auto parentBonesCollection = avatarRenderer->getParentBonesProperty();
        const std::vector<Matrix> bindPose(
            bindPoseCollection.begin(), bindPoseCollection.end());
        const std::vector<int> parentBones(
            parentBonesCollection.begin(), parentBonesCollection.end());
        UpdateTransforms(avatarWorldTransforms, avatarLocalTransforms,
                         avatarRenderer->getWorldProperty(), bindPose,
                         avatarBoneTransforms, parentBones);
    }

    void IKSample::AvatarLookAt(const Vector3& position)
    {
        const int headIndex = static_cast<int>(AvatarBone::Head);
        Vector3 target = position - avatarWorldTransforms[headIndex].getTranslationProperty();
        target.X = -target.X;
        avatarBoneTransforms[headIndex] =
            Matrix::CreateLookAt(Vector3::Zero, target, Vector3::Up);
    }

    void IKSample::UpdateCylinderChainIK()
    {
        while (cylinderChainIndex < static_cast<int>(cylinderChain.size()))
        {
            UpdateBone(cylinderChainTransforms, cylinderChain[cylinderChainIndex],
                       cylinderChain[0], cat->getPositionProperty(),
                       cylinderWorldTransforms, cylinderLocalTransforms);
            UpdateTransforms(cylinderWorldTransforms, cylinderLocalTransforms,
                             cylinderRootWorldTransform, cylinderChainBindPose,
                             cylinderChainTransforms, cylinderChainParentBones);
            ++cylinderChainIndex;
            if (singleStep)
            {
                break;
            }
        }

        if (cylinderChainIndex >= static_cast<int>(cylinderChain.size()))
        {
            cylinderChainIndex = 1;
        }
    }

    void IKSample::UpdateBone(std::vector<Matrix>& transforms, const int curBone,
                              const int endEffector, const Vector3& goal,
                              const std::vector<Matrix>& worldTransforms,
                              const std::vector<Matrix>& localTransforms)
    {
        const Matrix curBoneWorld = worldTransforms[curBone];
        Vector3 goalInBoneSpace = Vector3::Transform(goal, Matrix::Invert(curBoneWorld));

        const Matrix endEffectorWorld = worldTransforms[endEffector];
        Vector3 endEffectorInBoneSpace =
            (endEffectorWorld * Matrix::Invert(curBoneWorld)).getTranslationProperty();

        endEffectorInBoneSpace.Normalize();
        goalInBoneSpace.Normalize();

        Vector3 axis = Vector3::Cross(endEffectorInBoneSpace, goalInBoneSpace);
        axis = Vector3::TransformNormal(axis, localTransforms[curBone]);
        axis.Normalize();

        Single dot = Vector3::Dot(goalInBoneSpace, endEffectorInBoneSpace);
        dot = MathHelper::Clamp(dot, -1.0f, 1.0f);
        Single angle = static_cast<Single>(std::acos(dot));
        angle = MathHelper::WrapAngle(angle);

        const Matrix rotation = Matrix::CreateFromAxisAngle(axis, angle);
        transforms[curBone] = transforms[curBone] * rotation;
    }

    void IKSample::UpdateTransforms(std::vector<Matrix>& worldTransforms,
                                    std::vector<Matrix>& localTransforms,
                                    const Matrix& rootWorldTransform,
                                    const std::vector<Matrix>& bindPose,
                                    const std::vector<Matrix>& animationTransforms,
                                    const std::vector<int>& parentBones)
    {
        localTransforms[0] = animationTransforms[0] * bindPose[0];
        worldTransforms[0] = localTransforms[0] * rootWorldTransform;

        for (std::size_t curBone = 1; curBone < worldTransforms.size(); ++curBone)
        {
            const Matrix local = animationTransforms[curBone] * bindPose[curBone];
            const Matrix parentMatrix =
                worldTransforms[static_cast<std::size_t>(parentBones[curBone])];
            localTransforms[curBone] = local;
            worldTransforms[curBone] = local * parentMatrix;
        }
    }

    void IKSample::Draw(const GameTime& gameTime)
    {
        static_cast<void>(gameTime);
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);
        getGraphicsDeviceProperty().setBlendStateProperty(BlendState::AlphaBlend);
        getGraphicsDeviceProperty().setDepthStencilStateProperty(DepthStencilState::Default);
        DrawCylinderChain();
        DrawAvatar();

        const Vector3 cameraPosition = Matrix::Invert(view).getTranslationProperty();
        cat->Draw(cameraPosition, view, projection);
        DrawHUD();
    }

    void IKSample::DrawHUD()
    {
#if defined(XBOX)
        DrawXboxSpecificHUD();
#else
        DrawWindowsSpecificHUD();
#endif
    }

    void IKSample::DrawXboxSpecificHUD()
    {
        std::string pausedText = runSimulation ? "Simulation: Running" : "Simulation: Paused";
        pausedText += "\nPress 'A' to toggle";
        std::string singleStepText = singleStep ? "Single Step is: ON" : "Single Step is: OFF";
        singleStepText += "\nPress 'Start' to toggle";
        const std::string stepThrough =
            singleStep || !runSimulation ? "\nPress 'B' to step once" : "";
        const std::string controlsText =
            "-Controls-\n"
            "Move camera: Right Thumbstick\n"
            "Zoom camera: Left/Right Trigger\n"
            "Move cat: Left Thumbstick\n"
            "Zoom cat: X/Y Button\n"
            "Reset: Thumbstick Down";

        spriteBatch->Begin();
        spriteBatch->DrawString(*font, pausedText, Vector2(100.0f, 80.0f), Color::White);
        spriteBatch->DrawString(*font, singleStepText, Vector2(100.0f, 120.0f), Color::White);
        spriteBatch->DrawString(*font, stepThrough, Vector2(100.0f, 160.0f), Color::White);
        spriteBatch->DrawString(*font, controlsText, Vector2(100.0f, 300.0f), Color::White);
        spriteBatch->End();
    }

    void IKSample::DrawWindowsSpecificHUD()
    {
        std::string pausedText = runSimulation ? "Simulation: Running" : "Simulation: Paused";
        pausedText += "\nPress 'P' to toggle";
        std::string singleStepText = singleStep ? "Single Step is: ON" : "Single Step is: OFF";
        singleStepText += "\nPress 'Enter' to toggle";
        const std::string stepThrough =
            singleStep || !runSimulation ? "\nPress 'Space' to step once" : "";
        const std::string controlsText =
            "-Controls-\n"
            "Move camera: Arrow Keys\n"
            "Zoom camera: Z/X Key\n"
            "Move cat: W,A,S,D Keys\n"
            "Zoom cat: Q/E Key\n"
            "Reset: R Key";

        spriteBatch->Begin();
        spriteBatch->DrawString(*font, pausedText, Vector2(100.0f, 80.0f), Color::White);
        spriteBatch->DrawString(*font, singleStepText, Vector2(100.0f, 120.0f), Color::White);
        spriteBatch->DrawString(*font, stepThrough, Vector2(100.0f, 160.0f), Color::White);
        spriteBatch->DrawString(*font, controlsText, Vector2(100.0f, 300.0f), Color::White);
        spriteBatch->End();
    }

    void IKSample::DrawCylinderChain()
    {
        const Matrix modelTransform = Matrix::CreateScale(0.04f, 0.025f, 0.04f);
        DrawBones(*cylinderModel, modelTransform, cylinderChainIndex,
                  cylinderChain, cylinderWorldTransforms);
    }

    void IKSample::DrawAvatar()
    {
        avatarRenderer->setViewProperty(view);
        avatarRenderer->setProjectionProperty(projection);
        avatarRenderer->Draw(avatarBoneTransforms, AvatarExpression());

        const Matrix scale = Matrix::CreateScale(0.04f, 0.01f, 0.04f);
        const Matrix rotation = Matrix::CreateRotationZ(MathHelper::ToRadians(90.0f));
        const Matrix modelTransform = scale * rotation;

        if (avatarRenderer->getStateProperty() == AvatarRendererState::Ready)
        {
            DrawBones(*cylinderModel, modelTransform, avatarChainIndex,
                      avatarBoneChain, avatarWorldTransforms);
        }
    }

    void IKSample::DrawBones(Model& model, const Matrix& modelTransform,
                             const int boneToColor, const std::vector<int>& boneChain,
                             const std::vector<Matrix>& worldTransforms)
    {
        const Vector3 red = Color::Red.ToVector3();
        const Vector3 black = Color::Black.ToVector3();
        const Vector3 lightGray = Color::LightGray.ToVector3();

        for (const int curBone : boneChain)
        {
            Vector3 diffuseColor = lightGray;
            if (singleStep)
            {
                if (boneChain[boneToColor] == curBone)
                {
                    diffuseColor = red;
                }
                else if (curBone == boneChain[0])
                {
                    diffuseColor = black;
                }
            }

            for (ModelMesh* mesh : model.getMeshesProperty())
            {
                for (Effect* effect : mesh->getEffectsProperty())
                {
                    if (auto* basicEffect = dynamic_cast<BasicEffect*>(effect))
                    {
                        basicEffect->setWorldProperty(modelTransform * worldTransforms[curBone]);
                        basicEffect->setViewProperty(view);
                        basicEffect->setProjectionProperty(projection);
                        basicEffect->setDiffuseColorProperty(diffuseColor);
                    }
                }
                mesh->Draw();
            }
        }
    }

    void IKSample::HandleInput(const GameTime& gameTime)
    {
        const Single time = static_cast<Single>(
            gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty());

        prevGamePadState = currentGamePadState;
        prevKeyboardState = currentKeyboardState;
        currentGamePadState = GamePad::GetState(PlayerIndex::One);
        currentKeyboardState = Keyboard::GetState();

        if (IsDown(Buttons::Back) || IsDown(Keys::Escape))
        {
            Exit();
        }
        if (IsTriggered(Buttons::LeftStick) || IsTriggered(Buttons::RightStick)
            || IsTriggered(Keys::R))
        {
            Reset();
        }
        if (IsTriggered(Buttons::A) || IsTriggered(Keys::P))
        {
            runSimulation = !runSimulation;
        }
        if (IsTriggered(Buttons::B) || IsTriggered(Keys::Space))
        {
            runSimulation = false;
        }
        if (IsTriggered(Buttons::Start) || IsTriggered(Keys::Enter))
        {
            singleStep = !singleStep;
        }

        Single moveSpeed = 0.1f;
        Vector2 movement = currentGamePadState.getThumbSticksProperty().getLeftProperty()
                         * moveSpeed;
        if (IsDown(Keys::W)) movement.Y += moveSpeed;
        if (IsDown(Keys::S)) movement.Y -= moveSpeed;
        if (IsDown(Keys::A)) movement.X -= moveSpeed;
        if (IsDown(Keys::D)) movement.X += moveSpeed;

        Single zoom = 0.0f;
        if (IsDown(Buttons::Y) || IsDown(Keys::Q)) zoom = 0.008f;
        if (IsDown(Buttons::X) || IsDown(Keys::E)) zoom = -0.008f;

        movement.X = -movement.X;
        cat->setPositionProperty(
            cat->getPositionProperty() + Vector3(movement, zoom * time));

        moveSpeed = 0.1f;
        movement = currentGamePadState.getThumbSticksProperty().getRightProperty()
                 * moveSpeed;
        if (IsDown(Keys::Up)) movement.Y += moveSpeed;
        if (IsDown(Keys::Down)) movement.Y -= moveSpeed;
        if (IsDown(Keys::Left)) movement.X -= moveSpeed;
        if (IsDown(Keys::Right)) movement.X += moveSpeed;

        zoom = 0.0f;
        zoom += currentGamePadState.getTriggersProperty().getRightProperty() * 0.01f;
        zoom -= currentGamePadState.getTriggersProperty().getLeftProperty() * 0.01f;
        if (IsDown(Keys::Z)) zoom = 0.007f;
        if (IsDown(Keys::X)) zoom = -0.007f;

        cameraRotX += movement.Y * time;
        cameraRotY += movement.X * time;
        cameraRadius += zoom * time;
    }

    void IKSample::Reset()
    {
        cat->setPositionProperty(Vector3(-1.0f, 0.25f, -2.0f));
        cameraRotX = 0.0f;
        cameraRotY = 0.0f;
        cameraRadius = 5.0f;
        runSimulation = true;
        singleStep = false;
        InitializeCylinderChain();
        LoadAvatar();
    }

    bool IKSample::IsTriggered(const Buttons button) const
    {
        return currentGamePadState.IsButtonDown(button)
            && !prevGamePadState.IsButtonDown(button);
    }

    bool IKSample::IsTriggered(const Keys key) const
    {
        return currentKeyboardState.IsKeyDown(key)
            && !prevKeyboardState.IsKeyDown(key);
    }

    bool IKSample::IsDown(const Buttons button) const
    {
        return currentGamePadState.IsButtonDown(button);
    }

    bool IKSample::IsDown(const Keys key) const
    {
        return currentKeyboardState.IsKeyDown(key);
    }
}
