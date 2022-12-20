#include "pch.h"
#include "opencv.h"
#include "log.h"
#include "screenshot.h"
#include "GameData.h"
#include "keyboardDriver.h"
#include "procData.h"


ROOMCOOR OpenCV::read(CString tmp_png)
{
	//OpenCV::read_lock.lock();

	Mat img; Mat tmp; Mat result;
	double minVal;
	Point matchLoc;

	ScreenShot::savePic();

	img = imread(PDATA.screenshot_file_s, IMREAD_COLOR); // 读取图片

	CString tmp1 = PDATA.cur_dir;
	tmp1.Append(tmp_png);
	string template_file = CW2A(tmp1.GetString());
	tmp = imread(template_file, IMREAD_COLOR); // 读取模板

	Mat img_display;
	img.copyTo(img_display);

	// 图片匹配
	matchTemplate(img, tmp, result, TM_SQDIFF);

	// 获取匹配的坐标点
	minMaxLoc(result, &minVal, NULL, &matchLoc);

	//CString msg;
	//msg.Format(L"当前匹配度:%f", minVal);
	//Log.info(msg, true);

	if (minVal < 10)
	{
		Log.info(L"匹配成功，准备移动鼠标", true);
	}

	int coor[1] = { 0 };
	//double maxVal;
	//minMaxIdx(result, &minVal, &maxVal);

	//printf("当前匹配度：%d\n", minVal);

	//if (minVal > 10) 
	//{
	//	// 匹配结果太差
	//	cout << "没有找到匹配的位置" << endl;
	//	return EXIT_FAILURE;
	//}

	// 计算中心点
	int cent_x = (int)ceil(tmp.cols / 2) + matchLoc.x;
	int cent_y = (int)ceil(tmp.rows / 2) + matchLoc.y;

	ROOMCOOR cent = { cent_x,cent_y };

	return cent;
}

// 每日图标768,532
ROOMCOOR OpenCV::getDailyIcon1()
{
	CString tmp_png = L"template\\daily_icon1.png";
	ROOMCOOR cent = read(tmp_png);

	// 判定是否在有效范围
	int diff_x, diff_y;
	diff_x = abs(cent.x - 768);
	diff_y = abs(cent.y - 532);
	if (diff_x > 20 || diff_y > 20)
	{
		cent.x = 0;
		cent.y = 0;
		return cent;
	}

	//CString cent_msg;
	//cent_msg.Format(L"游戏中坐标X:%d Y:%d", cent.x, cent.y);
	//Log.info(cent_msg, true);

	// 计算窗口左上角位置
	int window_left = PDATA.dnf.dnf_rect.left;
	int window_top = PDATA.dnf.dnf_rect.top;

	cent.x += window_left;
	cent.y += window_top;

	//CString coor_msg;
	//coor_msg.Format(L"实际坐标X:%d Y:%d", cent.x, cent.y);
	//Log.info(coor_msg, true);

	return cent;
}

ROOMCOOR OpenCV::getDailyIcon2()
{
	CString tmp_png = L"template\\daily_icon2.png";
	ROOMCOOR cent = read(tmp_png);

	// 判定是否在有效范围
	int diff_x, diff_y;
	diff_x = abs(cent.x - 768);
	diff_y = abs(cent.y - 532);
	if (diff_x > 20 || diff_y > 20)
	{
		cent.x = 0;
		cent.y = 0;
		return cent;
	}

	//CString cent_msg;
	//cent_msg.Format(L"游戏中坐标X:%d Y:%d", cent.x, cent.y);
	//Log.info(cent_msg, true);

	// 计算窗口左上角位置
	int window_left = PDATA.dnf.dnf_rect.left;
	int window_top = PDATA.dnf.dnf_rect.top;

	cent.x += window_left;
	cent.y += window_top;

	//CString coor_msg;
	//coor_msg.Format(L"实际坐标X:%d Y:%d", cent.x, cent.y);
	//Log.info(coor_msg, true);

	return cent;
}

// 每日-传送按钮494,445
ROOMCOOR OpenCV::getDelivery() 
{
	CString tmp_png = L"template\\daily_delievry.png";
	ROOMCOOR cent = read(tmp_png);

	CString cent_msg;
	cent_msg.Format(L"游戏中坐标X:%d Y:%d", cent.x, cent.y);
	Log.info(cent_msg, true);

	// 判定是否在有效范围
	int diff_x, diff_y;
	diff_x = abs(cent.x - 494);
	diff_y = abs(cent.y - 445);
	if (diff_x > 20 || diff_y > 20)
	{
		cent.x = 0;
		cent.y = 0;
		return cent;
	}

	// 计算窗口左上角位置
	int window_left = PDATA.dnf.dnf_rect.left;
	int window_top = PDATA.dnf.dnf_rect.top;

	cent.x += window_left;
	cent.y += window_top;

	CString coor_msg;
	coor_msg.Format(L"实际坐标X:%d Y:%d", cent.x, cent.y);
	Log.info(coor_msg, true);

	return cent;
}

// 每日确定图标495,342
ROOMCOOR OpenCV::getDailySpace()
{
	CString tmp_png = L"template\\dailay_space.png";
	ROOMCOOR cent = read(tmp_png);

	CString cent_msg;
	cent_msg.Format(L"游戏中坐标X:%d Y:%d", cent.x, cent.y);
	Log.info(cent_msg, true);

	// 判定是否在有效范围
	int diff_x, diff_y;
	diff_x = abs(cent.x - 495);
	diff_y = abs(cent.y - 342);
	if (diff_x > 20 || diff_y > 20)
	{
		cent.x = 0;
		cent.y = 0;
		return cent;
	}

	// 计算窗口左上角位置
	int window_left = PDATA.dnf.dnf_rect.left;
	int window_top = PDATA.dnf.dnf_rect.top;

	cent.x += window_left;
	cent.y += window_top;

	CString coor_msg;
	coor_msg.Format(L"实际坐标X:%d Y:%d", cent.x, cent.y);
	Log.info(coor_msg, true);

	return cent;
}

// 每日-完成按钮495,445
ROOMCOOR OpenCV::getDailySubmit() 
{
	CString tmp_png = L"template\\daily_ok.png";
	ROOMCOOR cent = read(tmp_png);

	CString cent_msg;
	cent_msg.Format(L"游戏中坐标X:%d Y:%d", cent.x, cent.y);
	Log.info(cent_msg, true);

	// 判定是否在有效范围
	int diff_x, diff_y;
	diff_x = abs(cent.x - 495);
	diff_y = abs(cent.y - 445);
	if (diff_x > 20)
	{
		cent.x = 0;
		cent.y = 0;
		return cent;
	}

	// 计算窗口左上角位置
	int window_left = PDATA.dnf.dnf_rect.left;
	int window_top = PDATA.dnf.dnf_rect.top;

	cent.x += window_left;
	cent.y += window_top;

	CString coor_msg;
	coor_msg.Format(L"实际坐标X:%d Y:%d", cent.x, cent.y);
	Log.info(coor_msg, true);

	return cent;
}