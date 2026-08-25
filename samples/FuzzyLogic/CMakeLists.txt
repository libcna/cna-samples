cna_add_sample(FuzzyLogic
    SOURCES
        src/Behaviors/Behavior.cpp
        src/Behaviors/ChaseBehavior.cpp
        src/Behaviors/EvadeBehavior.cpp
        src/Behaviors/WanderBehavior.cpp
        src/Entities/Entity.cpp
        src/Entities/Mouse.cpp
        src/Entities/Tank.cpp
        src/FuzzyLogicGame.cpp
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)
# The original's Program.cs entry point sits inside `#if WINDOWS || XBOX`, so only the
# Windows and Xbox configurations of the upstream solution have one. The desktop and web
# builds of this port are that configuration, so they define it too.
target_compile_definitions(FuzzyLogic_cna_samples PRIVATE WINDOWS)
