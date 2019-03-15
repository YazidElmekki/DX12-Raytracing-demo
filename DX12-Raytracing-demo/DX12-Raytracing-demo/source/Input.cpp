#include "Input.h"

void Input::Initialize()
{
	for (int i = 0; i < 256; ++i)
	{
		_keys[i] = false;
	}
}

void Input::KeyDown(unsigned int key)
{
	_keys[key] = true;
}

void Input::KeyUp(unsigned int key)
{
	_keys[key] = false;
}

bool Input::IsKeyDown(unsigned int key)
{
	return _keys[key];
}

void Input::MouseDown(unsigned int mouseButton)
{
	if (mouseButton == 1)
	{
		_mouseButtons[0] = true;
	}
	else if (mouseButton == 2)
	{
		_mouseButtons[1] = true;
	}
	else if (mouseButton == 16)
	{
		_mouseButtons[2] = true;
	}
}

void Input::MouseUp(unsigned int mouseButton)
{
	if (mouseButton == 1)
	{
		_mouseButtons[0] = false;
	}
	else if (mouseButton == 2)
	{
		_mouseButtons[1] = false;
	}
	else if (mouseButton == 16)
	{
		_mouseButtons[2] = false;
	}
}