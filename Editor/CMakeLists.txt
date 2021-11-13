cmake_minimum_required(VERSION 3.20)

message("Building Generic Haku Engine Editor")

set(PROJECT_NAME "Haku_Editor")

file(GLOB_RECURSE g_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE g_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${g_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${g_src})

add_executable(${PROJECT_NAME}
	${g_inc}
	${g_src})

target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Haku/)
target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/wil/include/)

message("Linking Haku api")
target_link_libraries(${PROJECT_NAME} Haku_Api)