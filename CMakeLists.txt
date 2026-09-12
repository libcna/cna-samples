cmake_minimum_required(VERSION 3.20)

project(CNASamples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Development dependencies — consume the active sibling checkouts directly so a sample audit can
# fix CNA and sharp-runtime in the same session.
# ---------------------------------------------------------------------------

if(EMSCRIPTEN)
    set(_cna_samples_reference_renderer "WEBGL2")
else()
    set(_cna_samples_reference_renderer "OPENGLES3")
endif()
set(CNA_GRAPHICS_RENDERER "${_cna_samples_reference_renderer}" CACHE STRING
    "CNA Samples reference renderer" FORCE)

option(CNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS
    "Enable the shared-memory Emscripten ABI for samples that use System.Threading" OFF)
set(CNA_ENABLE_EMSCRIPTEN_THREADS "${CNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS}" CACHE BOOL
    "Build CNA's Emscripten ABI with shared-memory pthread support" FORCE)
if(EMSCRIPTEN AND CNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS)
    # Sharp Runtime preallocates one worker by default. Threaded samples can create another loading
    # thread before the first has returned to the browser event loop, so their application-wide
    # build uses the campaign's qualified pool size. Single-threaded samples remain compatible
    # with static hosting that cannot provide SharedArrayBuffer/COOP/COEP headers.
    set(SHARP_RUNTIME_EMSCRIPTEN_PTHREAD_POOL_SIZE "8" CACHE STRING
        "Preallocated Emscripten pthread workers for threaded XNA samples" FORCE)
endif()

set(CNA_SAMPLES_CNA_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/../cna"
    CACHE PATH "CNA checkout used by CNA Samples")
if(NOT EXISTS "${CNA_SAMPLES_CNA_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR
        "CNA Samples: CNA_SAMPLES_CNA_ROOT does not name a CNA checkout: "
        "${CNA_SAMPLES_CNA_ROOT}")
endif()

set(CNA_SHARP_RUNTIME_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/../sharp-runtime"
    CACHE PATH "sharp-runtime checkout used by CNA Samples" FORCE)
if(NOT EXISTS "${CNA_SHARP_RUNTIME_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR
        "CNA Samples: CNA_SHARP_RUNTIME_ROOT does not name a sharp-runtime checkout: "
        "${CNA_SHARP_RUNTIME_ROOT}")
endif()

set(CNA_BUILD_TESTS         OFF CACHE BOOL "Build CNA tests"         FORCE)
set(CNA_BUILD_EXAMPLES      OFF CACHE BOOL "Build CNA examples"      FORCE)
set(SHARP_RUNTIME_BUILD_TESTS OFF CACHE BOOL "Build sharp-runtime tests" FORCE)
set(SHARP_RUNTIME_COMPONENTS
    "Core.Base;Console;IO;IO.IsolatedStorage;Collections.Core;Collections.ObjectModel;Runtime;Threading;Threading.Tasks;Text;Globalization;Resources;Storage;Security.Cryptography;Diagnostics;Uri;Net;Net.Sockets;Net.Http;Xml;Xml.Linq;Xml.Serialization;ServiceModel"
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

add_subdirectory("${CNA_SAMPLES_CNA_ROOT}" CNA_BUILD)

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SampleHelpers)

# ---------------------------------------------------------------------------
# Samples — add each subdirectory as it is implemented.
# Comment-out lines for samples that are not yet ported.
#
# CNA_SAMPLES_ONLY narrows that list. Empty (the default) configures the whole
# corpus, exactly as before; a list of directory names configures only those.
#
# It exists because a per-sample artifact root under /rv/tmp/samples configures
# this project to build ONE sample, and got build scaffolding for all ~94 --
# which is cheap in disk (about 3 MB) but expensive in two ways that are not:
# `cmake --build <tree>` with no target builds the entire corpus, which is how
# rules.md's "one sample carried a full build of 15 unrelated samples" happened,
# and every configure pays for 94 subdirectories it will never build.
#
#     cmake -S . -B <tree> -DCNA_SAMPLES_ONLY=DynamicMenu
#     cmake -S . -B <tree> -DCNA_SAMPLES_ONLY="CollisionSample;Bounce"
#
# A directory may declare more than one target (CollisionSample does), so the
# filter is by directory rather than by target on purpose.
# ---------------------------------------------------------------------------
set(CNA_SAMPLES_ONLY "" CACHE STRING
    "Semicolon-separated sample directory names to configure; empty means all of them.")

function(cna_sample NAME)
    if(NOT CNA_SAMPLES_ONLY OR "${NAME}" IN_LIST CNA_SAMPLES_ONLY)
        add_subdirectory(samples/${NAME})
    endif()
endfunction()

# Phase 1 — Foundation
cna_sample(PrimitivesSample)
cna_sample(Primitives3D)
cna_sample(TexturesAndColors)
cna_sample(ShapeRendering)
cna_sample(GeneratedGeometry)
cna_sample(SpriteEffects)
cna_sample(SpriteSheet)
cna_sample(SafeArea)
cna_sample(InputSequence)
cna_sample(InputReporter)
cna_sample(ReachGraphicsDemo)

# Phase 2 — 2D Games  (uncomment as implemented)
cna_sample(Platformer)
cna_sample(CollisionSample)
cna_sample(PerPixelCollision)
cna_sample(TransformedCollision)
cna_sample(TransformedCollisionTest)
cna_sample(RectangleCollision)
cna_sample(Bounce)
cna_sample(PathDrawing)
cna_sample(Pathfinding)
cna_sample(FlockingSample)
cna_sample(Spacewar)
cna_sample(TicTacToe)
cna_sample(ParticleSample)

# Phase 3 — 3D Graphics
cna_sample(ChaseCamera)
cna_sample(HeightmapCollision)
cna_sample(BloomSample)
cna_sample(ShadowMapping)
# cna_sample(NormalMapping)
cna_sample(SkinningSample)
cna_sample(Particles3D)
cna_sample(Particles2DPipeline)
cna_sample(BillboardSample)
cna_sample(InstancedModel)
cna_sample(LensFlare)

# Phase 4 — Full Games
cna_sample(MarbleMaze)
cna_sample(RolePlayingGame)
cna_sample(NetRumble)
cna_sample(HoneycombRush)
cna_sample(NinjAcademy)
cna_sample(CardsStarterKit)
cna_sample(ShipGame)

# Phase 5 — Audio, AI, Misc
cna_sample(Audio3D)
cna_sample(SoundAndMusic)
cna_sample(ChaseAndEvade)
cna_sample(AimingSample)
cna_sample(FuzzyLogic)
cna_sample(InverseKinematics)
cna_sample(WaypointSample)
cna_sample(SplitScreen)
cna_sample(CameraShake)

# Phase 6/7 — Game frameworks & UI
cna_sample(GameStateManagement)
cna_sample(CatapultWars)
cna_sample(CatapultWarsTrainingHealthBar)
cna_sample(CatapultWarsTrainingSecondHuman)
cna_sample(CatapultWarsTrainingShotAngle)
cna_sample(CatapultWarsTrainingShotGuide)
cna_sample(CatapultWarsTrainingSupplyCrate)
cna_sample(CatapultWarsTrainingAllFeatures)
cna_sample(CatapultWarsTrainingScrollingScreen)
cna_sample(Yacht)
cna_sample(GesturesSample)
cna_sample(TouchThumbsticks)
cna_sample(LocalizationSample)
cna_sample(SnowShovel)
cna_sample(DynamicMenu)
cna_sample(UISample)
cna_sample(PerformanceMeasuring)
cna_sample(PerformanceUtility)
cna_sample(SoccerPitch)
cna_sample(Orientation)

# ---------------------------------------------------------------------------
# Deferred / not-yet-ported samples — placeholder directories only
# (<Name>.htm + missing.md, no src/CMakeLists.txt yet). Uncomment and add a
# CMakeLists.txt once each sample's CNA blocker (see DEFERRED.md) is fixed and
# real source is added. See ignored.md for samples that will never appear here.
# ---------------------------------------------------------------------------

# Phase 1/2 stragglers missed by earlier add_subdirectory sweeps
cna_sample(ColorReplacement)
cna_sample(TiledSprites)

# Phase 3 — 3D Graphics & Shaders (remaining)
cna_sample(DistortionSample)
cna_sample(NonPhotoRealistic)
cna_sample(NormalMappingEffect)
cna_sample(PerPixelLighting)
cna_sample(VertexLighting)
cna_sample(RimLighting)
cna_sample(ShatterEffect)
cna_sample(XmlParticles)
cna_sample(Graphics3D)
cna_sample(PickingSample)
cna_sample(TrianglePicking)

# Phase 4 — Models & Animation
cna_sample(SimpleAnimation)
cna_sample(CustomModelAnimation)
cna_sample(CustomModelClass)
cna_sample(CustomModelEffect)
cna_sample(SkinnedModelExtensions)
cna_sample(CPUSkinning)

# Phase 6 — Full Games (remaining)
cna_sample(TankOnHeightmap)

# Deferred — Phone Hardware / Networking (PLAN.md appendix; the only 6 of ~27
# kept as future candidates — see ignored.md for the rest)
cna_sample(AccelerometerSample)
cna_sample(TiltPerspective)
cna_sample(MicrophoneEcho)
cna_sample(ClientServerSample)
cna_sample(NetworkPrediction)
cna_sample(PeerToPeer)
cna_sample(ContentManifestExtensions)
cna_sample(ModelImporterSample)
