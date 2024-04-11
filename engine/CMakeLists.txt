cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME "haku_engine")
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(glfw)

file(GLOB_RECURSE global_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE global_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx)
file(GLOB_RECURSE platform_inc ${CMAKE_CURRENT_SOURCE_DIR}/platform/*.hpp)
file(GLOB_RECURSE platform_src ${CMAKE_CURRENT_SOURCE_DIR}/platform/*.cxx)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${platform_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${platform_src})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_src})

add_compile_options(-Wall)
add_library(${PROJECT_NAME} 
	STATIC
	${global_inc}
	${global_src}
	)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

