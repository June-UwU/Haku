#pragma once
#include "DirectXMath.h"

struct VertexData
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 RGBA;
};

struct ConstData
{
	DirectX::XMMATRIX Rotate;
	// this is important as : D3D12 ERROR: ID3D12Device::CreateConstantBufferView: Size of 64 is invalid.  Device
	// requires SizeInBytes be a multiple of 256. [ STATE_CREATION ERROR #650: CREATE_CONSTANT_BUFFER_VIEW_INVALID_DESC]
	float padding[48];
	// DirectX::XMFLOAT3 Translate;
};
