#include "InputClass.h"

void InputClass::Initialize()
{
	for (int i = 0; i < 256; ++i)
	{
		_keys[i] = false;
	}
}

void InputClass::KeyDown(unsigned int key)
{
	_keys[key] = true;
}

void InputClass::KeyUp(unsigned int key)
{
	_keys[key] = false;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	return _keys[key];
}