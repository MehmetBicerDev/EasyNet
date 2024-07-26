#pragma once
#include "c_buffer.h"

namespace easynet
{
	namespace core
	{
		class c_message
		{
		public:
			c_message() { }
			c_message(uint16_t opcode, data::c_buffer buf) : message_opcode(opcode), buffer(buf) { }

			uint16_t       message_opcode = 0;
			data::c_buffer buffer;

			data::c_buffer write()
			{
				data::c_buffer mbuffer;

				mbuffer.write_uint16(message_opcode);
				mbuffer.write_bytes(buffer);

				return mbuffer;
			}

			bool read(data::c_buffer& sbuffer)
			{
				return (sbuffer.read_uint16(message_opcode) && sbuffer.read_bytes(buffer));
			}
		};
	}
}