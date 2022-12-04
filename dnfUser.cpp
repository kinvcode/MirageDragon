﻿#include "pch.h"
#include "dnfUser.h"
#include "MirageDragon.h"
#include "MirageDragonDlg.h"
#include "Lock.h"
#include "dnfBase.h"
#include "dnfPacket.h"
#include "dnfCALL.h"
#include "dnfMap.h"
#include "keyboardDriver.h"
#include "baseAddress.h"
#include "GameData.h"
#include "dnfData.h"
#include "http.h"
#include "stringutils.h"
#include "log.h"

// 切换地图&建筑穿透
void penetrate(bool on)
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	InstanceLock wind(mainWindow);
	if (on) {
		writeInt(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_PENETRATE_MAP"), -255);
		writeInt(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_PENETRATE_BUILDING"), -255);
		GLOBAL.penetrate_status = 1;
		mainWindow->Log(L"已开启穿透");
	}
	else {
		writeInt(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_PENETRATE_MAP"), 10);
		writeInt(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_PENETRATE_BUILDING"), 40);
		GLOBAL.penetrate_status = 0;
		mainWindow->Log(L"已关闭穿透");
	}
}

// 修改角色名称
void changeUserName(CString name)
{
	__int64 userNameAddress = readLong(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_NAME_OFFSET"));
	if (userNameAddress)
	{
		if (writeString(userNameAddress, name)) {
			CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
			InstanceLock wind(mainWindow);
			mainWindow->Log(L"修改名字成功");
		}
	}
}

// 三速
void threeSpeed(int attack, int casting, int move)
{
	long long shoePointer = readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_SHOE_OFFSET"); // 鞋子指针

	encrypt(readLong(shoePointer) + ADDR.x64("C_ATTACK_SPEED"), attack);
	encrypt(readLong(shoePointer) + ADDR.x64("C_MOVE_SPEED"), move);
	encrypt(readLong(shoePointer) + ADDR.x64("C_CASTING_SPEED"), casting);
}

// 技能冷却
void skillCoolDown(float num)
{
	encrypt(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_FLOAT_COOL_DOWN2"), (int)num);
}

// HOOK伤害
void hookDamage(bool on)
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	InstanceLock wind(mainWindow);

	__int64 damage_value;
	CString value;
	mainWindow->page2._damage_value.GetWindowText(value);
	damage_value = _ttoi(value);

	__int64 damage_address = ADDR.x64("C_GLOBAL_ADDRESS");
	vector<byte>damge_data;
	if (on) {
		damge_data = readByteArray(damage_address, 10);
		writeByteArray(damage_address, (makeByteArray({ 72,190 }) + longToBytes(damage_value)));
	}
	else {
		writeByteArray(damage_address, damge_data);
	}
}

// 获取冷却技能键位
int getCoolDownKey()
{
	__int64 address;
	address = readLong(ADDR.x64("C_USER_ADDRESS"));
	address = readLong(address + ADDR.x64("C_SKILL_LIST"));
	address = readLong(address + ADDR.x64("C_SKILL_LIST_OFFSET"));

	// 0 ~ 16
	__int64 position;
	//position = rand() % 17;

	for (position = 0; position < 17; position++)
	{
		if (position == 6)
		{
			continue;
		}

		__int64 skill_p = readLong(address + position * 8);

		if (skill_p <= 0) {
			return 0;
		}

		__int64 emptyAddress = ADDR.x64("C_EMPTY_ADDRESS") + 3000;
		std::vector<byte>asm_code;

		asm_code = makeByteArray({ 72,131,236,32 });
		asm_code = asm_code + makeByteArray({ 49,210 });
		asm_code = asm_code + makeByteArray({ 72,185 }) + longToBytes(skill_p);
		asm_code = asm_code + makeByteArray({ 255,21,2,0,0,0,235,8 });
		asm_code = asm_code + longToBytes(ADDR.x64("C_COOL_DOWN_JUDGE_CALL"));
		asm_code = asm_code + makeByteArray({ 72,162 }) + longToBytes(emptyAddress);
		asm_code = asm_code + makeByteArray({ 72,131,196,32 });

		memoryAssambly(asm_code);

		if (readLong(emptyAddress) < 1) {
			switch (position)
			{
			case 0:
				return Keyboard_a;
				break;
			case 1:
				return Keyboard_s;
				break;
			case 2:
				return Keyboard_d;
				break;
			case 3:
				return Keyboard_f;
				break;
			case 4:
				return Keyboard_g;
				break;
			case 5:
				return Keyboard_h;
				break;
			case 7:
				return Keyboard_q;
				break;
			case 8:
				return Keyboard_w;
				break;
			case 9:
				return Keyboard_e;
				break;
			case 10:
				return Keyboard_r;
				break;
			case 11:
				return Keyboard_t;
				break;
			case 12:
				return Keyboard_y;
				break;
			case 14:
				return Keyboard_LeftAlt;
				break;
			case 15:
				return Keyboard_LeftControl;
				break;
			case 6: // 因为普通攻击无冷却，所有会卡技能
			case 13:
			case 16:
			default:
				return Keyboard_x;
				break;
			}
		}
		handleEvents();
	}
	// 如果全部冷却中，则返回X键位
	return Keyboard_x;
}

// 获取当前疲劳
__int64 getUserFatigue()
{
	return (decrypt(ADDR.x64("C_FATIGUE_MAX")) - decrypt(ADDR.x64("C_FATIGUE_CURRENT")));
}

// 自动进图
void autoEntryDungeon()
{
	programDelay(100, 0);
	if (getUserFatigue() <= 0)
	{
		switchUser();
	}

	autoCalcTask();
	programDelay(2000, 0);
}

// 切换角色
void switchUser()
{
	GLOBAL.play_user_index += 1;	// 需要修改
	programDelay(20, 0);
	roleList();
	programDelay(500, 0);
	selectRole(GLOBAL.play_user_index);
	programDelay(500, 0);
}

// 自动计算任务、自动进图
void autoCalcTask()
{
	if (GLOBAL.auto_play_type == 1)
	{
		areaCall(GLOBAL.autoMapNumber);
		selectMap();
		programDelay(200, 0);
		while (true)
		{
			if (GLOBAL.game_status != 2)
			{
				return;
			}
			if (mapCodeOfSelected() == GLOBAL.autoMapNumber)
			{
				break;
			}
			programDelay(100, 0);
			MSDK_keyPress(Keyboard_UpArrow, 1);
		}

		while (true)
		{
			if (GLOBAL.game_status != 2)
			{
				return;
			}
			int difficulty = 0;
			int user_level = getUserLevel();
			if (user_level < 110)
			{
				difficulty = 4;
			}
			else {
				int user_prestige = getUserPrestige();
				if (user_prestige >= 33989) {
					difficulty = 9;
				}
				else if (user_prestige >= 32523) {
					difficulty = 8;
				}
				else if (user_prestige >= 30946) {
					difficulty = 7;
				}
				else if (user_prestige >= 29369) {
					difficulty = 6;
				}
				else if (user_prestige >= 25837) {
					difficulty = 5;
				}
				else if (user_prestige >= 23529) {
					difficulty = 4;
				}
				else if (user_prestige >= 21675) {
					difficulty = 3;
				}
				else if (user_prestige >= 13195) {
					difficulty = 2;
				}
				else if (user_prestige >= 8602) {
					difficulty = 1;
				}
				else if (user_prestige >= 4176) {
					difficulty = 0;
				}
				else {
					goto end;
				}
			}
			for (int i = difficulty; i > 0; i--)
			{
				entryDungeon(GLOBAL.autoMapNumber, i, 0, 0);
				programDelay(500, 0);
				if (GLOBAL.game_status == 3)
				{
					goto end;
				}
				MSDK_keyPress(Keyboard_LeftArrow, 1);
			}
		}
	}
	else {

	}
end:
	return;
}

// 获取角色等级
int getUserLevel()
{
	return (int)readLong(ADDR.x64("C_USER_LEVEL"));
}

// 获取角色名望值
int getUserPrestige()
{
	return (int)decrypt(readLong(ADDR.x64("C_USER")) + ADDR.x64("C_USER_PRESTIGE"));
}

void updateHookNumber(int number)
{
	hookDamage(true);
}

void setFullHMP()
{
	__int64 user_pointer = getUserPointer();
	int max_hp = readInt(user_pointer + ADDR.x64("C_FULL_HP_OFFSET"));
	int max_mp = readInt(user_pointer + ADDR.x64("C_FULL_MP_OFFSET"));
	int cur_hp = readInt(user_pointer + ADDR.x64("C_MONSTER_BLOOD"));
	int cur_mp = readInt(user_pointer + ADDR.x64("C_CUR_MP"));

	if (cur_hp / max_hp * 100 <= 80) {
		writeInt(user_pointer + ADDR.x64("C_FULL_HP_OFFSET"), 100);
	}
	if (cur_mp / max_mp * 100 <= 80) {
		writeInt(user_pointer + ADDR.x64("C_FULL_MP_OFFSET"), 100);
	}
}

__int64 getUserPointer()
{
	__int64 emptyAddress = ADDR.x64("C_EMPTY_ADDRESS") + 4000;
	std::vector<byte>asm_code;

	asm_code = makeByteArray({ 72, 131, 236, 100 });
	asm_code = asm_code + makeByteArray({ 72, 184 }) + longToBytes(ADDR.x64("C_USER_CALL"));
	asm_code = asm_code + makeByteArray({ 255,208 });
	asm_code = asm_code + makeByteArray({ 72, 184 }) + longToBytes(emptyAddress);
	asm_code = asm_code + makeByteArray({ 72, 131, 196, 100 });
	memoryAssambly(asm_code);
	return readLong(emptyAddress);
}

void getRoleList()
{
	if (GLOBAL.game_status != 0)
	{
		return;
	}

	// 构建JSON数据
	json role_list;

	// 读取角色指针
	__int64 role_pointer = readLong(ADDR.x64("C_ROLE_POINTER"));
	// 角色数组头指针
	__int64 head = readLong(role_pointer + 0x288);// 角色数组头偏移
	// 角色数组尾指针
	__int64 end = readLong(role_pointer + 0x290);// 角色数组尾偏移

	// 角色数量
	int numbers = (int)((end - head) / 0x5D0); // 一个数组元素大小为5D0
	int qq = getQQAccount();
	// 遍历列表
	for (int i = 0; i < numbers; i++)
	{
		__int64 p_item = head + i * 0x5D0; // 元素指针
		if (p_item == 0) continue;
		CString name = readString(readLong(p_item), 12);
		int character = readInt(p_item + 0x8);        // 角色职业
		int advancement = readInt(p_item + 0xC);      // 角色转职
		int awakening = readInt(p_item + 0x10);	      // 觉醒次数
		int level = (int)decrypt(p_item + 0x18);	  // 角色等级
		int prestige = (int)readInt(p_item + 0x5C4);  // 角色名望
		int position = i;							  // 角色位置
		string name_string = StringUtils::LocalCpToUtf8(name);

		role_list[i] = {
			{"account",qq},
			{"name",name_string},
			{"character",character},
			{"advancement",advancement},
			{"awakening",awakening},
			{"level",level},
			{"prestige",prestige},
			{"position",position}
		};
	}

	if (role_list.size() > 0) {
		// 更新
		//Log.info(role_list.dump());
		http.updateRoles(role_list);
	}

}

void getFavoriteRoleList()
{
	if (GLOBAL.game_status != 0)
	{
		return;
	}

	json role_list;

	// 读取角色指针
	__int64 role_pointer = readLong(ADDR.x64("C_ROLE_POINTER"));
	// 角色数量
	int numbers = readInt(role_pointer + 0x2C8);
	// 角色数组头指针
	__int64 head = readLong(role_pointer + 0x2C0);// 角色数组头偏移
	int qq = getQQAccount();

	for (int i = 0; i < numbers; i++)
	{
		__int64 p_item = readLong(head + i * 8) + 0x28; // 元素指针
		if (p_item == 0) continue;
		CString name = readString(readLong(p_item), 12);
		int character = readInt(p_item + 0x8);        // 角色职业
		int advancement = readInt(p_item + 0xC);      // 角色转职
		int awakening = readInt(p_item + 0x10);	      // 觉醒次数
		int level = (int)decrypt(p_item + 0x18);	  // 角色等级
		int prestige = (int)readInt(p_item + 0x5C4);  // 角色名望
		int position = i;							  // 角色位置
		string name_string = StringUtils::LocalCpToUtf8(name);

		role_list[i] = {
			{"account",qq},
			{"name",name_string},
			{"character",character},
			{"advancement",advancement},
			{"awakening",awakening},
			{"level",level},
			{"prestige",prestige},
			{"position",position}
		};
	}

	if (role_list.size() > 0) {
		// 更新
	}
}