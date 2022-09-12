cmake_minimum_required(VERSION 3.20)

message("${API}")
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

file(GLOB_RECURSE renderer_inc	${CMAKE_CURRENT_SOURCE_DIR}/renderer/*.hpp)
file(GLOB_RECURSE renderer_src	${CMAKE_CURRENT_SOURCE_DIR}/renderer/*.cpp)

file(GLOB_RECURSE renderer_backends_inc	${CMAKE_CURRENT_SOURCE_DIR}/renderer/backends/*.hpp)
file(GLOB_RECURSE renderer_backends_src	${CMAKE_CURRENT_SOURCE_DIR}/renderer/backends/*.cpp)

file(GLOB_RECURSE renderer_backends_inc	${CMAKE_CURRENT_SOURCE_DIR}/memory/*.hpp)
file(GLOB_RECURSE renderer_backends_src	${CMAKE_CURRENT_SOURCE_DIR}/memory/*.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${memory_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${memory_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${renderer_backends_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${renderer_backends_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${renderer_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${renderer_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${generic_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${generic_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${core_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${core_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${platform_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${platform_src})

set(DEPEND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../vendor)

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
	${renderer_inc}
	${renderer_src}
	${renderer_backends_inc}
	${renderer_backends_src}
	${memory_inc}
	${memory_src}
	)


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${DEPEND_DIR}/Haku_math/include)
add_dependencies(${PROJECT_NAME} shaders)