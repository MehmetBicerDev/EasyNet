#pragma once
#include "c_buffer.h"

namespace easynet
{
	namespace core
	{
		class c_clientkey_exchange
		{
		public:
			c_clientkey_exchange() { }

			data::c_buffer aeskey;

			data::c_buffer write()
			{
				data::c_buffer buffer;

				buffer.write_bytes(aeskey);

				return buffer;
			}

			bool read(data::c_buffer& buffer)
			{
				return buffer.read_bytes(aeskey);
			}
		};
	}
}