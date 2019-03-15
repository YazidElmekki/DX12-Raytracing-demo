#include "DirectXRHI.h"

#include <algorithm>

#undef max

bool DirectXRHI::Initialize(uint32_t width, uint32_t height, HWND hwnd, bool fullscreen, bool enableVsync, float cameraNear, float cameraFar)
{
	_vsyncEnabled = enableVsync;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_1;

	//TODO : CHOOSE THE VIDEO CARD COMPATIBLE WITH FEATURE LEVEL 12_1
	bool result = D3D12CreateDevice(NULL, featureLevel, __uuidof(ID3D12Device), (void**)&_device);

	if (FAILED(result))
	{
		MessageBox(hwnd, L"Failed to create D3D12, video card doesn't support DirectX 12.1", L"DirectX Device Failure", MB_OK);
		return false;
	}

	result = D3D12GetDebugInterface(IID_PPV_ARGS(&_debugInterface));

	if (FAILED(result))
	{
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	ZeroMemory(&commandQueueDesc, sizeof(commandQueueDesc));

	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;

	result = _device->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&_commandQueue);

	if (FAILED(result))
	{
		return false;
	}

	IDXGIFactory7* factory = nullptr;
	IDXGIAdapter* adapter = nullptr;
	IDXGIOutput* adapterOutput = nullptr;
	unsigned int numModes = 0;
	DXGI_MODE_DESC* displayModeList = nullptr;

	result = CreateDXGIFactory2(0, __uuidof(IDXGIFactory7), (void**)&factory);

	if (FAILED(result))
	{
		return false;
	}

	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	result = adapter->EnumOutputs(0, &adapterOutput);

	if (FAILED(result))
	{
		return false;
	}

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);

	if (FAILED(result))
	{
		return false;
	}

	unsigned int numeratorRefreshRate = 1;
	unsigned int denominatorRefreshRate = 1;

	for (unsigned int i = 0; i < numModes; ++i)
	{
		if (displayModeList[i].Width == (unsigned int)width && displayModeList[i].Height == height)
		{
			numeratorRefreshRate = displayModeList[i].RefreshRate.Numerator;
			denominatorRefreshRate = displayModeList[i].RefreshRate.Denominator;
		}
	}

	DXGI_ADAPTER_DESC adapterDesc;
	result = adapter->GetDesc(&adapterDesc);

	if (FAILED(result))
	{
		return false;
	}

	_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	size_t stringLength = 0;
	int error = wcstombs_s(&stringLength, _videoCardDescription, sizeof(_videoCardDescription), adapterDesc.Description, sizeof(_videoCardDescription));
	if (error != 0)
	{
		return false;
	}

	delete[] displayModeList;
	displayModeList = nullptr;

	adapterOutput->Release();
	adapterOutput = nullptr;

	adapter->Release();
	adapter = nullptr;

	_currentFramebufferWidth = width;
	_currentFramebufferHeight = height;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 2;

	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;

	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow = hwnd;

	swapChainDesc.Windowed = !fullscreen;

	swapChainDesc.BufferDesc.RefreshRate.Numerator = _vsyncEnabled ? numeratorRefreshRate : 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = _vsyncEnabled ? denominatorRefreshRate : 0;

	//Multisampling
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.Flags = 0;

	IDXGISwapChain* swapChain = nullptr;
	result = factory->CreateSwapChain(_commandQueue, &swapChainDesc, &swapChain);
	if (FAILED(result))
	{
		return false;
	}

	result = swapChain->QueryInterface(__uuidof(IDXGISwapChain4), (void**)&_swapChain);

	if (FAILED(result))
	{
		return false;
	}

	swapChain = nullptr;
	factory->Release();
	factory = nullptr;

	D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc;
	ZeroMemory(&renderTargetViewHeapDesc, sizeof(renderTargetViewHeapDesc));

	renderTargetViewHeapDesc.NumDescriptors = 2;
	renderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	renderTargetViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	result = _device->CreateDescriptorHeap(&renderTargetViewHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&_renderTargetHeap);

	if (FAILED(result))
	{
		return false;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle = _renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
	unsigned int renderTargetViewDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	
	result = _swapChain->GetBuffer(0, __uuidof(ID3D12Resource), (void**)&_frameBuffers[0]);

	if (FAILED(result))
	{
		return false;
	}

	_device->CreateRenderTargetView(_frameBuffers[0], NULL, renderTargetViewHandle);
	renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;

	result = _swapChain->GetBuffer(1, __uuidof(ID3D12Resource), (void**)&_frameBuffers[1]);

	if (FAILED(result))
	{
		return false;
	}

	_device->CreateRenderTargetView(_frameBuffers[1], NULL, renderTargetViewHandle);

	_currentFramebufferIndex = _swapChain->GetCurrentBackBufferIndex();

	result = _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&_commandAllocator);
	if (FAILED(result))
	{
		return false;
	}

	result = _device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&_commandList);

	if (FAILED(result))
	{
		return false;
	}

	result = _commandList->Close();

	if (FAILED(result))
	{
		return false;
	}

	result = _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&_fence);
	if (FAILED(result))
	{
		return false;
	}

	_fenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
	if (_fenceEvent == NULL)
	{
		return false;
	}

	_fenceValue = 1;

	return true;
}

void DirectXRHI::Shutdown()
{
	if (_swapChain)
	{
		_swapChain->SetFullscreenState(false, NULL);
	}

	CloseHandle(_fenceEvent);

	if (_debugInterface)
	{
		_debugInterface->Release();
		_debugInterface = nullptr;
	}

	if (_fence)
	{
		_fence->Release();
		_fence = nullptr;
	}

	if (_pipelineState)
	{
		_pipelineState->Release();
		_pipelineState = nullptr;
	}

	if (_commandList)
	{
		_commandList->Release();
		_commandList = nullptr;
	}

	if (_commandAllocator)
	{
		_commandAllocator->Release();
		_commandAllocator = nullptr;
	}

	if (_frameBuffers[0])
	{
		_frameBuffers[0]->Release();
		_frameBuffers[0] = nullptr;
	}
	if (_frameBuffers[1])
	{
		_frameBuffers[1]->Release();
		_frameBuffers[1] = nullptr;
	}

	if (_renderTargetHeap)
	{
		_renderTargetHeap->Release();
		_renderTargetHeap = nullptr;
	}

	if (_swapChain)
	{
		_swapChain->Release();
		_swapChain = nullptr;
	}

	if (_commandQueue)
	{
		_commandQueue->Release();
		_commandQueue = nullptr;
	}

	if (_device)
	{
		_device->Release();
		_device = nullptr;
	}
}

bool DirectXRHI::Render()
{
	HRESULT result = _commandAllocator->Reset();

	if (FAILED(result))
	{
		return false;
	}

	result = _commandList->Reset(_commandAllocator, _pipelineState);

	if (FAILED(result))
	{
		return false;
	}

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = _frameBuffers[_currentFramebufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	_commandList->ResourceBarrier(1, &barrier);

	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle = _renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
	unsigned int renderTargetViewHandleSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	if (_currentFramebufferIndex == 1)
	{
		renderTargetViewHandle.ptr += renderTargetViewHandleSize;
	}

	_commandList->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, NULL);

	float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	_commandList->ClearRenderTargetView(renderTargetViewHandle, color, 0, NULL);

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	_commandList->ResourceBarrier(1, &barrier);

	result = _commandList->Close();

	if (FAILED(result))
	{
		return false;
	}

	ID3D12CommandList* commandLists[1] = { _commandList };
	_commandQueue->ExecuteCommandLists(1, commandLists);

	if (_vsyncEnabled)
	{
		result = _swapChain->Present(1, 0);
		if (FAILED(result))
		{
			return false;
		}
	}
	else
	{
		result = _swapChain->Present(0, 0);
		if (FAILED(result))
		{
			return false;
		}
	}

	UINT64 fenceToWaitFor = _fenceValue;

	_frameFenceValues[_currentFramebufferIndex] = Signal(_commandQueue, _fence, _fenceValue);
	
	WaitForFenceValue(_fence, _frameFenceValues[_currentFramebufferIndex], _fenceEvent);

	_currentFramebufferIndex = (_currentFramebufferIndex + 1) % 2;

	return true;
}

void DirectXRHI::Resize(uint32_t width, uint32_t height)
{
	if (width != _currentFramebufferWidth || height != _currentFramebufferHeight)
	{
		_currentFramebufferWidth = std::max(1u, width);
		_currentFramebufferHeight = std::max(1u, height);

		Flush(_commandQueue, _fence, _fenceValue, _fenceEvent);

		for (int i = 0; i < 2; ++i)
		{
			_frameBuffers[i]->Release();
			_frameFenceValues[i] = _frameFenceValues[_currentFramebufferIndex];
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		ThrowIfFailed(_swapChain->GetDesc(&swapChainDesc));
		ThrowIfFailed(_swapChain->ResizeBuffers(2, _currentFramebufferWidth, _currentFramebufferHeight, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));
		_currentFramebufferIndex = _swapChain->GetCurrentBackBufferIndex();

		UpdateFramebuffers();
	}
}

void DirectXRHI::UpdateFramebuffers()
{
	auto rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHande(_renderTargetHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < 2; ++i)
	{
		ID3D12Resource* frameBuffer;
		ThrowIfFailed(_swapChain->GetBuffer(i, IID_PPV_ARGS(&frameBuffer)));

		_device->CreateRenderTargetView(frameBuffer, nullptr, rtvHande);

		_frameBuffers[i] = frameBuffer;

		rtvHande.Offset(rtvDescriptorSize);
	}
}

void DirectXRHI::Flush(ID3D12CommandQueue* commandQueue, ID3D12Fence* fence, uint64_t& fenceValue, HANDLE fenceEvent)
{
	uint64_t fenceValueForSignal = Signal(commandQueue, _fence, fenceValue);
	WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
}

uint64_t DirectXRHI::Signal(ID3D12CommandQueue* commandQueue, ID3D12Fence* fence, uint64_t& fenceValue)
{
	uint64_t fenceValueForSignal = ++fenceValue;
	ThrowIfFailed(commandQueue->Signal(fence, fenceValueForSignal));

	return fenceValueForSignal;
}

void DirectXRHI::WaitForFenceValue(ID3D12Fence* fence, uint64_t fenceValue, HANDLE fenceEvent)
{
	if (fence->GetCompletedValue() < fenceValue)
	{
		ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}