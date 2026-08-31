cna_add_sample(SplitScreen
    SOURCES
        src/Program.cpp
        src/SplitScreenGame.cpp
        src/Tank.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

# The selected upstream Windows project owns Program.cs under `#if WINDOWS || XBOX`.
target_compile_definitions(SplitScreen_cna_samples PRIVATE WINDOWS)
