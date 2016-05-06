#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "userlistagent.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    for(auto agent : m_agentsList)
    {
        delete agent;
    }
    delete ui;
}

void MainWindow::slot_timerTick()
{
    for(auto agent : m_agentsList)
    {
        agent->run();
    }
}

void MainWindow::on_pushButton_LogIn_clicked()
{
    QString name = ui->lineEdit_Name->text();

    if(!name.isEmpty())
    {
        auto userListDetector = new UserListAgent(5555, name);
        m_agentsList.push_back(userListDetector);

        connect(userListDetector, &UserListAgent::send_userIn, this, &MainWindow::slot_UserIn);
        connect(userListDetector, &UserListAgent::send_userOut, this, &MainWindow::slot_UserOut);

        connect(&m_timer, &QTimer::timeout, this, &MainWindow::slot_timerTick);
        m_timer.setInterval(200);
        m_timer.start();

        ui->stackedWidget->setCurrentIndex(1);
    }
}

void MainWindow::slot_UserIn(QString name, boost::asio::ip::udp::endpoint endpoint)
{
    QListWidgetItem* item = new QListWidgetItem(name);
    item->setData(Qt::UserRole, QVarian<boost::asio::ip::udp::endpoint>(endpoint));
    ui->listWidgetUsers->addItem(item);
}

void MainWindow::slot_UserOut(QString name)
{
    for(int i = 0; i < ui->listWidgetUsers->count(); ++i)
    {
        QListWidgetItem* item = ui->listWidgetUsers->item(i);
        if(item->text() == name)
        {
            ui->listWidgetUsers->removeItemWidget(item);
            break;
        }
    }
}

void MainWindow::on_listWidgetUsers_doubleClicked(const QModelIndex &index)
{
    QString userName = index.data().value<QString>();
    if(!userName.isEmpty())
    {
        auto udpEndpoint = m_
    }
}
