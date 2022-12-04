#pragma once
#include <httplib.h>
#include "json.h"

#ifndef HTTP_CONSTANT
#define HTTP_CONSTANT

// ����������
#define HTTP_SERVER_DOMAIN "https://dragon.kinvcode.com"

// API ·��
#define API_BASE_ADDRESS "/api/base-address" // ��ַAPI

#endif // HTTP_CONSTANT


class HttpClient
{
public:
	HttpClient(const std::string& path);
	json response;
};
