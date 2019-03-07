#ifndef __GRAPHICDEVICE_H_INCLUDED__
#define __GRAPHICDEVICE_H_INCLUDED__

#include <Windows.h>

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const bool SCREEN_DEPTH = 1000.0f;
const bool SCREEN_NEAR = 0.1f;

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

private:
	bool Render();
};

#endif // !__GRAPHICDEVICE_H_INCLUDED__