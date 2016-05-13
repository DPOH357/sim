#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "userslistagent.h"
#include "usersdialogagent.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_usersListAgent(nullptr),
    m_usersDialogAgent(nullptr)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    ui->pushButtonSend->setDisabled(true);

    m_usersListModel = new QStringListModel(this);
    m_dialogModel = new QStringListModel(this);

    ui->listViewUsers->setModel(m_usersListModel);
    ui->listViewDialog->setModel(m_dialogModel);
}

MainWindow::~MainWindow()
{
    if(m_usersDialogAgent)
        delete m_usersDialogAgent;

    if(m_usersListAgent)
        delete m_usersListAgent;

    delete ui;
}

void MainWindow::slot_timerTick()
{
    m_usersListAgent->run();
    m_usersDialogAgent->run();
}

void MainWindow::slot_UserIn(QString addressStr, QString userName)
{
    auto i = m_usersList.find(addressStr);
    if(i != m_usersList.end())
    {
        if(i.value().name != userName)
        {
            i.value().name = userName;
            refreshUsersList();
        }
    }
    else
    {
        UserData ud(addressStr, userName);
        m_usersList.insert(addressStr, ud);
        refreshUsersList();
    }
}

void MainWindow::slot_UserOut(QString addressStr, QString userName)
{
    auto i = m_usersList.find(addressStr);
    if(i != m_usersList.end())
    {
        m_usersList.erase(i);
        refreshUsersList();
    }
}

void MainWindow::slot_messageReceived(QString addressStr, QString text)
{
    auto i = m_usersList.find(addressStr);
    if(i != m_usersList.end())
    {
        UserData& ud(i.value());
        QString message = generateMessage(ud.name, text);
        ud.dialog.push_back(message);

        if(ud.address == m_currentUserAddress)
        {
            m_dialogModel->setStringList(ud.dialog);
        }
    }
}

void MainWindow::on_pushButton_LogIn_clicked()
{
    QString name = ui->lineEdit_Name->text();

    if(!name.isEmpty())
    {
        const unsigned short port = 8888;
        m_usersListAgent = new UsersListAgent(port, name);

        m_usersDialogAgent = new UserDialog(name, port);

        connect(m_usersListAgent, &UsersListAgent::send_userIn, this, &MainWindow::slot_UserIn);
        connect(m_usersListAgent, &UsersListAgent::send_userOut, this, &MainWindow::slot_UserOut);

        connect(m_usersDialogAgent, &UserDialog::send_messageReceived, this, &MainWindow::slot_messageReceived);

        connect(&m_timer, &QTimer::timeout, this, &MainWindow::slot_timerTick);
        m_timer.setInterval(500);
        m_timer.start();

        ui->stackedWidget->setCurrentIndex(1);
    }
}

void MainWindow::on_pushButtonSend_clicked()
{
    QString text = ui->textEditMessage->toPlainText();

    if(!text.isEmpty())
    {
        auto i = m_usersList.find(m_currentUserAddress);
        if(i != m_usersList.end())
        {
            UserData& ud(i.value());

            m_usersDialogAgent->sendMessage(ud.address, text);

            QString message = generateMessage(ud.name, text);
            ud.dialog.push_back(message);
            m_dialogModel->setStringList(ud.dialog);
        }

        ui->textEditMessage->clear();
    }
}

QString MainWindow::generateMessage(QString name, QString text)
{
    QTime time = QTime::currentTime();
    return QString("[") + time.toString("hh:mm:ss") + QString("] ") + name + QString(": ") + text;
}

void MainWindow::refreshUsersList()
{
    QStringList users;
    for(auto ud : m_usersList)
    {
        users.push_back(ud.name);
    }
    users.sort();
    m_usersListModel->setStringList(users);
}

void MainWindow::on_listViewUsers_activated(const QModelIndex &index)
{
    if(index.isValid())
    {
        QString name = index.data().value<QString>();

        for(auto ud : m_usersList)
        {
            if(ud.name == name)
            {
                m_currentUserAddress = name;

                ui->textEditMessage->clear();
                m_dialogModel->setStringList(ud.dialog);

                break;
            }
        }
    }
    else
    {
        m_currentUserAddress = "";
    }
}
