cmake_minimum_required(VERSION 3.15)

project(sim)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

set(CMAKE_CXX_FLAGS "-DGL_GLEXT_PROTOTYPES -lXt -lX11 -lGL -lm -lstdc++ -Wall")

set(SRC_COMMON common/Linux/MicroGlut.c common/GL_utilities.c common/LoadTGA.c)

set(SRC_GAME src/main.cpp src/game.cpp src/object.cpp src/renderer.cpp src/player.cpp src/bus.cpp src/road.cpp src/terrain.cpp)

include_directories(include)
include_directories(common)
add_executable(sim ${SRC_COMMON} ${SRC_GAME})
target_link_libraries(sim ${OPENGL_LIBRARIES})
target_link_libraries(sim ${X11_LIBRARIES})