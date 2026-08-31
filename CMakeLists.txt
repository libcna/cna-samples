cmake_minimum_required(VERSION 3.20)

project(CNASamples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Development dependencies — consume the active next-generation sibling
# checkouts directly so a sample audit can fix CNA and sharp-runtime in the
# same session.
# ---------------------------------------------------------------------------

if(EMSCRIPTEN)
    set(_cna_samples_reference_renderer "WEBGL2")
    # MarbleMaze uses the original System.Threading.Thread loading/calibration paths. WebAssembly
    # threads change the memory ABI of the complete application, so select CNA's supported
    # application-wide mode before any framework or third-party target is created.
    set(CNA_ENABLE_EMSCRIPTEN_THREADS ON CACHE BOOL
        "Enable Emscripten pthreads for threaded XNA samples" FORCE)
else()
    set(_cna_samples_reference_renderer "OPENGLES3")
endif()
set(CNA_GRAPHICS_RENDERER "${_cna_samples_reference_renderer}" CACHE STRING
    "CNA Samples reference renderer" FORCE)

set(CNA_SHARP_RUNTIME_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/../sharp-runtimenext"
    CACHE PATH "sharp-runtimenext checkout used by CNA Samples" FORCE)

set(CNA_BUILD_TESTS         OFF CACHE BOOL "Build CNA tests"         FORCE)
set(CNA_BUILD_EXAMPLES      OFF CACHE BOOL "Build CNA examples"      FORCE)
set(SHARP_RUNTIME_BUILD_TESTS OFF CACHE BOOL "Build sharp-runtime tests" FORCE)
set(SHARP_RUNTIME_COMPONENTS
    "Core.Base;IO;IO.IsolatedStorage;Collections.Core;Collections.ObjectModel;Runtime;Threading;Text;Globalization;Storage;Security.Cryptography;Xml.Linq"
    CACHE STRING "Sharp Runtime components required by CNA Samples" FORCE)
set(EASYGL_BUILD_TESTS      OFF CACHE BOOL "Build EasyGL tests"       FORCE)
set(EASYGL_BUILD_EXAMPLES   OFF CACHE BOOL "Build EasyGL examples"    FORCE)
set(CNA_EASYGL_COMPILED_EFFECTS ON CACHE BOOL
    "Enable XNA compiled effects for the EasyGL reference renderers" FORCE)

# CNA_Net/CNA_GamerServices (NetworkSession, GamerServicesComponent, Guide, ...) are
# separate CMake targets in cna, gated behind this option and OFF by default. Several
# samples (ClientServerSample, NetworkPrediction, PeerToPeer, and eventually NetRumble)
# need them.
set(CNA_ENABLE_NET          ON  CACHE BOOL "Enable CNA Net/GamerServices targets" FORCE)

add_subdirectory(../cnanext CNA_BUILD)

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SampleHelpers)

# ---------------------------------------------------------------------------
# Samples — add each subdirectory as it is implemented.
# Comment-out lines for samples that are not yet ported.
# ---------------------------------------------------------------------------

# Phase 1 — Foundation
add_subdirectory(samples/PrimitivesSample)
add_subdirectory(samples/Primitives3D)
add_subdirectory(samples/TexturesAndColors)
add_subdirectory(samples/ShapeRendering)
add_subdirectory(samples/GeneratedGeometry)
add_subdirectory(samples/SpriteEffects)
add_subdirectory(samples/SpriteSheet)
add_subdirectory(samples/SafeArea)
add_subdirectory(samples/InputSequence)
add_subdirectory(samples/InputReporter)
add_subdirectory(samples/ReachGraphicsDemo)

# Phase 2 — 2D Games  (uncomment as implemented)
add_subdirectory(samples/Platformer)
add_subdirectory(samples/CollisionSample)
add_subdirectory(samples/PerPixelCollision)
add_subdirectory(samples/TransformedCollision)
add_subdirectory(samples/TransformedCollisionTest)
add_subdirectory(samples/RectangleCollision)
add_subdirectory(samples/Bounce)
add_subdirectory(samples/PathDrawing)
add_subdirectory(samples/Pathfinding)
add_subdirectory(samples/FlockingSample)
add_subdirectory(samples/Spacewar)
add_subdirectory(samples/TicTacToe)
add_subdirectory(samples/ParticleSample)

# Phase 3 — 3D Graphics
add_subdirectory(samples/ChaseCamera)
add_subdirectory(samples/HeightmapCollision)
add_subdirectory(samples/BloomSample)
add_subdirectory(samples/ShadowMapping)
# add_subdirectory(samples/NormalMapping)
add_subdirectory(samples/SkinningSample)
add_subdirectory(samples/Particles3D)
add_subdirectory(samples/Particles2DPipeline)
add_subdirectory(samples/BillboardSample)
add_subdirectory(samples/InstancedModel)
add_subdirectory(samples/LensFlare)

# Phase 4 — Full Games
add_subdirectory(samples/MarbleMaze)
add_subdirectory(samples/RolePlayingGame)
# add_subdirectory(samples/NetRumble)
add_subdirectory(samples/HoneycombRush)
add_subdirectory(samples/NinjAcademy)
add_subdirectory(samples/CardsStarterKit)
# add_subdirectory(samples/ShipGame)

# Phase 5 — Audio, AI, Misc
add_subdirectory(samples/Audio3D)
add_subdirectory(samples/SoundAndMusic)
add_subdirectory(samples/ChaseAndEvade)
add_subdirectory(samples/AimingSample)
add_subdirectory(samples/FuzzyLogic)
add_subdirectory(samples/InverseKinematics)
add_subdirectory(samples/WaypointSample)
# add_subdirectory(samples/SplitScreen)
add_subdirectory(samples/CameraShake)

# Phase 6/7 — Game frameworks & UI
add_subdirectory(samples/GameStateManagement)
add_subdirectory(samples/CatapultWars)
add_subdirectory(samples/CatapultWarsTrainingHealthBar)
add_subdirectory(samples/CatapultWarsTrainingSecondHuman)
add_subdirectory(samples/CatapultWarsTrainingShotAngle)
add_subdirectory(samples/CatapultWarsTrainingShotGuide)
add_subdirectory(samples/CatapultWarsTrainingSupplyCrate)
add_subdirectory(samples/CatapultWarsTrainingAllFeatures)
add_subdirectory(samples/CatapultWarsTrainingScrollingScreen)
add_subdirectory(samples/Yacht)
add_subdirectory(samples/GesturesSample)
add_subdirectory(samples/TouchThumbsticks)
add_subdirectory(samples/LocalizationSample)
add_subdirectory(samples/SnowShovel)
add_subdirectory(samples/DynamicMenu)
add_subdirectory(samples/UISample)
add_subdirectory(samples/PerformanceMeasuring)
add_subdirectory(samples/SoccerPitch)
add_subdirectory(samples/Orientation)

# ---------------------------------------------------------------------------
# Deferred / not-yet-ported samples — placeholder directories only
# (<Name>.htm + missing.md, no src/CMakeLists.txt yet). Uncomment and add a
# CMakeLists.txt once each sample's CNA blocker (see DEFERRED.md) is fixed and
# real source is added. See ignored.md for samples that will never appear here.
# ---------------------------------------------------------------------------

# Phase 1/2 stragglers missed by earlier add_subdirectory sweeps
add_subdirectory(samples/ColorReplacement)

# Phase 3 — 3D Graphics & Shaders (remaining)
add_subdirectory(samples/DistortionSample)
add_subdirectory(samples/NonPhotoRealistic)
add_subdirectory(samples/NormalMappingEffect)
add_subdirectory(samples/PerPixelLighting)
add_subdirectory(samples/VertexLighting)
add_subdirectory(samples/RimLighting)
add_subdirectory(samples/ShatterEffect)
add_subdirectory(samples/XmlParticles)
add_subdirectory(samples/Graphics3D)
add_subdirectory(samples/PickingSample)
add_subdirectory(samples/TrianglePicking)

# Phase 4 — Models & Animation
add_subdirectory(samples/SimpleAnimation)
add_subdirectory(samples/CustomModelAnimation)
add_subdirectory(samples/CustomModelClass)
add_subdirectory(samples/CustomModelEffect)
add_subdirectory(samples/SkinnedModelExtensions)
add_subdirectory(samples/CPUSkinning)

# Phase 6 — Full Games (remaining)
# add_subdirectory(samples/TankOnHeightmap)

# Deferred — Phone Hardware / Networking (PLAN.md appendix; the only 6 of ~27
# kept as future candidates — see ignored.md for the rest)
add_subdirectory(samples/AccelerometerSample)
add_subdirectory(samples/TiltPerspective)
add_subdirectory(samples/MicrophoneEcho)
add_subdirectory(samples/ClientServerSample)
add_subdirectory(samples/NetworkPrediction)
add_subdirectory(samples/PeerToPeer)
