#pragma once
#include <tlhelp32.h>
#include <windef.h>
#include <vector>

int getProcessPID(wchar_t Name[]);

int asSystem(int Pid, char FileName[]);

void asSystemRunning();

void asDNFUserRunning();

bool windowIsTop();

bool programDelay(int time, int delayUnit);

void handleEvents();

void reloadProcess();