#pragma once
#include "easynet_constants.h"
#include "c_crypt.h"
#include "c_message.h"
#include "c_packet.h"
#include "c_timer.h"




namespace easynet
{
	class c_session;
	typedef std::function<void(c_session*, easynet::core::c_message)> on_message_received_t;
	typedef std::function<void(c_session*, bool success)> on_key_exchange_t;
	class c_session
	{
	public:
		inline c_session(const int p_client_socket = 0, bool p_created_by_server = false) : m_client_socket(p_client_socket), m_created_by_server(p_created_by_server) {}


	public:
		void create_crypt();
		std::string get_ip_address();
		float get_ping();
		void disconnect();
		bool is_disconnected();
		void update_ping();
		void reset_ping();
		bool should_send_ping();
		

	public:
		void send_packet(core::c_packet message);
		void send_message(uint16_t message_opcode, data::c_buffer buffer);
		void send_data( data::c_buffer message, packet_type_opcode opcode);
		void send_encrypted(packet_type_opcode opcode, data::c_buffer message);

		void read_packet();
		void read_data();
		void handle();

	private:
		uint64_t m_id = 0;
		int m_client_socket = 0;
		bool m_stop = false;
		bool m_created_by_server = false;

	public: // data handlers
		on_message_received_t on_message_received_fn = 0;
		on_key_exchange_t on_key_exchange_fn = 0;
		std::shared_ptr<core::c_crypt> m_crypt;
		bool m_connection_established = false;

	private:
		friend class c_tcpclient;
		friend class c_tcpserver;
		std::deque<easynet::core::c_packet> m_recv_packets;
		std::deque<easynet::core::c_packet> m_send_packets;

		data::c_buffer m_received_buffer;
		BYTE* read_buffer = nullptr;


		std::thread m_recv_thread;
		std::thread m_packet_thread;
	private:
		util::c_timer m_ping_timer;
		float m_ping = 0.0f;

		std::mutex mtx;


	private:
		std::string ip_address = "";


	};
}
