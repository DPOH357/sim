#include <iostream>

#include "net/beacon/beacon.h"
#include "net/tcp_connection.hpp"


int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

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
        return 1;
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

    return 0;
}


