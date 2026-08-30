# cmake/SampleHelpers.cmake
# Helpers for registering CNA sample executables.

# cna_add_sample(target_name
#     SOURCES src/Foo.cpp src/Bar.cpp
#     [CONTENT_DIR path/to/Content]   # optional: copies assets next to the exe
#     [GAMER_SERVICES]                 # optional: links CNA_GamerServices
#     [NET]                            # optional: links CNA_Net/GamerServices
# )
#
# Creates an executable named `<target_name>_cna_samples` (use exact directory
# name, e.g. SafeArea, Platformer) and links it against CNA's public aggregate
# target. CNA itself supplies the selected renderer and sharp-runtime component
# closure transitively.
function(cna_add_sample target_name)
    cmake_parse_arguments(ARG "GAMER_SERVICES;NET" "CONTENT_DIR" "SOURCES" ${ARGN})

    set(full_target "${target_name}_cna_samples")

    add_executable(${full_target} ${ARG_SOURCES})

    target_include_directories(${full_target} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    target_link_libraries(${full_target} PRIVATE CNA)

    if(EMSCRIPTEN)
        set_target_properties(${full_target} PROPERTIES SUFFIX ".html")
        target_link_options(${full_target} PRIVATE
            -sALLOW_MEMORY_GROWTH=1
            -sFORCE_FILESYSTEM=1
            "-sMIN_WEBGL_VERSION=2"
            "-sMAX_WEBGL_VERSION=2"
        )
    endif()

    if(TARGET SDL3::SDL3main)
        target_link_libraries(${full_target} PRIVATE SDL3::SDL3main)
    endif()

    # CNA_Net pulls CNA_GamerServices transitively. Keep that optional subsystem out
    # of unrelated native and web sample dependency closures.
    if(ARG_NET)
        if(NOT TARGET CNA_Net)
            message(FATAL_ERROR "${target_name} requires CNA_Net, but CNA_ENABLE_NET is OFF")
        endif()
        target_link_libraries(${full_target} PRIVATE CNA_Net)
    elseif(ARG_GAMER_SERVICES)
        if(NOT TARGET CNA_GamerServices)
            message(FATAL_ERROR
                "${target_name} requires CNA_GamerServices, but gamer services are unavailable")
        endif()
        target_link_libraries(${full_target} PRIVATE CNA_GamerServices)
    endif()

    if(WIN32)
        set_target_properties(${full_target} PROPERTIES WIN32_EXECUTABLE TRUE)
        if(COMMAND cna_copy_sdl_runtime)
            cna_copy_sdl_runtime(${full_target})
        endif()
    endif()

    # Native builds load Content beside the executable. Web builds package the
    # same directory into Emscripten's virtual filesystem at /Content.
    if(ARG_CONTENT_DIR)
        if(EMSCRIPTEN)
            target_link_options(${full_target} PRIVATE
                --preload-file "${ARG_CONTENT_DIR}@/Content"
            )
        else()
            set(content_target "${full_target}_content")
            add_custom_target(${content_target}
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${ARG_CONTENT_DIR}"
                    "$<TARGET_FILE_DIR:${full_target}>/Content"
                VERBATIM
            )
            add_dependencies(${full_target} ${content_target})
        endif()
    endif()
endfunction()
