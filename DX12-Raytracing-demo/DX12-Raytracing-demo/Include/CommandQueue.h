#ifndef __COMMANDQUEUE_H__
#define __COMMANDQUEUE_H__

#include <d3d12.h>

#include <cstdint>
#include <queue>

class CommandQueue
{
public:
	CommandQueue(ID3D12Device5* device, D3D12_COMMAND_LIST_TYPE type);
	virtual ~CommandQueue();

	ID3D12GraphicsCommandList4* GetCommandList();
	uint64_t ExecuteCommandList(ID3D12GraphicsCommandList4* commandList);

	uint64_t Signal();
	bool IsFenceComplete(uint64_t fenceValue);
	void WaitForFenceValue(uint64_t fenceValue);
	void Flush();

	ID3D12CommandQueue* GetD3D12CommandQueue() const;

protected:
	ID3D12CommandAllocator* CreateCommandAllocator();
	ID3D12GraphicsCommandList4* CreateCommandList(ID3D12CommandAllocator* allocator);


private:
	struct CommandAllocatorEntry
	{
		uint64_t fenceValue;
		ID3D12CommandAllocator* commandAllocator;
	};

	using CommandAllocatorQueue = std::queue<CommandAllocatorEntry>;
	using CommandListQueue = std::queue<ID3D12GraphicsCommandList4*>;

	D3D12_COMMAND_LIST_TYPE _commandListType;
	ID3D12Device5* _device;
	
	ID3D12CommandQueue* _commandQueue;
	ID3D12Fence* _fence;
	HANDLE _fenceEvent;
	uint64_t _fenceValue;

	CommandAllocatorQueue _commandAllocatorQueue;
	CommandListQueue _commandListQueue;
};

#endif // !__COMMANDQUEUE_H__