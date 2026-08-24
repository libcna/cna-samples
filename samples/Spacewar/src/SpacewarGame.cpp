// SPDX-License-Identifier: MS-PL

#include "SpacewarGame.hpp"

#include <cmath>

#include "EvolvedScreen.hpp"
#include "Font.hpp"
#include "Particles.hpp"
#include "RetroScreen.hpp"
#include "Screen.hpp"
#include "SelectionScreen.hpp"
#include "ShipUpgradeScreen.hpp"
#include "Sound.hpp"
#include "TitleScreen.hpp"
#include "VictoryScreen.hpp"
#include "XInputHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/ClearOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/DisplayMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsAdapter.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PresentationParameters.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTargetUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/Environment.hpp"
#include "System/OperatingSystem.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    Content::ContentManager* SpacewarGame::contentManager_ = nullptr;
    Settings SpacewarGame::settings_;
    std::unique_ptr<Camera> SpacewarGame::camera_;
    std::array<Player, 2> SpacewarGame::players_{};
    GameState SpacewarGame::gameState_ = GameState::Started;
    int SpacewarGame::gameLevel_ = 0;
    std::unique_ptr<Screen> SpacewarGame::currentScreen_;
    System::PlatformID SpacewarGame::currentPlatform_ = System::PlatformID::Other;
    KeyboardState SpacewarGame::keyState_;

    SpacewarGame::SpacewarGame()
        : graphics_(this)
    {
        graphics_.setPreferredBackBufferWidthProperty(preferredWindowWidth_);
        graphics_.setPreferredBackBufferHeightProperty(preferredWindowHeight_);
        setIsFixedTimeStepProperty(false);
    }

    SpacewarGame::~SpacewarGame()
    {
        // C# leaves these static references to the GC. C++ must release their graphics and audio
        // resources while Game still owns the renderer and audio backends.
        if (currentScreen_)
            currentScreen_->Shutdown();
        currentScreen_.reset();
        Particles::Dispose();
        Font::Dispose();
        Sound::Shutdown();
    }

    const std::string& SpacewarGame::GetTypeName() const
    {
        static const std::string name = "Spacewar.SpacewarGame";
        return name;
    }

    GameState SpacewarGame::getGameStateProperty() { return gameState_; }
    int SpacewarGame::getGameLevelProperty() { return gameLevel_; }
    void SpacewarGame::setGameLevelProperty(int value) { gameLevel_ = value; }
    Camera& SpacewarGame::getCameraProperty() { return *camera_; }
    Settings& SpacewarGame::getSettingsProperty() { return settings_; }
    std::array<Player, 2>& SpacewarGame::getPlayersProperty() { return players_; }
    Content::ContentManager& SpacewarGame::getContentManagerProperty() { return *contentManager_; }
    System::PlatformID SpacewarGame::getCurrentPlatformProperty() { return currentPlatform_; }
    const KeyboardState& SpacewarGame::getKeyStateProperty() { return keyState_; }

    void SpacewarGame::Initialize()
    {
        settings_ = Settings::Load("settings.xml");
        currentPlatform_ = System::Environment::getOSVersionProperty().getPlatformProperty();
        Sound::Initialize();
        getWindowProperty().setTitleProperty(settings_.WindowTitle);
        Game::Initialize();
    }

    void SpacewarGame::BeginRun()
    {
        Sound::PlayCue(Sounds::TitleMusic);
        ChangeState(GameState::LogoSplash);
        camera_ = std::make_unique<Camera>(3.14159265358979323846f / 4.0f,
            static_cast<float>(FixedDrawingWidth) / static_cast<float>(FixedDrawingHeight), 10.0f, 700.0f);
        camera_->setViewPositionProperty(Vector3(0, 0, 500));
        Game::BeginRun();
    }

    void SpacewarGame::Update(GameTime& gameTime)
    {
        const System::TimeSpan elapsedTime = gameTime.getElapsedGameTimeProperty();
        const System::TimeSpan time = gameTime.getTotalGameTimeProperty();
        GameState changeState = GameState::None;
        keyState_ = Keyboard::GetState();
        XInputHelper::Update(*this, keyState_);

        if ((keyState_.IsKeyDown(Keys::RightAlt) || keyState_.IsKeyDown(Keys::LeftAlt)) &&
            keyState_.IsKeyDown(Keys::Enter) && !justWentFullScreen_)
        {
            ToggleFullScreen();
            justWentFullScreen_ = true;
        }
        if (keyState_.IsKeyUp(Keys::Enter)) justWentFullScreen_ = false;

        if (XInputHelper::getGamePadsProperty()[PlayerIndex::One].getBackPressedProperty() ||
            XInputHelper::getGamePadsProperty()[PlayerIndex::Two].getBackPressedProperty())
        {
            if (gameState_ == GameState::PlayEvolved || gameState_ == GameState::PlayRetro)
                paused_ = !paused_;
            if (gameState_ == GameState::LogoSplash)
                Exit();
        }

        if (!paused_)
        {
            changeState = currentScreen_->Update(time, elapsedTime);
            Sound::Update();
            if (XInputHelper::getGamePadsProperty()[PlayerIndex::One].getStartPressedProperty() ||
                XInputHelper::getGamePadsProperty()[PlayerIndex::Two].getStartPressedProperty())
                changeState = GameState::LogoSplash;
            if (changeState != GameState::None)
                ChangeState(changeState);
        }
        Game::Update(gameTime);
    }

    bool SpacewarGame::BeginDraw()
    {
        if (!Game::BeginDraw()) return false;
        BeginDrawScaling();
        return true;
    }

    void SpacewarGame::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(ClearOptions::DepthBuffer, Color::CornflowerBlue, 1.0f, 0);
        Game::Draw(gameTime);
        currentScreen_->Render();
    }

    void SpacewarGame::EndDraw()
    {
        EndDrawScaling();
        Game::EndDraw();
    }

    void SpacewarGame::ChangeState(GameState nextState)
    {
        if (nextState == GameState::LogoSplash)
        {
            if (currentScreen_) currentScreen_->Shutdown();
            currentScreen_ = std::make_unique<TitleScreen>(this);
            gameState_ = GameState::LogoSplash;
        }
        else if (gameState_ == GameState::LogoSplash && nextState == GameState::ShipSelection)
        {
            Sound::PlayCue(Sounds::MenuAdvance);
            players_ = {Player(), Player()};
            gameLevel_ = 1;
            currentScreen_->Shutdown();
            currentScreen_ = std::make_unique<SelectionScreen>(this);
            gameState_ = GameState::ShipSelection;
        }
        else if (gameState_ == GameState::PlayEvolved && nextState == GameState::ShipUpgrade)
        {
            currentScreen_->Shutdown();
            currentScreen_ = std::make_unique<ShipUpgradeScreen>(this);
            gameState_ = GameState::ShipUpgrade;
        }
        else if ((gameState_ == GameState::ShipSelection || gameState_ == GameState::ShipUpgrade) &&
                 nextState == GameState::PlayEvolved)
        {
            Sound::PlayCue(Sounds::MenuAdvance);
            currentScreen_->Shutdown();
            currentScreen_ = std::make_unique<EvolvedScreen>(this);
            gameState_ = GameState::PlayEvolved;
        }
        else if (gameState_ == GameState::LogoSplash && nextState == GameState::PlayRetro)
        {
            players_ = {Player(), Player()};
            currentScreen_->Shutdown();
            currentScreen_ = std::make_unique<RetroScreen>(this);
            gameState_ = GameState::PlayRetro;
        }
        else if (gameState_ == GameState::PlayEvolved && nextState == GameState::Victory)
        {
            currentScreen_->Shutdown();
            currentScreen_ = std::make_unique<VictoryScreen>(this);
            gameState_ = GameState::Victory;
        }
    }

    void SpacewarGame::LoadContent()
    {
        Game::LoadContent();
        contentManager_ = &getContentProperty();
        contentManager_->setRootDirectoryProperty("");
        if (currentScreen_) currentScreen_->OnCreateDevice();
        Font::Init(this);
        if (enableDrawScaling_)
        {
            const PresentationParameters& pp = getGraphicsDeviceProperty().getPresentationParametersProperty();
            drawBuffer_ = std::make_unique<RenderTarget2D>(getGraphicsDeviceProperty(),
                FixedDrawingWidth, FixedDrawingHeight, true, SurfaceFormat::Color,
                DepthFormat::Depth24Stencil8, pp.getMultiSampleCountProperty(),
                RenderTargetUsage::DiscardContents);
            spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        }
    }

    void SpacewarGame::UnloadContent()
    {
        Game::UnloadContent();
        if (drawBuffer_) drawBuffer_->Dispose();
        drawBuffer_.reset();
        if (spriteBatch_) spriteBatch_->Dispose();
        spriteBatch_.reset();
        Font::Dispose();
        if (contentManager_) contentManager_->Dispose();
        contentManager_ = nullptr;
    }

    void SpacewarGame::ToggleFullScreen()
    {
        const PresentationParameters& presentation = getGraphicsDeviceProperty().getPresentationParametersProperty();
        if (presentation.getIsFullScreenProperty())
        {
            graphics_.setPreferredBackBufferWidthProperty(preferredWindowWidth_);
            graphics_.setPreferredBackBufferHeightProperty(preferredWindowHeight_);
        }
        else
        {
            const DisplayMode mode = getGraphicsDeviceProperty().getAdapterProperty().getCurrentDisplayModeProperty();
            graphics_.setPreferredBackBufferWidthProperty(mode.getWidthProperty());
            graphics_.setPreferredBackBufferHeightProperty(mode.getHeightProperty());
        }
        graphics_.ToggleFullScreen();
    }

    void SpacewarGame::BeginDrawScaling()
    {
        if (enableDrawScaling_ && drawBuffer_)
            getGraphicsDeviceProperty().SetRenderTarget(drawBuffer_.get());
    }

    void SpacewarGame::EndDrawScaling()
    {
        if (!enableDrawScaling_ || !drawBuffer_) return;
        getGraphicsDeviceProperty().SetRenderTarget(nullptr);
        const PresentationParameters& presentation = getGraphicsDeviceProperty().getPresentationParametersProperty();
        const float outputAspect = static_cast<float>(presentation.getBackBufferWidthProperty()) /
                                   static_cast<float>(presentation.getBackBufferHeightProperty());
        const float preferredAspect = static_cast<float>(FixedDrawingWidth) / static_cast<float>(FixedDrawingHeight);
        Rectangle destination;
        if (outputAspect <= preferredAspect)
        {
            const int presentHeight = static_cast<int>(presentation.getBackBufferWidthProperty() / preferredAspect + 0.5f);
            const int barHeight = (presentation.getBackBufferHeightProperty() - presentHeight) / 2;
            destination = Rectangle(0, barHeight, presentation.getBackBufferWidthProperty(), presentHeight);
        }
        else
        {
            const int presentWidth = static_cast<int>(presentation.getBackBufferHeightProperty() * preferredAspect + 0.5f);
            const int barWidth = (presentation.getBackBufferWidthProperty() - presentWidth) / 2;
            destination = Rectangle(barWidth, 0, presentWidth, presentation.getBackBufferHeightProperty());
        }
        getGraphicsDeviceProperty().Clear(ClearOptions::Target, Color::Black, 1.0f, 0);
        spriteBatch_->Begin(SpriteSortMode::Immediate, BlendState::Opaque);
        spriteBatch_->Draw(*drawBuffer_, destination, Color::White);
        spriteBatch_->End();
    }
}
