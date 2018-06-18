cmake_minimum_required(VERSION 3.7)

project(vulkanEngine)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} ${SOURCES} "main.cpp")
target_link_libraries(${PROJECT_NAME} glfw ${Vulkan_LIBRARY})