#pragma once
#include "DirectXMath.h"
#include "directx/d3dx12.h"

constexpr DXGI_SAMPLE_DESC sampledesc{ 1u, 0u }; // no anti aliasing for now

constexpr D3D12_INPUT_ELEMENT_DESC InputElementDesc[]{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
};

constexpr uint32_t InputElementCount = sizeof(InputElementDesc);

constexpr D3D12_INPUT_LAYOUT_DESC InputDesc{ InputElementDesc, 2 };

struct VertexData
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 color;
};

struct ConstData
{
	DirectX::XMMATRIX Matrix;
	// this is important as : D3D12 ERROR: ID3D12Device::CreateConstantBufferView: Size of 64 is invalid.  Device
	// requires SizeInBytes be a multiple of 256. [ STATE_CREATION ERROR #650: CREATE_CONSTANT_BUFFER_VIEW_INVALID_DESC]
	float padding[48];
	// DirectX::XMFLOAT3 Translate;
};
