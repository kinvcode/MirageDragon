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

	// ��ȡÿ��ͼ��
	static ROOMCOOR getDailyIcon1();
	static ROOMCOOR getDailyIcon2();

	// ��ȡÿ��-���Ͱ�ť
	static ROOMCOOR getDelivery();

	// ��ȡÿ��-����ȷ�ϰ�ť
	static ROOMCOOR getDailySpace();
	
	// ��ȡÿ��-��ɰ�ť
	static ROOMCOOR getDailySubmit();
};