#include "c_session.h"

#include "c_message.h"
#include "packet_core_handler.h"

void easynet::c_session::create_crypt()
{
	m_crypt = std::make_shared<core::c_crypt>();
}

std::string easynet::c_session::get_ip_address()
{
	return ip_address;
}

float easynet::c_session::get_ping()
{
	return m_ping;
}

void easynet::c_session::disconnect()
{
	m_stop = true;
}

bool easynet::c_session::is_disconnected()
{
	return m_stop;
}

void easynet::c_session::update_ping()
{
	m_ping = GetTickCount64() - m_ping_timer.start_time() - 5000;
}

void easynet::c_session::reset_ping()
{
	m_ping_timer.reset();
}

bool easynet::c_session::should_send_ping()
{
	return m_ping_timer.time_elapsed() > 5000;
}

void easynet::c_session::send_packet(core::c_packet message)
{
	auto header = message.write_header();
	auto buffer = message.body;

	this->m_send_packets.push_back(message);
}

void easynet::c_session::send_message(uint16_t message_opcode, data::c_buffer buffer)
{
	core::c_message message;

	message.message_opcode = message_opcode;
	message.buffer = buffer;

	send_data(message.write(), packet_type_opcode::message);
}

void easynet::c_session::send_data(data::c_buffer message, packet_type_opcode opcode)
{
	core::c_packet packet;
	packet.body = message;
	packet.opcode = (int8_t)opcode;
	packet.create_packet();

	send_packet(packet);
}

void easynet::c_session::send_encrypted(packet_type_opcode opcode, data::c_buffer message)
{
	if (m_crypt && m_crypt->AES_KEY) {
		bool res = false;
		data::c_buffer core_struct;

		core_struct.write_uint8((uint8_t)opcode);
		core_struct.write_bytes(message, false);

		auto encrypted_data = m_crypt->encrypt(core_struct, res);

		if (res)
		{
			data::c_buffer encrypted_message;
			encrypted_message.write_int8(1);
			encrypted_message.write_bytes(encrypted_data);

			send_data(encrypted_message, packet_type_opcode::encrypted_message);
		}
	}
}

void easynet::c_session::read_packet()
{
	while (!is_disconnected()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		easynet::core::c_packet packet;

		if (m_received_buffer.size() >= 7)
		{
			data::c_buffer buffer;

			buffer.write_bytes(m_received_buffer, false);

			core::c_packet packet;
			if (buffer.read_uint16(packet.net_version) && buffer.read_uint((packet.body_length)) && buffer.read_uint8(packet.opcode)) {
				packet.body_length -= 0x9A2112;

				if (packet.body_length == 0)
				{
					m_received_buffer.remove(7);

					packet.is_valid = true;

					m_recv_packets.push_back(packet);
				}
				else if (packet.body_length <= m_received_buffer.size() - 7)
				{
					buffer.read_bytes(packet.body, packet.body_length);
					m_recv_packets.push_back(packet);

					packet.is_valid = true;

					m_received_buffer.remove(packet.body_length + 7);
				}
			}
		}
	}
}

void easynet::c_session::read_data()
{
	while (!m_client_socket)
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	size_t buf_size = 1024 * 1024;
	read_buffer = new BYTE[buf_size];

	while (!is_disconnected()) {
		ZeroMemory(read_buffer, buf_size);
		int bytes_received = recv(m_client_socket, (char*)read_buffer, buf_size, 0);
		if (bytes_received <= 0) {
			m_stop = true;
		}
		else {
			m_received_buffer.write_bytes((uint8_t*)read_buffer, bytes_received);
		}
	}
}

void easynet::c_session::handle()
{
	m_recv_thread = std::thread([this] {this->read_data(); });
	m_packet_thread = std::thread([this] {this->read_packet(); });
	while (!m_stop)
	{
		while (!m_recv_packets.empty())
		{
			auto m_packet = m_recv_packets.front();

			if (m_created_by_server) core::handle_packet_server(this, m_packet);
			else core::handle_packet_client(this, m_packet);
			m_recv_packets.pop_front();
		}
	}
}