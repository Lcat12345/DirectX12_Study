#include "pch.h"
#include "Input.h"

void Input::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
}

void Input::Update()
{
	HWND hwnd = GetActiveWindow();

	if (_hwnd != hwnd)
	{
		for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
			_states[key] = KEY_STATE::NONE;

		return;
	}
	
	for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
	{
		KEY_STATE& state = _states[key];
		// Ű�� �Է��� �Ǿ����ϴ�.
		if (GetAsyncKeyState(key) & 0x8000)
		{			
			// �������� �����ֽ��ϴ�.
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			// �������� �������� �ʾҽ��ϴ�. ó�� ����
			else
				state = KEY_STATE::DOWN;
		}
		// Ű �Է��� �����ϴ�.
		else
		{
			// ������ �������� ���Ƚ��ϴ�.
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			// �������� �������ϴ�.
			else
				state = KEY_STATE::NONE;

		}
	}

}
