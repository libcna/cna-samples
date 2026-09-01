cna_add_sample(PerformanceMeasuring
    SOURCES
        src/Program.cpp
        src/Primitives/VertexPositionNormal.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
    NET
)

# The audited upstream product is PerformanceMeasuring (Windows).csproj. Its
# WINDOWS constant selects the desktop instructions and the SystemLink client
# side of RemoteDebugCommand; retain that source configuration on every CNA
# host, including the browser build.
target_compile_definitions(PerformanceMeasuring_cna_samples PRIVATE WINDOWS)
