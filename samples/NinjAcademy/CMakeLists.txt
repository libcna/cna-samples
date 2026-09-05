cna_add_sample(NinjAcademy
    GAMER_SERVICES
    SOURCES
        src/ContentReaders.cpp
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)

target_link_libraries(NinjAcademy_cna_samples PRIVATE
    SharpRuntime::IO.IsolatedStorage
)
