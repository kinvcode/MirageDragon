#pragma once

__int64 passRoomData(int direction);

int judgeNextRoomDiretion(ROOMCOORDINATE current, ROOMCOORDINATE boss);

DUNGEONMAP mapData();

int getWay(vector<int> aisle, int width, int height, COORDINATE begin, COORDINATE end, vector<COORDINATE>& way);

void createMap(int width, int height, vector<int> aisle, vector<vector<AISLEDATA>>& map);

void showMap(vector<vector<AISLEDATA>> map, int width, int height, vector<vector<AISLEDATA>>& tag);

bool judgeDirection(int aisle, int direction);

void pathCalc(vector<vector<AISLEDATA>> map_tag, COORDINATE begin, COORDINATE end, int width, int height, vector<COORDINATE>& cross_way);

int arrangeCoor(vector<COORDINATE>imitation, vector<COORDINATE>& real_cross);

int mapCodeOfSelected();

int mapDifficultyOfSelected();
