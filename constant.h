#pragma once

#ifndef CONSTANT_H
#define CONSTANT_H

extern __int64 C_USER;
extern __int64 C_USER_POINTER;
////////////////////////[ 七 度 基 址 区 域  ]///////////////////////////////////

#define C_USER_ADDRESS 0x1498916F0 // 人物基址
#define C_USER_CALL 0x143E8FD60 // 人物CALL
#define C_USER_LEVEL 0x1495C9CB0 // 角色等级
#define C_SCORE_ADDRESS 0x1495E2728 // 评分基址
#define C_TOWN_WORLD 0x1495A693C // 城镇大区域
#define C_TOWN_AREA 0x1495A6940 // 城镇小区域
#define C_TOWN_X 0x1495E4DE8 // 城镇坐标X
#define C_TOWN_Y 0x1495E4DEC // 城镇坐标Y
#define C_GAME_STATUS 0x1492C46A4 // 游戏状态
#define C_EMPTY_ADDRESS 0x13FCA04C0 // 全局空白
#define C_ASM_CALL 0x13FDC0000 // 汇编CALL
#define C_SKILL_CALL 0x1439CD830 // 技能CALL
#define C_AUTO_PICKUP 0x143D9D4F3 // 自动捡物
#define C_MAP_SELECTED 0x1498D7F30 // 选图基址
#define C_MAP2_SELECTED 0x1495E37E0 // 选图基址2
#define C_MAP_CODE_SELECTED 0x3B0 // 选中地图编号
#define C_MAP_DIFFICULTY_SELECTED 0x2C0 // 选中地图难度
#define C_DIALOGUE 0x1498DDFC0 // 对话基址
#define C_DIALOGUE_B 0x1495E4A0C // 对话基址B
#define C_DIALOGUE_ESC 0x1495E4A6C // Esc对话基址
#define C_MAP_NUMBER 0x1495C9C40 // 副本编号
#define C_TIME_ADDRESS 0x20A050 // 时间基址
#define C_ROOM_NUMBER 0x1495E3B90 // 房间编号
#define C_FATIGUE_MAX 0x1498916B8 // 最大疲劳
#define C_FATIGUE_CURRENT 0x1498916B0 // 当前疲劳
#define C_AREA_PARAM 0x1498E2168 // 区域参数
#define C_AREA_CALL 0x144D3A820 // 区域CALL
#define C_AREA_OFFSET 0x84988 // 区域偏移
#define C_COORDINATE_PASS_ROOM 0x142E12F30 // 坐标顺图CALL
#define C_COOL_DOWN_JUDGE_CALL 0x143E7BE00 // 冷却判断CALL
#define C_PACKET_SEND 0x1498E3418 // 发包基址
#define C_BUFFER_CALL 0x144DD5DD0 // 缓冲CALL
#define C_PACKET_SEND_CALL 0x144DD77C0 // 发包CALL
#define C_ENCRYPT_CALL 0x144DD8187 // 加密CALL
#define C_SUMMON_MONSTER 0x143A91D30 // 召唤怪物CALL
#define C_SUMMON_FIGURE 0x143AA0850 // 召唤人偶CALL
#define C_HIDDEN_CALL 0x144DEB0D0 // 透明CALL
#define C_GLOBAL_ADDRESS 0x143D539EE // 全局基址
#define C_CASTING_SPEED 0xD10 // 释放速度
#define C_ATTACK_SPEED 0xD08 // 攻击速度
#define C_MOVE_SPEED 0xD00 // 移动速度
#define C_USER_PRESTIGE 0x30E0 // 冒险家名望
#define C_FLOAT_COOL_DOWN2 0x2288 // 浮点冷却2
#define C_MOVEMENT_ID 0x5378 // 动作ID
#define C_MAP_HEAD 0x1C8 // 地图开始2
#define C_MAP_END 0x1D0 // 地图结束2
#define C_MAP_OFFSET 0x200 // 地图偏移
#define C_TYPE_OFFSET 0x1D4 // 类型偏移
#define C_CE_SCORE 0xC98 // C_E_评分
#define C_MONSTER_BLOOD 0x5F68 // 怪物血量
#define C_CAMP_OFFSET 0xE20 // 阵营偏移
#define C_CODE_OFFSET 0x8C0 // 代码偏移
#define C_NAME_OFFSET 0x8C8 // 名称偏移
#define C_PENETRATE_MAP 0x8D0 // 地图穿透
#define C_PENETRATE_BUILDING 0x8D4 // 建筑穿透
#define C_READ_COORDINATE 0x3C0 // 读取坐标
#define C_OBJECT_COORDINATE 0x1E8 // 对象坐标
#define C_DOOR_OFFSET 0x264 // 是否开门
#define C_CURRENT_ROOM_X 0x1BD8 // 当前房间X
#define C_CURRENT_ROOM_Y 0x1BDC // 当前房间Y
#define C_BOSS_ROOM_X 0x1CE4 // BOSS房间X
#define C_BOSS_ROOM_Y 0x1CEC // BOSS房间Y
#define C_BONFIRE_JUDGE 0x1DA8 // 篝火判断
#define C_MAP_CODE 0x1CCC // 索引偏移
#define C_DOOR_TYPE_OFFSET 0x178 // 门型偏移
#define C_WH_OFFSET 0x7A0 // 宽高偏移
#define C_AISLE_OFFSET 0x7C0 // 数组偏移
#define C_BEGIN_ROOM_X 0x180 // 起始坐标X
#define C_BEGIN_ROOM_Y 0x184 // 起始坐标Y
#define C_MAP_NAME 0x320 // 地图名称
#define C_PASS_ROOM_OFFSET 0xF0 // 顺图偏移
#define C_COOR_CALL_OFFSET 0x1C8 // 坐标CALL偏移
#define C_SKILL_LIST 0xAB00 // 技能栏
#define C_SKILL_LIST_OFFSET 0x90 // 技能栏偏移
#define C_SHOE_OFFSET 0x5288 // 鞋子偏移


////////////////////////[ 心 悦 基 址 区 域  ]///////////////////////////////////

#endif