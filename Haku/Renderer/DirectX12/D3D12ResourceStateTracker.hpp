#pragma once

#include "macros.hpp"
#include "hakupch.hpp"
#include "HakuLog.hpp"
#include <directx/d3d12.h>

namespace Haku
{
namespace Renderer
{
class ResourceState
{
	using StateMap = std::map<uint32_t, D3D12_RESOURCE_STATES>;

public:
	// this is for a an all resource state set
	ResourceState() = default;
	ResourceState(const std::vector<D3D12_RESOURCE_STATES>& vector) noexcept;
	ResourceState(const D3D12_RESOURCE_STATES* ptr, size_t ptr_count) noexcept;
	ResourceState(uint32_t subresource_count, D3D12_RESOURCE_STATES state) noexcept;

	ResourceState(const ResourceState& ref) noexcept;
	ResourceState(const ResourceState&& ref) noexcept;
	ResourceState& operator=(const ResourceState& ref) noexcept;
	ResourceState& operator=(const ResourceState&& ref) noexcept;

	D3D12_RESOURCE_STATES operator[](uint32_t pos) const;

	const D3D12_RESOURCE_STATES GetSubResourceState(uint32_t pos);
	void						SetGeneralState(D3D12_RESOURCE_STATES state);
	uint32_t					GetCount() noexcept { return m_SubResourceCount; }
	const D3D12_RESOURCE_STATES GetGeneralState() noexcept { return m_GeneralState; }
	const bool					GeneralStateSet() noexcept { return m_StateMap.empty(); }
	void						SetSubResourceState(uint32_t pos, const D3D12_RESOURCE_STATES state);

public:
	StateMap			  m_StateMap;
	D3D12_RESOURCE_STATES m_GeneralState;
	uint32_t			  m_SubResourceCount;
};

class ResourceStateTracker
{
	using ResourceMap	   = std::unordered_map<ID3D12Resource*, ResourceState>;
	using ResourceBarriers = std::vector<D3D12_RESOURCE_BARRIER>;

public:
	ResourceStateTracker() = default;
	DISABLE_MOVE(ResourceStateTracker)
	DISABLE_COPY(ResourceStateTracker)

	// Local function
	void FlushPendingBarriers();
	void ResetLocalStructures() noexcept;
	void TransitionBarriers(
		ID3D12Resource*		  resource,
		D3D12_RESOURCE_STATES state_after,
		uint32_t			  slot = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) noexcept;

	// Global functions
	static void UnRegisterResource(ID3D12Resource* ptr);
	static void RegisterResource(ID3D12Resource* ptr, const ResourceState& state) noexcept;
	static void RegisterResource(ID3D12Resource* ptr, const ResourceState&& state) noexcept;

private:
	ResourceBarriers m_Barriers;
	ResourceBarriers m_PendingBarriers;

	static ResourceMap m_StateMap;
};
} // namespace Renderer
} // namespace Haku
