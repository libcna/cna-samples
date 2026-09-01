cna_add_sample(LocalizationSample
    SOURCES
        src/LocalizationGame.cpp
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
        src/Strings.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

target_link_libraries(LocalizationSample_cna_samples PRIVATE SharpRuntime::Resources)
