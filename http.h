#pragma once
#include <httplib.h>
#include "json.h"

#ifndef HTTP_CONSTANT
#define HTTP_CONSTANT

// 服务器配置
#define HTTP_SERVER_DOMAIN "dragon.test"

// API 路径
#define API_BASE_ADDRESS "/api/base-address" // 基址API

#endif // HTTP_CONSTANT

static httplib::Client client(HTTP_SERVER_DOMAIN);

class HttpClient
{
public:
	HttpClient();

	// 获取基址
	json getBaseAddress();

	// 更新角色列表
	bool updateRoles(json data, bool is_favorite); 
	
	// 获取城镇坐标列表
	json getTownCoordinates();

	// 获取任务列表
	json getJobs();
};

extern HttpClient http;
