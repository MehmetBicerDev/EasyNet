#pragma once
#include "includes.h"

enum class net_direction
{
	none,
	server,
	client
};

enum class packet_type_opcode : uint8_t
{
	none,
	// SERVER
	server_connection_settings,
	connection_success,
	// CLIENT
	client_keyexchange,
	//COMMON
	message,
	encrypted_message,
	ping_pong
};