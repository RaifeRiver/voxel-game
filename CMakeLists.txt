cmake_minimum_required(VERSION 4.4)

project(VoxelGame)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE COMMON_SRC CONFIGURE_DEPENDS src/common/*.cpp lib/*.cpp)

file(GLOB_RECURSE CLIENT_SRC CONFIGURE_DEPENDS src/client/*.cpp)
add_executable(VoxelGameClient ${COMMON_SRC} ${CLIENT_SRC})

target_include_directories(VoxelGameClient PUBLIC lib)
target_include_directories(VoxelGameClient PUBLIC src)