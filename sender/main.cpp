#include <iostream>
#include <base/tools.h>
#include <net/tcp_sender.h>

int main()
{
    sim::base::log::set_log_level( sim::base::log::level::Debug);

    const unsigned short port(9876);
    auto sender = sim::net::tcp_sender::create();

    unsigned int i = 0;
    while(true)
    {
        sim::base::raw_data rd;
        rd.set(i);
        sender->send_message(rd, "172.16.18.106", port); // 172.16.18.49
        std::cout << "send " << i << std::endl;
        i++;

        boost::this_thread::sleep_for( boost::chrono::milliseconds(1000) );
    }

    return 0;
}
