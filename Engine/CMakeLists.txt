cmake_minimum_required(VERSION 3.20)


message("${API}")
message("Building Haku Shared Library")

set(PROJECT_NAME "Haku_Engine")

include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/0.9.9.8
)

FetchContent_MakeAvailable(glm)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

file(GLOB_RECURSE global_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE global_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

file(GLOB_RECURSE core_inc ${CMAKE_CURRENT_SOURCE_DIR}/core/*.hpp)
file(GLOB_RECURSE core_src ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp)

file(GLOB_RECURSE platform_inc ${CMAKE_CURRENT_SOURCE_DIR}/platform/*.hpp)
file(GLOB_RECURSE platform_src ${CMAKE_CURRENT_SOURCE_DIR}/platform/*.cpp)

file(GLOB_RECURSE renderer_inc ${CMAKE_CURRENT_SOURCE_DIR}/renderer/*.hpp)
file(GLOB_RECURSE renderer_src ${CMAKE_CURRENT_SOURCE_DIR}/renderer/*.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${platform_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${platform_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${core_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${core_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${renderer_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${renderer_src})

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
	${renderer_inc}
	${renderer_src}
	)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/core)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/platform)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Libs/GLFW/include)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:ENGINE_DEBUG>)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} pthread glm::glm)

if (UNIX)
	add_compile_definitions(HAKU_LINUX)	
	target_link_libraries(${PROJECT_NAME} PRIVATE X11 dl Xrandr Xi Xxf86vm glfw)
endif (UNIX)

