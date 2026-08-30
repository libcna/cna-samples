cna_add_sample(CPUSkinning
    SOURCES
        src/CpuSkinningDataTypes/Animation/AnimationClip.cpp
        src/CpuSkinningDataTypes/Animation/AnimationPlayer.cpp
        src/CpuSkinningDataTypes/Animation/Keyframe.cpp
        src/CpuSkinningDataTypes/Animation/SkinningData.cpp
        src/CpuSkinningDataTypes/CpuSkinnedModel.cpp
        src/CpuSkinningDataTypes/CpuSkinnedModelPart.cpp
        src/CpuSkinningDataTypes/CpuSkinningHelpers.cpp
        src/CpuSkinningDataTypes/CpuSkinningContentReaders.cpp
        src/CpuSkinningDemoGame.cpp
        src/FrameRateCounter.cpp
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)
