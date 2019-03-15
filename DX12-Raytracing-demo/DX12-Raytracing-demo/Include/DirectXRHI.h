#ifndef __DIRECTXRHI_H__
#define __DIRECTXRHI_H__

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>

#include <stdint.h>
#include <exception>

class DirectXRHI
{
public:
	DirectXRHI() = default;
	DirectXRHI(const DirectXRHI&) = delete;
	DirectXRHI(DirectXRHI&&) = delete;
	~DirectXRHI() = default;

	bool Initialize(uint32_t screenWidth, uint32_t screenHeight, HWND hwnd, bool fullscreen, bool enableVSync, float cameraFar, float cameraNear);

	void Shutdown();

	bool Render();

	void Resize(uint32_t width, uint32_t height);

	static inline void ThrowIfFailed(HRESULT result)
	{
		if (FAILED(result))
		{
			throw std::exception();
		}
	}


private:
	void Flush(ID3D12CommandQueue* commandQueue, ID3D12Fence* fence, uint64_t& fenceValue, HANDLE fenceEvent);
	uint64_t Signal(ID3D12CommandQueue* commandQueue, ID3D12Fence* fence, uint64_t& fenceValue);
	void WaitForFenceValue(ID3D12Fence* fence, uint64_t fenceValue, HANDLE fenceEvent);
	void UpdateFramebuffers();


private:
	bool _vsyncEnabled = false;
	ID3D12Device5* _device = nullptr;
	ID3D12CommandQueue* _commandQueue = nullptr;
	char _videoCardDescription[128];
	IDXGISwapChain4* _swapChain = nullptr;
	uint32_t _currentFramebufferWidth = 0;
	uint32_t _currentFramebufferHeight = 0;

	ID3D12DescriptorHeap* _renderTargetHeap;
	ID3D12Resource* _frameBuffers[2] = { nullptr, nullptr };
	unsigned int _currentFramebufferIndex = 0;
	ID3D12CommandAllocator* _commandAllocator = nullptr;
	ID3D12GraphicsCommandList* _commandList = nullptr;
	ID3D12PipelineState* _pipelineState = nullptr;
	ID3D12Fence* _fence;
	HANDLE _fenceEvent;
	uint64_t _fenceValue;
	uint64_t _frameFenceValues[2] = {0,0};

	int _videoCardMemory;

	ID3D12Debug* _debugInterface = nullptr;
};

#endif // !__DIRECTXRHI_H__