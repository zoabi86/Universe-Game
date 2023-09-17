cmake_minimum_required(VERSION 3.10)

project(OpenGLProject)

include_directories(${GLM_INCLUDE_DIRS} "/usr/include/glm/")

set(CMAKE_CXX_STANDARD 20)
set(OPENGL_LIBRARIES GL GL X11)

find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_executable(universe_game main.cpp glad/glad.c OpenGLRenderer.cpp Planet.cpp)

target_link_libraries(universe_game GLEW::GLEW glfw ${OPENGL_LIBRARIES} ${GLM_LIBRARIES})
