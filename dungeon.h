#pragma once

class DungeonLogic
{
public:

	static queue<DUNGEONJOB> dg_list; // �����б�

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
};