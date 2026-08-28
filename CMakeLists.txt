cmake_minimum_required(VERSION 4.4)

project(VoxelGame)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.3)
FetchContent_MakeAvailable(glm)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.5.1)
FetchContent_MakeAvailable(glfw)
FetchContent_Declare(spirv_cross GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git GIT_TAG vulkan-sdk-1.4.357.0)
FetchContent_MakeAvailable(spirv_cross)
FetchContent_Declare(vma GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git GIT_TAG v3.4.0)
FetchContent_MakeAvailable(vma)
FetchContent_Declare(volk GIT_REPOSITORY https://github.com/zeux/volk.git GIT_TAG vulkan-sdk-1.4.357.0)
FetchContent_MakeAvailable(volk)

file(GLOB_RECURSE COMMON_SRC CONFIGURE_DEPENDS src/common/*.cpp lib/*.c lib/*.cpp)

file(GLOB_RECURSE CLIENT_SRC CONFIGURE_DEPENDS src/client/*.cpp)
add_executable(VoxelGameClient ${COMMON_SRC} ${CLIENT_SRC})

target_include_directories(VoxelGameClient PUBLIC lib)
target_include_directories(VoxelGameClient PUBLIC src)

target_compile_definitions(VoxelGameClient PUBLIC $<$<CONFIG:Debug>:VG_DEBUG>)
target_compile_definitions(VoxelGameClient PUBLIC LOG_PROJECT_NAME=VoxelGame)
target_compile_definitions(VoxelGameClient PUBLIC VG_SIDE=CLIENT)

target_compile_definitions(VoxelGameClient PUBLIC GLM_ENABLE_EXPERIMENTAL)
target_compile_definitions(VoxelGameClient PUBLIC VK_NO_PROTOTYPES)
target_compile_definitions(VoxelGameClient PUBLIC VMA_VULKAN_HEADERS_ALREADY_INCLUDED)

target_link_libraries(VoxelGameClient PUBLIC glfw)
target_link_libraries(VoxelGameClient PUBLIC glm::glm)
target_link_libraries(VoxelGameClient PUBLIC GPUOpen::VulkanMemoryAllocator)
target_link_libraries(VoxelGameClient PUBLIC spirv-cross-core)
target_link_libraries(VoxelGameClient PUBLIC spirv-cross-glsl)
target_link_libraries(VoxelGameClient PUBLIC volk)

find_program(GLSL_VALIDATOR glslangValidator)

file(GLOB_RECURSE SHADER_SRC CONFIGURE_DEPENDS res/assets/voxel_game/shaders/*.frag res/assets/voxel_game/shaders/*.vert res/assets/voxel_game/shaders/*.comp)
file(GLOB_RECURSE SHADER_DEPENDENCIES CONFIGURE_DEPENDS res/assets/voxel_game/shaders/*.glsl)

foreach(SHADER ${SHADER_SRC})
    set(SPIRV "${SHADER}.spv")
    add_custom_command(OUTPUT ${SPIRV} COMMAND ${GLSL_VALIDATOR} --target-env vulkan1.3 -V ${SHADER} -o ${SPIRV} DEPENDS ${SHADER} ${SHADER_DEPENDENCIES})
    list(APPEND SPIRV_FILES ${SPIRV})
endforeach(SHADER)
add_custom_target(Shaders DEPENDS ${SPIRV_FILES})

add_dependencies(VoxelGameClient Shaders)