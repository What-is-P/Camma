cmake_minimum_required(VERSION 3.16)
project(OpenGLApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_SHARED_LIBS "Build shared libs" OFF)

# GLFW
include(FetchContent)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# GLAD (OpenGL 3.3 core)
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

add_executable(OpenGLApp
  src/main.cpp
)

add_library(gladlib STATIC
  ${glad_SOURCE_DIR}/src/glad.c
)
target_include_directories(gladlib PUBLIC
  ${glad_SOURCE_DIR}/include
)

target_link_libraries(OpenGLApp
  PRIVATE glfw gladlib
)

if(UNIX AND NOT APPLE)
  find_package(Threads REQUIRED)
  target_link_libraries(OpenGLApp PRIVATE Threads::Threads dl)
endif()

target_include_directories(OpenGLApp PRIVATE
  ${glad_SOURCE_DIR}/include
)

add_custom_command(TARGET OpenGLApp POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:OpenGLApp>/shaders
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/shaders/vertex.glsl
    ${CMAKE_SOURCE_DIR}/shaders/fragment.glsl
    $<TARGET_FILE_DIR:OpenGLApp>/shaders
)
