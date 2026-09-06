cmake_minimum_required(VERSION 4.4)

project(VoxelGame)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.5.1)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.3)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(spirv_headers GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers.git GIT_TAG vulkan-sdk-1.4.357.0)
FetchContent_MakeAvailable(spirv_headers)

FetchContent_Declare(spirv_tools GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools.git GIT_TAG vulkan-sdk-1.4.357.0)
FetchContent_MakeAvailable(spirv_tools)

set(ENABLE_HLSL OFF)
FetchContent_Declare(glslang GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git GIT_TAG vulkan-sdk-1.4.357.0)
FetchContent_MakeAvailable(glslang)
target_compile_options(glslang PRIVATE "-Wno-array-bounds")

set(SPIRV_CROSS_CLI OFF)
set(SPIRV_CROSS_ENABLE_CPP OFF)
set(SPIRV_CROSS_ENABLE_C_API OFF)
set(SPIRV_CROSS_ENABLE_HLSL OFF)
set(SPIRV_CROSS_ENABLE_MSL OFF)
set(SPIRV_CROSS_ENABLE_TESTS OFF)
set(SPIRV_CROSS_ENABLE_UTIL OFF)
set(SPIRV_CROSS_SKIP_INSTALL ON)
FetchContent_Declare(spirv_cross GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git GIT_TAG vulkan-sdk-1.4.357.0)
FetchContent_MakeAvailable(spirv_cross)

set(SHADERC_ENABLE_HLSL OFF)
set(SHADERC_SKIP_COPYRIGHT_CHECK ON)
set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_SKIP_EXECUTABLES ON)
set(SHADERC_SKIP_INSTALL ON)
set(SHADERC_SKIP_TESTS ON)
FetchContent_Declare(shaderc GIT_REPOSITORY https://github.com/google/shaderc.git GIT_TAG v2026.3)
FetchContent_MakeAvailable(shaderc)

set(TRACY_ENABLE ON)
FetchContent_Declare(tracy GIT_REPOSITORY https://github.com/wolfpld/tracy.git GIT_TAG v0.14.1)
FetchContent_MakeAvailable(tracy)

FetchContent_Declare(vma GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git GIT_TAG v3.4.0)
FetchContent_MakeAvailable(vma)

FetchContent_Declare(volk GIT_REPOSITORY https://github.com/zeux/volk.git GIT_TAG vulkan-sdk-1.4.357.0)
FetchContent_MakeAvailable(volk)

set(ALWAYS_RELEASE_MODE glfw glm glslang shaderc spirv-cross-core spirv-cross-glsl spirv-cross-reflect SPIRV-Tools-static SPIRV-Tools-opt TracyClient volk)
foreach(target ${ALWAYS_RELEASE_MODE})
    target_compile_options(${target} PRIVATE "-O3" "-g0")
endforeach()

file(GLOB_RECURSE COMMON_SRC CONFIGURE_DEPENDS src/common/*.cpp lib/*.c lib/*.cpp)

file(GLOB_RECURSE CLIENT_SRC CONFIGURE_DEPENDS src/client/*.cpp)
add_executable(VoxelGameClient ${COMMON_SRC} ${CLIENT_SRC})

target_include_directories(VoxelGameClient PUBLIC lib)
target_include_directories(VoxelGameClient PUBLIC src)

target_compile_definitions(VoxelGameClient PUBLIC $<$<CONFIG:Debug>:VG_DEBUG>)
target_compile_definitions(VoxelGameClient PUBLIC LOG_PROJECT_NAME=VoxelGame)
target_compile_definitions(VoxelGameClient PUBLIC VG_SIDE=CLIENT)

target_compile_definitions(VoxelGameClient PUBLIC GLM_ENABLE_EXPERIMENTAL)
target_compile_definitions(VoxelGameClient PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_compile_definitions(VoxelGameClient PUBLIC VK_NO_PROTOTYPES)
target_compile_definitions(VoxelGameClient PUBLIC VMA_VULKAN_HEADERS_ALREADY_INCLUDED)

target_link_libraries(VoxelGameClient PUBLIC glfw)
target_link_libraries(VoxelGameClient PUBLIC glm)
target_link_libraries(VoxelGameClient PUBLIC shaderc)
target_link_libraries(VoxelGameClient PUBLIC spirv-cross-core)
target_link_libraries(VoxelGameClient PUBLIC spirv-cross-glsl)
target_link_libraries(VoxelGameClient PUBLIC TracyClient)
target_link_libraries(VoxelGameClient PUBLIC volk)
target_link_libraries(VoxelGameClient PUBLIC VulkanMemoryAllocator)