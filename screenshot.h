#pragma once
#include<string>
#include<atlimage.h>
#include <mutex>

class ScreenShot
{
public:
	//static mutex img_lock;
	static void savePic();
};