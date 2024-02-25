cmake_minimum_required(VERSION 3.20)


message("${API}")
message("Building Haku Shared Library")

set(PROJECT_NAME "Haku_Engine")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp REQUIRED)

file(GLOB_RECURSE global_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE global_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

file(GLOB_RECURSE core_inc ${CMAKE_CURRENT_SOURCE_DIR}/core/*.hpp)
file(GLOB_RECURSE core_src ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp)

file(GLOB_RECURSE platform_inc ${CMAKE_CURRENT_SOURCE_DIR}/platform/*.hpp)
file(GLOB_RECURSE platform_src ${CMAKE_CURRENT_SOURCE_DIR}/platform/*.cpp)

file(GLOB_RECURSE renderer_inc ${CMAKE_CURRENT_SOURCE_DIR}/renderer/*.hpp)
file(GLOB_RECURSE renderer_src ${CMAKE_CURRENT_SOURCE_DIR}/renderer/*.cpp)

file(GLOB_RECURSE model_inc ${CMAKE_CURRENT_SOURCE_DIR}/renderer/model/*.hpp)
file(GLOB_RECURSE model_src ${CMAKE_CURRENT_SOURCE_DIR}/renderer/model/*.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${platform_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${platform_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${core_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${core_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${renderer_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${renderer_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${model_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${model_src})

add_compile_definitions(HAKU_API_BUILD GLM_FORCE_RADIANS GLM_FORCE_DEPTH_ZERO_TO_ONE)
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
	${model_inc}
	${model_src}
	)

target_include_directories(${PROJECT_NAME} PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/core
	${CMAKE_CURRENT_SOURCE_DIR}/platform
	${CMAKE_CURRENT_SOURCE_DIR}/../Libs/GLFW/include)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:ENGINE_DEBUG>)
include_directories(${ASSIMP_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${ASSIMP_LIBRARIES} pthread glm::glm-header-only gli)

if (UNIX)
	add_compile_definitions(HAKU_LINUX)	
	target_link_libraries(${PROJECT_NAME} PRIVATE X11 dl Xrandr Xi Xxf86vm glfw)
endif (UNIX)

