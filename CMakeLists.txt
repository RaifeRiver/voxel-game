cmake_minimum_required(VERSION 3.31)

project(voxel_game)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

include(FetchContent)

FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
FetchContent_MakeAvailable(glfw)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.3)
FetchContent_MakeAvailable(glm)
FetchContent_Declare(vkb GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git GIT_TAG v1.4.344)
FetchContent_MakeAvailable(vkb)
FetchContent_Declare(vma GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git v3.3.0)
FetchContent_MakeAvailable(vma)

file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS src/common/*.cpp src/client/*.cpp)
add_executable(voxel_game_client ${CLIENT_SOURCES})

target_include_directories(voxel_game_client PUBLIC src)

target_compile_definitions(voxel_game_client PUBLIC VG_CLIENT_SIDE)
target_compile_definitions(voxel_game_client PUBLIC $<$<CONFIG:Debug>:VG_DEBUG>)

target_link_libraries(voxel_game_client PUBLIC glfw)
target_link_libraries(voxel_game_client PUBLIC glm::glm)
target_link_libraries(voxel_game_client PUBLIC GPUOpen::VulkanMemoryAllocator)
target_link_libraries(voxel_game_client PUBLIC vk-bootstrap::vk-bootstrap)
target_link_libraries(voxel_game_client PUBLIC Vulkan::Vulkan)