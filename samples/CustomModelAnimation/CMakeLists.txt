cna_add_sample(CustomModelAnimation
    SOURCES
        src/CustomModelAnimationSampleGame.cpp
        src/ModelAnimationClip.cpp
        src/ModelAnimationPlayerBase.cpp
        src/ModelData.cpp
        src/ModelDataContentReaders.cpp
        src/ModelKeyframe.cpp
        src/RigidAnimationPlayer.cpp
        src/RootAnimationPlayer.cpp
        src/SkinnedAnimationPlayer.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)
