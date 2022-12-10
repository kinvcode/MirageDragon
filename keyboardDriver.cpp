#include "pch.h"
#include "keyboardDriver.h"
#include "msdk.h"
#include "usbhidkeycode.h"
#include "GameData.h"

HANDLE MSDK_Open(int pid)
{
	return Ap5isNXMcaWr(pid);
}

int MSDK_ResolutionUsed(int x, int y)
{
	return qEPoFZf2uldH(GLOBAL.MSDK_HANDLE, x, y);
}

int MSDK_GetMouseState(int code)
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return -1;
	}
	return kLNkJH89D(GLOBAL.MSDK_HANDLE, code);
}

void MSDK_LeftClick()
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	eEXShIvePH(GLOBAL.MSDK_HANDLE, 1);
}

void MSDK_LeftDown()
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	SndzmqrfS6YH(GLOBAL.MSDK_HANDLE);
}

void MSDK_LeftUp()
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	EuWssRuD2(GLOBAL.MSDK_HANDLE);
}

void MSDK_RightClick()
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}
	bz4u95VFqV(GLOBAL.MSDK_HANDLE, 1);
}

void MSDK_RightDown()
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	Z5edmlUJUUn(GLOBAL.MSDK_HANDLE);
}

void MSDK_RightUp()
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	Q4q22sHu8g(GLOBAL.MSDK_HANDLE);
}

void MSDK_ReleaseAllMouse()
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	Rb9Lzwm1DQ(GLOBAL.MSDK_HANDLE);
}

void MSDK_MouseWheel(int num)
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}
	ghBhQngjohp(GLOBAL.MSDK_HANDLE, num);
}

void MSDK_ResetMousePos()
{
	sYgPZrEaaNs(GLOBAL.MSDK_HANDLE);
}

void MSDK_MoveR(int x, int y)
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}
	sIH622mf7b(GLOBAL.MSDK_HANDLE, x, y);
}

void MSDK_MoveTo(int x, int y)
{
	if (!GLOBAL.MSDK_HANDLE || !GLOBAL.window_top) {
		return;
	}

	UVj1USQTjVH(GLOBAL.MSDK_HANDLE, x, y);
}

//ROOMCOOR MSDK_GetPos()
//{
//	int x, y;
//	HrsNUDjFBO(GLOBAL.MSDK_HANDLE, &x, &y);
//	ROOMCOOR coor = { x,y };
//	return coor;
//}

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