add_executable(tests
    src/Main.cpp

    src/SelfTests.cpp

    src/parsing/FootnoteTests.cpp
    src/parsing/MarkdownTests.cpp
    src/parsing/UrlTests.cpp
    src/parsing/UrlTranslationTests.cpp
    src/parsing/QuoteTests.cpp
)

Include(FetchContent)

# Not yet sourcing stc, so this won't work yet
set(CATCH_CONFIG_DEFAULT_REPORTER "explicit" CACHE STRING "" FORCE)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.14.0
)

FetchContent_MakeAvailable(Catch2)

target_link_libraries(tests Catch2::Catch2 violetsrc stc stc::testutil)

target_include_directories(tests PUBLIC src/)

target_compile_definitions(tests PUBLIC -DTEST_ROOT_PATH="${CMAKE_CURRENT_SOURCE_DIR}")

# vim:ft=cmake
