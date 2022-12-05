#include "pch.h"
#include "http.h"

HttpClient::HttpClient()
{
	// 初始化服务器
	// 配置HTTP客户端
	client.set_connection_timeout(0, 3000000); // 连接超时3秒
	// 配置请求头
	client.set_default_headers({ { "Accept", "application/json" } });
	client.set_keep_alive(true); // 长连接
}

HttpClient http;

json HttpClient::getBaseAddress()
{
	json response;
	if (auto res = client.Get("/api/base-address"))
	{
		if (res->status == 200)
		{
			std::string json_data = res->body;
			response = json::parse(json_data);
		}
	}
	return response;
}

bool HttpClient::updateRoles(json list, bool is_favorite)
{
	string url;
	if (is_favorite) {
		url = "/api/favorite-roles";
	}
	else {
		url = "/api/general-roles";
	}

	json response;
	string body = list.dump();
	if (auto res = client.Post(url, body, "application/json"))
	{
		if (res->status == 200)
		{
			return true;
		}
	}

	return false;
}