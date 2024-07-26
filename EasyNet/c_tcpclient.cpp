#include "c_tcpclient.h"

bool easynet::c_tcpclient::connect_to_server()
{
	if ((m_client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		return false;
	}

	memset(&m_server_addr, 0, sizeof(m_server_addr));
	m_server_addr.sin_family = AF_INET;
	m_server_addr.sin_port = htons(m_server_port);

	// Convert IP address from string to binary
	if (inet_pton(AF_INET, m_server_ip.c_str(), &m_server_addr.sin_addr) <= 0) {
		//   LOG_ERR("Invalid address");
		closesocket(m_client_socket);
		return false;
	}

	// Connect to the server
	if (connect(m_client_socket, (struct sockaddr*)&m_server_addr, sizeof(m_server_addr)) == -1) {
		//  LOG_ERR("Connection failed " + std::string(strerror(errno)));

		closesocket(m_client_socket);
		return false;
	}
	m_session = std::make_shared<c_session>(m_client_socket);

	m_session.get()->on_message_received_fn = on_message_received_fn;

	m_session.get()->on_key_exchange_fn = on_key_exchange_fn;

	m_session.get()->m_crypt = std::make_shared< core::c_crypt>();

	m_session.get()->m_crypt->generate_aes_key();

	m_send_thread = std::thread([this] {this->send_handler(); });

	m_handle_thread = std::thread(&c_session::handle, m_session);

	return true;
}

void easynet::c_tcpclient::send_packet(core::c_packet message)
{
	m_session->send_packet(message);
}

void easynet::c_tcpclient::send_data(data::c_buffer message, packet_type_opcode opcode)
{
	m_session->send_data(message, opcode);
}

void easynet::c_tcpclient::send_encrypted(packet_type_opcode opcode, data::c_buffer message)
{
	m_session->send_encrypted(opcode, message);
}

void easynet::c_tcpclient::send_handler()
{
	while (!m_session->is_disconnected())
	{
		while (!m_session->m_send_packets.empty()) {
			auto packet = m_session->m_send_packets.front();

			auto header = packet.write_header();
			auto buffer = packet.body;

			send(m_client_socket, (char*)header.buffer(), header.size(), 0);
			send(m_client_socket, (char*)buffer.buffer(), buffer.size(), 0);

			m_session->m_send_packets.pop_front();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}