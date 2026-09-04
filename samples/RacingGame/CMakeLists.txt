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

if(ANDROID)
    set(_racing_graphics_renderer OPENGLES3)
elseif(EMSCRIPTEN)
    set(_racing_graphics_renderer WEBGL2)
else()
    set(_racing_graphics_renderer OPENGL33)
endif()
if(NOT DEFINED CNA_GRAPHICS_RENDERER)
    set(CNA_GRAPHICS_RENDERER "${_racing_graphics_renderer}" CACHE STRING
        "Platform-specific Racing renderer")
endif()
set(CNA_BUILD_TESTS OFF CACHE BOOL "Build CNA tests" FORCE)
set(CNA_BUILD_EXAMPLES OFF CACHE BOOL "Build CNA examples" FORCE)
set(CNA_ENABLE_NET OFF CACHE BOOL "Build CNA networking" FORCE)
if(EMSCRIPTEN)
    set(CNA_ENABLE_EMSCRIPTEN_THREADS ON CACHE BOOL
        "Enable the shared-memory ABI required by Racing's asynchronous saves" FORCE)
endif()
if(ANDROID)
    set(CNA_DEVICES ON CACHE BOOL
        "Enable the display safe-area extension used by Racing on Android" FORCE)
endif()
set(CNA_ENABLE_DRACO OFF CACHE BOOL
    "Racing authentic XNB content does not use glTF Draco" FORCE)
set(CNA_EASYGL_COMPILED_EFFECTS ON CACHE BOOL
    "Enable authentic XNA compiled effects" FORCE)
set(EASYGL_BUILD_TESTS OFF CACHE BOOL "Build EasyGL tests" FORCE)
set(EASYGL_BUILD_EXAMPLES OFF CACHE BOOL "Build EasyGL examples" FORCE)
set(SHARP_RUNTIME_BUILD_TESTS OFF CACHE BOOL "Build Sharp Runtime tests" FORCE)
set(SHARP_RUNTIME_COMPONENTS
    "Core.Base;IO;IO.IsolatedStorage;Collections.Core;Collections.ObjectModel;Runtime;Threading;Text;Globalization;Storage;Security.Cryptography;Xml;Xml.Serialization"
    CACHE STRING "Sharp Runtime components required by Racing" FORCE)

if(NOT TARGET CNA)
    add_subdirectory("${CNA_SOURCE_DIR}" CNA_BUILD)
endif()

if(ANDROID AND NOT TARGET SDL3::SDL3)
    find_package(SDL3 CONFIG REQUIRED)
endif()

add_library(RacingGameCore STATIC
    src/GameLogic/Input.cpp
    src/GameLogic/MobileControls.cpp
    src/GameLogic/MobileInput.cpp
    src/GameLogic/BasePlayer.cpp
    src/GameLogic/CarControlState.cpp
    src/GameLogic/CarPhysics.cpp
    src/GameLogic/ChaseCamera.cpp
    src/GameLogic/Player.cpp
    src/GameLogic/Replay.cpp
    src/GameLogic/ScreenshotCapturer.cpp
    src/GameLogic/Physics/SpringPhysicsObject.cpp
    src/GameScreens/CarSelection.cpp
    src/GameScreens/GameScreen.cpp
    src/GameScreens/Help.cpp
    src/GameScreens/Highscores.cpp
    src/GameScreens/LoadingScreen.cpp
    src/GameScreens/MainMenu.cpp
    src/GameScreens/Options.cpp
    src/GameScreens/SplashScreen.cpp
    src/GameScreens/TrackSelection.cpp
    src/Graphics/CarModelHierarchy.cpp
    src/Graphics/LensFlare.cpp
    src/Graphics/LineManager2D.cpp
    src/Graphics/LineManager3D.cpp
    src/Graphics/Material.cpp
    src/Graphics/MeshRenderManager.cpp
    src/Graphics/Model.cpp
    src/Graphics/ResolutionMapper.cpp
    src/Graphics/TangentVertex.cpp
    src/Graphics/Texture.cpp
    src/Graphics/TextureFont.cpp
    src/Graphics/TextureFontBigNumbers.cpp
    src/Graphics/UIRenderer.cpp
    src/Helpers/ColorHelper.cpp
    src/Helpers/Log.cpp
    src/Helpers/RandomHelper.cpp
    src/Helpers/Vector3Helper.cpp
    src/Landscapes/Landscape.cpp
    src/Properties/GameSettings.cpp
    src/Rendering/LandscapeObjectRenderer.cpp
    src/Rendering/ShadowMapRenderer.cpp
    src/Rendering/StaticTrackScene.cpp
    src/Rendering/CarRenderer.cpp
    src/Shaders/PostScreenGlow.cpp
    src/Shaders/PostScreenMenu.cpp
    src/Shaders/RenderToTexture.cpp
    src/Shaders/VBScreenHelper.cpp
    src/Sounds/Sound.cpp
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
target_link_libraries(RacingGameCore PUBLIC CNA SharpRuntime::IO.IsolatedStorage
    SharpRuntime::Xml.Serialization)

add_library(RacingGameApplication STATIC
    src/Platform/ContentDelivery.cpp
    src/Platform/PersistentStorage.cpp
    src/RacingGameManager.cpp
)
target_link_libraries(RacingGameApplication PUBLIC RacingGameCore)

if(ANDROID)
    add_library(RacingGame_cna_samples SHARED src/Program.cpp)
    set_target_properties(RacingGame_cna_samples PROPERTIES OUTPUT_NAME main)
else()
    add_executable(RacingGame_cna_samples src/Program.cpp)
endif()
target_link_libraries(RacingGame_cna_samples PRIVATE RacingGameApplication)

if(EMSCRIPTEN)
    set_target_properties(RacingGame_cna_samples PROPERTIES SUFFIX ".html")
    target_link_libraries(RacingGame_cna_samples PRIVATE CNA::EmscriptenAsyncify)
    target_link_options(RacingGame_cna_samples PRIVATE
        -sALLOW_MEMORY_GROWTH=1
        -sFORCE_FILESYSTEM=1
        -lidbfs.js
        "-sMIN_WEBGL_VERSION=2"
        "-sMAX_WEBGL_VERSION=2"
        "-sEXPORTED_RUNTIME_METHODS=callMain"
        --shell-file "${CMAKE_CURRENT_SOURCE_DIR}/scripts/web-shell.html"
    )
    set_property(TARGET RacingGame_cna_samples APPEND PROPERTY LINK_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/web-shell.html")

    set(RACING_WEB_CONTENT_ROOT "" CACHE PATH
        "Authentic XNA output Content directory packaged into the Web build")
    option(RACING_WEB_PROGRESSIVE_CONTENT
        "Split authentic Racing content across its existing loading stages" ON)
    if(RACING_WEB_CONTENT_ROOT)
        if(NOT EXISTS "${RACING_WEB_CONTENT_ROOT}/Models/Car.xnb")
            message(FATAL_ERROR
                "RACING_WEB_CONTENT_ROOT must name the authentic XNA output Content "
                "directory; missing Models/Car.xnb below ${RACING_WEB_CONTENT_ROOT}")
        endif()
        if(RACING_WEB_PROGRESSIVE_CONTENT)
            find_package(Python3 REQUIRED COMPONENTS Interpreter)
            set(_racing_web_content_staging
                "${CMAKE_CURRENT_BINARY_DIR}/racing-web-content")
            file(GLOB_RECURSE _racing_web_content_inputs CONFIGURE_DEPENDS
                "${RACING_WEB_CONTENT_ROOT}/*")
            add_custom_command(
                OUTPUT "${_racing_web_content_staging}/manifest.json"
                COMMAND "${Python3_EXECUTABLE}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/package-web-content.py"
                    --content-root "${RACING_WEB_CONTENT_ROOT}"
                    --output-root "${_racing_web_content_staging}"
                DEPENDS
                    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/package-web-content.py"
                    ${_racing_web_content_inputs}
                COMMENT "Staging lossless Racing Web content groups"
                VERBATIM
            )

            set(_racing_web_package_outputs)
            foreach(_racing_web_group IN ITEMS models landscape textures)
                set(_racing_web_package_base
                    "${CMAKE_CURRENT_BINARY_DIR}/RacingGame-content-${_racing_web_group}")
                add_custom_command(
                    OUTPUT
                        "${_racing_web_package_base}.data"
                        "${_racing_web_package_base}.js"
                    COMMAND "${Python3_EXECUTABLE}"
                        "${EMSCRIPTEN_ROOT_PATH}/tools/file_packager.py"
                        "RacingGame-content-${_racing_web_group}.data"
                        --preload
                        "${_racing_web_content_staging}/${_racing_web_group}@/Content"
                        "--js-output=RacingGame-content-${_racing_web_group}.js"
                        --use-preload-cache
                        --no-node
                    DEPENDS
                        "${_racing_web_content_staging}/manifest.json"
                    COMMENT
                        "Packaging Racing ${_racing_web_group} Web content"
                    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                    VERBATIM
                )
                list(APPEND _racing_web_package_outputs
                    "${_racing_web_package_base}.data"
                    "${_racing_web_package_base}.js")
            endforeach()
            add_custom_target(RacingGameWebContentPackages
                DEPENDS ${_racing_web_package_outputs})
            add_dependencies(
                RacingGame_cna_samples RacingGameWebContentPackages)
            target_compile_definitions(RacingGameApplication PRIVATE
                RACING_WEB_PROGRESSIVE_CONTENT=1)
            target_link_options(RacingGame_cna_samples PRIVATE
                --preload-file
                "${_racing_web_content_staging}/bootstrap@/Content"
                --use-preload-cache
            )
        else()
            target_link_options(RacingGame_cna_samples PRIVATE
                --preload-file "${RACING_WEB_CONTENT_ROOT}@/Content"
                --use-preload-cache
            )
        endif()
    else()
        message(STATUS
            "Racing Web content packaging is disabled; set RACING_WEB_CONTENT_ROOT "
            "for a runnable browser bundle")
    endif()
endif()

if(ANDROID)
    target_link_libraries(RacingGame_cna_samples PRIVATE SDL3::SDL3 android)
endif()

if(TARGET SDL3::SDL3main)
    target_link_libraries(RacingGame_cna_samples PRIVATE SDL3::SDL3main)
endif()

if(WIN32)
    set_target_properties(RacingGame_cna_samples PROPERTIES WIN32_EXECUTABLE TRUE)
    if(COMMAND cna_copy_sdl_runtime)
        cna_copy_sdl_runtime(RacingGame_cna_samples)
    endif()
endif()

if(NOT ANDROID)
add_executable(RacingGameTrackProbe
    src/Diagnostics/TrackProbe.cpp
)
target_link_libraries(RacingGameTrackProbe PRIVATE RacingGameCore)

add_executable(RacingGamePhysicsProbe
    src/Diagnostics/PhysicsProbe.cpp
)
target_link_libraries(RacingGamePhysicsProbe PRIVATE RacingGameCore)

add_executable(RacingGameDrivableSceneProbe
    src/Diagnostics/DrivableSceneProbe.cpp
)
target_link_libraries(RacingGameDrivableSceneProbe PRIVATE RacingGameApplication)

add_executable(RacingGameScreenFlowProbe
    src/Diagnostics/ScreenFlowProbe.cpp
)
target_link_libraries(RacingGameScreenFlowProbe PRIVATE RacingGameApplication)

add_executable(RacingGameSettingsProbe
    src/Diagnostics/SettingsProbe.cpp
)
target_link_libraries(RacingGameSettingsProbe PRIVATE RacingGameCore)

add_executable(RacingGameMenuScreensProbe
    src/Diagnostics/MenuScreensProbe.cpp
)
target_link_libraries(RacingGameMenuScreensProbe PRIVATE RacingGameApplication)

add_executable(RacingGameRaceReturnProbe
    src/Diagnostics/RaceReturnProbe.cpp
)
target_link_libraries(RacingGameRaceReturnProbe PRIVATE RacingGameApplication)
if(EMSCRIPTEN AND RACING_WEB_CONTENT_ROOT)
    set_target_properties(RacingGameRaceReturnProbe PROPERTIES SUFFIX ".html")
    target_link_libraries(RacingGameRaceReturnProbe PRIVATE
        CNA::EmscriptenAsyncify)
    target_link_options(RacingGameRaceReturnProbe PRIVATE
        -sALLOW_MEMORY_GROWTH=1
        -sFORCE_FILESYSTEM=1
        -lidbfs.js
        "-sMIN_WEBGL_VERSION=2"
        "-sMAX_WEBGL_VERSION=2"
        "-sEXPORTED_RUNTIME_METHODS=callMain"
        --shell-file "${CMAKE_CURRENT_SOURCE_DIR}/scripts/web-shell.html"
    )
    set_property(TARGET RacingGameRaceReturnProbe APPEND PROPERTY LINK_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/web-shell.html")
    if(RACING_WEB_PROGRESSIVE_CONTENT)
        add_dependencies(
            RacingGameRaceReturnProbe RacingGameWebContentPackages)
        target_link_options(RacingGameRaceReturnProbe PRIVATE
            --preload-file
            "${_racing_web_content_staging}/bootstrap@/Content"
            --use-preload-cache)
    else()
        target_link_options(RacingGameRaceReturnProbe PRIVATE
            --preload-file "${RACING_WEB_CONTENT_ROOT}@/Content"
            --use-preload-cache)
    endif()
endif()

add_executable(RacingGameDesktopRuntimeProbe
    src/Diagnostics/DesktopRuntimeProbe.cpp
)
target_link_libraries(RacingGameDesktopRuntimeProbe PRIVATE RacingGameApplication)

add_executable(RacingGameDeviceResetProbe
    src/Diagnostics/DeviceResetProbe.cpp
)
target_link_libraries(RacingGameDeviceResetProbe PRIVATE RacingGameApplication)

add_executable(RacingGameMobileControlsProbe
    src/Diagnostics/MobileControlsProbe.cpp
)
target_link_libraries(RacingGameMobileControlsProbe PRIVATE RacingGameCore)

add_executable(RacingGameMobileOverlayProbe
    src/Diagnostics/MobileOverlayProbe.cpp
)
target_link_libraries(RacingGameMobileOverlayProbe PRIVATE RacingGameApplication)
endif()
