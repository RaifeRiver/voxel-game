cmake_minimum_required(VERSION 4.4)

project(VoxelGame)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

include(FetchContent)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.3)
FetchContent_MakeAvailable(glm)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.5.1)
FetchContent_MakeAvailable(glfw)

file(GLOB_RECURSE COMMON_SRC CONFIGURE_DEPENDS src/common/*.cpp lib/*.cpp)

file(GLOB_RECURSE CLIENT_SRC CONFIGURE_DEPENDS src/client/*.cpp)
add_executable(VoxelGameClient ${COMMON_SRC} ${CLIENT_SRC})

target_include_directories(VoxelGameClient PUBLIC lib)
target_include_directories(VoxelGameClient PUBLIC src)

target_link_libraries(VoxelGameClient PUBLIC glfw)
target_link_libraries(VoxelGameClient PUBLIC glm::glm)
target_link_libraries(VoxelGameClient PUBLIC Vulkan::Vulkan)