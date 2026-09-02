cna_add_sample(TiledSprites
    SOURCES
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

option(CNA_SAMPLES_BUILD_DIAGNOSTICS "Build sample-only audit probes" OFF)
if(CNA_SAMPLES_BUILD_DIAGNOSTICS AND NOT EMSCRIPTEN)
    add_executable(TiledSprites_behavior_probe
        tests/TiledSpritesBehaviorProbe.cpp
    )
    target_include_directories(TiledSprites_behavior_probe PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/../cnanext/modules/platform/tests
    )
    target_link_libraries(TiledSprites_behavior_probe PRIVATE CNA)
endif()
