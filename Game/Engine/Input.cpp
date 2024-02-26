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
		// 키가 입력이 되었습니다.
		if (GetAsyncKeyState(key) & 0x8000)
		{			
			// 이전에도 눌려있습니다.
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			// 이전에는 눌려있지 않았습니다. 처음 눌림
			else
				state = KEY_STATE::DOWN;
		}
		// 키 입력이 없습니다.
		else
		{
			// 하지만 이전에는 눌렸습니다.
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			// 이전에도 없었습니다.
			else
				state = KEY_STATE::NONE;

		}
	}

}
