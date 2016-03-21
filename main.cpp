#include <iostream>

#include "net/beacon/beacon.h"
#include "net/tcp_connection.hpp"


int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    const unsigned short port = 8888;

    sim::beacon::beacon beacon(port);

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

    return 0;
}


