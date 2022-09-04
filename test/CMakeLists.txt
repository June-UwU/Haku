cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME "Unit-test")

file(GLOB_RECURSE g_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${g_src})

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)


add_executable(${PROJECT_NAME}
	${g_src}
	)



target_link_libraries(${PROJECT_NAME} PUBLIC gtest_main)
target_link_libraries(${PROJECT_NAME} PRIVATE Haku_Engine)
add_test(NAME Delve_test COMMAND ${PROJECT_NAME})


target_include_directories(${PROJECT_NAME}  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/)