#include "CommandQueue.h"

#include "DirectXRHI.h"

#include <cassert>


CommandQueue::CommandQueue(ID3D12Device5* device, D3D12_COMMAND_LIST_TYPE type)
	: _fenceValue(0)
	, _commandListType(type)
	, _device(device)
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = type;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	DirectXRHI::ThrowIfFailed(_device->CreateCommandQueue(&desc ,IID_PPV_ARGS(&_commandQueue)));
	DirectXRHI::ThrowIfFailed(_device->CreateFence(_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));

	_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(_fenceEvent && "FAILED TO CREATE FENCE EVENT HANDLE");
}

CommandQueue::~CommandQueue()
{
	if (_commandQueue)
	{
		_commandQueue->Release();
		_commandQueue = nullptr;
	}

	if (_fence)
	{
		_fence->Release();
		_fence = nullptr;
	}
}

ID3D12CommandAllocator* CommandQueue::CreateCommandAllocator()
{
	ID3D12CommandAllocator* commandAllocator;
	DirectXRHI::ThrowIfFailed(_device->CreateCommandAllocator(_commandListType, IID_PPV_ARGS(&commandAllocator)));

	return commandAllocator;
}

ID3D12GraphicsCommandList4* CommandQueue::CreateCommandList(ID3D12CommandAllocator* allocator)
{
	ID3D12GraphicsCommandList4* commandList;
	DirectXRHI::ThrowIfFailed(_device->CreateCommandList(0, _commandListType, allocator, nullptr, IID_PPV_ARGS(&commandList)));

	return commandList;
}

ID3D12GraphicsCommandList4* CommandQueue::GetCommandList()
{
	ID3D12CommandAllocator* commandAllocator;
	ID3D12GraphicsCommandList4* commandList;

	if (!_commandAllocatorQueue.empty() && IsFenceComplete(_commandAllocatorQueue.front().fenceValue))
	{
		commandAllocator = _commandAllocatorQueue.front().commandAllocator;
		_commandAllocatorQueue.pop();

		DirectXRHI::ThrowIfFailed(commandAllocator->Reset());
	}
	else
	{
		commandAllocator = CreateCommandAllocator();
	}

	if (_commandListQueue.empty())
	{
		commandList = _commandListQueue.front();
		_commandListQueue.pop();

		DirectXRHI::ThrowIfFailed(commandList->Reset(commandAllocator, nullptr));
	}
	else
	{
		commandList = CreateCommandList(commandAllocator);
	}

	commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator);

	return commandList;
}

uint64_t CommandQueue::ExecuteCommandList(ID3D12GraphicsCommandList4* commandList)
{
	commandList->Close();

	ID3D12CommandAllocator* commandAllocator;
	UINT dataSize = sizeof(commandAllocator);
	DirectXRHI::ThrowIfFailed(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator));

	ID3D12CommandList* const commandsLists[] = { commandList };

	_commandQueue->ExecuteCommandLists(1, commandsLists);
	uint64_t fenceValue = Signal();

	_commandAllocatorQueue.emplace(CommandAllocatorEntry{ fenceValue, commandAllocator });
	_commandListQueue.push(commandList);

	commandAllocator->Release();

	return fenceValue;
}

ID3D12CommandQueue* CommandQueue::GetD3D12CommandQueue() const
{
	return _commandQueue;
}

uint64_t CommandQueue::Signal()
{
	uint64_t fenceValueForSignal = ++_fenceValue;
	DirectXRHI::ThrowIfFailed(_commandQueue->Signal(_fence, fenceValueForSignal));

	return fenceValueForSignal;
}

bool CommandQueue::IsFenceComplete(uint64_t fenceValue)
{
	if (_fence->GetCompletedValue() >= fenceValue)
		return true;

	return false;
}

void CommandQueue::WaitForFenceValue(uint64_t fenceValue)
{
	if (!IsFenceComplete(fenceValue))
	{
		DirectXRHI::ThrowIfFailed(_fence->SetEventOnCompletion(fenceValue, _fenceEvent));
		WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

void CommandQueue::Flush()
{
	uint64_t fenceValueForSignal = Signal();
	WaitForFenceValue(fenceValueForSignal);
}