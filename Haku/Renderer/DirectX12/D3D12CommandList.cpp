#include "D3D12Renderer.hpp"
#include "D3D12CommandList.hpp"
namespace Haku
{
namespace Renderer
{
Utils::HK_Queue_mt<D3DCommandList*> CommandList::m_FreeList;
Utils::HK_Queue_mt<D3DCommandList*> CommandList::m_StaleList;

D3DCommandList::D3DCommandList(D3D12_COMMAND_LIST_TYPE type)
	: m_Type(type)
{
	auto Device = RenderEngine::GetDeviceD3D();
	HAKU_SOK_ASSERT(Device->CreateCommandAllocator(
		type, __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(&m_CommandAllocator)))
	HAKU_SOK_ASSERT(Device->CreateCommandList(
		0u, type, m_CommandAllocator, nullptr, __uuidof(ID3D12CommandList), reinterpret_cast<void**>(&m_CommandList)))
}
void D3DCommandList::Reset() noexcept
{
	m_CommandList->Reset(m_CommandAllocator, nullptr);
}
void D3DCommandList::ShutDown() noexcept
{
	m_CommandList->Release();
	m_CommandAllocator->Release();
}
D3DCommandList::~D3DCommandList()
{
	ShutDown();
}
D3DCommandList* CreateD3DCommandList(D3D12_COMMAND_LIST_TYPE type)
{
	return new D3DCommandList(type);
}
CommandList* CreateCommandList()
{
	return new CommandList();
}
} // namespace Renderer
} // namespace Haku
