cna_add_sample(ChaseAndEvade
    SOURCES
        src/ChaseAndEvadeGame.cpp
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

# The audited original is the Windows configuration, which defines WINDOWS and
# includes the nested Program entry point under #if WINDOWS || XBOX.
target_compile_definitions(ChaseAndEvade_cna_samples PRIVATE WINDOWS)
