#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "userlistagent.h"
#include "userdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    ui->pushButtonSend->setDisabled(true);
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
        const unsigned short port = 8888;
        auto userListDetector = new UserListAgent(port, name);
        m_agentsList.push_back(userListDetector);

        auto userDialog = new UserDialog(name, port);
        m_agentsList.push_back(userDialog);

        connect(userListDetector, &UserListAgent::send_userIn, this, &MainWindow::slot_UserIn);
        connect(userListDetector, &UserListAgent::send_userOut, this, &MainWindow::slot_UserOut);

        connect(this, &MainWindow::send_getDialog, userDialog, &UserDialog::slot_getDialog);
        connect(this, &MainWindow::send_sendMessage, userDialog, &UserDialog::slot_sendMessage);
        connect(userDialog, &UserDialog::send_dialog, this, &MainWindow::slot_setDialog);
        connect(userDialog, &UserDialog::send_messageReceived, this, &MainWindow::slot_messageReceived);

        connect(userListDetector, &UserListAgent::send_userIn, userDialog, &UserDialog::slot_userSignIn);
        connect(userListDetector, &UserListAgent::send_userOut, userDialog, &UserDialog::slot_userSignOut);

        connect(&m_timer, &QTimer::timeout, this, &MainWindow::slot_timerTick);
        m_timer.setInterval(200);
        m_timer.start();

        ui->stackedWidget->setCurrentIndex(1);
    }
}

void MainWindow::slot_UserIn(QString addressStr, QString userName)
{
    QListWidgetItem* item = new QListWidgetItem(userName);
    item->setData(Qt::UserRole, QVariant(addressStr));

    ui->listWidgetUsers->addItem(item);
}

void MainWindow::slot_UserOut(QString addressStr, QString userName)
{
    auto itemsList = ui->listWidgetUsers->findItems(userName, Qt::MatchFixedString);
    for(auto item : itemsList)
    {
        ui->listWidgetUsers->removeItemWidget(item);
    }
}

void MainWindow::slot_setDialog(const QStringList &dialog)
{
    for(auto str : dialog)
        ui->textEditDialog->insertPlainText(str);
}

void MainWindow::slot_messageReceived(QString addressStr, QString message)
{
    QListWidgetItem* item = ui->listWidgetUsers->currentItem();
    if(item
    && item->data(Qt::UserRole).value<QString>() == addressStr)
    {
        ui->textEditDialog->insertPlainText(message);
    }
}

void MainWindow::on_listWidgetUsers_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);

    ui->pushButtonSend->setEnabled(current);

    ui->textEditMessage->clear();
    ui->textEditDialog->clear();

    if(current)
    {
        QString addressStr = current->data(Qt::UserRole).value<QString>();
        emit send_getDialog(addressStr);
    }
}

void MainWindow::on_pushButtonSend_clicked()
{
    QListWidgetItem* item = ui->listWidgetUsers->currentItem();
    if(item)
    {
        QString addressStr = item->data(Qt::UserRole).value<QString>();
        QString message = ui->textEditMessage->toPlainText();

        emit send_sendMessage(addressStr, message);
    }

    ui->textEditMessage->clear();
}
