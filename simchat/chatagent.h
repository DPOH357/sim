#ifndef CHAT_AGENT
#define CHAT_AGENT

#include <QObject>

class ChatAgent : public QObject
{
    Q_OBJECT

public:
    virtual ~ChatAgent() {}
    virtual void run() = 0;
};

#endif // CHAT_AGENT

