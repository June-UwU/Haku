#pragma once
#include "../macros.hpp"
#include "../hakupch.hpp"
#include "directx/d3dx12.h"
#include <dxgi1_6.h>
#include <d3d12.h>

namespace Haku
{
class HAKU_API UI
{
public:
	static void render();
	static void DarkMode();
	static void DX12_newframe();
	static void CreateContext();
	static void DX12_shutdown();
	static void win32_shutdown();
	static void DestroyContext();
	static void win32_newframe();
	static void generic_new_frame();
	static void win32_Init(void* hwnd);
	static void DX12_Init(
		ID3D12Device*				device,
		int							num_frames_in_flight,
		DXGI_FORMAT					rtv_format,
		ID3D12DescriptorHeap*		cbv_srv_heap,
		D3D12_CPU_DESCRIPTOR_HANDLE font_srv_cpu_desc_handle,
		D3D12_GPU_DESCRIPTOR_HANDLE font_srv_gpu_desc_handle);
	static void DemoWindow(bool cond = true);
};
} // namespace Haku
