cmake_minimum_required(VERSION 3.19)
project(project)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_CXX_FLAGS "-Wall -Wno-deprecated-declarations -Wno-reorder-ctor -Wno-unused-variable -g -stdlib=libc++ -std=c++17")
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
find_library(GL_LIB OpenGL)
find_library(GLFW glfw)
add_definitions(-DAPPLE)
set(INCLUDE_DIRS
        /System/Library/Frameworks
        external/include
        /usr/local/include)

set(CORE ${GLFW} ${GL_LIB})

include_directories(${INCLUDE_DIRS})
link_directories(${LIBRARY_DIRS})

set(RENDER_SOURCES
        src/util/AGL.h
        src/util/AGLM.h
        src/util/AGLM.cpp
        src/util/image.h
        src/util/image.cpp
        src/renderer.h
        src/renderer.cpp
        )

set(PSYS_SOURCES
        src/marioparticlesystem.h
        src/marioparticlesystem.cpp
        src/particlesystem.h
        src/particlesystem.cpp
        )

set(SHADERS
        shaders/billboard.fs
        shaders/billboard.vs)

add_executable(demo src/main.cpp ${PSYS_SOURCES} ${RENDER_SOURCES} ${SHADERS})
target_link_libraries(demo ${CORE})
