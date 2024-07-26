# EasyNet: A Simple and Efficient TCP Server/Client Library for C++
EasyNet is an easy-to-use TCP server/client library written in C++. It provides a straightforward interface for creating TCP servers and clients, allowing developers to focus on their application logic without worrying about the intricacies of network programming. This library is ideal for both beginners and experienced developers looking for a reliable and efficient networking solution.

## Features
Simple and intuitive API
Supports synchronous communication
Built-in support for message encryption
Easy integration with existing projects
Lightweight and efficient
Getting Started
Prerequisites
Visual Studio (or any C++ compiler)
Windows SDK

### Installation
Clone the repository:

git clone https://github.com/mehmetbicerdev/easynet.git
Include the easynet directory in your project.

Link against the ws2_32 and EasyNet libraries:

```
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "EasyNet")
```

## Easy Usage
EasyNet provides a simplified API for quick setup and use. The simplified API abstracts much of the boilerplate code required to set up a TCP server or client, making it easier to get started.

Initialize the library.
### Server
```
    easynet::net_config config;
    config.direction = net_direction::server;
    config.direction_port = 32004;

    easynet::initialize(config);

    if (easynet::start(on_message_received, nullptr))
    {
        printf("Initialized server \n");
    }
```
### Client
```
  easynet::net_config config;
    config.direction = net_direction::client;
    config.direction_address = "127.0.0.1";
    config.direction_port = 32004;

    easynet::initialize(config);

    if (easynet::start(nullptr, on_message_received))
    {
        printf("Initialized client \n");
    }
```

Handle Packets
```
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
```

### Manual Usage
Callbacks in EasyNet
EasyNet relies heavily on the use of callbacks to handle various events in the TCP server and client lifecycle. These callbacks allow developers to define custom behavior for events such as message reception and connection success.

Server Callbacks
When creating a TCP server, you need to define a callback function that will be called whenever a message is received. Here's how it works:

Define the Callback Function: The callback function must match the signature expected by EasyNet. For example:

```
void on_message_received(easynet::c_session* session, easynet::core::c_message message)
{
    // Handle the message
}
```
Set the Callback Function: When creating your server class, set the callback function to the on_message_received_fn member.

```
class c_testserver : public easynet::c_tcpserver
{
public:
    c_testserver(uint16_t p_port) : easynet::c_tcpserver(p_port)
    {
        on_message_received_fn = on_message_received;
    }

    void message_received(easynet::c_session* session, easynet::core::c_message message)
    {
        // Process the message
    }
};
```
Handle the Callback: Implement the logic for handling incoming messages within your callback function.

```
static void on_message_received(easynet::c_session* session, easynet::core::c_message message)
{
    test_server->message_received(session, message);
}
```
Client Callbacks
Similarly, clients use callbacks to handle events such as successful connections and message reception.

Define the Callback Functions: You need to define callback functions for connection success and message reception.

```
void on_connection_success(easynet::c_session* session, bool success)
{
    // Handle connection success
}

void on_message_received(easynet::c_session* session, easynet::core::c_message message)
{
    // Handle the message
}
```
Set the Callback Functions: When creating your client class, set the callback functions to the respective member functions.

```
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
};
```
Handle the Callbacks: Implement the logic for handling connection success and incoming messages within your callback functions.

```
void on_connection_success(easynet::c_session* session, bool success)
{
    printf("Connection success \n");
    // Additional logic for connection success
}

void on_message_received(easynet::c_session* session, easynet::core::c_message message)
{
    printf("Message %d \n", message.message_opcode);
    // Additional logic for message handling
}
```
## Example Projects
The repository includes example projects that demonstrate how to use the EasyNet library with proper callback functions. These projects show how to set up and use the EasyNet library for both servers and clients.

Test Server Project: Demonstrates how to create a server, handle incoming messages, and respond to clients.
Test Client Project: Demonstrates how to connect to a server, send messages, and handle server responses.
Simplified Server and Client Projects: Showcases how to use the simplified API for quick setup and testing.
Contributing
We welcome contributions from the community. If you have suggestions, bug reports, or patches, please feel free to submit a pull request or open an issue on GitHub.
