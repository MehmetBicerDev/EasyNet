#include <easynet.h>

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "EasyNet")
void on_message_received(easynet::c_session* session, easynet::core::c_message message)
{
    printf("Message %d \n", message.message_opcode);

    switch (message.message_opcode)
    {
    case 16:
    {
        std::string msg = "";
        if (message.buffer.read_string(msg))
        {
            printf("Server responded with %s \n", msg.c_str());
        }

    }
    }
}

void on_connection_success(easynet::c_session* session, bool success)
{
    printf("Connection success. \n");


    easynet::data::c_buffer message_req;
    message_req.write_string("Test Message");

    session->send_encrypted(packet_type_opcode::message, easynet::core::c_message(15, message_req).write());
}
int main()
{
    easynet::net_config config;
    config.direction = net_direction::client;
    config.direction_port = 32004;
    config.server_ip = "127.0.0.1";

    easynet::initialize(config);

    if (easynet::start(on_message_received, on_connection_success))
    {
        printf("initialized client \n");
    }


    std::cin.get();



}

