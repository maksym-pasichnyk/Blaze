cmake_minimum_required(VERSION 3.18)
project(blaze)

include(FetchContent)

set(GLFW_LIBRARY_TYPE "STATIC")
set(PHYSFS_BUILD_SHARED FALSE)
set(PHYSFS_TARGETNAME_UNINSTALL physfs_uninstall)

find_package(Vulkan REQUIRED)

FetchContent_Declare(range-v3
    GIT_REPOSITORY https://github.com/ericniebler/range-v3.git
)
FetchContent_Declare(SPIRV-Cross
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
    GIT_TAG main
)
FetchContent_Declare(optional
    GIT_REPOSITORY https://github.com/TartanLlama/optional.git
)
FetchContent_Declare(physfs
    GIT_REPOSITORY https://github.com/icculus/physfs.git
    GIT_TAG main
)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
)
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.x
)
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
)
FetchContent_Declare(VulkanMemoryAllocator
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
)

FetchContent_MakeAvailable(range-v3 SPIRV-Cross optional physfs glm spdlog imgui glfw VulkanMemoryAllocator)

add_library(blaze STATIC
    internal/vk_mem_alloc.cpp
    internal/VulkanGfxDevice.cpp
    internal/VulkanGfxDevice.hpp
    internal/VulkanSwapchain.cpp
    internal/VulkanSwapchain.hpp
    internal/VulkanGraphicsBuffer.hpp
    internal/VulkanCommandBuffer.hpp
    internal/VulkanTexture.hpp
    internal/VulkanMaterial.hpp

    src/Graphics.hpp
    src/Graphics.cpp
    src/Screen.cpp
    src/Screen.hpp
    src/Display.cpp
    src/Display.hpp
    src/CommandPool.cpp
    src/CommandPool.hpp
    src/GraphicsBuffer.cpp
    src/GraphicsBuffer.hpp
    src/Blaze.cpp
    src/Blaze.hpp
    src/Texture.cpp
    src/Texture.hpp
    src/Input.cpp
    src/Input.hpp
    src/UserInterface.cpp
    src/UserInterface.hpp
    src/Material.hpp
    src/Resource.hpp
    src/Resources.cpp
    src/Resources.hpp
    src/Json.hpp
    src/Json.cpp
    src/Utility.hpp
    src/Material.cpp
    src/Mesh.hpp
    src/CommandBuffer.cpp
    src/CommandBuffer.hpp
    src/TextureData.cpp
    src/TextureData.hpp
    src/ThreadPool.cpp
    src/ThreadPool.hpp
    src/Time.cpp
    src/Time.hpp
    src/GraphicsFence.cpp
    src/GraphicsFence.hpp
    src/Signal.hpp
    src/Delegate.hpp
)

set_target_properties(blaze PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(blaze
    PUBLIC
        "${physfs_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
#    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/internal"
        "${VulkanMemoryAllocator_SOURCE_DIR}/include"
)

target_compile_definitions(blaze PUBLIC
    -DVULKAN_HPP_NO_STRUCT_CONSTRUCTORS
    -DVULKAN_HPP_NO_UNION_CONSTRUCTORS
    -DVMA_STATIC_VULKAN_FUNCTIONS=0
    -DVMA_DYNAMIC_VULKAN_FUNCTIONS=1
    -DVK_NO_PROTOTYPES
)

add_library(imgui STATIC
    "${imgui_SOURCE_DIR}/imgui.h"
    "${imgui_SOURCE_DIR}/imconfig.h"
    "${imgui_SOURCE_DIR}/imgui_internal.h"
    "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h"
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
)
set_target_properties(imgui PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR}")
target_compile_definitions(imgui PUBLIC -DIMGUI_DEFINE_MATH_OPERATORS)

if(WIN32)
    target_compile_options(blaze PRIVATE "/MP")
    target_compile_options(imgui PRIVATE "/MP")
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(blaze PRIVATE glfw)

    target_compile_definitions(blaze PUBLIC
        -DGLFW_INCLUDE_NONE
        -DGLFW_INCLUDE_VULKAN
    )
    target_compile_definitions(blaze PUBLIC
        -DVK_USE_PLATFORM_WIN32_KHR
    )
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_compile_definitions(blaze PUBLIC
        -DVK_USE_PLATFORM_XCB_KHR
    )
elseif (CMAKE_SYSTEM_NAME MATCHES "Android")
    target_sources(blaze PUBLIC src/android.cpp)

    target_compile_definitions(blaze PUBLIC
        -DVK_USE_PLATFORM_ANDROID_KHR
    )

    target_link_libraries(blaze PUBLIC android log)
else()
    message("System: ${CMAKE_SYSTEM_NAME}")

    target_link_libraries(blaze PRIVATE glfw)

    target_compile_definitions(blaze PUBLIC
        -DGLFW_INCLUDE_NONE
        -DGLFW_INCLUDE_VULKAN
    )
endif()

target_link_libraries(blaze
    PUBLIC
        glm
        imgui
        spdlog
        range-v3
        tl::optional
        physfs-static
#    PRIVATE
        Vulkan::Vulkan
        spirv-cross-glsl
        spirv-cross-hlsl
        spirv-cross-cpp
        spirv-cross-reflect
        spirv-cross-msl
        spirv-cross-util
        spirv-cross-core
)

function(target_compile_shaders TARGET)
    set(SPIRV_BINARY_FILES)

    foreach(SOURCE_FILE ${ARGN})
#        get_filename_component(FILE_NAME ${SOURCE_FILE} NAME)

        set(SPIRV "${SOURCE_FILE}.spv")
        add_custom_command(
            OUTPUT ${SPIRV}
#            COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders"
            COMMAND glslc ${SOURCE_FILE} -o ${SPIRV}
            DEPENDS ${SOURCE_FILE}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        )
        list(APPEND SPIRV_BINARY_FILES ${SPIRV})
    endforeach()

    add_custom_target(${TARGET}_shaders DEPENDS ${SPIRV_BINARY_FILES})
    add_dependencies(${TARGET} ${TARGET}_shaders)
endfunction()