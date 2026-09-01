cna_add_sample(PerformanceUtility
    SOURCES
        src/PerformanceUtilityGame.cpp
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
    NET
)

# The selected upstream product is PerformanceUtilityWin.csproj. Its WINDOWS
# constant selects the desktop instruction text and SystemLink client side of
# RemoteDebugCommand; preserve that source configuration on every CNA host.
target_compile_definitions(PerformanceUtility_cna_samples PRIVATE WINDOWS)
