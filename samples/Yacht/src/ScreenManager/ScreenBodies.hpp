#pragma once

// ScreenBodies.hpp -- the bodies of GameScreen and ScreenManager that need each other complete.
//
// A screen removes itself through its manager and loads content through the manager's game,
// while the manager owns the screens. C# resolves the cycle for itself; here the members are
// declared where they belong and closed once both classes are whole.

#include <memory>
#include <string>
#include <vector>

#include "GameScreen.hpp"
#include "ScreenManager.hpp"

namespace GameStateManagement {

inline void GameScreen::Update(GameTime& gameTime, bool otherScreenHasFocus,
                               bool coveredByOtherScreen)
{
    otherScreenHasFocus_ = otherScreenHasFocus;

    if (isExiting_) {
        // If the screen is going away to die, it should transition off.
        screenState_ = ScreenState::TransitionOff;

        if (!UpdateTransition(gameTime, transitionOffTime_, 1)) {
            // When the transition finishes, remove the screen.
            getScreenManagerProperty()->RemoveScreen(this);
        }
    } else if (coveredByOtherScreen) {
        // If the screen is covered by another, it should transition off.
        if (UpdateTransition(gameTime, transitionOffTime_, 1)) {
            // Still busy transitioning.
            screenState_ = ScreenState::TransitionOff;
        } else {
            // Transition finished!
            screenState_ = ScreenState::Hidden;
        }
    } else {
        // Otherwise the screen should transition on and become active.
        if (UpdateTransition(gameTime, transitionOnTime_, -1)) {
            // Still busy transitioning.
            screenState_ = ScreenState::TransitionOn;
        } else {
            // Transition finished!
            screenState_ = ScreenState::Active;
        }
    }
}

inline void GameScreen::ExitScreen()
{
    if (getTransitionOffTimeProperty() == TimeSpan::Zero) {
        // If the screen has a zero transition time, remove it immediately.
        getScreenManagerProperty()->RemoveScreen(this);
    } else {
        // Otherwise flag that it should transition off and then exit.
        isExiting_ = true;
    }
}

template <typename T>
T GameScreen::Load(const std::string& assetName)
{
    return getScreenManagerProperty()
        ->getGameProperty()
        .getContentProperty()
        .template Load<T>(assetName);
}

inline void ScreenManager::SerializeState()
{
    auto storage = System::IO::IsolatedStorage::IsolatedStorageFile::GetUserStoreForApplication();

    if (storage.DirectoryExists("ScreenManager")) {
        DeleteState(storage);
    } else {
        storage.CreateDirectory("ScreenManager");
    }

    // Write out the full list of screens by name. The original writes each type's
    // assembly-qualified name and resolves it with reflection on the way back; here the name
    // is the screen's own and a registered factory rebuilds it.
    {
        std::string list;
        for (const auto& screen : screens_) {
            if (screen->getIsSerializableProperty()) {
                list += screen->GetTypeName();
                list += '\n';
            }
        }
        auto stream = storage.CreateFile("ScreenManager/ScreenList.dat");
        std::vector<SharpRuntime::bytecs> bytes(list.begin(), list.end());
        if (!bytes.empty()) {
            stream.Write(bytes.data(), 0, static_cast<SharpRuntime::intcs>(bytes.size()));
        }
    }

    // Make each screen serialize its own state.
    int screenIndex = 0;
    for (const auto& screen : screens_) {
        if (screen->getIsSerializableProperty()) {
            auto stream =
                storage.CreateFile("ScreenManager/Screen" + std::to_string(screenIndex) + ".dat");
            screen->Serialize(stream);
            screenIndex++;
        }
    }
}

inline bool ScreenManager::DeserializeState()
{
    auto storage = System::IO::IsolatedStorage::IsolatedStorageFile::GetUserStoreForApplication();

    if (!storage.DirectoryExists("ScreenManager")) {
        return false;
    }

    try {
        if (storage.FileExists("ScreenManager/ScreenList.dat")) {
            auto stream = storage.OpenFile("ScreenManager/ScreenList.dat",
                                           System::IO::FileMode::Open);
            const auto length = stream.getLengthProperty();
            std::vector<SharpRuntime::bytecs> bytes(static_cast<std::size_t>(length));
            if (length > 0) {
                (void)stream.Read(bytes.data(), 0, length);
            }

            std::string list(bytes.begin(), bytes.end());
            std::size_t at = 0;
            while (at < list.size()) {
                const std::size_t end = list.find('\n', at);
                const std::string line =
                    list.substr(at, end == std::string::npos ? std::string::npos : end - at);
                at = end == std::string::npos ? list.size() : end + 1;

                if (line.empty()) {
                    continue;
                }
                const auto factory = ScreenFactories().find(line);
                if (factory == ScreenFactories().end()) {
                    // No factory for that name, the same position .NET is in when the type
                    // cannot be resolved: the screen is skipped rather than guessed at.
                    continue;
                }
                AddScreen(factory->second(), PlayerIndex::One);
            }
        }

        // Give each screen a chance to deserialize from the disk.
        for (std::size_t i = 0; i < screens_.size(); i++) {
            auto stream = storage.OpenFile("ScreenManager/Screen" + std::to_string(i) + ".dat",
                                           System::IO::FileMode::Open);
            screens_[i]->Deserialize(stream);
        }

        return true;
    } catch (...) {
        DeleteState(storage);
    }

    return false;
}

inline void ScreenManager::DeleteState(
    System::IO::IsolatedStorage::IsolatedStorageFile& storage) const
{
    // Get all the files in the directory and delete them.
    for (const std::string& file : storage.GetFileNames("ScreenManager/*")) {
        storage.DeleteFile("ScreenManager/" + file);
    }
}

} // namespace GameStateManagement
