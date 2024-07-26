#include <easynet.h>

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "EasyNet")
void on_message_received(easynet::c_session* session, easynet::core::c_message message)
{
	printf("Message Received %d \n", message.message_opcode);

	switch (message.message_opcode)
	{
	case 15:
	{
		std::string msg = "";
		if (message.buffer.read_string(msg)) {
			printf("Client says %s \n", msg.c_str());

			easynet::data::c_buffer response;
			response.write_string("Test Message Server");

			session->send_encrypted(packet_type_opcode::message, easynet::core::c_message(16, response).write());
		}
		break;
	}
	}
}
int main()
{
	easynet::net_config config;
	config.direction = net_direction::server;
	config.direction_port = 32004;

	easynet::initialize(config);

	if (easynet::start(on_message_received, nullptr))
	{
		printf("initialized server \n");
	}

	std::cin.get();
}