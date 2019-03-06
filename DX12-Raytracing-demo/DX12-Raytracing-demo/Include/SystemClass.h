#ifndef __SYSTEMCLASS_H__
#define __SYSTEMCLASS_h__

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include "InputClass.h"
#include "GraphicsClass.h"


class SystemClass
{
public:
	SystemClass() = default;
	SystemClass(const SystemClass&) = delete;
	SystemClass(SystemClass&&) = delete;

	~SystemClass() = default;

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

	InputClass* _input = nullptr;
	GraphicsClass* _graphics = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* ApplicationHandle = 0;

#endif // !__SYSTEMCLASS_H__