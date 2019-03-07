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

	~Engine() = default;

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

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