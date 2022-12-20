#pragma once
#include "dnfData.h"
#include "log.h"
#include "scripts.h"
#include "dnfUser.h"
#include "dnfCALL.h"
#include "MirageDragonDlg.h"
#include "MirageDragon.h"
#include "http.h"

class DailyJobLogic 
{
public:
	static int dungeon_times; // ˢͼ����

	static bool daily_complete; // �������

	static bool com_hook; // ��ͨ����HOOK�˺�����

	static bool boss_hook; // ��ͨ����HOOK�˺�����

	static bool job_faild; // �����Ƿ�ʧ��

	// ��ʼ������
	static void initData();

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

	// ��������
	static void updateData();

	// ����HOOK�˺�
	static void setGeneralHook();
	static void setBossHook();
};