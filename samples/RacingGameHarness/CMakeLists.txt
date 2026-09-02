# SPDX-License-Identifier: MS-PL

cmake_minimum_required(VERSION 3.20)

project(RacingGameHarness LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

get_filename_component(_racing_default_workspace "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
set(CNA_SOURCE_DIR "${_racing_default_workspace}/cnanext" CACHE PATH
    "CNA checkout used by the Racing milestone harness")
set(CNA_SHARP_RUNTIME_ROOT "${_racing_default_workspace}/sharp-runtimenext" CACHE PATH
    "Sharp Runtime checkout used by the Racing milestone harness")

# plan_racing.md deliberately qualifies the reference implementation on this renderer first.
set(CNA_GRAPHICS_RENDERER OPENGL33 CACHE STRING
    "Pinned Racing reference renderer" FORCE)
set(CNA_BUILD_TESTS OFF CACHE BOOL "Build CNA tests" FORCE)
set(CNA_BUILD_EXAMPLES OFF CACHE BOOL "Build CNA examples" FORCE)
set(CNA_ENABLE_NET OFF CACHE BOOL "Build CNA networking" FORCE)
set(CNA_EASYGL_COMPILED_EFFECTS ON CACHE BOOL
    "Keep the future Racing compiled-Effect route enabled in the harness build" FORCE)
set(EASYGL_BUILD_TESTS OFF CACHE BOOL "Build EasyGL tests" FORCE)
set(EASYGL_BUILD_EXAMPLES OFF CACHE BOOL "Build EasyGL examples" FORCE)
set(SHARP_RUNTIME_BUILD_TESTS OFF CACHE BOOL "Build Sharp Runtime tests" FORCE)
set(SHARP_RUNTIME_COMPONENTS
    "Core.Base;IO;IO.IsolatedStorage;Collections.Core;Collections.ObjectModel;Runtime;Threading;Text;Globalization;Resources;Storage;Security.Cryptography;Xml.Linq"
    CACHE STRING "Sharp Runtime components required by the Racing harness" FORCE)

add_subdirectory("${CNA_SOURCE_DIR}" CNA_BUILD)

add_executable(RacingGameHarness_cna_samples
    src/Program.cpp
)
target_link_libraries(RacingGameHarness_cna_samples PRIVATE CNA)

if(TARGET SDL3::SDL3main)
    target_link_libraries(RacingGameHarness_cna_samples PRIVATE SDL3::SDL3main)
endif()

if(WIN32)
    set_target_properties(RacingGameHarness_cna_samples PROPERTIES WIN32_EXECUTABLE TRUE)
    if(COMMAND cna_copy_sdl_runtime)
        cna_copy_sdl_runtime(RacingGameHarness_cna_samples)
    endif()
endif()
