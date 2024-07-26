#include "c_tcpserver.h"

#include "c_server_connection_settings.h"
#include "packet_core_handler.h"

bool easynet::c_tcpserver::start()
{
	if ((m_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		return false;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(m_server_port);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(m_server_socket, (struct sockaddr*)&hint, sizeof(hint)) == -1) {
		return false;
	}

	if (listen(m_server_socket, SOMAXCONN) == -1) {
		return false;
	}

	m_server_crypt.generate_key();

	m_rsa_bytes = m_server_crypt.get_rsa_public_key();

	m_worker_thread = std::thread([this] { this->run(); });

	m_data_thread = std::thread([this] {this->handle_data(); });

	m_recv_thread = std::thread([this] {this->recv_handler(); });

	m_send_thread = std::thread([this] {this->send_handler(); });

	return true;
}

void easynet::c_tcpserver::stop_server()
{
}

void easynet::c_tcpserver::run()
{
	size_t buf_size = 101000;
	char* read_buffer = new char[buf_size];

	fd_set master;

	auto base_server_socket = m_server_socket;
	FD_ZERO(&master);

	FD_SET(base_server_socket, &master);

	while (!m_stop) {
		fd_set copy = master;

		int count = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < count; i++) {
			SOCKET csock = copy.fd_array[i];

			if (csock == base_server_socket) {
				SOCKET connected_client = accept(base_server_socket, 0, 0);

				auto session = std::make_shared<c_session>(connected_client);

				session->m_client_socket = connected_client;

				on_server_client_connected(session);

				m_sessions[connected_client] = (session);

				FD_SET(connected_client, &master);
			}
			else {
				if (m_sessions.find(csock) == m_sessions.end())
				{
					closesocket(csock);
					FD_CLR(csock, &master);

					continue;
				}
				auto session = m_sessions.at(csock);

				session->mtx.lock();
				ZeroMemory(read_buffer, buf_size);

				int bytes_received = recv(csock, read_buffer, buf_size, 0);

				if (bytes_received <= 0) {
					session->m_stop = true;
					closesocket(csock);
					FD_CLR(csock, &master);
				}
				else {
					session->m_received_buffer.write_bytes((const uint8_t*)read_buffer, bytes_received);
				}
				session->mtx.unlock();
			}
		}
	}

	delete[] read_buffer;
	read_buffer = nullptr;
}

void easynet::c_tcpserver::handle_data()
{
	while (!m_stop)
	{
		for (auto& _sessiontree : m_sessions)
		{
			auto _session = _sessiontree.second;
			try {
				if (_session->mtx.try_lock()) {
					if (_session->is_disconnected()) {
						m_sessions.erase(_sessiontree.first);
						continue;
					}

					if (_session->m_received_buffer.size() >= 7)
					{
						data::c_buffer buffer;

						buffer.write_bytes(_session->m_received_buffer, false);

						core::c_packet packet;

						if (buffer.read_uint16(packet.net_version) && buffer.read_uint(packet.body_length) && buffer.read_uint8(packet.opcode))
						{
							packet.body_length -= 0x9A2112;

							if (packet.body_length == 0)
							{
								_session->m_received_buffer.remove(7);
								_session->m_recv_packets.push_back(packet);
							}
							else if (packet.body_length <= _session->m_received_buffer.size() - 7)
							{
								buffer.read_bytes(packet.body, packet.body_length);
								_session->m_recv_packets.push_back(packet);
								_session->m_received_buffer.remove(packet.body_length + 7);
							}
						}
					}

					_session->mtx.unlock();
				}
			}
			catch (std::exception ex)
			{
			}
		}
	}
}

void easynet::c_tcpserver::send_handler()
{
	while (!m_stop)
	{
		for (auto& _sessiontree : m_sessions)
		{
			auto _session = _sessiontree.second;

			if (_session->is_disconnected()) continue;
			while (!_session->m_send_packets.empty())
			{
				auto packet = _session->m_send_packets.front();

				auto header = packet.write_header();
				auto buffer = packet.body;

				send(_session->m_client_socket, (char*)header.buffer(), header.size(), 0);
				send(_session->m_client_socket, (char*)buffer.buffer(), buffer.size(), 0);

				_session->m_send_packets.pop_front();
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void easynet::c_tcpserver::recv_handler()
{
	while (!m_stop)
	{
		for (auto& _sessiontree : m_sessions)
		{
			auto _session = _sessiontree.second;

			if (_session->is_disconnected()) continue;
			while (!_session->m_recv_packets.empty())
			{
				auto packet = _session->m_recv_packets.front();

				core::handle_packet_server(_session.get(), packet);

				_session->m_recv_packets.pop_front();
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void easynet::c_tcpserver::on_server_client_connected(std::shared_ptr<c_session> session)
{
	++m_connection_count;

	session->m_id = m_connection_count;
	session->create_crypt();

	session->m_crypt->rsa = m_server_crypt.rsa;

	session->on_message_received_fn = on_message_received_fn;

	session->on_key_exchange_fn = on_key_exchange_fn;

	session->m_created_by_server = true;

	core::c_server_connection_settings settings;

	settings.rsa_encryption_key = m_rsa_bytes;

	session->send_data(settings.write(), packet_type_opcode::server_connection_settings);
}