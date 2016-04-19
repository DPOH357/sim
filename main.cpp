#include <iostream>

#include "net/beacon/beacon.h"
#include <net/broadcast.h>
#include "net/tcp_connection.h"

#include <base/raw_data.h>
#include <base/tools.h>

void test_net_beacon()
{
    std::cout << "*** Net beacon testing ***" << std::endl;

    char name[32];
    char str[32];

    std::cout << "Enter name: ";
    std::cin >> name;
    std::cout << std::endl;

    std::cout << "Enter address: ";
    std::cin >> str;
    std::cout << std::endl;

    boost::system::error_code error_code;
    boost::asio::ip::address address = boost::asio::ip::address::from_string(str, error_code);
    if(error_code)
    {
        return;
    }

    const unsigned short port = 8888;


    sim::beacon::detector detector(name, address, port);

    if(!error_code)
    {
        unsigned int id = 0;
        while(true)
        {
            detector.run();

            unsigned int _id = detector.get_id();
            if(id != _id)
            {
                id = _id;
                std::cout << "Beacon id " << id << std::endl;
            }
        }
    }
}

struct test_message
{
    enum { text_length = 64 };
    char text[text_length];

    test_message()
    {
        text[0] = 0;
    }

    test_message( const char _text[])
    {
        strcpy(text, _text);
    }

    test_message(const test_message& mesage)
    {
        strcpy(text, mesage.text);
    }

    void operator=(const test_message& mesage)
    {
        strcpy(text, mesage.text);
    }

    void operator=(const char _text[])
    {
        strcpy(text, _text);
    }
};

void test_tcp_connection()
{
    std::cout << "*** TCP connection testing ***" << std::endl;
    std::cout << "Select mode:" << std::endl;
    std::cout << "1. Server" << std::endl;
    std::cout << "2. Client" << std::endl;

    int mode;
    std::cin >> mode;

    const unsigned short port(9999);

    boost::shared_ptr<sim::net::tcp_connection> connection;

    switch(mode)
    {
    case 1:
    {
        std::cout << "-Server-" << std::endl;

        connection = sim::net::tcp_connection::create_wait_connection(port, sizeof(test_message));
    }
    break;

    case 2:
    {
        std::cout << "-Client-" << std::endl;

        boost::system::error_code error_code;
        boost::asio::ip::address address(boost::asio::ip::address::from_string("127.0.0.1", error_code));

        if(!error_code)
        {
            connection = sim::net::tcp_connection::create_connect(address, port, sizeof(test_message));
        }
        else
        {
            sim::base::log::message(std::string("Error convert address: ") + error_code.message());
            return;
        }
    }
    break;

    default:
        return;
    }

    sim::base::raw_data raw_data(sizeof(test_message));
    test_message message;

    int command(-1);
    while(command)
    {
        std::cout << "Select command:" << std::endl;
        std::cout << "1. Check input messages" << std::endl;
        std::cout << "2. Send messages" << std::endl;
        std::cout << "0. Exit" << std::endl;

        std::cin >> command;

        switch (command)
        {
        case 1:
        {
            while(connection->get_message(raw_data))
            {
                if(raw_data.get_data(message))
                {
                    std::cout << message.text << std::endl;
                }
            }
        }
        break;

        case 2:
        {
            std::cout << std::endl << "Message: ";
            std::cin >> message.text;

            connection->send_message(message);
        }
        break;

        default:
            break;
        }
    }
}

void test_broadcast()
{
    int mode = -1;

    const unsigned int port(9999);

    test_message message;

    while(true)
    {
        std::cout << "Select mode" << std::endl;
        std::cout << "1. Sender" << std::endl;
        std::cout << "2. Receiver" << std::endl;
        std::cout << "0. Exit" << std::endl;

        std::cin >> mode;

        switch(mode)
        {
        case 0: return;

        case 1:
        {
            auto sender = sim::net::broadcast::create(port, sizeof(test_message));

            while(true)
            {
                std::cout << "Message: ";
                std::cin >> message.text;
                std::cout << std::endl;

                sender->send_message(message);
            }
        }
        break;

        case 2:
        {
            auto receiver = sim::net::broadcast_receiver<test_message>::create(port, new sim::net::gate_message<test_message>(12, 1));

            while(true)
            {
                if(receiver->get_message(message))
                {
                    std::cout << message.text << std::endl;
                }
                boost::this_thread::sleep_for( boost::chrono::milliseconds(200) );
            }
        }
        break;
        }
    }
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    std::cout << "Select test:" << std::endl;
    std::cout << "1. Net beacon" << std::endl;
    std::cout << "2. Net tcp connection" << std::endl;
    std::cout << "3. Net broadcast" << std::endl;

    std::cout << std::endl;
    std::cout << "0. Exit" << std::endl;

    int command(-1);

    do
    {
        std::cin >> command;

        switch(command)
        {
        case 0: return 0;
        case 1: test_net_beacon(); return 0;
        case 2: test_tcp_connection(); return 0;
        case 3: test_broadcast(); return 0;
        default: break;
        }
    }
    while(command);

    return 0;
}


