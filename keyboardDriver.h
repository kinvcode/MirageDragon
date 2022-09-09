#pragma once

int MSDK_KeyState(int code);

void MSDK_KeyDown(int code);

void MSDK_KeyUp(int code);

void MSDK_ReleaseAllKey();

void MSDK_keyPress(int code, int repeats);

void MSDK_DelayRandom(int min, int max);