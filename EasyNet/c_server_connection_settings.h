#pragma once
#include "c_buffer.h"








namespace easynet
{
	namespace core
	{
		class c_server_connection_settings
		{
		public:
			c_server_connection_settings() { }

			data::c_buffer rsa_encryption_key;

			data::c_buffer write()
			{
				data::c_buffer buffer;

				buffer.write_bytes(rsa_encryption_key);

				return buffer;
			}

			bool read( data::c_buffer& buffer)
			{
				return buffer.read_bytes(rsa_encryption_key);
			}


		};
	}
}