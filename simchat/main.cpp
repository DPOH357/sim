#include <QApplication>
#include "mainwindow.h"


#include <net/broadcast.h>
#include <net/tcp_sender.h>
#include <net/tcp_receiver.h>

int main(int argc, char* argv[])
{
#if 0
    sim::base::raw_data rd;
    int x;

    auto bc = sim::net::broadcast::create(7778);

    rd.set(x);
    bc->send_message(rd);

    boost::asio::ip::udp::endpoint ep;
    while(!bc->get_message(rd, &ep))
    {

    }
    std::string addr = ep.address().to_string();

    auto sender = sim::net::tcp_sender::create();
    sender->send_message(rd, addr, 7777);

    auto receiver = sim::net::tcp_receiver::create(7777);

    while(!receiver->get_message(rd))
    {

    }
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
    return 0;
}
