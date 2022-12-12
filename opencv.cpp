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

	img = imread(PDATA.screenshot_file_s, IMREAD_COLOR); // ��ȡͼƬ

	CString tmp1 = PDATA.cur_dir;
	tmp1.Append(L"template\\2.png");
	string template_file = CW2A(tmp1.GetString());
	tmp = imread(template_file, IMREAD_COLOR); // ��ȡģ��

	Mat img_display;
	img.copyTo(img_display);

	// ͼƬƥ��
	matchTemplate(img, tmp, result, TM_SQDIFF);

	// ��ȡƥ��������
	minMaxLoc(result, &minVal, NULL, &matchLoc);
	CString msg;
	msg.Format(L"��ǰƥ���:%f", minVal);
	Log.info(msg, true);
	
	if (minVal < 10)
	{
		Log.info(L"ƥ��ɹ���׼���ƶ����", true);
	}
	
	int coor[1] = { 0 };
	//double maxVal;
	//minMaxIdx(result, &minVal, &maxVal);

	//printf("��ǰƥ��ȣ�%d\n", minVal);

	//if (minVal > 10) 
	//{
	//	// ƥ����̫��
	//	cout << "û���ҵ�ƥ���λ��" << endl;
	//	return EXIT_FAILURE;
	//}

	// �������ĵ�
	int cent_x = (int)ceil(tmp.cols / 2) + matchLoc.x;
	int cent_y = (int)ceil(tmp.rows / 2) + matchLoc.y;

	// ���㴰�����Ͻ�λ��
	int window_left = PDATA.dnf.dnf_rect.left;
	int window_top = PDATA.dnf.dnf_rect.top;

	cent_x += window_left;
	cent_y += window_top;

	CString coor_msg;
	coor_msg.Format(L"����ƶ���X:%d Y:%d", cent_x,cent_y);
	Log.info(coor_msg, true);

	MSDK_MoveTo(cent_x, cent_y);

	// ����ͼƬ����
	//rectangle(result, matchLoc, Point(matchLoc.x + tmp.cols, matchLoc.y + tmp.rows), Scalar::all(0), 2, LINE_8);
	//circle(result, Point(cent_x, cent_y), 2, Scalar::all(0), 2, LINE_8);
	//imwrite("x64/debug/res.png", result);

	//rectangle(img_display, matchLoc, Point(matchLoc.x + tmp.cols, matchLoc.y + tmp.rows), Scalar::all(0xADFF2F), 2, LINE_8);
	//circle(img_display, Point(cent_x, cent_y), 2, Scalar::all(0xADFF2F), 2, LINE_8);
	//imwrite("E:/VSRepos/ConsoleTest1/x64/Debug/res.png", img_display);


	//OpenCV::read_lock.unlock();
}