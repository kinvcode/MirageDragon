#include "pch.h"
#include "dnfUser.h"
#include "MirageDragon.h"
#include "MirageDragonDlg.h"
#include "Lock.h"
#include "dnfBase.h"

// 切换地图&建筑穿透
void penetrate(bool on)
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	InstanceLock wind(mainWindow);
	if (on) {
		writeInt(readLong(C_USER) + C_PENETRATE_MAP, -255);
		writeInt(readLong(C_USER) + C_PENETRATE_BUILDING, -255);
		penetrate_status = 1;
		mainWindow->Log(L"已开启穿透");
	}
	else {
		writeInt(readLong(C_USER) + C_PENETRATE_MAP, 10);
		writeInt(readLong(C_USER) + C_PENETRATE_BUILDING, 40);
		penetrate_status = 0;
		mainWindow->Log(L"已关闭穿透");
	}
}

// 修改角色名称
void changeUserName(wstring name)
{
	__int64 userNameAddress = readLong(readLong(C_USER) + C_NAME_OFFSET);
	if (userNameAddress)
	{
		if (writeByteArray(userNameAddress, wstringToBytes(name))) {
			CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
			InstanceLock wind(mainWindow);
			mainWindow->Log(L"修改名字成功");
		}
	}
}

// 三速
void threeSpeed(int attack, int casting, int move)
{
	long long shoePointer = readLong(C_USER) + C_SHOE_OFFSET; // 鞋子指针

	encrypt(readLong(shoePointer) + C_ATTACK_SPEED, attack);
	encrypt(readLong(shoePointer) + C_MOVE_SPEED, move);
	encrypt(readLong(shoePointer) + C_CASTING_SPEED, casting);
}

// 召唤人偶&怪物
void summonFigureOrMonster(int code, int type = 1)
{
	if (type == 1) {
		// 人偶
		__int64 object_type = C_SUMMON_FIGURE;
	}
	else {
		// 怪物
		__int64 object_type = C_SUMMON_MONSTER;
	}

	std::vector<byte>asm_code;

	__int64 emptyAddress = C_EMPTY_ADDRESS + 2900;

	writeLong(emptyAddress, code);
	writeLong(emptyAddress + 4, 135);
	writeLong(emptyAddress + 8, 135);
	writeLong(emptyAddress + 12, 600000);
	writeLong(emptyAddress + 16, 1);

	asm_code = asm_code + makeByteArray({ 72, 131, 236, 32 });
	asm_code = asm_code + makeByteArray({ 73, 184 }) + longToBytes(emptyAddress);
	asm_code = asm_code + makeByteArray({ 72, 186 }) + longToBytes(C_USER);
	asm_code = asm_code + makeByteArray({ 72, 139, 18, 72, 185, 0, 0, 0, 0, 0, 0, 0, 0, 72, 184 }) + longToBytes(C_SUMMON_FIGURE);
	asm_code = asm_code + makeByteArray({ 255, 208, 72, 131, 196, 32 });

	memoryAssambly(asm_code);
}

// 透明CALL
void hiddenUser()
{
	__int64 target = readLong(C_USER);

	std::vector<byte>asm_code;
	asm_code = makeByteArray({ 72, 129, 236, 0, 2, 0, 0 });
	asm_code = asm_code + makeByteArray({ 65, 191, 255, 255, 255, 255 });
	asm_code = asm_code + makeByteArray({ 199, 68, 36, 32, 255, 255, 0, 0 });
	asm_code = asm_code + makeByteArray({ 65, 185, 1, 0, 0, 0 });
	asm_code = asm_code + makeByteArray({ 73, 184, 1, 0, 0, 0, 0, 0, 0, 0 });
	asm_code = asm_code + makeByteArray({ 186, 1, 0, 0, 0 });
	asm_code = asm_code + makeByteArray({ 72, 185 }) + longToBytes(target);
	asm_code = asm_code + makeByteArray({ 72, 184 }) + longToBytes(C_HIDDEN_CALL);
	asm_code = asm_code + makeByteArray({ 255, 208, 72, 129, 196, 0, 2, 0, 0 });

	memoryAssambly(asm_code);
}

// 超级评分
void superScore()
{
	writeLong(readLong(C_SCORE_ADDRESS) + C_CE_SCORE, 999999);
}

// 技能冷却
void skillCoolDown(float num)
{
	encrypt(readLong(C_USER) + C_FLOAT_COOL_DOWN2, (int)num);
}

// HOOK伤害
void hookDamage(bool on)
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	InstanceLock wind(mainWindow);

	CString value;
	mainWindow->page2._damage_value.GetWindowText(value);

	vector<byte>damge_data;

	__int64 damage_address = C_GLOBAL_ADDRESS;
	__int64 damage_value = _ttoi(value);

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
	address = readLong(C_USER);
	address = readLong(address + C_SKILL_LIST);
	address = readLong(address + C_SKILL_LIST_OFFSET);

	__int64 position = rand() % 17;
	//__int64 position = 0;

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
		case 6:
			return Keyboard_x;
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
		case 13:
		case 15:
		case 16:
		default:
			return Keyboard_x;
			break;
		}
	}
	else {
		return Keyboard_x;
	}
	return Keyboard_x;
}