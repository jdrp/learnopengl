cmake_minimum_required(VERSION 3.28)

# Project name
set(PROJECT_NAME ImGui)
project(${PROJECT_NAME})

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# Source files
set(SOURCES
        main.cpp
        glad.c
        glutils/glutils.cpp
        glutils/glshader.cpp
        stb_image.cpp
        glutils/glcamera.cpp
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_impl_glfw.cpp
        imgui/imgui_impl_opengl3.cpp
        glutils/gllight.cpp
        include/imguiutils/imguilights.h
        imguiutils/imguilights.cpp
        glutils/glmesh.cpp
        include/glutils/glmesh.h
        glutils/glmodel.cpp
        include/glutils/glmodel.h
        include/glutils/glkernel.h
        glutils/gltexture.cpp
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${OpenGL_LIBRARIES} glfw)


# Assimp
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${PROJECT_SOURCE_DIR}/libs CACHE PATH "Missing description." FORCE)
FetchContent_Declare(assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG master)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(assimp)
target_include_directories(${PROJECT_NAME} PRIVATE libs/assimp-src/include)
target_link_libraries(${PROJECT_NAME} assimp)

# Set include directories
include_directories(include)
include_directories(imgui)

