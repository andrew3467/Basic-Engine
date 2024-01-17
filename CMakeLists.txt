cmake_minimum_required(VERSION 3.26)
project(Basic_Engine)

set(CMAKE_CXX_STANDARD 23)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_package(GLFW3 REQUIRED)
find_package(GLM REQUIRED)



include_directories(External)
include_directories(src/Engine)
include_directories(src/Systems)


file(GLOB_RECURSE EXTERNAL_FILES
        External/*/*.h
        External/*/*.cpp
)

file(GLOB_RECURSE PRJ_SOURCE
        src/*.cpp
        src/*.h
)


add_executable(Basic_Engine
        main.cpp
        ${PRJ_SOURCE}
        ${EXTERNAL_FILES}
        ${SHADERS}
)


target_link_libraries(Basic_Engine PRIVATE
        Vulkan::Vulkan
        glfw
        glm::glm
)
