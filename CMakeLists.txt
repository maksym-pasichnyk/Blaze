cmake_minimum_required(VERSION 3.18)
project(blaze)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-stdlib=libc++ -ffast-math)

add_subdirectory(blaze)
add_subdirectory(sandbox)

target_compile_shaders(blaze
    assets/blaze/shaders/imgui.frag
    assets/blaze/shaders/imgui.vert
)
target_compile_shaders(sandbox
    assets/sandbox/shaders/texture.frag
    assets/sandbox/shaders/texture.vert
    assets/sandbox/shaders/gfx.frag
    assets/sandbox/shaders/gfx.vert
)

