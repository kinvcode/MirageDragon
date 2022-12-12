#include "pch.h"
#include "screenshot.h"
#include "GameData.h"
#include <atlimage.h>
#include "keyboardDriver.h"
#include "log.h"
#include "procData.h"

void ScreenShot::savePic()
{
	//img_lock.lock();

	HDC hDc = NULL;
	HWND hWnd = PDATA.dnf.dnf_window;

	hDc = GetDC(hWnd); //��ȡDC
	if (hDc == NULL) return;
	int bitOfPix = GetDeviceCaps(hDc, BITSPIXEL); //��ȡDC���صĴ�С��32��

	RECT  rect;

	if (!GetWindowRect(hWnd, &rect))
	{
		return;
	}

	int width = rect.right - rect.left;
	int hight = rect.bottom - rect.top;

	CImage image;
	image.Create(width, hight, bitOfPix); //Ϊͼ���ഴ���봰��DC��ͬ��С��DC
	BitBlt(image.GetDC(), 0, 0, width, hight, hDc, 0, 0, SRCCOPY); //������DCͼ���Ƶ�image

	image.Save(PDATA.screenshot_file_c, Gdiplus::ImageFormatPNG); //����Ϊpng��ʽͼƬ�ļ�
	image.ReleaseDC(); //�ͷ�DC
	ReleaseDC(hWnd, hDc); //�ͷ�DC��Դ

	//img_lock.unlock();
}