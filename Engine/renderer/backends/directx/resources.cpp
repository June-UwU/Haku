/*****************************************************************//**
 * \file   resources.cpp
 * \brief  impl
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#include "resources.hpp"
#include "directx_backend.hpp"
#include "core/logger.hpp"


ID3D12Resource* create_resource(u64 height, u64 width, u32 depth, D3D12_RESOURCE_DIMENSION dimension, D3D12_HEAP_TYPE type, D3D12_HEAP_FLAGS h_flags
    , DXGI_FORMAT  format,  D3D12_RESOURCE_STATES  state,D3D12_RESOURCE_FLAGS resource_flags)
{
    ID3D12Resource* ret_res = nullptr;
    directx_device* dev = get_device();

    D3D12_RESOURCE_DESC res_desc{};
    res_desc.Dimension = dimension;
    res_desc.Alignment = 0;
    res_desc.Height = height;
    res_desc.Width = width;
    res_desc.DepthOrArraySize = depth;
    res_desc.MipLevels = 1u;
    res_desc.Format = format;
    res_desc.SampleDesc.Count = 1u;
    res_desc.SampleDesc.Quality = 0u;
    res_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    res_desc.Flags = resource_flags;


    HRESULT dx_ret = dev->logical_device->CreateCommittedResource(&HEAP_PROPS[type - 1], h_flags, &res_desc, state, nullptr, IID_PPV_ARGS(&ret_res));
    if (H_OK != dx_ret)
    {
        HLCRIT("failed to create a resource");
        return nullptr;
    }

    return ret_res;
}

void destroy_resource(ID3D12Resource* resource)
{
    resource->Release();
}
