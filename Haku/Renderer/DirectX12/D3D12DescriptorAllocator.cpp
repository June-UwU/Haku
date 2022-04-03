#include "D3D12DescriptorAllocator.hpp"
namespace Haku
{
namespace Renderer
{
DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t defaultpagesize)
	: m_DefaultPageSize(defaultpagesize)
	, m_type(type)
{
}
void DescriptorAllocator::ReleaseStaleAllocations() noexcept
{
	// a function that poll every page and adds allocatable pages to the vector
	for (size_t i = 0; i < m_PagePool.size(); i++)
	{
		m_PagePool[i]->ReleaseStaleAllocations();
		if (m_PagePool[i]->size()) // checking if it has free pages
		{
			m_FreePageIndexSet.emplace(i);
		}
	}
}
DescriptorAllocation DescriptorAllocator::Allocate(uint32_t numberofpages) noexcept
{
	if (!m_FreePageIndexSet.size()) // checking if the exists a free page if not creating a new page
	{
		CreateNewPage();
		return m_PagePool[m_PagePool.size() - 1]->Allocate(numberofpages);
		// Allocate form the last created page , this makes the assumption that the last place is
		// where a new page is stored when a page is created
	}
	else
	{
		for (auto it = m_FreePageIndexSet.cbegin(); it != m_FreePageIndexSet.cend(); it++)
		{
			if (numberofpages < m_PagePool[*it]->size())
			{
				auto ret_val = m_PagePool[*it]->Allocate(numberofpages);
				if (ret_val.Is_Null())
				{
					continue;
				}
				// clean up the allcated page for math error and empty pages so that useless pages are avoided
				if (m_PagePool[*it]->size() <= 0)
				{
					m_FreePageIndexSet.erase(it);
				}

				return ret_val;
			}
		}
		CreateNewPage(); // make sure new page with sufficent descriptor is added then call for Allocate;
		return m_PagePool[m_PagePool.size() - 1]->Allocate(numberofpages);
		// Allocate form the last created page , this makes the assumption that the last place is
		// where a new page is stored when a page is created
	}
}
void DescriptorAllocator::CreateNewPage() noexcept
{
	m_PagePool.emplace_back(std::make_shared<DescriptorPage>(m_type));
	m_FreePageIndexSet.emplace(m_PagePool.size() - 1);
}
} // namespace Renderer
} // namespace Haku
