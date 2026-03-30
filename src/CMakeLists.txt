add_executable(violet
    violet/Main.cpp
)
add_library(violetsrc INTERFACE
)
target_include_directories(violetsrc INTERFACE .)

target_link_libraries(violet violetsrc)

