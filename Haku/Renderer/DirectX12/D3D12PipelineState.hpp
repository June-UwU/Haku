#pragma once
#include "../../hakupch.hpp"
#include "directx/d3d12.h"
#include "D3D12CommandQueue.hpp"
#include "D3D12RenderDevice.hpp"

//TODO : Rework this and abstract the shader code and compilers


namespace Haku
{
namespace Renderer
{
class D3D12PipelineState
{
public:
	D3D12PipelineState(
		D3D12RenderDevice*	 Device,
		ID3D12RootSignature* RootSignature,
		std::wstring		 VertexShader,
		std::wstring		 PixelShader);
	void				 ShutDown() noexcept;
	ID3D12PipelineState* Get() { return m_PipelineState.Get(); }
	void				 SetPipelineState(D3D12CommandQueue& Command);

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
};
} // namespace Renderer
} // namespace Haku
