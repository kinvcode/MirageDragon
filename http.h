#pragma once
#include <httplib.h>
#include "json.h"

#ifndef HTTP_CONSTANT
#define HTTP_CONSTANT

// ����������
#define HTTP_SERVER_DOMAIN "dragon.kinvcode.com"

// API ·��
#define API_BASE_ADDRESS "/api/base-address" // ��ַAPI

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
