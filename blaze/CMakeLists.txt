cmake_minimum_required(VERSION 3.18)
project(blaze)

set(CMAKE_CXX_STANDARD 20)
set(PHYSFS_TARGETNAME_UNINSTALL physfs_uninstall)

option(PHYSFS_BUILD_SHARED FALSE)
option(VMA_STATIC_VULKAN_FUNCTIONS OFF)
option(VMA_DYNAMIC_VULKAN_FUNCTIONS OFF)

find_package(Vulkan REQUIRED)

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
    src/util/match.hpp
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
)

target_include_directories(blaze
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/physfs/src"
#    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/internal"
)

target_compile_definitions(blaze PUBLIC
    -DVULKAN_HPP_NO_STRUCT_CONSTRUCTORS
    -DVULKAN_HPP_NO_UNION_CONSTRUCTORS
    -DVK_NO_PROTOTYPES
)

add_library(imgui STATIC
    third_party/imgui/imgui.h
    third_party/imgui/imconfig.h
    third_party/imgui/imgui_internal.h
    third_party/imgui/misc/cpp/imgui_stdlib.h
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/misc/cpp/imgui_stdlib.cpp
)
target_include_directories(imgui PUBLIC third_party/imgui)
target_compile_definitions(imgui PUBLIC -DIMGUI_DEFINE_MATH_OPERATORS)

add_subdirectory(third_party/glm)
add_subdirectory(third_party/spdlog)
add_subdirectory(third_party/physfs)
add_subdirectory(third_party/optional)
add_subdirectory(third_party/range-v3)
add_subdirectory(third_party/SPIRV-Cross)
add_subdirectory(third_party/VulkanMemoryAllocator)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_subdirectory(third_party/glfw)
    target_link_libraries(blaze PRIVATE glfw)

    target_compile_definitions(blaze PUBLIC
        -DGLFW_INCLUDE_NONE
        -DGLFW_INCLUDE_VULKAN
    )
    target_compile_definitions(blaze PUBLIC
        -DVK_USE_PLATFORM_WIN32_KHR
    )
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_subdirectory(third_party/glfw)
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

    add_subdirectory(third_party/glfw)
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
        VulkanMemoryAllocator
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