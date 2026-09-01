// SPDX-License-Identifier: MS-PL
#pragma once

// DebugSystem.hpp — C++ port of GameDebugTools/DebugSystem.cs (XNA 4.0
// PerformanceUtility sample). Streamlines creation of the GameDebugTools
// pieces (DebugManager, DebugCommandUI, FpsCounter, TimeRuler) and adds them
// to the game's Components collection.
#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Game.hpp"

#include "DebugCommandUI.hpp"
#include "DebugManager.hpp"
#include "FpsCounter.hpp"
#include "RemoteDebugCommand.hpp"
#include "TimeRuler.hpp"

namespace PerformanceUtility::GameDebugTools {

using Microsoft::Xna::Framework::Game;

// Helper class that streamlines the creation of the GameDebugTools pieces.
// Port of GameDebugTools/DebugSystem.cs.
class DebugSystem {
public:
    static DebugSystem& Initialize(Game& game, const std::string& debugFont) {
        if (instance_ != nullptr)
            return *instance_;

        instance_ = std::unique_ptr<DebugSystem>(new DebugSystem(game));

        instance_->debugManager_ = std::make_shared<DebugManager>(game, debugFont);
        game.getComponentsProperty().Add(instance_->debugManager_.get());

        instance_->debugCommandUI_ = std::make_shared<DebugCommandUI>(game);
        game.getComponentsProperty().Add(instance_->debugCommandUI_.get());

        instance_->fpsCounter_ = std::make_shared<FpsCounter>(game);
        game.getComponentsProperty().Add(instance_->fpsCounter_.get());

        instance_->timeRuler_ = std::make_shared<TimeRuler>(game);
        game.getComponentsProperty().Add(instance_->timeRuler_.get());

#if !defined(WINDOWS_PHONE)
        instance_->remoteDebugCommand_ = std::make_shared<RemoteDebugCommand>(game);
        game.getComponentsProperty().Add(instance_->remoteDebugCommand_.get());
#endif

        return *instance_;
    }

    static DebugSystem& Instance() { return *instance_; }

    static void Shutdown() {
        if (instance_ == nullptr)
            return;

        Game& game = *instance_->game_;
#if !defined(WINDOWS_PHONE)
        (void)game.getComponentsProperty().Remove(instance_->remoteDebugCommand_.get());
#endif
        (void)game.getComponentsProperty().Remove(instance_->timeRuler_.get());
        (void)game.getComponentsProperty().Remove(instance_->fpsCounter_.get());
        (void)game.getComponentsProperty().Remove(instance_->debugCommandUI_.get());
        (void)game.getComponentsProperty().Remove(instance_->debugManager_.get());

        game.getServicesProperty().RemoveService<TimeRuler>();
        game.getServicesProperty().RemoveService<IDebugCommandHost>();
        game.getServicesProperty().RemoveService<DebugManager>();

        instance_.reset();
    }

    DebugManager& getDebugManagerProperty() { return *debugManager_; }
    DebugCommandUI& getDebugCommandUIProperty() { return *debugCommandUI_; }
    FpsCounter& getFpsCounterProperty() { return *fpsCounter_; }
    TimeRuler& getTimeRulerProperty() { return *timeRuler_; }
#if !defined(WINDOWS_PHONE)
    RemoteDebugCommand& getRemoteDebugCommandProperty() { return *remoteDebugCommand_; }
#endif

private:
    explicit DebugSystem(Game& game) : game_(&game) {}

    static std::unique_ptr<DebugSystem> instance_;
    Game* game_;

    // Owned by DebugSystem so their lifetime outlives Game::Components' raw
    // pointers; the components themselves are non-owning-registered there
    // (GameComponentCollection stores IGameComponent* without taking ownership).
    std::shared_ptr<DebugManager> debugManager_;
    std::shared_ptr<DebugCommandUI> debugCommandUI_;
    std::shared_ptr<FpsCounter> fpsCounter_;
    std::shared_ptr<TimeRuler> timeRuler_;
#if !defined(WINDOWS_PHONE)
    std::shared_ptr<RemoteDebugCommand> remoteDebugCommand_;
#endif
};

inline std::unique_ptr<DebugSystem> DebugSystem::instance_;

} // namespace PerformanceUtility::GameDebugTools
