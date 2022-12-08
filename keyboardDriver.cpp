#include "pch.h"
#include "keyboardDriver.h"
#include "msdk.h"
#include "usbhidkeycode.h"
#include "GameData.h"

HANDLE MSDK_Open(int pid) 
{
	return Ap5isNXMcaWr(pid);
}

// 获取按键状态: -1失败 0弹起状态 1按下状态
int MSDK_KeyState(int code)
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return -1;
	}

	return rFpti6KthJ(GLOBAL.MSDK_HANDLE, code);
}

// 按下某个按键
void MSDK_KeyDown(int code)
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	if (rFpti6KthJ(GLOBAL.MSDK_HANDLE, code) == 0) {
		rdpWSY0nFv(GLOBAL.MSDK_HANDLE, code);
	}
}

// 弹起某个按键
void MSDK_KeyUp(int code)
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	if (rFpti6KthJ(GLOBAL.MSDK_HANDLE, code) == 1) {
		oihUU7DIfVcX(GLOBAL.MSDK_HANDLE, code);
	}
}

// 弹起所有按键
void MSDK_ReleaseAllKey()
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	Na8FeYnvveiZ(GLOBAL.MSDK_HANDLE);
}

// 按键以及次数
void MSDK_keyPress(int code, int repeats)
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	nnN3A2qfth(GLOBAL.MSDK_HANDLE, code, repeats);
}


// 随机延迟
void MSDK_DelayRandom(int min, int max)
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	YYyWL2SSFlkB(min, max);
}

void closeRunKey()
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	if (rFpti6KthJ(GLOBAL.MSDK_HANDLE, Keyboard_UpArrow) == 1) {
		oihUU7DIfVcX(GLOBAL.MSDK_HANDLE, Keyboard_UpArrow);
	}

	if (rFpti6KthJ(GLOBAL.MSDK_HANDLE, Keyboard_RightArrow) == 1) {
		oihUU7DIfVcX(GLOBAL.MSDK_HANDLE, Keyboard_RightArrow);
	}

	if (rFpti6KthJ(GLOBAL.MSDK_HANDLE, Keyboard_DownArrow) == 1) {
		oihUU7DIfVcX(GLOBAL.MSDK_HANDLE, Keyboard_DownArrow);
	}

	if (rFpti6KthJ(GLOBAL.MSDK_HANDLE, Keyboard_LeftArrow) == 1) {
		oihUU7DIfVcX(GLOBAL.MSDK_HANDLE, Keyboard_LeftArrow);
	}
}