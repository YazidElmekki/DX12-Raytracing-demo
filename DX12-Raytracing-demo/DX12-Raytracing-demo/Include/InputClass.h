#ifndef __INPUTCLASS_H__
#define __INPUTCLASS_H__

class InputClass
{
public:
	InputClass() = default;
	InputClass(const InputClass&) = delete;
	InputClass(InputClass&&) = delete;
	~InputClass() = default;

	void Initialize();

	void KeyDown(unsigned int key);
	void KeyUp(unsigned int key);
	
	bool IsKeyDown(unsigned int);

private:
	bool _keys[256];
};

#endif // !__INPUTCLASS_H__