#include "pch.h"
#include "http.h"
#include "dnfData.h"
#include "baseAddress.h"

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

	string body = list.dump();
	if (auto res = client.Post(url, body, "application/json"))
	{
		if (res->status == 200)
		{
			return true;
		}
	}
	list.clear();
	return false;
}

json HttpClient::getTownCoordinates()
{
	json response;
	if (auto res = client.Get("/api/town-coordinates"))
	{
		if (res->status == 200)
		{
			std::string json_data = res->body;
			response = json::parse(json_data);
		}
	}
	return response;
}

json HttpClient::getJobs()
{
	json response;
	ostringstream tmpStr;
	int qq = getQQAccount();
	CString server = readString(readLong(C_SERVER_AREA), 20);
	string utf8_server = DataConvert::Utf16ToUtf8(server);
	tmpStr << "/api/account/" << qq << "/jobs?server=";
	string path = tmpStr.str();
	path = path + utf8_server;
	if (auto res = client.Get(path))
	{
		if (res->status == 200)
		{
			std::string json_data = res->body;
			response = json::parse(json_data);
		}
	}
	return response;
}

/**
 * account	QQ号
 * name		角色名称
 * ID		角色ID
 */
bool HttpClient::updateRoleID(int account, CString name, int id)
{

	json params;
	params["account"] = account;
	params["name"] = DataConvert::Utf16ToUtf8(name);
	params["id"] = id;
	string body = params.dump();
	if (auto res = client.Post("/api/role/id", body, "application/json"))
	{
		if (res->status == 200)
		{
			return true;
		}
	}

	return false;
}

bool HttpClient::updateJob(int job_type)
{
	json params;
	params["type"] = job_type;
	string body = params.dump();

	ostringstream tmpStr;
	int qq = getQQAccount();
	tmpStr << "/api/account/" << qq << "/jobs";
	string path = tmpStr.str();

	if (auto res = client.Post(path, body, "application/json"))
	{
		if (res->status == 200)
		{
			return true;
		}
	}

	return false;
}