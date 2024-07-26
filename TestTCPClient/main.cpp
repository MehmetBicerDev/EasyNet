#include <c_tcpclient.h>
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "EasyNet")

void on_connection_success(easynet::c_session* session, bool success);
void on_message_received(easynet::c_session* session, easynet::core::c_message message);

class c_testclient : public easynet::c_tcpclient
{
public:
	inline c_testclient(const std::string& p_server_address, int p_port) : easynet::c_tcpclient(p_server_address, p_port) {}

	bool start_connection()
	{
		on_message_received_fn = on_message_received;
		on_key_exchange_fn = on_connection_success;

		return connect_to_server();
	}
	std::thread ping_thread;

private:
	std::mutex connection_mtx;
};
std::shared_ptr<c_testclient> test_client;

void on_connection_success(easynet::c_session* session, bool success)
{
	printf("Connection success \n");
	test_client->ping_thread = std::thread([&]
		{
			uint64_t start_time = GetTickCount64();
			while (1)
			{
				if (GetTickCount64() - start_time > 5000)
				{
					test_client->send_data(easynet::data::c_buffer(), packet_type_opcode::ping_pong);
					start_time = GetTickCount64();
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		});

	test_client->ping_thread.detach();

	easynet::data::c_buffer message_req;
	message_req.write_string("Test Message");

	test_client->send_encrypted(packet_type_opcode::message, easynet::core::c_message(15, message_req).write());
}
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

int main()
{
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	test_client = std::make_shared<c_testclient>("127.0.0.1", 30497);

	std::thread m_connthread = std::thread([&]
		{
			test_client->start_connection();
		});

	m_connthread.detach();

	std::cin.get();
}