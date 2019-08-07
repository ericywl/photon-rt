cmake_minimum_required(VERSION 3.0)
project(asg5)
set(SOURCE main.cpp)
set(CMAKE_CXX_STANDARD 11)

cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED) 
find_package(FLTK REQUIRED)
include_directories(library ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${FLTK_INCLUDE_DIR})

include_directories(
    ${PROJECT_SOURCE_DIR}/include/primitives
    ${PROJECT_SOURCE_DIR}/include/maps
    ${PROJECT_SOURCE_DIR}/include/tracer
    ${PROJECT_SOURCE_DIR}/include/utils
    ${PROJECT_SOURCE_DIR}/vecmath/include
)

file(GLOB_RECURSE ALL_SRCS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/vecmath/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${ALL_SRCS} ${SOURCE})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${FLTK_LIBRARIES})