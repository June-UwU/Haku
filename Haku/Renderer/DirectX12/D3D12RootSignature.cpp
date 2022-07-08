#include "D3D12Renderer.hpp"
#include "D3D12RootSignature.hpp"

namespace Haku
{
namespace Renderer
{
RootSignature::RootSignature(const RootSigLayout& layout)
	: m_Layout(layout)
{
	auto							  Device = RenderEngine::GetDeviceD3D();
	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootsigfeature{};
	rootsigfeature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	if (S_OK != Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootsigfeature, sizeof(rootsigfeature)))
	{
		rootsigfeature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	ID3DBlob* error_blob;
	ID3DBlob* root_blob;

	m_Description = m_Layout.GenerateVersionedRootSignature(
		rootsigfeature.HighestVersion, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	auto hr =
		D3DX12SerializeVersionedRootSignature(&m_Description, rootsigfeature.HighestVersion, &root_blob, &error_blob);
	if (error_blob)
	{
		OutputDebugStringA(reinterpret_cast<char*>(error_blob->GetBufferPointer()));
	}
	HAKU_SOK_ASSERT(hr);
	hr = Device->CreateRootSignature(
		0,
		root_blob->GetBufferPointer(),
		root_blob->GetBufferSize(),
		__uuidof(ID3D12RootSignature),
		reinterpret_cast<void**>(&m_Signature));
	HAKU_SOK_ASSERT(hr);

	root_blob->Release();
}
ID3D12RootSignature* RootSignature::Get()
{
	return m_Signature;
}

void RootSignature::Release() noexcept
{
	m_Signature->Release();
}

RootSigLayout::RootSigLayout()
{
	for (auto& ref : m_ParameterTracker)
	{
		ref = ROOT_PARAMETER_NONE;
	}
}

RootSigLayout::RootSigLayout(const RootSigLayout& rhs)
	: m_Position(rhs.m_Position)
	, m_ParameterCount(rhs.m_ParameterCount)
	, m_TablePropArray(rhs.m_TablePropArray)
	, m_ParameterTracker(rhs.m_ParameterTracker)
{
}

RootSigLayout::RootSigLayout(const RootSigLayout&& rhs)
	: m_Position(std::move(rhs.m_Position))
	, m_ParameterCount(std::move(rhs.m_ParameterCount))
	, m_TablePropArray(std::move(rhs.m_TablePropArray))
	, m_ParameterTracker(std::move(rhs.m_ParameterTracker))
{
}

RootSigLayout& RootSigLayout::operator=(const RootSigLayout& rhs)
{
	m_Position		   = rhs.m_Position;
	m_ParameterCount   = rhs.m_ParameterCount;
	m_TablePropArray   = rhs.m_TablePropArray;
	m_ParameterTracker = rhs.m_ParameterTracker;
	return *this;
}

RootSigLayout& RootSigLayout::operator=(const RootSigLayout&& rhs)
{
	m_Position		   = std::move(rhs.m_Position);
	m_ParameterCount   = std::move(rhs.m_ParameterCount);
	m_TablePropArray   = std::move(rhs.m_TablePropArray);
	m_ParameterTracker = std::move(rhs.m_ParameterTracker);
	return *this;
}

void RootSigLayout::AddRootConstant(uint8_t shaderreg, uint8_t registerspace)
{
	assert(m_Position < MAX_ROOT_PARAMETER_COUNT);
	m_ParameterTracker[m_Position]		   = ROOT_PARAMETER_INLINE_CONST;
	m_TablePropArray[m_Position].shaderreg = shaderreg;
	m_TablePropArray[m_Position].regspace  = registerspace;
	m_Position++;
	m_ParameterCount++;
}

void RootSigLayout::AddRootSRV(uint8_t shaderreg, uint8_t registerspace)
{
	assert(m_Position < MAX_ROOT_PARAMETER_COUNT);
	m_ParameterTracker[m_Position]		   = ROOT_PARAMETER_INLINE_SRV;
	m_TablePropArray[m_Position].shaderreg = shaderreg;
	m_TablePropArray[m_Position].regspace  = registerspace;
	m_Position++;
	m_ParameterTracker[m_Position]		   = ROOT_PARAMETER_INLINE_SRV1;
	m_TablePropArray[m_Position].shaderreg = shaderreg;
	m_TablePropArray[m_Position].regspace  = registerspace;
	m_Position++;
	m_ParameterCount++;
}

void RootSigLayout::AddRootUAV(uint8_t shaderreg, uint8_t registerspace)
{
	assert(m_Position < MAX_ROOT_PARAMETER_COUNT);
	m_ParameterTracker[m_Position]		   = ROOT_PARAMETER_INLINE_UAV;
	m_TablePropArray[m_Position].shaderreg = shaderreg;
	m_TablePropArray[m_Position].regspace  = registerspace;
	m_Position++;
	m_ParameterTracker[m_Position]		   = ROOT_PARAMETER_INLINE_UAV1;
	m_TablePropArray[m_Position].shaderreg = shaderreg;
	m_TablePropArray[m_Position].regspace  = registerspace;
	m_Position++;
	m_ParameterCount++;
}

void RootSigLayout::AddRootCBV(uint8_t shaderreg, uint8_t registerspace)
{
	assert(m_Position < MAX_ROOT_PARAMETER_COUNT);
	m_ParameterTracker[m_Position]		   = ROOT_PARAMETER_INLINE_CBV;
	m_TablePropArray[m_Position].shaderreg = shaderreg;
	m_TablePropArray[m_Position].regspace  = registerspace;
	m_Position++;
	m_ParameterTracker[m_Position]		   = ROOT_PARAMETER_INLINE_CBV1;
	m_TablePropArray[m_Position].shaderreg = shaderreg;
	m_TablePropArray[m_Position].regspace  = registerspace;
	m_Position++;
	m_ParameterCount++;
}

void RootSigLayout::AddDescriptorTableSRV(uint8_t shaderreg, uint8_t registerspace, uint8_t descriptorcount)
{
	assert(m_Position < MAX_ROOT_PARAMETER_COUNT);
	m_ParameterTracker[m_Position]				 = ROOT_PARAMETER_TABLE_SRV;
	m_TablePropArray[m_Position].descriptorcount = descriptorcount;
	m_TablePropArray[m_Position].regspace		 = registerspace;
	m_TablePropArray[m_Position].shaderreg		 = shaderreg;
	m_Position++;
	m_ParameterCount++;
}

void RootSigLayout::AddDescriptorTableUAV(uint8_t shaderreg, uint8_t registerspace, uint8_t descriptorcount)
{
	assert(m_Position < MAX_ROOT_PARAMETER_COUNT);
	m_ParameterTracker[m_Position]				 = ROOT_PARAMETER_TABLE_UAV;
	m_TablePropArray[m_Position].descriptorcount = descriptorcount;
	m_TablePropArray[m_Position].regspace		 = registerspace;
	m_TablePropArray[m_Position].shaderreg		 = shaderreg;
	m_Position++;
	m_ParameterCount++;
}

void RootSigLayout::AddDescriptorTableCBV(uint8_t shaderreg, uint8_t registerspace, uint8_t descriptorcount)
{
	assert(m_Position < MAX_ROOT_PARAMETER_COUNT);
	m_ParameterTracker[m_Position]				 = ROOT_PARAMETER_TABLE_CBV;
	m_TablePropArray[m_Position].descriptorcount = descriptorcount;
	m_TablePropArray[m_Position].regspace		 = registerspace;
	m_TablePropArray[m_Position].shaderreg		 = shaderreg;
	m_Position++;
	m_ParameterCount++;
}

D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootSigLayout::GenerateVersionedRootSignature(
	D3D_ROOT_SIGNATURE_VERSION version,
	uint32_t				   flags)
{
	switch (version)
	{
	case D3D_ROOT_SIGNATURE_VERSION_1_0:
	{
		return GenerateVersionedRootSignature_1_0(flags);
		break;
	}
	case D3D_ROOT_SIGNATURE_VERSION_1_1:
	{
		return GenerateVersionedRootSignature_1_1(flags);
		break;
	}
	default:
		return GenerateVersionedRootSignature_1_0(flags);
		break;
	}
}

D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootSigLayout::GenerateVersionedRootSignature_1_0(uint32_t flags)
{
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC ret_desc{};
	ret_desc.Version				= D3D_ROOT_SIGNATURE_VERSION_1_0;
	ret_desc.Desc_1_0.NumParameters = m_ParameterCount;
	D3D12_ROOT_PARAMETER* paramptr	= new D3D12_ROOT_PARAMETER[m_ParameterCount];
	ret_desc.Desc_1_0.pParameters	= paramptr;

	ret_desc.Desc_1_0.NumStaticSamplers = 0;
	ret_desc.Desc_1_0.pStaticSamplers	= nullptr;

	ret_desc.Desc_1_0.Flags = static_cast<D3D12_ROOT_SIGNATURE_FLAGS>(flags);

	uint8_t iter	  = 0;
	uint8_t param_pos = 0;
	while (iter < m_ParameterCount)
	{
		switch (m_ParameterTracker[param_pos])
		{
		case ROOT_PARAMETER_NONE:
		{
			goto exit_parameter_loop;
			break;
		}
		case ROOT_PARAMETER_INLINE_CONST:
		{
			paramptr[iter].ParameterType			= D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			paramptr[iter].Constants.Num32BitValues = 1;
			paramptr[iter].Constants.RegisterSpace	= m_TablePropArray[param_pos].regspace;
			paramptr[iter].Constants.ShaderRegister = m_TablePropArray[param_pos].shaderreg;
			param_pos++;
			break;
		}
		case ROOT_PARAMETER_INLINE_SRV:
		{
			paramptr[iter].ParameterType			 = D3D12_ROOT_PARAMETER_TYPE_SRV;
			paramptr[iter].Descriptor.RegisterSpace	 = m_TablePropArray[param_pos].regspace;
			paramptr[iter].Descriptor.ShaderRegister = m_TablePropArray[param_pos].shaderreg;
			param_pos += 2;
			break;
		}
		case ROOT_PARAMETER_INLINE_CBV:
		{
			paramptr[iter].ParameterType			 = D3D12_ROOT_PARAMETER_TYPE_CBV;
			paramptr[iter].Descriptor.RegisterSpace	 = m_TablePropArray[param_pos].regspace;
			paramptr[iter].Descriptor.ShaderRegister = m_TablePropArray[param_pos].shaderreg;
			param_pos += 2;
			break;
		}
		case ROOT_PARAMETER_INLINE_UAV:
		{
			paramptr[iter].ParameterType			 = D3D12_ROOT_PARAMETER_TYPE_UAV;
			paramptr[iter].Descriptor.RegisterSpace	 = m_TablePropArray[param_pos].regspace;
			paramptr[iter].Descriptor.ShaderRegister = m_TablePropArray[param_pos].shaderreg;
			param_pos += 2;
			break;
		}
		case ROOT_PARAMETER_TABLE_UAV:
		{
			D3D12_DESCRIPTOR_RANGE* range			 = new D3D12_DESCRIPTOR_RANGE;
			range->RangeType						 = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			range->RegisterSpace					 = m_TablePropArray[param_pos].regspace;
			range->BaseShaderRegister				 = m_TablePropArray[param_pos].shaderreg;
			range->NumDescriptors					 = m_TablePropArray[param_pos].descriptorcount;

			paramptr[iter].DescriptorTable.pDescriptorRanges   = range;
			paramptr[iter].ParameterType					   = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			paramptr[iter].DescriptorTable.NumDescriptorRanges = 1;
			param_pos++;
			break;
		}
		case ROOT_PARAMETER_TABLE_CBV:
		{
			D3D12_DESCRIPTOR_RANGE* range			 = new D3D12_DESCRIPTOR_RANGE;
			range->RangeType						 = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			range->RegisterSpace					 = m_TablePropArray[param_pos].regspace;
			range->BaseShaderRegister				 = m_TablePropArray[param_pos].shaderreg;
			range->NumDescriptors					 = m_TablePropArray[param_pos].descriptorcount;

			paramptr[iter].DescriptorTable.pDescriptorRanges   = range;
			paramptr[iter].ParameterType					   = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			paramptr[iter].DescriptorTable.NumDescriptorRanges = 1;

			param_pos++;
			break;
		}
		case ROOT_PARAMETER_TABLE_SRV:
		{
			D3D12_DESCRIPTOR_RANGE* range			 = new D3D12_DESCRIPTOR_RANGE;
			range->RangeType						 = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			range->RegisterSpace					 = m_TablePropArray[param_pos].regspace;
			range->BaseShaderRegister				 = m_TablePropArray[param_pos].shaderreg;
			range->NumDescriptors					 = m_TablePropArray[param_pos].descriptorcount;

			paramptr[iter].DescriptorTable.pDescriptorRanges   = range;
			paramptr[iter].ParameterType					   = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			paramptr[iter].DescriptorTable.NumDescriptorRanges = 1;

			param_pos++;
			break;
		}
		}
		paramptr[iter].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		iter++;
	}

exit_parameter_loop:
	return ret_desc;
}

D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootSigLayout::GenerateVersionedRootSignature_1_1(uint32_t flags)
{
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC ret_desc{};
	ret_desc.Version				= D3D_ROOT_SIGNATURE_VERSION_1_1;
	ret_desc.Desc_1_1.NumParameters = m_ParameterCount;
	D3D12_ROOT_PARAMETER1* paramptr = new D3D12_ROOT_PARAMETER1[m_ParameterCount];
	ret_desc.Desc_1_1.pParameters	= paramptr;

	ret_desc.Desc_1_1.NumStaticSamplers = 0;
	ret_desc.Desc_1_1.pStaticSamplers	= nullptr;
	ret_desc.Desc_1_1.Flags				= static_cast<D3D12_ROOT_SIGNATURE_FLAGS>(flags);

	uint8_t iter	  = 0;
	uint8_t param_pos = 0;
	while (iter < m_ParameterCount)
	{
		switch (m_ParameterTracker[param_pos])
		{
		case ROOT_PARAMETER_NONE:
		{
			goto exit_parameter_loop;
			break;
		}
		case ROOT_PARAMETER_INLINE_CONST:
		{
			paramptr[iter].ParameterType			= D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			paramptr[iter].Constants.Num32BitValues = 1;
			paramptr[iter].Constants.RegisterSpace	= m_TablePropArray[param_pos].regspace;
			paramptr[iter].Constants.ShaderRegister = m_TablePropArray[param_pos].shaderreg;
			param_pos++;
			break;
		}
		case ROOT_PARAMETER_INLINE_SRV:
		{
			paramptr[iter].ParameterType			 = D3D12_ROOT_PARAMETER_TYPE_SRV;
			paramptr[iter].Descriptor.Flags			 = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
			paramptr[iter].Descriptor.RegisterSpace	 = m_TablePropArray[param_pos].regspace;
			paramptr[iter].Descriptor.ShaderRegister = m_TablePropArray[param_pos].shaderreg;
			param_pos += 2;
			break;
		}
		case ROOT_PARAMETER_INLINE_CBV:
		{
			paramptr[iter].ParameterType			 = D3D12_ROOT_PARAMETER_TYPE_CBV;
			paramptr[iter].Descriptor.Flags			 = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
			paramptr[iter].Descriptor.RegisterSpace	 = m_TablePropArray[param_pos].regspace;
			paramptr[iter].Descriptor.ShaderRegister = m_TablePropArray[param_pos].shaderreg;
			param_pos += 2;
			break;
		}
		case ROOT_PARAMETER_INLINE_UAV:
		{
			paramptr[iter].ParameterType			 = D3D12_ROOT_PARAMETER_TYPE_UAV;
			paramptr[iter].Descriptor.Flags			 = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
			paramptr[iter].Descriptor.RegisterSpace	 = m_TablePropArray[param_pos].regspace;
			paramptr[iter].Descriptor.ShaderRegister = m_TablePropArray[param_pos].shaderreg;
			param_pos += 2;
			break;
		}
		case ROOT_PARAMETER_TABLE_UAV:
		{
			D3D12_DESCRIPTOR_RANGE1* range			 = new D3D12_DESCRIPTOR_RANGE1;
			range->RangeType						 = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			range->Flags							 = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
			range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			range->RegisterSpace					 = m_TablePropArray[param_pos].regspace;
			range->BaseShaderRegister				 = m_TablePropArray[param_pos].shaderreg;
			range->NumDescriptors					 = m_TablePropArray[param_pos].descriptorcount;

			paramptr[iter].DescriptorTable.pDescriptorRanges   = range;
			paramptr[iter].ParameterType					   = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			paramptr[iter].DescriptorTable.NumDescriptorRanges = 1;
			param_pos++;
			break;
		}
		case ROOT_PARAMETER_TABLE_CBV:
		{
			D3D12_DESCRIPTOR_RANGE1* range			 = new D3D12_DESCRIPTOR_RANGE1;
			range->RangeType						 = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			range->Flags							 = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
			range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			range->RegisterSpace					 = m_TablePropArray[param_pos].regspace;
			range->BaseShaderRegister				 = m_TablePropArray[param_pos].shaderreg;
			range->NumDescriptors					 = m_TablePropArray[param_pos].descriptorcount;

			paramptr[iter].DescriptorTable.pDescriptorRanges   = range;
			paramptr[iter].ParameterType					   = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			paramptr[iter].DescriptorTable.NumDescriptorRanges = 1;

			param_pos++;
			break;
		}
		case ROOT_PARAMETER_TABLE_SRV:
		{
			D3D12_DESCRIPTOR_RANGE1* range			 = new D3D12_DESCRIPTOR_RANGE1;
			range->RangeType						 = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			range->Flags							 = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
			range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			range->RegisterSpace					 = m_TablePropArray[param_pos].regspace;
			range->BaseShaderRegister				 = m_TablePropArray[param_pos].shaderreg;
			range->NumDescriptors					 = m_TablePropArray[param_pos].descriptorcount;

			paramptr[iter].DescriptorTable.pDescriptorRanges   = range;
			paramptr[iter].ParameterType					   = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			paramptr[iter].DescriptorTable.NumDescriptorRanges = 1;

			param_pos++;
			break;
		}
		}
		paramptr[iter].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		iter++;
	}

exit_parameter_loop:

	return ret_desc;
}

} // namespace Renderer
} // namespace Haku
