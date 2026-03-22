cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME simple-clock-sdl)
project(${PROJECT_NAME} CXX)

option(SANITIZE_ADDRESS "Add -fsanitize=address" OFF)
if (SANITIZE_ADDRESS)
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)
endif(SANITIZE_ADDRESS)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB hdrs "${CMAKE_CURRENT_SOURCE_DIR}/hdr/*.h*")
file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/hdr)

add_executable(${PROJECT_NAME} ${srcs} ${hdrs})

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
