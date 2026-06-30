add_executable(violet
    violet/Main.cpp
)
add_library(violetsrc STATIC
    violet/CommandLine.cpp

    violet/conf/ToJsonImpl.cpp

    violet/generate/FileManager.cpp
    violet/generate/SiteGenerator.cpp

    violet/generate/templates/InjaManager.cpp
    violet/generate/templates/DateTimeFunctions.cpp
    violet/generate/templates/FileFunctions.cpp
    violet/generate/templates/HtmlFunctions.cpp
    violet/generate/templates/LangFunctions.cpp

    violet/paginator/Paginator.cpp
    violet/paginator/SortPredicates.cpp

    violet/parsing/DateTimeParsing.cpp
    violet/parsing/Escaping.cpp
    violet/parsing/Markdown.cpp

    violet/parsing/markdown/DocumentContext.cpp
    violet/parsing/markdown/impl/MetaParser.cpp
    violet/parsing/markdown/impl/ParagraphParsing.cpp
    violet/parsing/markdown/impl/ParserCore.cpp
    violet/parsing/markdown/impl/Render.cpp
)

target_include_directories(violetsrc PUBLIC .)
target_link_libraries(violetsrc PUBLIC
    CLI12::CLI12
    nlohmann_json::nlohmann_json
    pantor::inja
    stc
    minilog
)

if (LINUX)
    target_link_libraries(violetsrc PUBLIC stdc++exp)
endif()

target_link_libraries(violet violetsrc)

install(TARGETS violet DESTINATION bin)
