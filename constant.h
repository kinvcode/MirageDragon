#pragma once

#ifndef CONSTANT_H
#define CONSTANT_H

extern __int64 C_USER;
extern __int64 C_USER_POINTER;
////////////////////////[ �� �� �� ַ �� ��  ]///////////////////////////////////

#define C_USER_ADDRESS 0x149CCF208 // �����ַ
#define C_USER_CALL 0x144098F70 // ����CALL
#define C_USER_LEVEL 0x149A07A20 // ��ɫ�ȼ�
#define C_SCORE_ADDRESS 0x149A20218 // ���ֻ�ַ
#define C_TOWN_WORLD 0x1499E442C // ���������
#define C_TOWN_AREA 0x1499E4430 // ����С����
#define C_TOWN_X 0x149A22738 // ��������X
#define C_TOWN_Y 0x149A2273C // ��������Y
#define C_GAME_STATUS 0x1496FB710 // ��Ϸ״̬
#define C_EMPTY_ADDRESS 0x13FC804C0 // ȫ�ֿհ�
#define C_ASM_CALL 0x13FDA0000 // ���CALL
#define C_SKILL_CALL 0x143BCBD00 // ����CALL
#define C_AUTO_PICKUP 0x143FA1AF3 // �Զ�����
#define C_MAP_SELECTED 0x149D17500 // ѡͼ��ַ
#define C_MAP2_SELECTED 0x149A212C0 // ѡͼ��ַ2
#define C_MAP_CODE_SELECTED 0x3B0 // ѡ�е�ͼ���
#define C_MAP_DIFFICULTY_SELECTED 0x2B8 // ѡ�е�ͼ�Ѷ�
#define C_DIALOGUE 0x149D1D6B0 // �Ի���ַ
#define C_DIALOGUE_B 0x149A2235C // �Ի���ַB
#define C_DIALOGUE_ESC 0x149A223BC // Esc�Ի���ַ
#define C_MAP_NUMBER 0x149A079B0 // �������
#define C_TIME_ADDRESS 0x20A050 // ʱ���ַ
#define C_ROOM_NUMBER 0x149A21520 // ������
#define C_FATIGUE_MAX 0x149CCF1D0 // ���ƣ��
#define C_FATIGUE_CURRENT 0x149CCF1C8 // ��ǰƣ��
#define C_AREA_PARAM 0x149D21C58 // �������
#define C_AREA_CALL 0x144F542E0 // ����CALL
#define C_AREA_OFFSET 0x8A8B0 // ����ƫ��
#define C_COORDINATE_PASS_ROOM 0x143011940 // ����˳ͼCALL
#define C_COOL_DOWN_JUDGE_CALL 0x144083E10 // ��ȴ�ж�CALL
#define C_PACKET_SEND 0x149D22D50 // ������ַ
#define C_BUFFER_CALL 0x144FF3390 // ����CALL
#define C_PACKET_SEND_CALL 0x144FF3FA0 // ����CALL
#define C_ENCRYPT_CALL 0x // ����CALL
#define C_ENCRYPT_PACKET_CALL 0x144FF4150 // ���ܰ�CALL
#define C_ENCRYPT_PACKET_CALL2 0x144FF4490 // ���ܰ�CALL2
#define C_ENCRYPT_PACKET_CALL4 0x144FF4170 // ���ܰ�CALL4
#define C_ENCRYPT_PACKET_CALL8 0x144FF4190 // ���ܰ�CALL8
#define C_SUMMON_MONSTER 0x143C920A0 // �ٻ�����CALL
#define C_SUMMON_FIGURE 0x143CA0910 // �ٻ���żCALL
#define C_HIDDEN_CALL 0x145008F60 // ͸��CALL
#define C_GLOBAL_ADDRESS 0x143F5689E // ȫ�ֻ�ַ
#define C_CASTING_SPEED 0xD10 // �ͷ��ٶ�
#define C_ATTACK_SPEED 0xD08 // �����ٶ�
#define C_MOVE_SPEED 0xD00 // �ƶ��ٶ�
#define C_FLOAT_COOL_DOWN2 0x21A0 // ������ȴ2
#define C_USER_PRESTIGE 0xE3C4 // ��������
#define C_MOVEMENT_ID 0x56B0 // ����ID
#define C_MAP_HEAD 0x1C8 // ��ͼ��ʼ2
#define C_MAP_END 0x1D0 // ��ͼ����2
#define C_MAP_OFFSET 0x1D0 // ��ͼƫ��
#define C_TYPE_OFFSET 0x1A4 // ����ƫ��
#define C_CE_SCORE 0xC98 // C_E_����
#define C_MONSTER_BLOOD 0x6600 // ����Ѫ��
#define C_CAMP_OFFSET 0xDD0 // ��Ӫƫ��
#define C_CODE_OFFSET 0x848 // ����ƫ��
#define C_NAME_OFFSET 0x850 // ����ƫ��
#define C_PENETRATE_MAP 0x858 // ��ͼ��͸
#define C_PENETRATE_BUILDING 0x85C // ������͸
#define C_READ_COORDINATE 0x390 // ��ȡ����
#define C_OBJECT_COORDINATE 0x1B8 // ��������
#define C_DOOR_OFFSET 0x264 // �Ƿ���
#define C_CURRENT_ROOM_X 0x1BE0 // ��ǰ����X
#define C_CURRENT_ROOM_Y 0x1BE4 // ��ǰ����Y
#define C_BOSS_ROOM_X 0x1CEC // BOSS����X
#define C_BOSS_ROOM_Y 0x1CF4 // BOSS����Y
#define C_BONFIRE_JUDGE 0x1DA0 // �����ж�
#define C_MAP_CODE 0x1CD4 // ����ƫ��
#define C_DOOR_TYPE_OFFSET 0x148 // ����ƫ��
#define C_WH_OFFSET 0x7C0 // ���ƫ��
#define C_AISLE_OFFSET 0x7E0 // ����ƫ��
#define C_BEGIN_ROOM_X 0x150 // ��ʼ����X
#define C_BEGIN_ROOM_Y 0x154 // ��ʼ����Y
#define C_MAP_NAME 0x340 // ��ͼ����
#define C_PASS_ROOM_OFFSET 0xD0 // ˳ͼƫ��
#define C_COOR_CALL_OFFSET 0x1B0 // ����CALLƫ��
#define C_SKILL_LIST 0xB568 // ������
#define C_SKILL_LIST_OFFSET 0x90 // ������ƫ��
#define C_SHOE_OFFSET 0x5390 // Ь��ƫ��


////////////////////////[ �� �� �� ַ �� ��  ]///////////////////////////////////

#endif