cmake_minimum_required(VERSION 3.31)

project(voxel_game)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.3)
FetchContent_MakeAvailable(glm)

file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS src/common/*.cpp src/client/*.cpp)
add_executable(voxel_game_client ${CLIENT_SOURCES})

target_include_directories(voxel_game_client PUBLIC src)

target_compile_definitions(voxel_game_client PUBLIC VG_CLIENT_SIDE)

target_link_libraries(voxel_game_client PUBLIC glm::glm)