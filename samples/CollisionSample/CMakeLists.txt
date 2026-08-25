cna_add_sample(CollisionSample
    SOURCES
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

if(NOT EMSCRIPTEN)
    cna_add_sample(CollisionSampleUnitTests
        SOURCES tests/CollisionUnitTests.cpp
    )
endif()
