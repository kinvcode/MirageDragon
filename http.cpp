#include "pch.h"
#include "http.h"

HttpClient::HttpClient(const std::string& path)
{
	// ��ʼ��������
	httplib::Client client(HTTP_SERVER_DOMAIN);

	// ����HTTP�ͻ���
	client.set_connection_timeout(0, 3000000); // ���ӳ�ʱ3��
	// ��������ͷ
	client.set_default_headers({ { "Accept", "application/json" } });
	client.set_keep_alive(true); // ������

	if (auto res = client.Get(path))
	{
		if (res->status == 200)
		{
			std::string json_data = res->body;
			response = json::parse(json_data);
		}
	}

}