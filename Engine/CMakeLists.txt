cmake_minimum_required(VERSION 3.20)

message("Building Haku Shared Library")

set(PROJECT_NAME "Haku_Engine")

file(GLOB_RECURSE global_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE global_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

file(GLOB_RECURSE core_inc ${CMAKE_CURRENT_SOURCE_DIR}/core/*.hpp)
file(GLOB_RECURSE core_src ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp)

file(GLOB_RECURSE platform_inc ${CMAKE_CURRENT_SOURCE_DIR}/platform/*.hpp)
file(GLOB_RECURSE platform_src ${CMAKE_CURRENT_SOURCE_DIR}/platform/*.cpp)

file(GLOB_RECURSE generic_inc ${CMAKE_CURRENT_SOURCE_DIR}/generics/*.hpp)
file(GLOB_RECURSE generic_src ${CMAKE_CURRENT_SOURCE_DIR}/generics/*.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${generic_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${generic_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${core_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${core_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${platform_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${platform_src})

add_compile_definitions(HAKU_API_BUILD)
add_library(${PROJECT_NAME} 
	SHARED 
	${generic_inc}
	${generic_src}
	${global_inc}
	${global_src}
	${core_inc}
	${core_src}
	${platform_inc}
	${platform_src}
	)


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})