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

	hDc = GetDC(hWnd); //获取DC
	if (hDc == NULL) return;
	int bitOfPix = GetDeviceCaps(hDc, BITSPIXEL); //获取DC像素的大小（32）

	RECT  rect;

	if (!GetWindowRect(hWnd, &rect))
	{
		return;
	}

	int width = rect.right - rect.left;
	int hight = rect.bottom - rect.top;

	CImage image;
	image.Create(width, hight, bitOfPix); //为图像类创建与窗口DC相同大小的DC
	BitBlt(image.GetDC(), 0, 0, width, hight, hDc, 0, 0, SRCCOPY); //将窗口DC图像复制到image

	image.Save(PDATA.screenshot_file_c, Gdiplus::ImageFormatPNG); //保存为png格式图片文件
	image.ReleaseDC(); //释放DC
	ReleaseDC(hWnd, hDc); //释放DC资源

	//img_lock.unlock();
}