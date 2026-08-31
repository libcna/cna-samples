cna_add_sample(HoneycombRush
    SOURCES
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
    GAMER_SERVICES
)

target_link_libraries(HoneycombRush_cna_samples PRIVATE
    SharpRuntime::IO.IsolatedStorage
    SharpRuntime::Xml.Linq
)
