#include "pch.h"
#include "dnfTask.h"
#include "keyboardDriver.h"
#include "usbhidkeycode.h"
#include "constant.h"


void dialogue()
{
	if (readLong(C_DIALOGUE) == 1 || readLong(C_DIALOGUE_B) == 1 || readLong(C_DIALOGUE_ESC) == 1)
	{
		MSDK_keyPress(Keyboard_KongGe, 1);
	}
}