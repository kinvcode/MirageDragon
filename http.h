#pragma once
#include <httplib.h>
#include "json.h"

#ifndef HTTP_CONSTANT
#define HTTP_CONSTANT

// 服务器配置
#define HTTP_SERVER_DOMAIN "dragon.kinvcode.com"

// API 路径
#define API_BASE_ADDRESS "/api/base-address" // 基址API

#endif // HTTP_CONSTANT

static httplib::Client client(HTTP_SERVER_DOMAIN);

class HttpClient
{
public:
	HttpClient();
	json getBaseAddress();
	bool updateRoles(json data);
};

extern HttpClient http;
