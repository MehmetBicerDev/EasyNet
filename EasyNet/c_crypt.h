#pragma once
#include "c_buffer.h"

#include <openssl/rsa.h>
#include <openssl/rand.h>








namespace easynet {
	namespace core
	{
		class c_crypt
		{
		public:
			inline c_crypt() = default;


			bool generate_key();

			void generate_aes_key();

			easynet::data::c_buffer get_rsa_public_key();

			easynet::data::c_buffer  encrypt(easynet::data::c_buffer ToEncrypt, bool& ret);
			easynet::data::c_buffer  decrypt(easynet::data::c_buffer  ToDecrypt, bool& ret);
			easynet::data::c_buffer  rsadecrypt(easynet::data::c_buffer  in, bool& result);
			easynet::data::c_buffer  rsaencrypt(easynet::data::c_buffer  ToEncrypt);
			easynet::data::c_buffer  encrypt_key(easynet::data::c_buffer  ToEncrypt, bool& ret);

			RSA* rsa;
			std::int8_t* AES_KEY = 0;
			std::mutex mtx;

			std::int16_t current_encrypt_counter = 0;
			std::int16_t current_decrypt_counter = 0;

		};

	}
}