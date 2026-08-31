cna_add_sample(GameStateManagement
    SOURCES
        src/GameStateManagementGame.cpp
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
        src/ScreenManager/GameScreen.cpp
        src/ScreenManager/InputState.cpp
        src/ScreenManager/ScreenManager.cpp
        src/Screens/BackgroundScreen.cpp
        src/Screens/GameplayScreen.cpp
        src/Screens/LoadingScreen.cpp
        src/Screens/MainMenuScreen.cpp
        src/Screens/MenuEntry.cpp
        src/Screens/MenuScreen.cpp
        src/Screens/MessageBoxScreen.cpp
        src/Screens/OptionsMenuScreen.cpp
        src/Screens/PauseMenuScreen.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

target_link_libraries(GameStateManagement_cna_samples PRIVATE SharpRuntime::Diagnostics)
