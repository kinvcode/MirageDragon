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

// 界面CALL
void panelCall(int id);

// 学习技能CALL
void learSkillCall();

// 获取技能空位
int getSkillNULPos();

// 移动技能CALL
void moveSkillCall(int source, int target);
