#pragma once
#include "hakupch.hpp"
#include "directx/d3d12.h"
#include "D3D12CommandQueue.hpp"
#include "D3D12RenderDevice.hpp"

// TODO : Rework this and abstract the shader code and compilers

namespace Haku
{
namespace Renderer
{
class RootSignature;
class PipeLineState
{
public:
	PipeLineState(
		RootSignature* signature,
		ID3DBlob*	   VS, // vertex shader is not optional
		ID3DBlob*	   PS = nullptr,
		ID3DBlob*	   DS = nullptr,
		ID3DBlob*	   HS = nullptr,
		ID3DBlob*	   GS = nullptr);
	DISABLE_COPY(PipeLineState)
	DISABLE_MOVE(PipeLineState)
	~PipeLineState();

	ID3D12PipelineState* Get() const noexcept;
	void				 Release() noexcept;

private:
	ID3D12PipelineState* m_PSO;
	ID3DBlob*			 m_HullShader;
	ID3DBlob*			 m_PixelShader;
	ID3DBlob*			 m_VertexShader;
	ID3DBlob*			 m_DomainShader;
	RootSignature*		 m_RootSignature;
	ID3DBlob*			 m_GeometryShader;
};

void	  InitializePSOWithUISRV();
ID3DBlob* CompileShaderFile(const wchar_t* filepath, const char* target, UINT compileflags = 0);
std::pair<uint64_t, PipeLineState*> CreatePSO_ReturnKey(
	RootSignature* signature,
	ID3DBlob*	   VS,
	ID3DBlob*	   PS,
	ID3DBlob*	   DS = nullptr,
	ID3DBlob*	   HS = nullptr,
	ID3DBlob*	   GS = nullptr);
PipeLineState* CreatePSO(
	RootSignature* signature,
	ID3DBlob*	   VS,
	ID3DBlob*	   PS,
	ID3DBlob*	   DS = nullptr,
	ID3DBlob*	   HS = nullptr,
	ID3DBlob*	   GS = nullptr);
void ShutDownPSO() noexcept;

PipeLineState*										GetPSO(uint64_t Key) noexcept;
static std::unordered_map<uint64_t, PipeLineState*> S_PSOMap;
static uint64_t										S_PSOID = 0;
} // namespace Renderer
} // namespace Haku
