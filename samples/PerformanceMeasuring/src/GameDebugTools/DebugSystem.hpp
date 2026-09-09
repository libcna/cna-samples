// SPDX-License-Identifier: MS-PL
#pragma once

// DebugSystem.hpp — C++ port of GameDebugTools/DebugSystem.cs (XNA 4.0
// PerformanceMeasuring sample). Streamlines creation of the GameDebugTools
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

namespace PerformanceMeasuring::GameDebugTools {

using Microsoft::Xna::Framework::Game;

/**
 * @brief Creates and owns the whole GameDebugTools set, so a game adds them in one call.
 *
 * Holds the components itself rather than leaving them to `Game::Components`, which stores raw
 * `IGameComponent*` without taking ownership. Shutdown() removes them while the game is still
 * alive; releasing them afterwards would destroy graphics resources whose device is already gone.
 */
class DebugSystem {
public:
    /**
     * @brief Creates the debug components and registers them with the game.
     *
     * Does nothing beyond returning the existing instance if called twice.
     *
     * @param game      Game to add the components to.
     * @param debugFont Content name of the font they draw with.
     * @return The single debug system.
     */
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

    /** @brief Gets the system created by Initialize(). @return The single debug system. */
    static DebugSystem& Instance() { return *instance_; }

    /**
     * @brief Removes the components and services while the game is still alive, then releases them.
     *
     * The order matters: a component released after the graphics device has gone would destroy its
     * textures and sprite batch against a dead device.
     */
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

    /** @brief Gets the shared graphics resources. @return The debug manager. */
    DebugManager& getDebugManagerProperty() { return *debugManager_; }

    /** @brief Gets the on-screen command console. @return The command UI. */
    DebugCommandUI& getDebugCommandUIProperty() { return *debugCommandUI_; }

    /** @brief Gets the frame-rate display. @return The FPS counter. */
    FpsCounter& getFpsCounterProperty() { return *fpsCounter_; }

    /** @brief Gets the frame-timing bars. @return The time ruler. */
    TimeRuler& getTimeRulerProperty() { return *timeRuler_; }
#if !defined(WINDOWS_PHONE)
    /** @brief Gets the network command listener, absent on Windows Phone. @return The remote command component. */
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

} // namespace PerformanceMeasuring::GameDebugTools
