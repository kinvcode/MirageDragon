#include "pch.h"
#include "dnfUser.h"
#include "MirageDragon.h"
#include "MirageDragonDlg.h"
#include "dnfBase.h"
#include "dnfPacket.h"
#include "dnfCALL.h"
#include "dnfMap.h"
#include "keyboardDriver.h"
#include "baseAddress.h"
#include "GameData.h"
#include "dnfData.h"
#include "http.h"
#include "dataConvert.h"
#include "log.h"
#include "procData.h"

// 切换地图&建筑穿透
void penetrate(bool on)
{
	if (on) {
		penetrateBuild(true);
		penetrateMap(true);
		GAME.penetrate_status = 1;
	}
	else {
		penetrateBuild(false);
		penetrateMap(false);
		GAME.penetrate_status = 0;
	}
}

void penetrateBuild(bool on)
{
	if (on) {
		writeInt(readLong(C_USER_ADDRESS) + C_PENETRATE_BUILDING, -255);
	}
	else {
		writeInt(readLong(C_USER_ADDRESS) + C_PENETRATE_BUILDING, 40);
	}
}

void penetrateMap(bool on)
{
	if (on) {
		writeInt(readLong(C_USER_ADDRESS) + C_PENETRATE_MAP, -255);
	}
	else {
		writeInt(readLong(C_USER_ADDRESS) + C_PENETRATE_MAP, 10);
	}
}

// 修改角色名称
void changeUserName(CString name)
{
	__int64 userNameAddress = readLong(readLong(C_USER_ADDRESS) + C_NAME_OFFSET);
	if (userNameAddress)
	{
		if (writeString(userNameAddress, name)) {
			CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
			mainWindow->Log(L"修改名字成功");
		}
	}
}

// 三速
void threeSpeed(int attack, int casting, int move)
{
	long long shoePointer = readLong(C_USER_ADDRESS) + C_SHOE_OFFSET; // 鞋子指针

	encrypt(readLong(shoePointer) + C_ATTACK_SPEED, attack);
	encrypt(readLong(shoePointer) + C_MOVE_SPEED, move);
	encrypt(readLong(shoePointer) + C_CASTING_SPEED, casting);
}

// 技能冷却
void skillCoolDown(float num)
{
	encrypt(readLong(C_USER_ADDRESS) + C_FLOAT_COOL_DOWN, (int)num);
}

// HOOK伤害
void hookDamage(bool on)
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	__int64 damage_value;
	CString value;
	mainWindow->page2._damage_value.GetWindowText(value);
	damage_value = _ttoi(value);

	if (damage_value > 1000000) {
		damage_value = 1000000;
		Log.info(L"伤害数值太高，已被重置为100W");
	}

	__int64 damage_address = C_GLOBAL_ADDRESS;
	vector<byte>damge_data;
	if (on) {
		damge_data = readByteArray(damage_address, 10);
		writeByteArray(damage_address, (makeByteArray({ 72,190 }) + longToBytes(damage_value)));
	}
	else {
		writeByteArray(damage_address, damge_data);
	}
}

void updateHookValue(int damage_value)
{
	if (damage_value > 1000000) {
		damage_value = 1000000;
		Log.info(L"伤害数值太高，已被重置为100W", true);
	}

	__int64 damage_address = C_GLOBAL_ADDRESS;
	vector<byte>damge_data;
	damge_data = readByteArray(damage_address, 10);
	writeByteArray(damage_address, (makeByteArray({ 72,190 }) + longToBytes(damage_value)));
}

// 获取冷却技能键位
int getCoolDownKey()
{
	__int64 address;
	address = readLong(C_USER_ADDRESS);
	address = readLong(address + C_SKILL_LIST);
	address = readLong(address + C_SKILL_LIST_OFFSET);

	for (auto position : GAME.skill_key)
	{
		__int64 skill_p = readLong(address + position * 8);

		if (skill_p <= 0) {
			return 0;
		}

		__int64 emptyAddress = C_EMPTY_ADDRESS + 3000;
		std::vector<byte>asm_code;

		asm_code = makeByteArray({ 72,131,236,32 });
		asm_code = asm_code + makeByteArray({ 49,210 });
		asm_code = asm_code + makeByteArray({ 72,185 }) + longToBytes(skill_p);
		asm_code = asm_code + makeByteArray({ 255,21,2,0,0,0,235,8 });
		asm_code = asm_code + longToBytes(C_COOL_DOWN_JUDGE_CALL);
		asm_code = asm_code + makeByteArray({ 72,162 }) + longToBytes(emptyAddress);
		asm_code = asm_code + makeByteArray({ 72,131,196,32 });

		memoryAssambly(asm_code);

		if (readLong(emptyAddress) < 1) {
			switch (position)
			{
			case 0:
				return Keyboard_a;
				break;
			case 7:
				return Keyboard_q;
				break;
			case 1:
				return Keyboard_s;
				break;
			case 8:
				return Keyboard_w;
				break;
			case 2:
				return Keyboard_d;
				break;
			case 9:
				return Keyboard_e;
				break;
			case 3:
				return Keyboard_f;
				break;
			case 10:
				return Keyboard_r;
				break;
			case 4:
				return Keyboard_g;
				break;
			case 11:
				return Keyboard_t;
				break;
			case 5:
				return Keyboard_h;
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
int getUserFatigue()
{
	return (int)decrypt2(C_FATIGUE_MAX) - (int)decrypt2(C_FATIGUE_CURRENT);
}

// 自动进图
void autoEntryDungeon()
{
	Sleep(100);
	if (getUserFatigue() <= 0)
	{
		switchUser();
	}

	autoCalcTask();
	Sleep(2000);
}

// 切换角色
void switchUser()
{
	GAME.play_user_index += 1;	// 需要修改
	Sleep(20);
	roleList();
	Sleep(500);
	selectRole(GAME.play_user_index);
	Sleep(500);
}

// 自动计算任务、自动进图
void autoCalcTask()
{
	areaCall(GAME.autoMapNumber);
	selectMap();
	Sleep(200);
	while (true)
	{
		if (GAME.game_status != 2)
		{
			return;
		}
		if (mapCodeOfSelected() == GAME.autoMapNumber)
		{
			break;
		}
		Sleep(100);
		MSDK_keyPress(Keyboard_UpArrow, 1);
	}

	while (true)
	{
		if (GAME.game_status != 2)
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
			entryDungeon(GAME.autoMapNumber, i, 0, 0);
			Sleep(500);
			if (GAME.game_status == 3)
			{
				goto end;
			}
			MSDK_keyPress(Keyboard_LeftArrow, 1);
		}
	}
end:
	return;
}

// 获取角色等级
int getUserLevel()
{
	return readInt(C_USER_LEVEL);
}

// 获取角色名望值
int getUserPrestige()
{
	return readInt(readLong(C_USER_ADDRESS) + C_USER_PRESTIGE);
}

void updateHookNumber(int number)
{
	hookDamage(true);
}

void setFullHMP()
{
	__int64 user_pointer = getUserPointer();
	int max_hp = readInt(user_pointer + C_FULL_HP_OFFSET);
	int max_mp = readInt(user_pointer + C_FULL_MP_OFFSET);
	int cur_hp = readInt(user_pointer + C_MONSTER_BLOOD);
	int cur_mp = readInt(user_pointer + C_CUR_MP);

	if (cur_hp / max_hp * 100 <= 80) {
		writeInt(user_pointer + C_FULL_HP_OFFSET, 100);
	}
	if (cur_mp / max_mp * 100 <= 80) {
		writeInt(user_pointer + C_FULL_MP_OFFSET, 100);
	}
}

__int64 getUserPointer()
{
	__int64 emptyAddress = C_EMPTY_ADDRESS + 4000;
	std::vector<byte>asm_code;

	asm_code = makeByteArray({ 72, 131, 236, 100 });
	asm_code = asm_code + makeByteArray({ 72, 184 }) + longToBytes(C_USER_CALL);
	asm_code = asm_code + makeByteArray({ 255,208 });
	asm_code = asm_code + makeByteArray({ 72, 184 }) + longToBytes(emptyAddress);
	asm_code = asm_code + makeByteArray({ 72, 131, 196, 100 });
	memoryAssambly(asm_code);
	return readLong(emptyAddress);
}

void getRoleList()
{
	Log.info(L"更新角色列表", true);
	getGeneralRoleList();
	//getFavoriteRoleList();
	PDATA.last_update_roles = time(nullptr);
}

void getGeneralRoleList()
{
	if (GAME.game_status != 0)
	{
		return;
	}

	// 读取角色指针
	__int64 role_pointer = readLong(C_ROLE_POINTER);
	// 角色数组头指针
	__int64 head = readLong(role_pointer + 0x258);// 角色数组头偏移
	// 角色数组尾指针
	__int64 end = readLong(role_pointer + 0x260);// 角色数组尾偏移

	// 角色数量
	int numbers = (int)((end - head) / 0x5D0); // 一个数组元素大小为5D0
	if (numbers <= 0 || numbers > 40) {
		return;
	}

	// 获取QQ
	int qq = getQQAccount();
	// 获取大区
	CString server = readString(readLong(C_SERVER_AREA), 20);
	// 角色容器
	vector<ROLEINFO>role_list;
	// 遍历列表
	for (int i = 0; i < numbers; i++)
	{
		__int64 p_item = head + i * 0x5D0; // 元素指针
		if (p_item == 0) continue;

		ROLEINFO info;
		info.account = qq;								// QQ账号
		info.name = readString(readLong(p_item), 20);   // 角色名称
		info.server = server;							// 角色大区
		info.character = readInt(p_item + 0x8);			// 角色职业
		info.advancement = readInt(p_item + 0xC);		// 角色转职
		info.awakening = readInt(p_item + 0x10);		// 觉醒次数
		info.level = (int)decrypt(p_item + 0x18);		// 角色等级
		info.prestige = (int)readInt(p_item + 0x5C4);	// 角色名望
		info.position = i;								// 角色位置
		role_list.push_back(info);
	}
	// 请求API
	http.updateRoles(role_list, false);
	role_list.clear();
}

void getFavoriteRoleList()
{
	if (GAME.game_status != 0)
	{
		return;
	}

	// 读取角色指针
	__int64 role_pointer = readLong(C_ROLE_POINTER);
	// 角色数量
	int numbers = readInt(role_pointer + 0x2C8);

	if (numbers == 0 || numbers > 40) {
		return;
	}

	// 角色数组头指针
	__int64 head = readLong(role_pointer + 0x2C0);// 角色数组头偏移
	int qq = getQQAccount();
	// 角色容器
	vector<ROLEINFO>role_list;
	for (int i = 1; i <= numbers; i++)
	{
		__int64 p_item = readLong(head + i * 8) + 0x28; // 元素指针
		if (readLong(p_item) == 0) continue;

		ROLEINFO info;
		info.account = qq;								// QQ账号
		info.name = readString(readLong(p_item), 20);	// 角色名称
		info.character = readInt(p_item + 0x8);			// 角色职业
		info.advancement = readInt(p_item + 0xC);		// 角色转职
		info.awakening = readInt(p_item + 0x10);		// 觉醒次数
		info.level = (int)decrypt(p_item + 0x18);		// 角色等级
		info.prestige = (int)readInt(p_item + 0x5C4);	// 角色名望
		info.position = i - 1;								// 角色位置
		role_list.push_back(info);
	}

	http.updateRoles(role_list, true);
	role_list.clear();
}

void getRoleID()
{
	CString name = readString(readLong(readLong(C_USER_ADDRESS) + C_NAME_OFFSET), 20);
	int qq = getQQAccount();
	int id = readInt(0x14A0A52A8);
	http.updateRoleID(qq, name, id);
	PDATA.last_update_roleid = time(nullptr);
}

int getRolePos(bool is_favorite)
{
	if (is_favorite) {
		return readInt(readLong(readLong(C_ROLE_ADDRESS) + C_ROLE_INIT_P) + C_FAVORITE_ROLE_POS);
	}
	else {
		return readInt(readLong(readLong(C_ROLE_ADDRESS) + C_ROLE_INIT_P) + C_COMMON_ROLE_POS);
	}
}

bool removeSkill()
{
	__int64 skill_p = readLong(readLong(readLong(C_USER_ADDRESS) + C_SKILL_LIST) + C_SKILL_LIST_OFFSET);

	for (int i = 0; i < 13; i++)
	{
		__int64 head = readLong(skill_p + i * 8);
		if (head == 0) {
			continue;
		}
		CString name = readString(readLong(head + C_SKILL_NAME), 50);
		if (name == L"后跳" || name == L"受身蹲伏")
		{
			// 移除技能
			moveSkillCall(i, getSkillNULPos());
		}
	}

	return false;
}

void flushRole()
{
	roleList();
	Sleep(500);
	selectRole(GAME.role_panel.index);
	Sleep(500);
}

void lockDurable()
{
	// 鞋子指针
	long long shoePointer = readLong(C_USER_ADDRESS) + C_SHOE_OFFSET; // 鞋子指针
	long long weaponPointer = readLong(C_USER_ADDRESS) + C_WEAPON_OFFSET; // 武器指针
	long long coatPointer = readLong(C_USER_ADDRESS) + C_COAT_OFFSET; // 上衣指针
	long long shoulderPointer = readLong(C_USER_ADDRESS) + C_SHOULDER_OFFSET; // 头肩指针
	long long bottomsPointer = readLong(C_USER_ADDRESS) + C_BOTTOMS_OFFSET; // 下装指针
	long long beltPointer = readLong(C_USER_ADDRESS) + C_BELT_OFFSET; // 腰带指针

	// 锁定耐久
	encrypt(readLong(shoePointer) + C_MAX_DURABLE, 0);
	encrypt(readLong(weaponPointer) + C_MAX_DURABLE, 0);
	encrypt(readLong(coatPointer) + C_MAX_DURABLE, 0);
	encrypt(readLong(shoulderPointer) + C_MAX_DURABLE, 0);
	encrypt(readLong(bottomsPointer) + C_MAX_DURABLE, 0);
	encrypt(readLong(beltPointer) + C_MAX_DURABLE, 0);
}