#include "packet_core_handler.h"

//packets
#include "c_clientkeyexchange.h"
#include "c_server_connection_settings.h"

#include "c_message.h"
#pragma warning(disable : 4996)

void easynet::core::handle_packet_client(easynet::c_session* session_ptr, easynet::core::c_packet packet)
{
	switch ((packet_type_opcode)packet.opcode)
	{
	case packet_type_opcode::server_connection_settings:
	{
		c_server_connection_settings server_connection_settings;
		c_clientkey_exchange clientkey_exchange;

		server_connection_settings.read(packet.body);

		auto rsa_key_size = server_connection_settings.rsa_encryption_key.size();
		auto rsa_key = new std::uint8_t[rsa_key_size];

		server_connection_settings.rsa_encryption_key.read_bytes(rsa_key, rsa_key_size);
		session_ptr->m_crypt->rsa = d2i_RSAPublicKey(0, (const unsigned char**)&rsa_key, rsa_key_size);

		clientkey_exchange.aeskey = session_ptr->m_crypt->rsaencrypt(easynet::data::c_buffer((int8_t*)session_ptr->m_crypt->AES_KEY, 16));

		session_ptr->send_data(clientkey_exchange.write(), packet_type_opcode::client_keyexchange);
		// handle
		break;
	}

	case packet_type_opcode::message:
	{
		c_message message;

		message.read(packet.body);

		if (session_ptr->on_message_received_fn)
			session_ptr->on_message_received_fn(session_ptr, message);

		break;
	}
	case packet_type_opcode::encrypted_message:
	{
		if (session_ptr->m_crypt->AES_KEY)
		{
			bool                    res = false;
			easynet::data::c_buffer encrypted_message;

			uint8_t                 encrypt_type = 0;
			if (!packet.body.read_uint8(encrypt_type) || !packet.body.read_bytes(encrypted_message)) return;

			if (encrypt_type == 1) // AES ENCRYPTION
			{
				auto decrypted = session_ptr->m_crypt->decrypt(encrypted_message, res);

				if (res)
				{
					easynet::data::c_buffer message;
					std::uint8_t padding = 0;
					std::uint32_t crc32 = 0;
					std::uint16_t encrypt_counter = 0;
					packet_type_opcode opcode = (packet_type_opcode)0;

					if (!decrypted.read_uint8(padding) || !decrypted.read_uint(crc32) || !decrypted.read_uint16(encrypt_counter) || !decrypted.read_uint8((uint8_t&)opcode) || !decrypted.read_bytes(message, decrypted.bytes_left())) return;

					easynet::core::c_packet new_packet;

					new_packet.body = message;
					new_packet.body_length = message.size();
					new_packet.opcode = (uint8_t)opcode;
					handle_packet_client(session_ptr, new_packet);
				}
			}
		}

		break;
	}

	case packet_type_opcode::connection_success:
	{
		if (session_ptr->on_key_exchange_fn)
			session_ptr->on_key_exchange_fn(session_ptr, true);
		break;
	}
	}
}

void easynet::core::handle_packet_server(easynet::c_session* session, easynet::core::c_packet packet)
{
	switch ((packet_type_opcode)packet.opcode)
	{
	case packet_type_opcode::client_keyexchange:
	{
		c_clientkey_exchange clientkey_exchange;

		clientkey_exchange.read(packet.body);

		auto encrypted_aes_key_size = clientkey_exchange.aeskey.size();
		auto encrypted_aes_key = new uint8_t[encrypted_aes_key_size];

		clientkey_exchange.aeskey.read_bytes(encrypted_aes_key, encrypted_aes_key_size);

		bool result = false;
		auto decrypted_aes = session->m_crypt->rsadecrypt(clientkey_exchange.aeskey, result);
		session->m_crypt->AES_KEY = new int8_t[16];
		memcpy(session->m_crypt->AES_KEY, decrypted_aes.buffer(), 16);

		session->m_connection_established = true;

		session->send_data(easynet::data::c_buffer(), packet_type_opcode::connection_success);

		session->reset_ping();

		break;
	}

	case packet_type_opcode::message:
	{
		c_message message;

		message.read(packet.body);

		if (session->on_message_received_fn)
			session->on_message_received_fn(session, message);

		break;
	}

	case packet_type_opcode::encrypted_message:
	{
		if (session->m_crypt->AES_KEY)
		{
			bool                    res = false;
			easynet::data::c_buffer encrypted_message;

			uint8_t                 encrypt_type = 0;
			if (!packet.body.read_uint8(encrypt_type) || !packet.body.read_bytes(encrypted_message)) return;

			if (encrypt_type == 1) // AES ENCRYPTION
			{
				auto decrypted = session->m_crypt->decrypt(encrypted_message, res);

				if (res)
				{
					easynet::data::c_buffer message;
					std::uint8_t padding = 0;
					std::uint32_t crc32 = 0;
					std::uint16_t encrypt_counter = 0;
					packet_type_opcode opcode = (packet_type_opcode)0;

					if (!decrypted.read_uint8(padding) || !decrypted.read_uint(crc32) || !decrypted.read_uint16(encrypt_counter) || !decrypted.read_uint8((uint8_t&)opcode) || !decrypted.read_bytes(message, decrypted.bytes_left())) return;

					easynet::core::c_packet new_packet;

					new_packet.body = message;
					new_packet.body_length = message.size();
					new_packet.opcode = (uint8_t)opcode;
					handle_packet_server(session, new_packet);
				}
			}
		}

		break;
	}

	case packet_type_opcode::ping_pong:
	{
		session->update_ping();

		session->reset_ping();
		if (session->should_send_ping()) {
			session->send_data(easynet::data::c_buffer(), packet_type_opcode::ping_pong);
		}
		break;
	}
	}
}