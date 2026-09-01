cna_add_sample(UISample
    SOURCES
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

target_compile_definitions(UISample_cna_samples PRIVATE WINDOWS_PHONE)

target_link_libraries(UISample_cna_samples PRIVATE
    SharpRuntime::Diagnostics
    SharpRuntime::IO.IsolatedStorage
)
