cna_add_sample(ShapeRendering
    SOURCES
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
)

target_compile_definitions(ShapeRendering_cna_samples PRIVATE
    $<$<CONFIG:Debug>:SHAPE_RENDERING_SAMPLE_DEBUG>
)
