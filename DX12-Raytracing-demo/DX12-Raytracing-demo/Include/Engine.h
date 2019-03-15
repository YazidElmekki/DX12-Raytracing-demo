#ifndef __ENGINE_H__
#define __ENGINE_h__

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include "Input.h"
#include "GraphicDevice.h"


class Engine
{
public:
	Engine() = default;
	Engine(const Engine&) = delete;
	Engine(Engine&&) = delete;

	virtual ~Engine() = default;

	virtual bool Initialize();
	virtual void Shutdown();
	void Run();

	virtual void OnUpdate() {};
	virtual void OnRender() {};
	virtual void OnMouseMove() {};
	virtual void OnKeyPress(unsigned int key) {};
	virtual void OnKeyUp(unsigned int key) {};
	virtual void OnMouseButtonPress(unsigned int mouseButton) {};
	virtual void OnMouseButtonUp(unsigned int mouseButton) {};
	virtual void OnResize(uint32_t width, uint32_t height) {};
	virtual void OnMouseWheel(int32_t dir) {};

	virtual bool LoadContent() {};
	virtual void UnloadContent() {};


	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	virtual void Resize(uint32_t width, uint32_t height);

	void GetWindowSize(uint32_t& width, uint32_t& height) const;
	
	Input* GetInput() const { return _input; }

private:
	bool Frame();
	void InitializeWindows(int& width, int& height);
	void ShutdownWindows();

	LPCWSTR _applicationName;
	HINSTANCE _hInstance;
	HWND _hwnd;

	Input* _input = nullptr;
	GraphicDevice* _graphics = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static Engine* ApplicationHandle = 0;

#endif // !__SYSTEMCLASS_H__