add_executable(violet
    violet/Main.cpp
)
add_library(violetsrc STATIC
    violet/CommandLine.cpp

    violet/generate/SiteGenerator.cpp
    violet/generate/FileManager.cpp

    violet/conf/ToJsonImpl.cpp
)
target_include_directories(violetsrc PUBLIC .)
target_link_libraries(violetsrc PUBLIC
    CLI11::CLI11
    nlohmann_json::nlohmann_json
    pantor::inja
)

target_link_libraries(violet violetsrc)

