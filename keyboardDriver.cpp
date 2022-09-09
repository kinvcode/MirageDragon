#include "pch.h"
#include "keyboardDriver.h"
#include "msdk.h"

// ��ȡ����״̬: -1ʧ�� 0����״̬ 1����״̬
int MSDK_KeyState(int code)
{
	if (!MSDK_HANDLE || !window_top) {
		return -1;
	}

	return M_KeyState(MSDK_HANDLE, code);
}

// ����ĳ������
void MSDK_KeyDown(int code)
{
	if (!MSDK_HANDLE || !window_top) {
		return;
	}

	if (M_KeyState(MSDK_HANDLE, code) == 0) {
		M_KeyDown(MSDK_HANDLE, code);
	}
}

// ����ĳ������
void MSDK_KeyUp(int code)
{
	if (!MSDK_HANDLE || !window_top) {
		return;
	}

	if (M_KeyState(MSDK_HANDLE, code) == 1) {
		M_KeyUp(MSDK_HANDLE, code);
	}
}

// �������а���
void MSDK_ReleaseAllKey()
{
	if (!MSDK_HANDLE || !window_top) {
		return;
	}

	M_ReleaseAllKey(MSDK_HANDLE);
}

// �����Լ�����
void MSDK_keyPress(int code, int repeats)
{
	if (!MSDK_HANDLE || !window_top) {
		return;
	}

	M_KeyPress(MSDK_HANDLE, code, repeats);
}


// ����ӳ�
void MSDK_DelayRandom(int min, int max)
{
	if (!MSDK_HANDLE || !window_top) {
		return;
	}

	M_DelayRandom(min, max);
}