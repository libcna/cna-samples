cna_add_sample(CardsStarterKit
    GAMER_SERVICES
    SOURCES
        src/Program.cpp
        src/BlackjackGame.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

target_link_libraries(CardsStarterKit_cna_samples PRIVATE
    SharpRuntime::IO.IsolatedStorage
)
