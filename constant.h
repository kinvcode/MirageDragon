#pragma once

#ifndef CONSTANT_H
#define CONSTANT_H

extern __int64 C_USER;
extern __int64 C_USER_POINTER;
////////////////////////[ 七 度 基 址 区 域  ]///////////////////////////////////

#define C_USER_ADDRESS 0x149CCF208 // 人物基址
#define C_USER_CALL 0x144098F70 // 人物CALL
#define C_USER_LEVEL 0x149A07A20 // 角色等级
#define C_SCORE_ADDRESS 0x149A20218 // 评分基址
#define C_TOWN_WORLD 0x1499E442C // 城镇大区域
#define C_TOWN_AREA 0x1499E4430 // 城镇小区域
#define C_TOWN_X 0x149A22738 // 城镇坐标X
#define C_TOWN_Y 0x149A2273C // 城镇坐标Y
#define C_GAME_STATUS 0x1496FB710 // 游戏状态
#define C_EMPTY_ADDRESS 0x13FC804C0 // 全局空白
#define C_ASM_CALL 0x13FDA0000 // 汇编CALL
#define C_SKILL_CALL 0x143BCBD00 // 技能CALL
#define C_AUTO_PICKUP 0x143FA1AF3 // 自动捡物
#define C_MAP_SELECTED 0x149D17500 // 选图基址
#define C_MAP2_SELECTED 0x149A212C0 // 选图基址2
#define C_MAP_CODE_SELECTED 0x3B0 // 选中地图编号
#define C_MAP_DIFFICULTY_SELECTED 0x2B8 // 选中地图难度
#define C_DIALOGUE 0x149D1D6B0 // 对话基址
#define C_DIALOGUE_B 0x149A2235C // 对话基址B
#define C_DIALOGUE_ESC 0x149A223BC // Esc对话基址
#define C_MAP_NUMBER 0x149A079B0 // 副本编号
#define C_TIME_ADDRESS 0x20A050 // 时间基址
#define C_ROOM_NUMBER 0x149A21520 // 房间编号
#define C_FATIGUE_MAX 0x149CCF1D0 // 最大疲劳
#define C_FATIGUE_CURRENT 0x149CCF1C8 // 当前疲劳
#define C_AREA_PARAM 0x149D21C58 // 区域参数
#define C_AREA_CALL 0x144F542E0 // 区域CALL
#define C_AREA_OFFSET 0x8A8B0 // 区域偏移
#define C_COORDINATE_PASS_ROOM 0x143011940 // 坐标顺图CALL
#define C_COOL_DOWN_JUDGE_CALL 0x144083E10 // 冷却判断CALL
#define C_PACKET_SEND 0x149D22D50 // 发包基址
#define C_BUFFER_CALL 0x144FF3390 // 缓冲CALL
#define C_PACKET_SEND_CALL 0x144FF3FA0 // 发包CALL
#define C_ENCRYPT_CALL 0x // 加密CALL
#define C_ENCRYPT_PACKET_CALL 0x144FF4150 // 加密包CALL
#define C_ENCRYPT_PACKET_CALL2 0x144FF4490 // 加密包CALL2
#define C_ENCRYPT_PACKET_CALL4 0x144FF4170 // 加密包CALL4
#define C_ENCRYPT_PACKET_CALL8 0x144FF4190 // 加密包CALL8
#define C_SUMMON_MONSTER 0x143C920A0 // 召唤怪物CALL
#define C_SUMMON_FIGURE 0x143CA0910 // 召唤人偶CALL
#define C_HIDDEN_CALL 0x145008F60 // 透明CALL
#define C_GLOBAL_ADDRESS 0x143F5689E // 全局基址
#define C_CASTING_SPEED 0xD10 // 释放速度
#define C_ATTACK_SPEED 0xD08 // 攻击速度
#define C_MOVE_SPEED 0xD00 // 移动速度
#define C_FLOAT_COOL_DOWN2 0x21A0 // 浮点冷却2
#define C_USER_PRESTIGE 0xE3C4 // 人物名望
#define C_MOVEMENT_ID 0x56B0 // 动作ID
#define C_MAP_HEAD 0x1C8 // 地图开始2
#define C_MAP_END 0x1D0 // 地图结束2
#define C_MAP_OFFSET 0x1D0 // 地图偏移
#define C_TYPE_OFFSET 0x1A4 // 类型偏移
#define C_CE_SCORE 0xC98 // C_E_评分
#define C_MONSTER_BLOOD 0x6600 // 怪物血量
#define C_CAMP_OFFSET 0xDD0 // 阵营偏移
#define C_CODE_OFFSET 0x848 // 代码偏移
#define C_NAME_OFFSET 0x850 // 名称偏移
#define C_PENETRATE_MAP 0x858 // 地图穿透
#define C_PENETRATE_BUILDING 0x85C // 建筑穿透
#define C_READ_COORDINATE 0x390 // 读取坐标
#define C_OBJECT_COORDINATE 0x1B8 // 对象坐标
#define C_DOOR_OFFSET 0x264 // 是否开门
#define C_CURRENT_ROOM_X 0x1BE0 // 当前房间X
#define C_CURRENT_ROOM_Y 0x1BE4 // 当前房间Y
#define C_BOSS_ROOM_X 0x1CEC // BOSS房间X
#define C_BOSS_ROOM_Y 0x1CF4 // BOSS房间Y
#define C_BONFIRE_JUDGE 0x1DA0 // 篝火判断
#define C_MAP_CODE 0x1CD4 // 索引偏移
#define C_DOOR_TYPE_OFFSET 0x148 // 门型偏移
#define C_WH_OFFSET 0x7C0 // 宽高偏移
#define C_AISLE_OFFSET 0x7E0 // 数组偏移
#define C_BEGIN_ROOM_X 0x150 // 起始坐标X
#define C_BEGIN_ROOM_Y 0x154 // 起始坐标Y
#define C_MAP_NAME 0x340 // 地图名称
#define C_PASS_ROOM_OFFSET 0xD0 // 顺图偏移
#define C_COOR_CALL_OFFSET 0x1B0 // 坐标CALL偏移
#define C_SKILL_LIST 0xB568 // 技能栏
#define C_SKILL_LIST_OFFSET 0x90 // 技能栏偏移
#define C_SHOE_OFFSET 0x5390 // 鞋子偏移


////////////////////////[ 心 悦 基 址 区 域  ]///////////////////////////////////

#endif