#include "usersdialogagent.h"

#include <QTime>

UserDialog::UserDialog(QString myName, unsigned short port)
    : m_port(port)
{
    m_receiver = sim::net::tcp_receiver::create(port);
    m_sender = sim::net::tcp_sender::create();
}

void UserDialog::run()
{
    sim::base::raw_data raw_data;
    std::string addressStr;
    unsigned short port;
    if(m_receiver->get_message(raw_data, &addressStr))
    {
        Message message;
        if(raw_data.get_data(message))
        {
            QString addressQStr = QString::fromLocal8Bit(addressStr.c_str());
            emit send_messageReceived(addressQStr, QString::fromLocal8Bit(message.text));
        }
    }
}

void UserDialog::sendMessage(QString addressStr, QString message)
{
    Message msg(message);
    sim::base::raw_data rawData(&msg, sizeof(Message));
    m_sender->send_message(rawData, addressStr.toLocal8Bit().constData(), m_port);
}
