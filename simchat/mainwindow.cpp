#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "userlistagent.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto userListDetector = new UserListAgent(5555);
    m_agentsList.push_back(userListDetector);


    connect(&m_timer, &QTimer::timeout, this, &MainWindow::slot_timerTick);
    m_timer.setInterval(200);
}

MainWindow::~MainWindow()
{
    for(auto agent : m_agentsList)
    {
        delete agent;
    }
    delete ui;
}
