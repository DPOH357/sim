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


        connect(&m_timer, &QTimer::timeout, this, &MainWindow::slot_timerTick);
        m_timer.setInterval(200);
        m_timer.start();

        ui->stackedWidget->setCurrentIndex(1);
    }
}
