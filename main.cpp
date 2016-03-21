#include <iostream>

#include "net/beacon/beacon.h"
#include "net/tcp_connection.hpp"


int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    char str[32];

    std::cin >> str;

    const unsigned short port = 8888;

    boost::system::error_code error_code;
    sim::beacon::beacon beacon(boost::asio::ip::address::from_string(str, error_code), port);

    if(!error_code)
    {
        unsigned int id = 0;
        while(true)
        {
            beacon.run();

            unsigned int _id = beacon.get_id();
            if(id != _id)
            {
                id = _id;
                std::cout << "Beacon id " << id << std::endl;
            }
        }
    }

    return 0;
}


