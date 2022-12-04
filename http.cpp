#include "pch.h"
#include "http.h"

HttpClient::HttpClient(const std::string& path)
{
	// 初始化服务器
	httplib::Client client(HTTP_SERVER_DOMAIN);

	// 配置HTTP客户端
	client.set_connection_timeout(0, 3000000); // 连接超时3秒
	// 配置请求头
	client.set_default_headers({ { "Accept", "application/json" } });
	client.set_keep_alive(true); // 长连接

	if (auto res = client.Get(path))
	{
		if (res->status == 200)
		{
			std::string json_data = res->body;
			response = json::parse(json_data);
		}
	}

}