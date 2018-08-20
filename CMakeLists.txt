cmake_minimum_required(VERSION 3.7)

project(vulkanEngine)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

file(GLOB_RECURSE SOURCES_ENGINE "src/Engine/*.cpp")
file(GLOB_RECURSE SOURCES_APP "src/App/*.cpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/Engine)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/App)

add_executable(${PROJECT_NAME} ${SOURCES_ENGINE} ${SOURCES_APP} "main.cpp")
target_link_libraries(${PROJECT_NAME} glm glfw ${Vulkan_LIBRARY})