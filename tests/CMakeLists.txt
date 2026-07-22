add_executable(tests
    src/Main.cpp

    src/fixtures/LoadWorkspace.cpp
    src/fixtures/TestSiteGenerator.cpp

    src/integration/BasicSiteGenerationTests.cpp
    src/integration/BlogIntegrationTests.cpp

    src/parsing/FootnoteTests.cpp
    src/parsing/ListTests.cpp
    src/parsing/MarkdownTests.cpp
    src/parsing/QuoteTests.cpp
    src/parsing/UrlTests.cpp
    src/parsing/UrlTranslationTests.cpp

    src/parsing/ext/SafeCastEnumTests.cpp

    src/parsing/markdown/EscapingTests.cpp
    src/parsing/markdown/HorizontalRuleTests.cpp
    src/parsing/markdown/IDifyTests.cpp
    src/parsing/markdown/ImageTests.cpp
    src/parsing/markdown/TemplateParsing.cpp
    src/parsing/markdown/ToCGenerationTests.cpp

    src/utils/FileReader.cpp
)

Include(FetchContent)

# set(CATCH_CONFIG_DEFAULT_REPORTER "explicit" CACHE STRING "" FORCE)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.15.1
)

FetchContent_MakeAvailable(Catch2)

target_link_libraries(tests Catch2::Catch2 violetsrc stc stc::testutil)

target_include_directories(tests PUBLIC src/)

target_compile_definitions(tests PUBLIC -DTEST_ROOT_PATH="${CMAKE_CURRENT_SOURCE_DIR}")

# vim:ft=cmake
