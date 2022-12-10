#pragma once

HANDLE MSDK_Open(int pid);

// 设置分辨率 0正确 -10不支持绝对移动
int MSDK_ResolutionUsed(int x, int y);

// 获取鼠标按键状态：-1失败，0弹起，1按下
int MSDK_GetMouseState(int code);

// 鼠标左键点击
void MSDK_LeftClick();

// 鼠标左键按下
void MSDK_LeftDown();

// 鼠标左键弹起
void MSDK_LeftUp();

// 鼠标右键点击
void MSDK_RightClick();

// 鼠标右键按键
void MSDK_RightDown();

// 鼠标右键弹起
void MSDK_RightUp();

// 弹起所有鼠标按键
void MSDK_ReleaseAllMouse();

// 鼠标滚轮 负数（向下）和正数（向上）
void MSDK_MouseWheel(int num);

// 重置鼠标位置（屏幕左上角），0,0
void MSDK_ResetMousePos();

// 相对当前位置移动
void MSDK_MoveR(int x,int y);

// 移动到指定位置
void MSDK_MoveTo(int x,int y);

// 获取当前鼠标位置(完全鼠标控制的电脑使用)
//ROOMCOOR MSDK_GetPos();

// 键盘按键状态
int MSDK_KeyState(int code);

// 键盘按下
void MSDK_KeyDown(int code);

// 键盘弹起
void MSDK_KeyUp(int code);

// 键盘所有键弹起
void MSDK_ReleaseAllKey();

// 按键
void MSDK_keyPress(int code, int repeats);

// 随机延迟
void MSDK_DelayRandom(int min, int max);

// 弹起上下左右方向键
void closeRunKey();