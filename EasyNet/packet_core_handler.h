#pragma once
#include "easynet_constants.h"
#include "c_session.h"

namespace easynet
{
	namespace core
	{
		void handle_packet_client(c_session* session, c_packet packet);
		void handle_packet_server(c_session* session, c_packet packet);
	}
}
