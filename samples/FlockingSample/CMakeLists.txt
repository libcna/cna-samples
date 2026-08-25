cna_add_sample(FlockingSample
    SOURCES
        src/Animals/Animal.cpp
        src/Animals/Bird.cpp
        src/Animals/Cat.cpp
        src/Behaviors/AlignBehavior.cpp
        src/Behaviors/CohesionBehavior.cpp
        src/Behaviors/FleeBehavior.cpp
        src/Behaviors/SeparationBehavior.cpp
        src/Flock.cpp
        src/FlockingSample.cpp
        src/InputState.cpp
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

# The original builds from `Pathfinding (Windows).csproj`-style Windows configuration,
# which defines WINDOWS; its `#if WINDOWS || XBOX` regions declare and load the three
# Xbox glyph textures the desktop HUD draws. The desktop and web builds of this port are
# that configuration, so they define it too.
target_compile_definitions(FlockingSample_cna_samples PRIVATE WINDOWS)
