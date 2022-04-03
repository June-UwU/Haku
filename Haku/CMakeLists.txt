cmake_minimum_required(VERSION 3.20)

message("Building Haku Shared Library")

set(PROJECT_NAME "Haku_Api")

file(GLOB_RECURSE global_inc ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE global_src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE core_inc ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.hpp)
file(GLOB_RECURSE core_src ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp)
file(GLOB_RECURSE renderer_interface_inc ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/*.hpp)
file(GLOB_RECURSE renderer_interface_src ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/*.hpp)
file(GLOB_RECURSE renderer_inc ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/DirectX12/*.hpp)
file(GLOB_RECURSE renderer_src ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/DirectX12/*.cpp)

#needs to be made platform dependant selections

file(GLOB_RECURSE windows_inc ${CMAKE_CURRENT_SOURCE_DIR}/platform/Windows/*.hpp)
file(GLOB_RECURSE windows_src ${CMAKE_CURRENT_SOURCE_DIR}/platform/Windows/*.cpp)


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${global_src})


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${core_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${core_src})


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${renderer_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${renderer_src})


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${windows_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${windows_src})


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${renderer_interface_inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${renderer_interface_src})


set(DEPEND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../vendor)

set(im_gui 
	${DEPEND_DIR}/ImGui/imgui.h
	${DEPEND_DIR}/ImGui/imgui.cpp	
	${DEPEND_DIR}/ImGui/imconfig.h
	${DEPEND_DIR}/ImGui/imgui_demo.cpp
	${DEPEND_DIR}/ImGui/imgui_draw.cpp
	${DEPEND_DIR}/ImGui/imgui_internal.h
	${DEPEND_DIR}/ImGui/imgui_tables.cpp
	${DEPEND_DIR}/ImGui/imstb_rectpack.h
	${DEPEND_DIR}/ImGui/imstb_textedit.h
	${DEPEND_DIR}/ImGui/imstb_truetype.h
	${DEPEND_DIR}/ImGui/imgui_widgets.cpp
	${DEPEND_DIR}/ImGui/backends/imgui_impl_dx12.h
	${DEPEND_DIR}/ImGui/backends/imgui_impl_win32.h
	${DEPEND_DIR}/ImGui/backends/imgui_impl_dx12.cpp
	${DEPEND_DIR}/ImGui/backends/imgui_impl_win32.cpp)



add_compile_definitions(HAKU_API_BUILD)
add_library(${PROJECT_NAME} 
	SHARED 
	${im_gui} 
	${core_inc}
	${core_src}
	${global_inc}
	${global_src}
	${windows_inc}
	${windows_src}
	${renderer_inc}
	${renderer_src}
	${renderer_interface_inc}
	${renderer_interface_src})
message("Dependencies : will : ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/wil/include/")
message("Dependencies : will : ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/spdlog/include/")

target_precompile_headers(${PROJECT_NAME} PRIVATE hakupch.hpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Core/)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Renderer/)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/ImGui/)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/DirectXTex/)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/wil/include/)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/spdlog/include/)

target_link_libraries(${PROJECT_NAME} PRIVATE DirectXTex)
target_link_libraries(${PROJECT_NAME} PRIVATE DirectXMath)
target_link_libraries(${PROJECT_NAME} PUBLIC  DirectX-Headers)

