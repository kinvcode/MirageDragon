#include "pch.h"
#include "http.h"

HttpClient::HttpClient()
{
	// ��ʼ��������
	// ����HTTP�ͻ���
	client.set_connection_timeout(0, 3000000); // ���ӳ�ʱ3��
	// ��������ͷ
	client.set_default_headers({ { "Accept", "application/json" } });
	client.set_keep_alive(true); // ������
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

bool HttpClient::updateRoles(json data) 
{
	json response;
	string body = data.dump();
	if (auto res = client.Post("/api/roles", body, "application/json"))
	{
		if (res->status == 200)
		{
			std::string json_data = res->body;
			response = json::parse(json_data);
		}
	}

	return false;
}