cmake_minimum_required(VERSION 3.0)

project(perfume)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)
add_executable(perfume
  ./src/perfume.cpp
  ./src/bvh.cpp
  ./src/wav.cpp
)

if (WIN32)
set(OPENGL_LIBRARY_DIR "${PROJECT_SOURCE_DIR}/freeglut/lib/x64")
set(GLUT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}freeglut/include)
endif()

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif(MSVC)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenAL REQUIRED)

if (WIN32)
include_directories( ${PROJECT_SOURCE_DIR}/freeglut/include )
else()
include_directories( ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} )
endif()

include_directories( ${OPENAL_INCLUDE_DIR} )

target_link_libraries(perfume
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARY}
  ${OPENAL_LIBRARY}
)
