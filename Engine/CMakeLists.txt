cmake_minimum_required(VERSION 3.20)

message("Building Haku Shared Library")

set(PROJECT_NAME "Haku_Engine")

file(GLOB_RECURSE global_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE global_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)



source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_src})

add_compile_definitions(HAKU_API_BUILD)
add_library(${PROJECT_NAME} 
	SHARED 
	${global_inc}
	${global_src}
	)
