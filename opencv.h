#pragma once
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
//#include <mutex>

using namespace cv;

class OpenCV
{
public:
	
	//mutex read_lock;

	static ROOMCOOR read(CString tmp_png);

	// 获取每日图标
	static ROOMCOOR getDailyIcon1();
	static ROOMCOOR getDailyIcon2();

	// 获取每日-传送按钮
	static ROOMCOOR getDelivery();

	// 获取每日-传送确认按钮
	static ROOMCOOR getDailySpace();
	
	// 获取每日-完成按钮
	static ROOMCOOR getDailySubmit();
};