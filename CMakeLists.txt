cmake_minimum_required(VERSION 3.20)


if (WIN32)
	add_compile_definitions(HWIN32)
endif (WIN32)

if (UNIX)
	add_compile_definitions(HUNIX)
endif (UNIX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PROJECT_NAME "Haku")

project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
message("Cmake output dir : ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
message("Cmake library output dir : ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
message("Cmake runtime dir : ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message("Cmake C++ standard : ${CMAKE_CXX_STANDARD}")

add_subdirectory("Engine")
add_subdirectory("Testbed")
add_subdirectory("Libs/GLFW")