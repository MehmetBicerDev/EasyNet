#include "c_crypt.h"

#include "crc32.h"
#pragma warning(disable : 4996)

bool easynet::core::c_crypt::generate_key( )
{
	rsa = RSA_new();
	if (!rsa)
		return false;

	BIGNUM* exponent = BN_new();
	if (!exponent)
	{
		RSA_free(rsa);
		return false;
	}

	BN_set_word(exponent, 65537);
	RSA_generate_key_ex(rsa, 1024, exponent, NULL);


	return true;
}

void easynet::core::c_crypt::generate_aes_key( )
{
	this->AES_KEY = new std::int8_t[16];
	RAND_bytes((unsigned char*)AES_KEY, 16);
}

easynet::data::c_buffer easynet::core::c_crypt::get_rsa_public_key( )
{
	std::vector<uint8_t> derPublicKey;
	{
		derPublicKey.resize(140);

		uint8_t* keyPtr = derPublicKey.data();
		int keyLength;

		if (!(keyLength = i2d_RSAPublicKey(rsa, &keyPtr)))
		{
			return data::c_buffer();
		}

		derPublicKey.resize(keyLength);
	}


	return data::c_buffer((int8_t*)derPublicKey.data(), derPublicKey.size());
}

easynet::data::c_buffer easynet::core::c_crypt::encrypt(easynet::data::c_buffer ToEncrypt, bool& ret)
{
	ret = false;
	std::uint32_t padding = 16 - (ToEncrypt.size() + 1 + 4 + 2) % 16;
	data::c_buffer EncryptStruct;
	data::c_buffer  HashStruct;
	HashStruct.write_int16(current_encrypt_counter);
	HashStruct.write_bytes(ToEncrypt.buffer(), ToEncrypt.size());
	EncryptStruct.write_int8((std::int8_t)padding);
	EncryptStruct.write_int(util::ComputeHash(HashStruct));
	EncryptStruct.write_bytes(HashStruct.buffer(), HashStruct.size());

	for (int i = 0; i < padding; i++)
		EncryptStruct.write_int8((std::int8_t)0);


	++current_encrypt_counter;
	size_t es = 0;
	EVP_CIPHER_CTX* ctx;
	ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
	{


		return data::c_buffer();

	}
	if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, (unsigned char*)AES_KEY, NULL) == 0)
	{
		return data::c_buffer();
	}
	EVP_CIPHER_CTX_set_padding(ctx, false);
	BYTE* out = new BYTE[EncryptStruct.size()];
	int outSize2 = 0;
	if (EVP_EncryptUpdate(ctx, out, &outSize2, (unsigned char*)EncryptStruct.buffer(), EncryptStruct.size()) == 0)
	{

		return data::c_buffer();
	}
	es += outSize2;
	outSize2 = 0;
	if (EVP_EncryptFinal_ex(ctx, (unsigned char*)EncryptStruct.buffer() + es, &outSize2) == 0)
	{

		return data::c_buffer();
	}
	es += outSize2;
	EVP_CIPHER_CTX_free(ctx);

	ret = true;
	data::c_buffer  encrypted((int8_t*)out, es);
	delete[] out;
	return encrypted;
}

easynet::data::c_buffer easynet::core::c_crypt::decrypt(easynet::data::c_buffer ToDecrypt, bool& ret)
{
	data::c_buffer decrypted;
	EVP_CIPHER_CTX* ctx;
	ctx = EVP_CIPHER_CTX_new();
	ret = false;
	if (ctx == NULL)
	{

		return decrypted;
	}
	if (!EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), 0, (unsigned char*)AES_KEY, 0))
	{

		return decrypted;
	}
	EVP_CIPHER_CTX_set_padding(ctx, false);

	std::int8_t* arr = new std::int8_t[ToDecrypt.size()];
	int ds = 0;
	size_t ds2 = 0;

	if (!EVP_DecryptUpdate(ctx, (unsigned char*)arr, &ds, (unsigned char*)ToDecrypt.buffer(), ToDecrypt.size()))
	{

		return decrypted;
	}
	ds2 += ds;
	ds = 0;
	if (!EVP_DecryptFinal_ex(ctx, (unsigned char*)(arr + ds2), &ds))
	{
		return decrypted;
	}
	ds2 += ds;
	EVP_CIPHER_CTX_free(ctx);
	ret = true;
	decrypted = data::c_buffer((int8_t*)arr, ds2);
	delete[] arr;
	return decrypted;
}

easynet::data::c_buffer easynet::core::c_crypt::rsadecrypt(easynet::data::c_buffer in, bool& result)
{
	int ret = -1;
	unsigned char* aes_key_decrypted = new unsigned char[16];
	ret = RSA_private_decrypt(in.size(), (unsigned char*)in.buffer(), aes_key_decrypted, rsa, RSA_PKCS1_OAEP_PADDING);
	result = ret != -1;
	return data::c_buffer((int8_t*)aes_key_decrypted, 16);
}

easynet::data::c_buffer easynet::core::c_crypt::rsaencrypt(easynet::data::c_buffer ToEncrypt)
{
	size_t outsize = 0;
	BYTE* ret = new BYTE[RSA_size(rsa)];
	outsize = RSA_public_encrypt(ToEncrypt.size(), (const unsigned char*)ToEncrypt.buffer(), (unsigned char*)ret, rsa, RSA_PKCS1_OAEP_PADDING);
	data::c_buffer encrypted((int8_t*)ret, outsize);
	delete[] ret;
	return encrypted;
}

easynet::data::c_buffer easynet::core::c_crypt::encrypt_key(easynet::data::c_buffer ToEncrypt, bool& ret)
{
	size_t es = 0;
	EVP_CIPHER_CTX* ctx;
	ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
	{

		return data::c_buffer();

	}
	if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, (unsigned char*)AES_KEY, NULL) == 0)
	{
		return data::c_buffer();
	}
	EVP_CIPHER_CTX_set_padding(ctx, false);
	BYTE* out = new BYTE[ToEncrypt.size()];
	int outSize2 = 0;
	if (EVP_EncryptUpdate(ctx, out, &outSize2, (unsigned char*)ToEncrypt.buffer(), ToEncrypt.size()) == 0)
	{
		return data::c_buffer();
	}
	es += outSize2;
	outSize2 = 0;
	if (EVP_EncryptFinal_ex(ctx, (unsigned char*)ToEncrypt.buffer() + es, &outSize2) == 0)
	{
		return data::c_buffer();
	}
	es += outSize2;
	EVP_CIPHER_CTX_free(ctx);

	ret = true;
	data::c_buffer encrypted((int8_t*)out, es);
	delete[] out;
	return encrypted;
}
