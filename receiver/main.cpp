#include <iostream>
#include <base/tools.h>
#include <net/tcp_receiver.h>

int main(int argc, char *argv[])
{
    sim::base::log::set_log_level( sim::base::log::level::Debug);

    const unsigned short port(9876);
    auto receiver = sim::net::tcp_receiver::create(port);

    while(true)
    {
        sim::base::raw_data rd;
        if(receiver->get_message(rd))
        {
            int i;
            if(rd.get_data(i))
                std::cout << "receive " << i << std::endl;
            else
                std::cout << "invalid received data" << std::endl;
        }

        boost::this_thread::sleep_for( boost::chrono::milliseconds(750) );
    }

    return 0;
}
