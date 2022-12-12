#pragma once

// 切换穿透状态
void penetrate(bool on);

// 穿透地图
void penetrateMap(bool on);

// 穿透建筑
void penetrateBuild(bool on);

// 角色改名
void changeUserName(CString name);

// 三速修改
void threeSpeed(int attack, int casting, int move);

// 技能百分比冷却
void skillCoolDown(float num);

// HOOK伤害
void hookDamage(bool on);

// 获取冷却的技能键位
int getCoolDownKey();

// 获取角色疲劳值
__int64 getUserFatigue();

// 自动进图
void autoEntryDungeon();

// 切换角色
void switchUser();

// 智能进图(剧情)
void autoCalcTask();

// 获取角色等级
int getUserLevel();

// 获取角色名望
int getUserPrestige();

// 更新HOOK数值
void updateHookNumber(int number);

// 满血满蓝
void setFullHMP();

// 获取人物指针
__int64 getUserPointer();

// 更新角色列表
void getRoleList();

// 更新普通角色栏列表
void getGeneralRoleList();

// 更新偏爱角色栏列表
void getFavoriteRoleList();