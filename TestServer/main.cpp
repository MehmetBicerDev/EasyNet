#include <c_tcpserver.h>

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "EasyNet")

void on_message_received(easynet::c_session* session, easynet::core::c_message message);

class c_testserver : public easynet::c_tcpserver
{
public:
    c_testserver(uint16_t p_port) : easynet::c_tcpserver ( p_port )
    {
        on_message_received_fn = on_message_received;
    }


    void message_received(easynet::c_session* session, easynet::core::c_message message)
    {
        printf("Message Received %d \n", message.message_opcode);

        switch(message.message_opcode)
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

};
 std::shared_ptr<c_testserver> test_server;

static void on_message_received(easynet::c_session* session, easynet::core::c_message message)
{
    test_server->message_received(session, message);
}


int main()
{
    WSAData data;
    WORD ver = MAKEWORD(2, 2);
    int wsResult = WSAStartup(ver, &data);
    test_server = std::make_shared<c_testserver>(30497);

    test_server->start();

    std::cin.get();

}

