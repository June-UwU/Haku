cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME "editor")

file(GLOB_RECURSE g_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE g_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${g_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${g_src})


add_executable(${PROJECT_NAME}
	${g_inc}
	${g_src}
	)

target_include_directories(${PROJECT_NAME} 	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../engine/include)
target_link_libraries(${PROJECT_NAME} PRIVATE haku_engine)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
