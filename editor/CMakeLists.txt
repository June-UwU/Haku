cmake_minimum_required(VERSION 3.20)

project("editor" LANGUAGES CXX)

file(GLOB_RECURSE g_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE g_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${g_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${g_src})


add_executable(${PROJECT_NAME}
	${g_inc}
	${g_src}
	)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../engine/include)
target_link_libraries(${PROJECT_NAME} PRIVATE "hengine")
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
