#include "pch.h"
#include "keyboardDriver.h"
#include "msdk.h"
#include "usbhidkeycode.h"
#include "GameData.h"
#include "procData.h"

HANDLE MSDK_Open(int pid)
{
	return Ap5isNXMcaWr(pid);
}

int MSDK_ResolutionUsed(int x, int y)
{
	return qEPoFZf2uldH(PDATA.MSDK_HANDLE, x, y);
}

int MSDK_GetMouseState(int code)
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return -1;
	}
	return kLNkJH89D(PDATA.MSDK_HANDLE, code);
}

void MSDK_LeftClick()
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return;
	}

	eEXShIvePH(PDATA.MSDK_HANDLE, 1);
}

void MSDK_LeftDown()
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return;
	}

	SndzmqrfS6YH(PDATA.MSDK_HANDLE);
}

void MSDK_LeftUp()
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return;
	}

	EuWssRuD2(PDATA.MSDK_HANDLE);
}

void MSDK_RightClick()
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return;
	}
	bz4u95VFqV(PDATA.MSDK_HANDLE, 1);
}

void MSDK_RightDown()
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return;
	}

	Z5edmlUJUUn(PDATA.MSDK_HANDLE);
}

void MSDK_RightUp()
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return;
	}

	Q4q22sHu8g(PDATA.MSDK_HANDLE);
}

void MSDK_ReleaseAllMouse()
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return;
	}

	Rb9Lzwm1DQ(PDATA.MSDK_HANDLE);
}

void MSDK_MouseWheel(int num)
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return;
	}
	ghBhQngjohp(PDATA.MSDK_HANDLE, num);
}

void MSDK_ResetMousePos()
{
	sYgPZrEaaNs(PDATA.MSDK_HANDLE);
}

void MSDK_MoveR(int x, int y)
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return;
	}
	sIH622mf7b(PDATA.MSDK_HANDLE, x, y);
}

void MSDK_MoveTo(int x, int y)
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return;
	}

	UVj1USQTjVH(PDATA.MSDK_HANDLE, x, y);
}

//ROOMCOOR MSDK_GetPos()
//{
//	int x, y;
//	HrsNUDjFBO(GAME.MSDK_HANDLE, &x, &y);
//	ROOMCOOR coor = { x,y };
//	return coor;
//}

// 获取按键状态: -1失败 0弹起状态 1按下状态
int MSDK_KeyState(int code)
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return -1;
	}

	return rFpti6KthJ(PDATA.MSDK_HANDLE, code);
}

// 按下某个按键
void MSDK_KeyDown(int code)
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return;
	}

	if (rFpti6KthJ(PDATA.MSDK_HANDLE, code) == 0) {
		rdpWSY0nFv(PDATA.MSDK_HANDLE, code);
	}
}

// 弹起某个按键
void MSDK_KeyUp(int code)
{
	if (rFpti6KthJ(PDATA.MSDK_HANDLE, code) == 1) {
		oihUU7DIfVcX(PDATA.MSDK_HANDLE, code);
	}
}

void MSDK_KeyUp2(int code)
{
	oihUU7DIfVcX(PDATA.MSDK_HANDLE, code);
}

// 弹起所有按键
void MSDK_ReleaseAllKey()
{
	Na8FeYnvveiZ(PDATA.MSDK_HANDLE);
}

// 按键以及次数
void MSDK_keyPress(int code, int repeats)
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return;
	}

	nnN3A2qfth(PDATA.MSDK_HANDLE, code, repeats);
}


// 随机延迟
void MSDK_DelayRandom(int min, int max)
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return;
	}

	YYyWL2SSFlkB(min, max);
}

void closeRunKey()
{
	if (!PDATA.MSDK_HANDLE || !PDATA.dnf.window_top) {
		return;
	}

	if (rFpti6KthJ(PDATA.MSDK_HANDLE, Keyboard_UpArrow) == 1) {
		oihUU7DIfVcX(PDATA.MSDK_HANDLE, Keyboard_UpArrow);
	}

	if (rFpti6KthJ(PDATA.MSDK_HANDLE, Keyboard_RightArrow) == 1) {
		oihUU7DIfVcX(PDATA.MSDK_HANDLE, Keyboard_RightArrow);
	}

	if (rFpti6KthJ(PDATA.MSDK_HANDLE, Keyboard_DownArrow) == 1) {
		oihUU7DIfVcX(PDATA.MSDK_HANDLE, Keyboard_DownArrow);
	}

	if (rFpti6KthJ(PDATA.MSDK_HANDLE, Keyboard_LeftArrow) == 1) {
		oihUU7DIfVcX(PDATA.MSDK_HANDLE, Keyboard_LeftArrow);
	}
}