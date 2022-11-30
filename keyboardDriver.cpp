#include "pch.h"
#include "keyboardDriver.h"
#include "msdk.h"
#include "usbhidkeycode.h"
#include "GameData.h"

// 获取按键状态: -1失败 0弹起状态 1按下状态
int MSDK_KeyState(int code)
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return -1;
	}

	return M_KeyState(GLOBAL.MSDK_HANDLE, code);
}

// 按下某个按键
void MSDK_KeyDown(int code)
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	if (M_KeyState(GLOBAL.MSDK_HANDLE, code) == 0) {
		M_KeyDown(GLOBAL.MSDK_HANDLE, code);
	}
}

// 弹起某个按键
void MSDK_KeyUp(int code)
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	if (M_KeyState(GLOBAL.MSDK_HANDLE, code) == 1) {
		M_KeyUp(GLOBAL.MSDK_HANDLE, code);
	}
}

// 弹起所有按键
void MSDK_ReleaseAllKey()
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	M_ReleaseAllKey(GLOBAL.MSDK_HANDLE);
}

// 按键以及次数
void MSDK_keyPress(int code, int repeats)
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	M_KeyPress(GLOBAL.MSDK_HANDLE, code, repeats);
}


// 随机延迟
void MSDK_DelayRandom(int min, int max)
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	M_DelayRandom(min, max);
}

void closeRunKey()
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	if (M_KeyState(GLOBAL.MSDK_HANDLE, Keyboard_UpArrow) == 1) {
		M_KeyUp(GLOBAL.MSDK_HANDLE, Keyboard_UpArrow);
	}

	if (M_KeyState(GLOBAL.MSDK_HANDLE, Keyboard_RightArrow) == 1) {
		M_KeyUp(GLOBAL.MSDK_HANDLE, Keyboard_RightArrow);
	}

	if (M_KeyState(GLOBAL.MSDK_HANDLE, Keyboard_DownArrow) == 1) {
		M_KeyUp(GLOBAL.MSDK_HANDLE, Keyboard_DownArrow);
	}

	if (M_KeyState(GLOBAL.MSDK_HANDLE, Keyboard_LeftArrow) == 1) {
		M_KeyUp(GLOBAL.MSDK_HANDLE, Keyboard_LeftArrow);
	}
}