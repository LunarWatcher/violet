cmake_minimum_required(VERSION 3.27)
project(violet)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(VIOLET_COVERAGE "Whether or not to build with test coverage" OFF)
option(VIOLET_SANITISE "Whether or not to -fsanitise=undefined. No effect on Windows" OFF)

message(STATUS "Violet build configuration:")
message(STATUS "\t- Sanitising: ${VIOLET_SANITISE}")
message(STATUS "\t- Coverage: ${VIOLET_COVERAGE}")

if (VIOLET_COVERAGE)
    string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" CMAKE_CXX_COMPILER_VERSION_REGEX_MATCH  ${CMAKE_CXX_COMPILER_VERSION})
    set(COMPILER_MAJOR ${CMAKE_MATCH_1})
    find_program(GCOV NAMES gcov-${COMPILER_MAJOR} gcov REQUIRED)
    find_program(LCOV lcov REQUIRED)
    find_program(GENHTML genhtml REQUIRED)

    add_compile_options(-fprofile-arcs -ftest-coverage)
    add_link_options(--coverage)
    add_custom_target(gen-coverage
        COMMAND ${LCOV} --capture --output coverage.info --include "${CMAKE_SOURCE_DIR}/src/\\*" --exclude "${CMAKE_BINARY_DIR}/_deps/\\*" --directory . --gcov-tool ${GCOV} --ignore-errors inconsistent,gcov
        COMMAND ${GENHTML} coverage.info --output coverage-html --ignore-errors inconsistent
    )
endif()

if (NOT WIN32 AND VIOLET_SANITISE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

set(INJA_USE_EMBEDDED_JSON OFF CACHE STRING "" FORCE)
set(BUILD_TESTING OFF CACHE STRING "" FORCE)
set(INJA_BUILD_TESTS OFF CACHE STRING "" FORCE)
set(INJA_EXPORT OFF CACHE STRING "" FORCE)
set(INJA_INSTALL OFF CACHE STRING "" FORCE)

include(FetchContent)
FetchContent_Declare(stc
    GIT_REPOSITORY https://github.com/LunarWatcher/stc
)
FetchContent_Declare(cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG v2.6.2
)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.12.0
)
FetchContent_Declare(inja
    GIT_REPOSITORY https://github.com/pantor/inja
    GIT_TAG v3.5.0
)
FetchContent_MakeAvailable(stc cli11 json inja)

add_subdirectory(src)

add_subdirectory(tests EXCLUDE_FROM_ALL)
add_custom_target(test
    COMMAND tests
    DEPENDS tests
    COMMENT "Test violet")
add_custom_target(run
    COMMAND violet
    DEPENDS violet
    COMMENT "Run violet")

# vim:ft=cmake
