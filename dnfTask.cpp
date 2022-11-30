#include "pch.h"
#include "dnfTask.h"
#include "keyboardDriver.h"
#include "usbhidkeycode.h"
#include "constant.h"
#include "baseAddress.h"


void dialogue()
{
	if (readLong(ADDR.x64("C_DIALOGUE")) == 1 || readLong(ADDR.x64("C_DIALOGUE_B")) == 1 || readLong(ADDR.x64("C_DIALOGUE_ESC")) == 1)
	{
		MSDK_keyPress(Keyboard_KongGe, 1);
	}
}