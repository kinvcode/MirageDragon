#pragma once

class MainLineLogic {
public:
	// ������Ϣ
	static DNFJOB job_info;

	// ѧϰ���ܵĵȼ�
	static vector<int> learn_skill_lv;

	// ˢͼ��������
	static vector<CString> dungeon_jobs;

	// ������������
	static vector<CString> town_jobs;

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

	// ����Ի�
	static void handleDialogue();

	// ������
	static void handleSkill();

	// �Ƿ������������
	static bool hasMainJob();

	// ��ȡ����״̬
	static int getJobStatus(int code);

	// ��������
	static int handleJob();

	// ������ϴ��� -1���¾��� 0���ϳ��� 1���ϲ��� 3������� 5δ�ҵ� 10��������
	static int handleJobMaterial(int code);

	// �˳����������߳�
	static void exitMainline();

	// ����Ҫ��0δ֪Ҫ�� 1�Ի� 2ˢͼ
	static int handleJobRequire();

	// ��ȡ�����ͼ
	static int getJobMap(int code);

	// ���������ͼ
	static int getJobSpecialMap(int code);

	// ��ȡ�ȼ���ߵ�ͼ
	static int getMaxLvMap();

	// ��������
	static void updateData();

	// ��������ı�������̫���������͹��
	static void getMainLineDungeonAllObj();

	// ����HOOK�˺�
	static void setGeneralHook();
	static void setBossHook();

	static void test();
};