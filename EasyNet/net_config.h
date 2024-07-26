#pragma once
#include "easynet_constants.h"









namespace easynet
{
	class net_config
	{
	public:
		net_config() = default;


		net_direction direction = net_direction::none;
		std::string server_ip = "127.0.0.1";
		uint16_t direction_port = 54007;


		bool use_keyexchange = false;
	};

	
}