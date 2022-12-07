#pragma once

__int64 passRoomData(int direction);

int judgeNextRoomDiretion(ROOMCOOR current, ROOMCOOR boss);

void updateRooms();

vector<ROOMDIRECT> mazeBFS(vector<vector<int>> mat, ROOMCOOR src, ROOMCOOR dest, int width, int height);

int mapCodeOfSelected();

int mapDifficultyOfSelected();
