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