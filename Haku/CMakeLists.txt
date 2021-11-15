cmake_minimum_required(VERSION 3.20)

message("Building Haku Shared Library")

set(PROJECT_NAME "Haku_Api")

file(GLOB_RECURSE g_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE g_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE c_inc ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.hpp)
file(GLOB_RECURSE c_src ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp)
file(GLOB_RECURSE r_inc ${CMAKE_CURRENT_SOURCE_DIR}/Renderer*.hpp)
file(GLOB_RECURSE r_src ${CMAKE_CURRENT_SOURCE_DIR}/Renderer*.cpp)

#needs to be made platform dependant selections

file(GLOB_RECURSE p_inc ${CMAKE_CURRENT_SOURCE_DIR}/platform/Windows/*.hpp)
file(GLOB_RECURSE p_src ${CMAKE_CURRENT_SOURCE_DIR}/platform/Windows/*.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${g_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${g_src})


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${c_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${c_src})


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${r_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${r_src})

add_compile_definitions(HAKU_API_BUILD)
add_library(${PROJECT_NAME} 
	SHARED 
	${g_inc}
	${g_src}
	${c_inc}
	${c_src}
	${r_inc}
	${r_src}
	${p_inc}
	${p_src} )
message("Dependencies : will : ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/wil/include/")
message("Dependencies : will : ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/spdlog/include/")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/wil/include/)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/spdlog/include/)
