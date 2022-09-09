#include "pch.h"
#include "keyboardDriver.h"
#include "msdk.h"

// 获取按键状态: -1失败 0弹起状态 1按下状态
int MSDK_KeyState(int code)
{
	if (!MSDK_HANDLE || !window_top) {
		return -1;
	}

	return M_KeyState(MSDK_HANDLE, code);
}

// 按下某个按键
void MSDK_KeyDown(int code)
{
	if (!MSDK_HANDLE || !window_top) {
		return;
	}

	if (M_KeyState(MSDK_HANDLE, code) == 0) {
		M_KeyDown(MSDK_HANDLE, code);
	}
}

// 弹起某个按键
void MSDK_KeyUp(int code)
{
	if (!MSDK_HANDLE || !window_top) {
		return;
	}

	if (M_KeyState(MSDK_HANDLE, code) == 1) {
		M_KeyUp(MSDK_HANDLE, code);
	}
}

// 弹起所有按键
void MSDK_ReleaseAllKey()
{
	if (!MSDK_HANDLE || !window_top) {
		return;
	}

	M_ReleaseAllKey(MSDK_HANDLE);
}

// 按键以及次数
void MSDK_keyPress(int code, int repeats)
{
	if (!MSDK_HANDLE || !window_top) {
		return;
	}

	M_KeyPress(MSDK_HANDLE, code, repeats);
}


// 随机延迟
void MSDK_DelayRandom(int min, int max)
{
	if (!MSDK_HANDLE || !window_top) {
		return;
	}

	M_DelayRandom(min, max);
}