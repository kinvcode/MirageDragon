#include "pch.h"
#include "opencv.h"
#include "log.h"
#include "screenshot.h"
#include "GameData.h"
#include "keyboardDriver.h"
#include "procData.h"


void OpenCV::read()
{
	//OpenCV::read_lock.lock();

	Mat img; Mat tmp; Mat result;
	double minVal;
	Point matchLoc;

	ScreenShot::savePic();

	img = imread(PDATA.screenshot_file_s, IMREAD_COLOR); // 读取图片

	CString tmp1 = PDATA.cur_dir;
	tmp1.Append(L"template\\2.png");
	string template_file = CW2A(tmp1.GetString());
	tmp = imread(template_file, IMREAD_COLOR); // 读取模板

	Mat img_display;
	img.copyTo(img_display);

	// 图片匹配
	matchTemplate(img, tmp, result, TM_SQDIFF);

	// 获取匹配的坐标点
	minMaxLoc(result, &minVal, NULL, &matchLoc);
	CString msg;
	msg.Format(L"当前匹配度:%f", minVal);
	Log.info(msg, true);
	
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

	// 计算窗口左上角位置
	int window_left = PDATA.dnf.dnf_rect.left;
	int window_top = PDATA.dnf.dnf_rect.top;

	cent_x += window_left;
	cent_y += window_top;

	CString coor_msg;
	coor_msg.Format(L"鼠标移动到X:%d Y:%d", cent_x,cent_y);
	Log.info(coor_msg, true);

	MSDK_MoveTo(cent_x, cent_y);

	// 保存图片部分
	//rectangle(result, matchLoc, Point(matchLoc.x + tmp.cols, matchLoc.y + tmp.rows), Scalar::all(0), 2, LINE_8);
	//circle(result, Point(cent_x, cent_y), 2, Scalar::all(0), 2, LINE_8);
	//imwrite("x64/debug/res.png", result);

	//rectangle(img_display, matchLoc, Point(matchLoc.x + tmp.cols, matchLoc.y + tmp.rows), Scalar::all(0xADFF2F), 2, LINE_8);
	//circle(img_display, Point(cent_x, cent_y), 2, Scalar::all(0xADFF2F), 2, LINE_8);
	//imwrite("E:/VSRepos/ConsoleTest1/x64/Debug/res.png", img_display);


	//OpenCV::read_lock.unlock();
}