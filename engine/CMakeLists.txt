cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME "haku_engine")

file(GLOB_RECURSE global_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE global_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_src})

add_compile_options(-Wall -Wextra -Wpedantic)
add_library(${PROJECT_NAME} 
	STATIC
	${global_inc}
	${global_src}
	)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
