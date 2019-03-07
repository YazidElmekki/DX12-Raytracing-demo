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
	
	bool IsKeyDown(unsigned int);

private:
	bool _keys[256];
};

#endif // !__INPUTCLASS_H__