cmake_minimum_required(VERSION 3.7)

# set the project name
project(QuadriMan)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# add the executables
add_executable(QuadriMan WIN32 quadriman.cpp)
target_link_libraries(QuadriMan ${OPENGL_LIBRARY} ${SDL2_LIBRARIES})
add_executable(TrioMan WIN32 trioman.cpp)
target_link_libraries(TrioMan ${OPENGL_LIBRARY} ${SDL2_LIBRARIES})

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/img
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/img)