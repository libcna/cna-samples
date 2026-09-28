cna_add_sample(NetworkStateManagement
    NET
    SOURCES
        src/MessageDisplayComponent.cpp
        src/NetworkStateManagementGame.cpp
        src/Networking/AvailableSessionMenuEntry.cpp
        src/Networking/CreateOrFindSessionScreen.cpp
        src/Networking/JoinSessionScreen.cpp
        src/Networking/LobbyScreen.cpp
        src/Networking/NetworkBusyScreen.cpp
        src/Networking/NetworkErrorScreen.cpp
        src/Networking/NetworkSessionComponent.cpp
        src/Networking/ProfileSignInScreen.cpp
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
        src/Screens/PauseMenuScreen.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

target_link_libraries(NetworkStateManagement_cna_samples PRIVATE SharpRuntime::Diagnostics)
