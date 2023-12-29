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

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${platform_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${platform_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${core_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${core_src})

set(DEPEND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../vendor)

add_compile_definitions(HAKU_API_BUILD)
add_compile_definitions(GLM_FORCE_RADIANS)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
add_compile_options(-Wall -Wextra -Wpedantic)
add_library(${PROJECT_NAME} 
	STATIC
	${global_inc}
	${global_src}
	${core_inc}
	${core_src}
	${platform_inc}
	${platform_src}
	)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/core)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/platform)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Libs/GLFW/include)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:ENGINE_DEBUG>)

if (UNIX)
	add_compile_definitions(HAKU_LINUX)
	target_link_libraries(${PROJECT_NAME} X11 pthread dl vulkan Xrandr Xi Xxf86vm glfw)
endif (UNIX)

