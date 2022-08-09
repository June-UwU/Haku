cmake_minimum_required(VERSION 3.20)

message("Building Testbed")

set(PROJECT_NAME "Test")

file(GLOB_RECURSE g_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE g_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${g_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${g_src})


add_executable(${PROJECT_NAME}
	${g_inc}
	${g_src}
	)

message("Dependencies : ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/")

target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/)

message("Linking Haku api")
target_link_libraries(${PROJECT_NAME} PRIVATE Haku_Engine)