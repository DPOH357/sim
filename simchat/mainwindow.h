#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>
#include <QTimer>
#include <QListWidgetItem>
#include <QStringListModel>
#include "userslistagent.h"
#include "usersdialogagent.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slot_timerTick();

    void slot_UserIn(QString addressStr, QString userName);
    void slot_UserOut(QString addressStr, QString userName);

    void slot_messageReceived(QString addressStr, QString text);

    void on_pushButton_LogIn_clicked();

    void on_pushButtonSend_clicked();

    void on_listViewUsers_activated(const QModelIndex &index);

    void on_textEditMessage_textChanged();

private:
    QString generateMessage(QString name, QString text);
    void refreshUsersList();

private:
    Ui::MainWindow *ui;

    QTimer          m_timer;

    UsersListAgent*  m_usersListAgent;
    UserDialog*      m_usersDialogAgent;

    QStringListModel* m_usersListModel;
    QStringListModel* m_dialogModel;

    struct UserData
    {
        QString name;
        QString address;
        QStringList dialog;

        UserData(QString _address, QString _name)
            : address(_address)
            , name(_name)
        {

        }
    };

    QHash<QString, UserData> m_usersList; // by address
    QString                  m_currentUserAddress;
};

#endif // MAINWINDOW_H
