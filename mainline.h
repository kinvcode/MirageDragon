#pragma once

class MainLineLogic {
public:

	// ѧϰ���ܵĵȼ�
	static vector<int> learn_skill_lv; 

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
};