cmake_minimum_required(VERSION 3.20)

##set(CMAKE_TOOLCHAIN_FILE @VCPKG_ROOT@/scripts/buildsystems/vcpkg.cmake)
set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "VCPKG Target Triplet to use")

project(Odyssean-Engine)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(soil2 CONFIG REQUIRED)
find_package(flecs CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_path(BSHOSHANY_THREAD_POOL_INCLUDE_DIRS "BS_thread_pool.hpp")
find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)

set(_Boost_STACKTRACE_BASIC_HEADERS     "boost/stacktrace.hpp")
set(_Boost_STACKTRACE_BACKTRACE_HEADERS "boost/stacktrace.hpp")
set(_Boost_STACKTRACE_ADDR2LINE_HEADERS "boost/stacktrace.hpp")
set(_Boost_STACKTRACE_NOOP_HEADERS      "boost/stacktrace.hpp")

find_package(Boost COMPONENTS stacktrace_basic)

## Include the source files
file(GLOB_RECURSE SOURCES 
    "Src/*.cpp"
    "Src/Engine/*.cpp"
    "Src/Nodes/*.cpp"
    "Src/Scenes/*.cpp"
    "Src/Engine/OpenGLAPI/*.cpp"
    "Src/Engine/VulkanAPI/*.cpp"
    "Src/Engine/VulkanAPI/Types/*.cpp"
    "Src/Engine/VulkanAPI/Helpers/*.cpp"
)

## Include the header files
file(GLOB_RECURSE HEADERS 
    "Src/*.h"
    "Src/Engine/*.h"
    "Src/Nodes/*.h"
    "Src/Scenes/*.h"
    "Src/Engine/OpenGLAPI/*.h"
    "Src/Engine/VulkanAPI/*.h"
    "Src/Engine/VulkanAPI/Types/*.h"
    "Src/Engine/VulkanAPI/Helpers/*.h"
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Src
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Engine
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Nodes
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Scenes
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Engine/OpenGLAPI
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Engine/VulkanAPI
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Engine/VulkanAPI/Types
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Engine/VulkanAPI/Helpers
    ${BSHOSHANY_THREAD_POOL_INCLUDE_DIRS}
    ${Stb_INCLUDE_DIR}
)

## Preprocessor definitions
add_definitions(-DGLFW_INCLUDE_NONE)
add_definitions(-DGLM_FORCE_RADIANS)
add_definitions(-DGLM_FORCE_DEPTH_ZERO_TO_ONE)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
add_definitions(-DGLM_FORCE_CTOR_INIT)
add_definitions(-DGLM_FORCE_LEFT_HANDED)

##STB_IMAGE_STATIC
add_definitions(-DSTB_IMAGE_STATIC)

add_executable(Odyssean-Engine ${SOURCES} ${HEADERS})

target_link_libraries(Odyssean-Engine PRIVATE glfw)
target_link_libraries(Odyssean-Engine PRIVATE GLEW::GLEW)
target_link_libraries(Odyssean-Engine PRIVATE glm::glm)
target_link_libraries(Odyssean-Engine PRIVATE imgui::imgui)
target_link_libraries(Odyssean-Engine PRIVATE soil2)
target_link_libraries(Odyssean-Engine PRIVATE flecs::flecs)
target_link_libraries(Odyssean-Engine PRIVATE Freetype::Freetype)
target_link_libraries(Odyssean-Engine PRIVATE assimp::assimp)
##target_link_libraries(Odyssean-Engine PRIVATE Boost::boost Boost::stacktrace Boost::filesystem)
target_link_libraries(Odyssean-Engine PRIVATE Vulkan::Vulkan)
target_link_libraries(Odyssean-Engine PRIVATE OpenGL::GL)

target_include_directories(Odyssean-Engine PRIVATE ${HEADERS})
target_include_directories(Odyssean-Engine PRIVATE ${BSHOSHANY_THREAD_POOL_INCLUDE_DIRS})
target_include_directories(Odyssean-Engine PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(Odyssean-Engine PRIVATE ${Vulkan_INCLUDE_DIRS})
target_include_directories(Odyssean-Engine PRIVATE ${OPENGL_INCLUDE_DIRS})
target_include_directories(Odyssean-Engine PRIVATE ${GLM_INCLUDE_DIRS})
target_include_directories(Odyssean-Engine PRIVATE ${SOIL2_INCLUDE_DIRS})
target_include_directories(Odyssean-Engine PRIVATE ${FREETYPE_INCLUDE_DIRS})
target_include_directories(Odyssean-Engine PRIVATE ${ASSIMP_INCLUDE_DIRS})
target_include_directories(Odyssean-Engine PRIVATE ${GLEW_INCLUDE_DIRS})
target_include_directories(Odyssean-Engine PRIVATE ${FLECS_INCLUDE_DIRS})
target_include_directories(Odyssean-Engine PRIVATE ${GLFW_INCLUDE_DIRS})
target_include_directories(Odyssean-Engine PRIVATE ${GLM_INCLUDE_DIRS})