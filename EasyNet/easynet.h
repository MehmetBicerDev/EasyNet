#pragma once
#include "c_tcpclient.h"
#include "c_tcpserver.h"
#include "net_config.h"

namespace easynet
{
	void initialize(net_config cfg);
	net_config get_config();

	bool start(on_message_received_t message_callback, on_key_exchange_t keyexchange_callback);
}