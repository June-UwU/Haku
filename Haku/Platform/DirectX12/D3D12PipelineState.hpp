#pragma once
#include "directx/d3d12.h"
#include "../../hakupch.hpp"
#include "D3D12CommandQueue.hpp"
#include "D3D12RenderDevice.hpp"
#include "../../Renderer/PipelineState.hpp"

namespace Haku
{
namespace Renderer
{
class D3D12PipelineState : public PipelineState
{
public:
	D3D12PipelineState(
		D3D12RenderDevice*	 Device,
		ID3D12RootSignature* RootSignature,
		std::wstring		 VertexShader,
		std::wstring		 PixelShader);
	void ShutDown() noexcept;
	void SetPipelineState(D3D12CommandQueue* Command);
	ID3D12PipelineState* get() { return m_PipelineState.Get(); }
private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
};
} // namespace Renderer
} // namespace Haku
