#include "easynet.h"

namespace easynet
{
	net_config easyconfig;

	c_tcpserver* easy_server = nullptr;
	c_tcpclient* easy_client = nullptr;
	void initialize(net_config cfg)
	{
		WSADATA data;
		WORD ver = MAKEWORD(2, 2);
		int wsResult = WSAStartup(ver, &data);

		easyconfig = cfg;

		if (cfg.direction == net_direction::server)
			easy_server = new c_tcpserver(cfg.direction_port);
		else if (cfg.direction == net_direction::client)
			easy_client = new c_tcpclient(cfg.server_ip, cfg.direction_port);
	}

	net_config get_config() { return easyconfig; }

	bool start(on_message_received_t message_callback, on_key_exchange_t keyexchange_callback)
	{
		if (easyconfig.direction == net_direction::server && message_callback)
		{
			easy_server->on_message_received_fn = message_callback;
			return easy_server->start();
		}
		else if (easyconfig.direction == net_direction::client && message_callback && keyexchange_callback)
		{
			easy_client->on_message_received_fn = message_callback;
			easy_client->on_key_exchange_fn = keyexchange_callback;

			return easy_client->connect_to_server();
		}

		return false;
	}
}