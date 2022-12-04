#pragma once
#include <httplib.h>
#include "json.h"

#ifndef HTTP_CONSTANT
#define HTTP_CONSTANT

// 服务器配置
#define HTTP_SERVER_DOMAIN "https://dragon.kinvcode.com"

// API 路径
#define API_BASE_ADDRESS "/api/base-address" // 基址API

#endif // HTTP_CONSTANT


class HttpClient
{
public:
	HttpClient(const std::string& path);
	json response;
};
