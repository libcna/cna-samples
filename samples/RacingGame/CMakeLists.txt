# SPDX-License-Identifier: MS-PL

cmake_minimum_required(VERSION 3.20)
project(RacingGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

get_filename_component(_racing_default_workspace "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
set(CNA_SOURCE_DIR "${_racing_default_workspace}/cnanext" CACHE PATH
    "CNA checkout used by the Racing Game port")
set(CNA_SHARP_RUNTIME_ROOT "${_racing_default_workspace}/sharp-runtimenext" CACHE PATH
    "Sharp Runtime checkout used by the Racing Game port")

set(CNA_GRAPHICS_RENDERER OPENGL33 CACHE STRING
    "Pinned Racing reference renderer" FORCE)
set(CNA_BUILD_TESTS OFF CACHE BOOL "Build CNA tests" FORCE)
set(CNA_BUILD_EXAMPLES OFF CACHE BOOL "Build CNA examples" FORCE)
set(CNA_ENABLE_NET OFF CACHE BOOL "Build CNA networking" FORCE)
set(CNA_EASYGL_COMPILED_EFFECTS ON CACHE BOOL
    "Enable authentic XNA compiled effects" FORCE)
set(EASYGL_BUILD_TESTS OFF CACHE BOOL "Build EasyGL tests" FORCE)
set(EASYGL_BUILD_EXAMPLES OFF CACHE BOOL "Build EasyGL examples" FORCE)
set(SHARP_RUNTIME_BUILD_TESTS OFF CACHE BOOL "Build Sharp Runtime tests" FORCE)
set(SHARP_RUNTIME_COMPONENTS
    "Core.Base;IO;Collections.Core;Collections.ObjectModel;Runtime;Threading;Text;Globalization;Storage;Security.Cryptography;Xml;Xml.Serialization"
    CACHE STRING "Sharp Runtime components required by Racing" FORCE)

if(NOT TARGET CNA)
    add_subdirectory("${CNA_SOURCE_DIR}" CNA_BUILD)
endif()

add_library(RacingGameCore STATIC
    src/GameLogic/BasePlayer.cpp
    src/GameLogic/CarControlState.cpp
    src/GameLogic/CarPhysics.cpp
    src/GameLogic/ChaseCamera.cpp
    src/GameLogic/Physics/SpringPhysicsObject.cpp
    src/Graphics/TangentVertex.cpp
    src/Helpers/Vector3Helper.cpp
    src/Landscapes/Landscape.cpp
    src/Rendering/StaticTrackScene.cpp
    src/Tracks/GuardRailGeometry.cpp
    src/Tracks/TrackCombiModels.cpp
    src/Tracks/TrackColumnsGeometry.cpp
    src/Tracks/TrackData.cpp
    src/Tracks/Track.cpp
    src/Tracks/TrackGeometry.cpp
    src/Tracks/TrackLine.cpp
    src/Tracks/TrackVertex.cpp
)
target_include_directories(RacingGameCore PUBLIC src)
target_link_libraries(RacingGameCore PUBLIC CNA SharpRuntime::Xml.Serialization)

add_executable(RacingGameTrackProbe
    src/Diagnostics/TrackProbe.cpp
)
target_link_libraries(RacingGameTrackProbe PRIVATE RacingGameCore)

add_executable(RacingGamePhysicsProbe
    src/Diagnostics/PhysicsProbe.cpp
)
target_link_libraries(RacingGamePhysicsProbe PRIVATE RacingGameCore)
