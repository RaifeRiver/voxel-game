cmake_minimum_required(VERSION 3.31)

project(voxel_game)

set(GLSL_VALIDATOR glslangValidator)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

include(FetchContent)

FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
FetchContent_MakeAvailable(glfw)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.3)
FetchContent_MakeAvailable(glm)
FetchContent_Declare(tracy GIT_REPOSITORY https://github.com/wolfpld/tracy.git GIT_TAG v0.13.1)
FetchContent_MakeAvailable(tracy)
FetchContent_Declare(vkb GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git GIT_TAG v1.4.344)
FetchContent_MakeAvailable(vkb)
FetchContent_Declare(vma GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git GIT_TAG v3.3.0)
FetchContent_MakeAvailable(vma)
FetchContent_Declare(xxHash GIT_REPOSITORY https://github.com/Cyan4973/xxHash.git GIT_TAG v0.8.3 SOURCE_SUBDIR cmake_unofficial)
FetchContent_MakeAvailable(xxHash)

file(GLOB_RECURSE GLSL_SOURCE_FILES CONFIGURE_DEPENDS "resources/shaders/*.frag" "resources/shaders/*.vert" "resources/shaders/*.comp")
foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/${FILE_NAME}.spv")
    add_custom_command(OUTPUT ${SPIRV} COMMAND ${GLSL_VALIDATOR} --target-env vulkan1.3 -V ${GLSL} -o ${SPIRV} DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)
add_custom_target(voxel_game_shaders DEPENDS ${SPIRV_BINARY_FILES})

file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS src/common/*.cpp src/client/*.cpp)
add_executable(voxel_game_client ${CLIENT_SOURCES})

add_dependencies(voxel_game_client voxel_game_shaders)

target_include_directories(voxel_game_client PUBLIC src)

target_compile_definitions(voxel_game_client PUBLIC VG_CLIENT_SIDE)
target_compile_definitions(voxel_game_client PUBLIC $<$<CONFIG:Debug>:VG_DEBUG>)

target_link_libraries(voxel_game_client PUBLIC glfw)
target_link_libraries(voxel_game_client PUBLIC glm::glm)
target_link_libraries(voxel_game_client PUBLIC GPUOpen::VulkanMemoryAllocator)
target_link_libraries(voxel_game_client PUBLIC TracyClient)
target_link_libraries(voxel_game_client PUBLIC vk-bootstrap::vk-bootstrap)
target_link_libraries(voxel_game_client PUBLIC Vulkan::Vulkan)
target_link_libraries(voxel_game_client PUBLIC xxHash::xxhash)