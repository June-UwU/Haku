cmake_minimum_required(VERSION 3.20)

project("hengine" LANGUAGES CXX)
find_package(Vulkan REQUIRED)
find_program(GLSLC_EXECUTABLE NAMES glslc)

file(GLOB_RECURSE global_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE global_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_src})

add_compile_options(-Wall)
add_library(${PROJECT_NAME} 
	STATIC
	${global_inc}
	${global_src}
	)


set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
# target_link_libraries(${PROJECT_NAME} PRIVATE )
target_include_directories(${PROJECT_NAME} PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}
	)


