#include "D3D12DescriptorAllocation.hpp"

namespace Haku
{
namespace Renderer
{
DescriptorAllocation::DescriptorAllocation()
{
	m_BaseHandle.ptr = NULL;
}
DescriptorAllocation::DescriptorAllocation(
	std::shared_ptr<DescriptorPage> owner,
	D3D12_CPU_DESCRIPTOR_HANDLE		base_ptr,
	uint64_t						base_offset,
	uint64_t						size)
	: m_OwningPage(owner)
	, m_BaseHandle(base_ptr)
	, m_BaseOffset(base_offset)
	, m_Size(size)
{
}
DescriptorAllocation::~DescriptorAllocation()
{
	m_OwningPage->Free(*this);
}
const uint64_t DescriptorAllocation::size() noexcept
{
	return m_Size;
}
const bool DescriptorAllocation::Is_Null() noexcept
{
	if (m_BaseHandle.ptr == NULL)
	{
		return true;
	}
	return false;
}
const uint64_t DescriptorAllocation::GetOffset() noexcept
{
	return m_BaseOffset;
}
} // namespace Renderer
} // namespace Haku
