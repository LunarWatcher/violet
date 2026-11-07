add_executable(violet
    violet/Main.cpp
)
set_source_files_properties(violet/data/GitVersion.cpp PROPERTIES COMPILE_DEFINITIONS -DVIOLET_VERSION="${VIOLET_VERSION}")
add_library(violetgitdata STATIC
    violet/data/GitVersion.cpp
    violet/data/GitVersion.hpp
)

set(VIOLET_SERVE_FILES
    violet/serve/SiteServer.cpp
    violet/serve/SiteServer.hpp
)

set(VIOLET_SRCS
    violet/CommandLine.cpp

    violet/conf/ToJsonImpl.cpp
    violet/conf/Frontmatter.cpp

    violet/data/RenderResult.cpp

    violet/generate/FileManager.cpp
    violet/generate/SiteGenerator.cpp

    violet/generate/buildmeta/BuildMeta.cpp
    violet/generate/buildmeta/BuildMetaJson.cpp

    violet/generate/templates/IrisManager.cpp
    violet/generate/templates/DateTimeFunctions.cpp
    violet/generate/templates/FileFunctions.cpp
    violet/generate/templates/EscapingFunctions.cpp
    violet/generate/templates/LangFunctions.cpp

    violet/generate/templates/types/RenderContext.cpp
    violet/generate/templates/types/TreePagesData.cpp

    violet/generate/watch/OneShotRebuildHandler.cpp
    violet/generate/watch/WatchRebuildHandler.cpp

    violet/init/InitModule.cpp

    violet/paginator/Paginator.cpp
    violet/paginator/SortPredicates.cpp

    violet/parsing/DateTimeParsing.cpp
    violet/parsing/Escaping.cpp
    violet/parsing/LinkTranslate.cpp
    violet/parsing/Markdown.cpp

    violet/parsing/markdown/DocumentContext.cpp
    violet/parsing/markdown/impl/CodeParsing.cpp
    violet/parsing/markdown/impl/MetaParser.cpp
    violet/parsing/markdown/impl/ParagraphParsing.cpp
    violet/parsing/markdown/impl/ParserCore.cpp
    violet/parsing/markdown/impl/Render.cpp

    violet/util/ErrorHandling.cpp
)

set(VIOLET_HDRS
    violet/CommandLine.hpp

    violet/conf/Config.hpp
    violet/conf/Frontmatter.hpp
    violet/conf/ThemeConfig.hpp

    violet/data/Constants.hpp
    violet/data/GenerateOpts.hpp
    violet/data/InitOpts.hpp
    violet/data/RenderResult.hpp
    violet/data/VioletGlobal.hpp

    violet/generate/FileManager.hpp
    violet/generate/ProcessedFileType.hpp
    violet/generate/SiteGenerator.hpp

    violet/generate/buildmeta/BuildMeta.hpp
    violet/generate/buildmeta/BuildMetaJson.hpp

    violet/generate/templates/IrisManager.hpp
    violet/generate/templates/DateTimeFunctions.hpp
    violet/generate/templates/FileFunctions.hpp
    violet/generate/templates/EscapingFunctions.hpp
    violet/generate/templates/LangFunctions.hpp

    violet/generate/templates/ext/SafeCastEnum.hpp

    violet/generate/templates/types/RenderContext.hpp
    violet/generate/templates/types/TreePagesData.hpp

    violet/generate/watch/AutoRebuildHandler.hpp
    violet/generate/watch/OneShotRebuildHandler.hpp
    violet/generate/watch/WatchRebuildHandler.hpp

    violet/init/InitModule.hpp

    violet/paginator/Paginator.hpp
    violet/paginator/SortPredicates.hpp

    violet/parsing/DateTimeParsing.hpp
    violet/parsing/Escaping.hpp
    violet/parsing/LinkTranslate.hpp
    violet/parsing/Markdown.hpp

    violet/parsing/markdown/CommonDefs.hpp
    violet/parsing/markdown/ContextConsumingNodes.hpp
    violet/parsing/markdown/ContextProvidingNodes.hpp
    violet/parsing/markdown/DocumentContext.hpp
    violet/parsing/markdown/DOMTree.hpp
    violet/parsing/markdown/ElementaryNodes.hpp

    violet/util/ErrorHandling.hpp
    violet/util/LockStreamPos.hpp
)

add_library(violetsrc STATIC
    ${VIOLET_SRCS}
    ${VIOLET_HDRS}
    $<$<BOOL:${VIOLET_ENABLE_SERVER}>:${VIOLET_SERVE_FILES}>
)


if (VIOLET_ENABLE_SERVER)
    target_compile_definitions(
        violetsrc PUBLIC -DVIOLET_ENABLE_SERVER=ON
    )
endif()

target_include_directories(violetsrc PUBLIC .)
target_link_libraries(violetsrc PUBLIC
    CLI12::CLI12
    nlohmann_json::nlohmann_json
    iris::iris
    stc
    minilog
    violetgitdata
    ${VIOLET_EXTRA_LIBRARIES}
)

if (LINUX)
    target_link_libraries(violetsrc PUBLIC stdc++exp)
endif()

target_link_libraries(violet violetsrc)

install(TARGETS violet DESTINATION bin)
