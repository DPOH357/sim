#ifndef CHAT_MANAGER
#define CHAT_MANAGER

#include <QObject>

class ChatManager : public QObject
{
    Q_OBJECT

public:
    virtual ~ChatManager() {}
    virtual void run() = 0;
};

#endif // CHAT_MANAGER

