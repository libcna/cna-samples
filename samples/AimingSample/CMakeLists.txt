cna_add_sample(AimingSample
    SOURCES
        src/AimingGame.cpp
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

# The audited original is the Windows configuration, whose entry point is
# compiled only when WINDOWS or XBOX is defined.
target_compile_definitions(AimingSample_cna_samples PRIVATE WINDOWS)
