/*****************************************************************//**
 * \file   rootsignature.cpp
 * \brief  root signature implementations
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include "rootsignature.hpp"
#include "core/logger.hpp"

i8 create_root_signature(directx_context* context,directx_root_signature* signature)
{
    ID3DBlob* serialized_root_sig;
    ID3DBlob* error_blob;

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC root_desc{};
    root_desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
    root_desc.Desc_1_1.NumParameters = count(signature->parameter_array);
    root_desc.Desc_1_1.pParameters = (D3D12_ROOT_PARAMETER1*)signature->parameter_array;
    root_desc.Desc_1_1.NumStaticSamplers = count(signature->sampler_array);
    root_desc.Desc_1_1.pStaticSamplers = (D3D12_STATIC_SAMPLER_DESC*)signature->sampler_array;

    HRESULT api_ret_val = D3D12SerializeVersionedRootSignature(&root_desc, &serialized_root_sig, &error_blob);
    if (H_OK != api_ret_val)
    {
        HLEMER("root signature serialization failure");
        HLEMER("\t cause : %s", (char*)error_blob->GetBufferPointer());
        return H_FAIL;
    }
    
    api_ret_val = context->logical_device->CreateRootSignature(0, serialized_root_sig->GetBufferPointer(), serialized_root_sig->GetBufferSize(),
        __uuidof(ID3D12RootSignature), (void**)&signature->root_signature);
    if (H_OK != api_ret_val)
    {
        serialized_root_sig->Release();
        HLEMER("root signature creation failure");
        return H_FAIL;
    }

    serialized_root_sig->Release();
    return H_OK;
}

void destroy_root_signature(directx_root_signature* signature)
{
    signature->root_signature->Release();
    D3D12_ROOT_PARAMETER1* param_ptr = (D3D12_ROOT_PARAMETER1*)signature->parameter_array;
    u64 size = count(signature->parameter_array);
    for (u64 i = 0; i < size; i++)
    {
        param_ptr++;
        if (D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE == param_ptr->ParameterType)
        {
            destroy_darray((darray)param_ptr->DescriptorTable.pDescriptorRanges);
        }
    }

}

i8 add_root_constant(directx_root_signature* signature, const u64 num_of_values, const u64 register_space)
{
    D3D12_ROOT_PARAMETER1 parameter{};
    parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    parameter.Constants.Num32BitValues = num_of_values;
    parameter.Constants.RegisterSpace = register_space;
    parameter.Constants.ShaderRegister = signature->cbv_base++;
    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    signature->parameter_array = push_back(signature->parameter_array, &parameter);
    signature->allocate_parameter += 1;

    if (signature->allocate_parameter > 64)
    {
        HLEMER("allocated parameter exceeded the root signature limit");
        return H_FAIL;
    }

    return H_OK;
}

i8 add_root_descriptor_srv(directx_root_signature* signature, const u64 register_space, D3D12_ROOT_DESCRIPTOR_FLAGS flags)
{
    D3D12_ROOT_PARAMETER1 parameter{};
    parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    parameter.Descriptor.RegisterSpace = register_space;
    parameter.Descriptor.ShaderRegister = signature->srv_base++;
    parameter.Descriptor.Flags = flags;
    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    signature->parameter_array = push_back(signature->parameter_array, &parameter);
    signature->allocate_parameter += 2;

    if (signature->allocate_parameter > 64)
    {
        HLEMER("allocated parameter exceeded the root signature limit");
        return H_FAIL;
    }

    return H_OK;
}

i8 add_root_descriptor_uav(directx_root_signature* signature, const u64 register_space, D3D12_ROOT_DESCRIPTOR_FLAGS flags)
{
    D3D12_ROOT_PARAMETER1 parameter{};
    parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
    parameter.Descriptor.RegisterSpace = register_space;
    parameter.Descriptor.ShaderRegister = signature->srv_base++;
    parameter.Descriptor.Flags = flags;
    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    signature->parameter_array = push_back(signature->parameter_array, &parameter);
    signature->allocate_parameter += 2;

    if (signature->allocate_parameter > 64)
    {
        HLEMER("allocated parameter exceeded the root signature limit");
        return H_FAIL;
    }

    return H_OK;
}

i8 add_root_descriptor_cbv(directx_root_signature* signature, const u64 register_space, D3D12_ROOT_DESCRIPTOR_FLAGS flags)
{
    D3D12_ROOT_PARAMETER1 parameter{};
    parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
    parameter.Descriptor.RegisterSpace = register_space;
    parameter.Descriptor.ShaderRegister = signature->srv_base++;
    parameter.Descriptor.Flags = flags;
    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    signature->parameter_array = push_back(signature->parameter_array, &parameter);
    signature->allocate_parameter += 2;

    if (signature->allocate_parameter > 64)
    {
        HLEMER("allocated parameter exceeded the root signature limit");
        return H_FAIL;
    }

    return H_OK;
}

i8 add_descriptor_table(directx_root_signature* signature, darray table_array)
{
    D3D12_ROOT_PARAMETER1 parameter{};
    parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    parameter.DescriptorTable.NumDescriptorRanges = count(table_array);
    parameter.DescriptorTable.pDescriptorRanges = (D3D12_DESCRIPTOR_RANGE1*)table_array;
    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    signature->parameter_array = push_back(signature->parameter_array, &parameter);
    signature->allocate_parameter += 1;

    if (signature->allocate_parameter > 64)
    {
        HLEMER("allocated parameter exceeded the root signature limit");
        return H_FAIL;
    }

    return H_OK;
}

i8 add_table_parameter(directx_root_signature* signature, darray* table,
    descriptor_type type, const u64 number_of_descriptor, const u64 register_space, D3D12_DESCRIPTOR_RANGE_FLAGS flags)
{
    D3D12_DESCRIPTOR_RANGE1 range;
    range.Flags = flags;
    range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    switch (type)
    {
        case CBV:
        {
            range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            range.BaseShaderRegister = signature->cbv_base++;
            range.NumDescriptors = number_of_descriptor;
            range.RegisterSpace = register_space;
            break;
        }
        case UAV:
        {
            range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            range.BaseShaderRegister = signature->uav_base++;
            range.NumDescriptors = number_of_descriptor;
            range.RegisterSpace = register_space;
            break;
        }
        case SRV:
        {
            range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            range.BaseShaderRegister = signature->srv_base++;
            range.NumDescriptors = number_of_descriptor;
            range.RegisterSpace = register_space;
            break;
        }
        case SAMPLER:
        {
            range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
            range.BaseShaderRegister = signature->sampler_base++;
            range.NumDescriptors = number_of_descriptor;
            range.RegisterSpace = register_space;
            break;
        }
        default:
        {
            HLCRIT("table parameter type is invalid");
            return  H_FAIL;
        }
    }

    *table = push_back(table, &range);

    return H_OK;
}
