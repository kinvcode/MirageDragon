#include "pch.h"
#include "keyboardDriver.h"
#include "msdk.h"

void keyboardUp(int key)
{
	if (!MSDK_HANDLE || !window_top) {
		return;
	}

	if (M_KeyState(MSDK_HANDLE, key) == 1) {
		M_KeyUp(MSDK_HANDLE, key);
	}
}

void MSDK_KeyboardUP(int code) 
{
	if (!MSDK_HANDLE || !window_top) {
		return;
	}

	if (M_KeyState(MSDK_HANDLE, code) == 1) {
		M_KeyUp(MSDK_HANDLE, code);
	}
}

void MSDK_keyPress(int code,int repeats)
{
	if (!MSDK_HANDLE || !window_top) {
		return;
	}

	M_KeyPress(MSDK_HANDLE, code, repeats);
}

void MSDK_DelayRandom(int min, int max) 
{
	if (!MSDK_HANDLE || !window_top) {
		return;
	}

	M_DelayRandom(min, max);
}