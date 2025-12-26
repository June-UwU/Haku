cmake_minimum_required(VERSION 3.20)

project("hengine" LANGUAGES CXX)
find_program(GLSLC_EXECUTABLE NAMES glslc)
find_package(Slang CONFIG HINTS "$ENV{VULKAN_SDK}/lib/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE global_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE global_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx)
file(GLOB_RECURSE window_inc ${CMAKE_CURRENT_SOURCE_DIR}/window/*.hpp)
file(GLOB_RECURSE window_src ${CMAKE_CURRENT_SOURCE_DIR}/window/*.cxx)
file(GLOB_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/renderer/ FILES ${render_inc})
file(GLOB_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/renderer/ FILES ${render_src})
file(GLOB_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/renderer/vulkan/ FILES ${vulkan_inc})
file(GLOB_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/renderer/vulkan/ FILES ${vulkan_src})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_src})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/window/ FILES ${window_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/window/ FILES ${window_src})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/renderer/ FILES ${render_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/renderer/ FILES ${render_src})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/renderer/vulkan/ FILES ${vulkan_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/renderer/vulkan/ FILES ${vulkan_src})

add_compile_options(-Wall)
add_library(${PROJECT_NAME} 
	STATIC
	${global_inc}
	${global_src}
	${window_inc}
	${window_src}
	${render_inc}
	${render_src}
	${vulkan_inc}
	${vulkan_src}
	)


set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw Vulkan::Vulkan)
target_include_directories(${PROJECT_NAME} PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/GLFW/include
)

