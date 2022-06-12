#pragma once
#include <memory>
#include <d3d12.h>
#include "wil/wrl.h"
#include "macros.hpp"
#include "Utils/Queue_MT.hpp"

// TODO : make ExecuteLists asynchronous

/*Command Queue*/
// This is the command queue that is a single instance object that is used for the explicit synchroization
// and execution of the commandlist reference that are in submitted to it and this will be also responsible
// for the management of the commandlist that are referenced to be resubmitted to the free queue

namespace Haku
{
namespace Renderer
{
class CommandList;
class CommandQueue
{
public:
	CommandQueue(
		D3D12_COMMAND_QUEUE_FLAGS flags	   = D3D12_COMMAND_QUEUE_FLAG_NONE,
		INT						  priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL);
	~CommandQueue();
	DISABLE_COPY(CommandQueue)
	DISABLE_MOVE(CommandQueue)

	void				Synchronize();
	void				Shutdown() noexcept;
	void				ExecuteLists() noexcept;
	ID3D12CommandQueue* Get(D3D12_COMMAND_LIST_TYPE type);


	//this needs to be ripped out
	void ExecuteTest(ID3D12GraphicsCommandList* ptr, size_t count);

private:
	std::array<ID3D12CommandQueue*, 3> m_CommandQueueArray;
};
} // namespace Renderer
} // namespace Haku
