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

	img = imread(PDATA.screenshot_file_s, IMREAD_COLOR); // ��ȡͼƬ

	CString tmp1 = PDATA.cur_dir;
	tmp1.Append(tmp_png);
	string template_file = CW2A(tmp1.GetString());
	tmp = imread(template_file, IMREAD_COLOR); // ��ȡģ��

	Mat img_display;
	img.copyTo(img_display);

	// ͼƬƥ��
	matchTemplate(img, tmp, result, TM_SQDIFF);

	// ��ȡƥ��������
	minMaxLoc(result, &minVal, NULL, &matchLoc);

	//CString msg;
	//msg.Format(L"��ǰƥ���:%f", minVal);
	//Log.info(msg, true);

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

	ROOMCOOR cent = { cent_x,cent_y };

	return cent;
}

// ÿ��ͼ��768,532
ROOMCOOR OpenCV::getDailyIcon1()
{
	CString tmp_png = L"template\\daily_icon1.png";
	ROOMCOOR cent = read(tmp_png);

	// �ж��Ƿ�����Ч��Χ
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
	//cent_msg.Format(L"��Ϸ������X:%d Y:%d", cent.x, cent.y);
	//Log.info(cent_msg, true);

	// ���㴰�����Ͻ�λ��
	int window_left = PDATA.dnf.dnf_rect.left;
	int window_top = PDATA.dnf.dnf_rect.top;

	cent.x += window_left;
	cent.y += window_top;

	//CString coor_msg;
	//coor_msg.Format(L"ʵ������X:%d Y:%d", cent.x, cent.y);
	//Log.info(coor_msg, true);

	return cent;
}

ROOMCOOR OpenCV::getDailyIcon2()
{
	CString tmp_png = L"template\\daily_icon2.png";
	ROOMCOOR cent = read(tmp_png);

	// �ж��Ƿ�����Ч��Χ
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
	//cent_msg.Format(L"��Ϸ������X:%d Y:%d", cent.x, cent.y);
	//Log.info(cent_msg, true);

	// ���㴰�����Ͻ�λ��
	int window_left = PDATA.dnf.dnf_rect.left;
	int window_top = PDATA.dnf.dnf_rect.top;

	cent.x += window_left;
	cent.y += window_top;

	//CString coor_msg;
	//coor_msg.Format(L"ʵ������X:%d Y:%d", cent.x, cent.y);
	//Log.info(coor_msg, true);

	return cent;
}

// ÿ��-���Ͱ�ť494,445
ROOMCOOR OpenCV::getDelivery() 
{
	CString tmp_png = L"template\\daily_delievry.png";
	ROOMCOOR cent = read(tmp_png);

	CString cent_msg;
	cent_msg.Format(L"��Ϸ������X:%d Y:%d", cent.x, cent.y);
	Log.info(cent_msg, true);

	// �ж��Ƿ�����Ч��Χ
	int diff_x, diff_y;
	diff_x = abs(cent.x - 494);
	diff_y = abs(cent.y - 445);
	if (diff_x > 20 || diff_y > 20)
	{
		cent.x = 0;
		cent.y = 0;
		return cent;
	}

	// ���㴰�����Ͻ�λ��
	int window_left = PDATA.dnf.dnf_rect.left;
	int window_top = PDATA.dnf.dnf_rect.top;

	cent.x += window_left;
	cent.y += window_top;

	CString coor_msg;
	coor_msg.Format(L"ʵ������X:%d Y:%d", cent.x, cent.y);
	Log.info(coor_msg, true);

	return cent;
}

// ÿ��ȷ��ͼ��495,342
ROOMCOOR OpenCV::getDailySpace()
{
	CString tmp_png = L"template\\dailay_space.png";
	ROOMCOOR cent = read(tmp_png);

	CString cent_msg;
	cent_msg.Format(L"��Ϸ������X:%d Y:%d", cent.x, cent.y);
	Log.info(cent_msg, true);

	// �ж��Ƿ�����Ч��Χ
	int diff_x, diff_y;
	diff_x = abs(cent.x - 495);
	diff_y = abs(cent.y - 342);
	if (diff_x > 20 || diff_y > 20)
	{
		cent.x = 0;
		cent.y = 0;
		return cent;
	}

	// ���㴰�����Ͻ�λ��
	int window_left = PDATA.dnf.dnf_rect.left;
	int window_top = PDATA.dnf.dnf_rect.top;

	cent.x += window_left;
	cent.y += window_top;

	CString coor_msg;
	coor_msg.Format(L"ʵ������X:%d Y:%d", cent.x, cent.y);
	Log.info(coor_msg, true);

	return cent;
}

// ÿ��-��ɰ�ť495,445
ROOMCOOR OpenCV::getDailySubmit() 
{
	CString tmp_png = L"template\\daily_ok.png";
	ROOMCOOR cent = read(tmp_png);

	CString cent_msg;
	cent_msg.Format(L"��Ϸ������X:%d Y:%d", cent.x, cent.y);
	Log.info(cent_msg, true);

	// �ж��Ƿ�����Ч��Χ
	int diff_x, diff_y;
	diff_x = abs(cent.x - 495);
	diff_y = abs(cent.y - 445);
	if (diff_x > 20)
	{
		cent.x = 0;
		cent.y = 0;
		return cent;
	}

	// ���㴰�����Ͻ�λ��
	int window_left = PDATA.dnf.dnf_rect.left;
	int window_top = PDATA.dnf.dnf_rect.top;

	cent.x += window_left;
	cent.y += window_top;

	CString coor_msg;
	coor_msg.Format(L"ʵ������X:%d Y:%d", cent.x, cent.y);
	Log.info(coor_msg, true);

	return cent;
}