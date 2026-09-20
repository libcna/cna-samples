cna_add_sample(CameraShake
    SOURCES
        src/Camera.cpp
        src/CameraShakeGame.cpp
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
        src/VibrationManager.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

# The original includes Program.cs only in the Windows and Xbox configurations.
target_compile_definitions(CameraShake_cna_samples PRIVATE WINDOWS)
