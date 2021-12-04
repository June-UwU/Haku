cmake_minimum_required(VERSION 3.20)

message("Building Haku Shared Library")

set(PROJECT_NAME "Haku_Api")

file(GLOB_RECURSE g_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE g_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE c_inc ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.hpp)
file(GLOB_RECURSE c_src ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp)
file(GLOB_RECURSE r_inc ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/*.hpp)
file(GLOB_RECURSE r_src ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/*.cpp)

#needs to be made platform dependant selections

file(GLOB_RECURSE w_inc ${CMAKE_CURRENT_SOURCE_DIR}/platform/Windows/*.hpp)
file(GLOB_RECURSE w_src ${CMAKE_CURRENT_SOURCE_DIR}/platform/Windows/*.cpp)
file(GLOB_RECURSE a_inc ${CMAKE_CURRENT_SOURCE_DIR}/platform/DirectX12/*.hpp)
file(GLOB_RECURSE a_src ${CMAKE_CURRENT_SOURCE_DIR}/platform/DirectX12/*.cpp)
set(DEPEND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../vendor)

set(im_gui 
	${DEPEND_DIR}/imgui/imgui.h
	${DEPEND_DIR}/imgui/imgui.cpp
	${DEPEND_DIR}/imgui/imconfig.h
	${DEPEND_DIR}/imgui/imgui_demo.cpp
	${DEPEND_DIR}/imgui/imgui_draw.cpp
	${DEPEND_DIR}/imgui/imgui_internal.h
	${DEPEND_DIR}/imgui/imgui_tables.cpp
	${DEPEND_DIR}/imgui/imgui_widgets.cpp
	${DEPEND_DIR}/imgui/imstb_rectpack.h
	${DEPEND_DIR}/imgui/imstb_textedit.h
	${DEPEND_DIR}/imgui/imstb_truetype.h
	${DEPEND_DIR}/imgui/backends/imgui_impl_win32.h
	${DEPEND_DIR}/imgui/backends/imgui_impl_win32.cpp
	${DEPEND_DIR}/imgui/backends/imgui_impl_dx12.h
	${DEPEND_DIR}/imgui/backends/imgui_impl_dx12.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${g_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${g_src})


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${c_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${c_src})


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${r_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${r_src})


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${w_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${w_src})


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${a_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${a_src})


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
	${p_src}
	${a_inc}
	${a_src}
	${im_gui} )
message("Dependencies : will : ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/wil/include/")
message("Dependencies : will : ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/spdlog/include/")

target_precompile_headers(${PROJECT_NAME} PRIVATE hakupch.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/wil/include/)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/spdlog/include/)
target_link_libraries(${PROJECT_NAME} DirectXMath)
target_link_libraries(${PROJECT_NAME} DirectX-Headers)

