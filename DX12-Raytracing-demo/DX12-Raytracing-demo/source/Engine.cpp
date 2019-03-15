#include "Engine.h"

bool Engine::Initialize()
{
	int screenWidth = 0;
	int screenHeight = 0;

	InitializeWindows(screenWidth, screenHeight);

	_input = new Input;
	if (!_input)
	{
		return false;
	}

	_input->Initialize();

	_graphics = new GraphicDevice;

	if (!_graphics)
	{
		return false;
	}

	bool result = _graphics->Initialize(screenWidth, screenHeight, _hwnd);

	return result;
}

void Engine::Shutdown()
{
	if (_graphics)
	{
		_graphics->Shutdown();
		delete _graphics;
		_graphics = nullptr;
	}

	if (_input)
	{
		delete _input;
		_input = nullptr;
	}

	ShutdownWindows();
}

void Engine::Run()
{
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	bool done = false;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			OnUpdate();

			bool result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}
}

bool Engine::Frame()
{
	if (_input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	bool result = _graphics->Frame();

	if (result)
	{
		OnUpdate();
	}

	return result;
}

LRESULT CALLBACK Engine::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		case WM_KEYDOWN:
		{
			_input->KeyDown((unsigned int)wparam);
			OnKeyPress((unsigned int)wparam);
			return 0;
		}
		case WM_KEYUP:
		{
			_input->KeyUp((unsigned int)wparam);
			OnKeyPress((unsigned int)wparam);
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			_input->MouseDown((unsigned int)wparam);
			OnMouseButtonPress((unsigned int)wparam);
			return 0;
		}
		case WM_MBUTTONUP:
		{
			_input->MouseUp((unsigned int)wparam);
			OnMouseButtonPress((unsigned int)wparam);
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			LPPOINT point;
			GetCursorPos(point);
			_input->SetMousePos(point->x, point->y);
			OnMouseMove();
			return 0;
		}
		case WM_MOUSEHWHEEL:
		{
			OnMouseWheel(WHEEL_DELTA > 0 ? 1 : -1);
			return 0;
		}
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

void Engine::InitializeWindows(int& width, int& height)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX;
	int posY;


	ApplicationHandle = this;

	_hInstance = GetModuleHandle(NULL);

	_applicationName = L"DX12-Raytracing-Demo";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPanningWidth = (unsigned long)width;
		dmScreenSettings.dmPanningHeight = (unsigned long)height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		width = 800;
		height = 600;

		posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	}

	_hwnd = CreateWindowEx(WS_EX_APPWINDOW, _applicationName, _applicationName, WS_TILEDWINDOW | WS_BORDER | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION, posX, posY, width, height, NULL, NULL, _hInstance, NULL);

	ShowWindow(_hwnd, SW_SHOW);
	SetForegroundWindow(_hwnd);
	SetFocus(_hwnd);
}

void Engine::ShutdownWindows()
{
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(_hwnd);
	_hwnd = NULL;

	UnregisterClass(_applicationName, _hInstance);
	_hInstance = NULL;

	ApplicationHandle = NULL;
}

void Engine::Resize(uint32_t width, uint32_t height)
{
	if (_graphics)
	{
		_graphics->Resize(width, height);
		OnResize(width, height);
	}
}

void Engine::GetWindowSize(uint32_t& width, uint32_t& height) const
{
	RECT rect;
	GetWindowRect(_hwnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		case WM_DESTROY:
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_SIZE:
			RECT rect;
			GetWindowRect(hwnd, &rect);
			ApplicationHandle->Resize(rect.right-rect.left,rect.bottom-rect.top);
			break;

		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}