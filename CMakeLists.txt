cmake_minimum_required(VERSION 3.7)

project(vulkanEngine)

find_package(Vulkan)

file(GLOB_RECURSE SOURCES "src/*.cpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} ${SOURCES} "main.cpp")