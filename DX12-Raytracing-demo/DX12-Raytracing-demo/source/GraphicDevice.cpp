#include "GraphicDevice.h"

bool GraphicDevice::Initialize(int width, int height, HWND hwnd)
{
	_directXRHI = new DirectXRHI;
	if (!_directXRHI)
	{
		return false;
	}

	bool result = _directXRHI->Initialize(width, height, hwnd, FULL_SCREEN, VSYNC_ENABLED, CAMERA_FAR, CAMERA_NEAR);

	if (!result)
	{
		MessageBox(hwnd, L"Failed to initialize DirectX RHI", L"Error" , MB_OK);
		return false;
	}

	return true;
}

void GraphicDevice::Shutdown()
{
	if (_directXRHI)
	{
		_directXRHI->Shutdown();
		delete _directXRHI;
		_directXRHI = nullptr;
	}
}

bool GraphicDevice::Frame()
{
	return Render();
}

bool GraphicDevice::Render()
{
	if (_directXRHI)
	{
		return _directXRHI->Render();
	}
	return false;
}

void GraphicDevice::Resize(uint32_t width, uint32_t height)
{
	if (_directXRHI)
	{
		_directXRHI->Resize(width, height);
	}
}