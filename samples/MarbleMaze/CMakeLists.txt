cna_add_sample(MarbleMaze
    SOURCES
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
        src/Screens/GameplayScreen.cpp
        src/Screens/HighScoreScreen.cpp
        src/Screens/LoadingAndInstructionScreen.cpp
        src/Screens/MainMenuScreen.cpp
        src/Screens/PauseScreen.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
    GAMER_SERVICES
)

target_link_libraries(MarbleMaze_cna_samples PRIVATE SharpRuntime::IO.IsolatedStorage)
