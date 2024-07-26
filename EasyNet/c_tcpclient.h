#pragma once
#include "c_session.h"

namespace easynet
{
	class c_tcpclient
	{
	public:
		inline c_tcpclient(const std::string p_server_address, const int p_port) : m_server_ip(p_server_address), m_server_port(p_port) {}

	public:
		bool connect_to_server();
		void send_packet(core::c_packet message);
		void send_data(data::c_buffer message, packet_type_opcode opcode);
		void send_encrypted(packet_type_opcode opcode, data::c_buffer message);

	public:
		on_message_received_t on_message_received_fn = 0;
		on_key_exchange_t on_key_exchange_fn = 0;
	private:
		std::string m_server_ip = "127.0.0.1";
		uint16_t m_server_port = 57008;
		SOCKET m_client_socket = 0;
		struct sockaddr_in m_server_addr;

	private:
		std::shared_ptr<c_session> m_session;
		std::thread m_send_thread;
		std::thread m_handle_thread;

		void send_handler();
	};
}
