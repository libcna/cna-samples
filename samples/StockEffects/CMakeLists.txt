if(EMSCRIPTEN)
    return()
endif()

add_executable(CompileEffect_cna_samples
    src/CustomImporterContext.cpp
    src/CustomLogger.cpp
    src/CustomProcessorContext.cpp
    src/Program.cpp
)

target_include_directories(CompileEffect_cna_samples PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(CompileEffect_cna_samples PRIVATE
    CNA::ContentPipelineBuild
    SharpRuntime::IO
)

set_target_properties(CompileEffect_cna_samples PROPERTIES
    OUTPUT_NAME CompileEffect
)
