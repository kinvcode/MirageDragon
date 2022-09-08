#include "pch.h"
#include "keyboardDriver.h"
#include "msdk.h"

void keyboardUp(int key)
{
	if (M_KeyState(MSDK_HANDLE, key) == 1) {
		M_KeyUp(MSDK_HANDLE, key);
	}
}