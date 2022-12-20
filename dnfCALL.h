#pragma once

// 技能CALL
void skillCall(__int64 pointer, int code, __int64 damage, int x, int y, int z, int skillSize);

// 坐标CALL
void coorCall(int x, int y, int z);

// 召唤怪物或人偶
void summonFigureOrMonster(int code, int type);

// 透明CALL
void hiddenUser();

// 超级评分
void superScore();

// 区域CALL
void areaCall(int code);

// 界面CALL:
// 0个人信息 1装备栏 2任务手册 3技能面板 4无(可能等级不够) 5冒险团 
// 6心情 7伙伴 8队伍信息 9社交 10邮箱
// 11(出现制裁提示) 12无 13拍卖行 14（需要100级以上） 15游戏指南
// 16百科词典 17录制视频 18（密码提示） 19游戏设置 20键盘设置
// 21(无) 22选择角色界面 23和24频道选择 25(游戏崩溃)
void panelCall(int id);

// 学习技能CALL
void learSkillCall();

// 获取技能空位
int getSkillNULPos();

// 移动技能CALL
void moveSkillCall(int source, int target);

// 完成任务CALL
void finishJobCall(int id, int times = 1);

// 提交任务CALL
void submitJobCall(int id);

// 接受任务CALL
void acceptJobCall(int id);
