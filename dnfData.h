#pragma once

// ��ȡ�û�ָ��
__int64 getUserPointer(__int64 emptyAddress);

// ��ȡ��������
wstring getMapName();

// ��ȡ�������
int getMapNumber();

// ��ȡ����
COORDINATE readCoordinate(__int64 address);

// ������������
void updateUserCoor();

// ���µ�ǰ���������ж�����Ϣ
void getDungeonAllObj();

void gatherAtUser(COORDINATE user, DUNGEONOBJ item);

// ��ʼ��������Ϣ
void initDungeonInfo();

// ���µ�ǰ����
void updateCurrentRoom();

// �ж��Ƿ�ͨ��
bool judgeClearance();

// �ж��Ƿ���
bool judgeDoorOpen();

// �ж��Ƿ���boss����
bool judgeIsBossRoom();