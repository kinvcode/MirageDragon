#pragma once

class DungeonLogic
{
public:

	static queue<DUNGEONJOB> dg_list; // �����б�

	static bool com_hook; // ��ͨ����HOOK�˺�����

	static bool boss_hook; // ��ͨ����HOOK�˺�����

	// ѡ���ɫ
	static void selectRole();

	// �ڳ�����
	static void inTown();

	// ѡ�񸱱�
	static void selectDungeon();

	// �ڸ�����
	static void inDungeon();

	// ��ͼ����
	static void firstRoom();

	// �ر�ͼ�ڹ���
	static void closeFunctions();

	// ����߼�
	static void attackMonsterLogic();

	// ͨ�ش���
	static void clearanceLogic();

	// ���ﶵ��
	static void finalGatherItems();

	// ��ʼ�������б�
	static void initDG();

	// ���¸����б�
	static void updateData();

	// ����HOOK�˺�
	static void setGeneralHook();
	static void setBossHook();

	// BOSS�����ж�
	static bool dungeonRoomIsBoss();
};