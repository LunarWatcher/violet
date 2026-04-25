add_executable(violet
    violet/Main.cpp
)
add_library(violetsrc STATIC
    violet/CommandLine.cpp

    violet/conf/ToJsonImpl.cpp

    violet/generate/SiteGenerator.cpp
    violet/generate/FileManager.cpp

    violet/generate/templates/InjaManager.cpp

    violet/parsing/DateTimeParsing.cpp
    violet/parsing/Markdown.cpp
    violet/parsing/Escaping.cpp

    violet/parsing/markdown/impl/Render.cpp
    violet/parsing/markdown/impl/ParagraphParsing.cpp
)

target_include_directories(violetsrc PUBLIC .)
target_link_libraries(violetsrc PUBLIC
    CLI11::CLI11
    nlohmann_json::nlohmann_json
    pantor::inja
    stc
)

if (LINUX)
    target_link_libraries(violetsrc PUBLIC stdc++exp)
endif()

target_link_libraries(violet violetsrc)

