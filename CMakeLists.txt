cmake_minimum_required(VERSION 3.18)
project(blaze)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-stdlib=libc++ -ffast-math)

#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_definitions(
    -DNOMINMAX
    -DWIN32_LEAN_AND_MEAN
    -DGLM_FORCE_DEFAULT_ALIGNED_GENTYPES
    -DGLM_FORCE_SWIZZLE
    -DGLM_FORCE_INLINE
)

add_subdirectory(blaze)
add_subdirectory(sandbox)
