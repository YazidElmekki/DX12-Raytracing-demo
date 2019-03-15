#ifndef __GRAPHICDEVICE_H_INCLUDED__
#define __GRAPHICDEVICE_H_INCLUDED__

#include "DirectXRHI.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float CAMERA_FAR = 1000.0f;
const float CAMERA_NEAR = 0.1f;

class GraphicDevice
{
public:
	GraphicDevice() = default;
	GraphicDevice(const GraphicDevice&) = delete;
	GraphicDevice(GraphicDevice&&) = delete;
	~GraphicDevice() = default;


	bool Initialize(int width, int height, HWND hwnd);
	void Shutdown();

	bool Frame();
	
	void Resize(uint32_t width, uint32_t height);

private:
	bool Render();

	DirectXRHI* _directXRHI = nullptr;
};

#endif // !__GRAPHICDEVICE_H_INCLUDED__