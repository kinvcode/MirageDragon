#pragma once

__int64 getUserPointer(__int64 emptyAddress);

wstring getMapName();

int getMapNumber();

COORDINATE readCoordinate(__int64 address);

vector<DUNGEONOBJ> getDungeonAllObj();

void getMonsterAndItems();

COORDINATE getBossRoom();

COORDINATE getCurrentRoom();

bool judgeClearance();

bool judgeDoorOpen();