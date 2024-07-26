#pragma once
#include "c_buffer.h"








namespace easynet {
	namespace core {
		class c_packet {
		public:
			c_packet() = default;

			bool is_valid = false;


			uint16_t net_version = 0x5713;
			uint32_t body_length = 0;
			uint8_t opcode = 0;
			data::c_buffer body;



			bool read(data::c_buffer& message)
			{
				if( (message.read_uint16 ( net_version ) && message.read_uint ( (body_length) ) && message.read_uint8 ( opcode ) && message.read_bytes ( body, -1 )))
				{
					body_length -= 0x9A2112;
					return true;
				}
				return false;
			}

			void create_packet()
			{
				net_version = 0x5713;
				body_length = body.size();

			}


			data::c_buffer write_header()
			{
				data::c_buffer message;

				message.write_uint16(net_version);
				message.write_uint(body_length + 0x9A2112);
				message.write_uint8(opcode);

				return message;
			}

			data::c_buffer write()
			{
				data::c_buffer message;

				message.write_uint16(net_version);
				message.write_uint(body_length + 0x9A2112);
				message.write_uint8(opcode);
				message.write_bytes(body);

			}
		};
	}
}