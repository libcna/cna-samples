cna_add_sample(ShipGame
    SOURCES
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

target_link_libraries(ShipGame_cna_samples PRIVATE
    SharpRuntime::Xml.Serialization
)
