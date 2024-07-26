#pragma once
#include "c_session.h"






namespace easynet
{
	class c_tcpserver
	{
	public:
		inline c_tcpserver(uint16_t p_server_port = 57006) : m_server_port ( p_server_port ){}

		bool start();

		void stop_server();
		on_message_received_t on_message_received_fn = 0;
		on_key_exchange_t on_key_exchange_fn = 0;
	private:
		SOCKET m_server_socket = 0;
		uint16_t m_server_port = 57006;
		bool m_stop = false;

		uint64_t m_connection_count = 0;
		struct sockaddr_in m_server_addr;

	private:
		std::unordered_map<uint64_t, std::shared_ptr<c_session>> m_sessions;

		data::c_buffer m_rsa_bytes;
		core::c_crypt m_server_crypt;



	private:
		std::thread m_data_thread;
		std::thread m_recv_thread;
		std::thread m_send_thread;
		std::thread m_worker_thread;

		void run();
		void handle_data();
		void send_handler();
		void recv_handler();

		void on_server_client_connected(std::shared_ptr<c_session> session);

	};
}