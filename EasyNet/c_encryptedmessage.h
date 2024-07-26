#pragma once
#include "c_buffer.h"
#include "easynet_constants.h"



namespace easynet
{
	namespace core
	{
		class c_encryptedmessage
		{
		public:
			c_encryptedmessage() { }


			packet_type_opcode message_opcode = packet_type_opcode::none;
			data::c_buffer     buffer;

			data::c_buffer write()
			{
				data::c_buffer mbuffer;

				mbuffer.write_uint8((uint8_t)message_opcode);
				mbuffer.write_bytes(buffer);

				return mbuffer;
			}
			bool read( data::c_buffer& buffer)
			{
				return (buffer.read_uint8 ( (uint8_t&)message_opcode ) && buffer.read_bytes ( buffer ));

			}
		};
	}
}