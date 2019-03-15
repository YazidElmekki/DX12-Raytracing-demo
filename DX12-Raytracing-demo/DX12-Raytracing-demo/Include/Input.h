#ifndef __INPUT_H__
#define __INPUT_H__

class Input
{
public:
	Input() = default;
	Input(const Input&) = delete;
	Input(Input&&) = delete;
	~Input() = default;

	void Initialize();

	void KeyDown(unsigned int key);
	void KeyUp(unsigned int key);
	
	void MouseUp(unsigned int mouseButton);
	void MouseDown(unsigned int mouseButton);

	bool IsKeyDown(unsigned int);

	void SetMousePos(int x, int y)
	{
		_mouseX = x;
		_mouseY = y;
	}

private:
	bool _keys[256];
	bool _mouseButtons[3];
	int _mouseX;
	int _mouseY;
};

#endif // !__INPUTCLASS_H__