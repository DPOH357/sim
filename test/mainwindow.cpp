#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>

#include <functional>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_bLogChange(false)
{
    ui->setupUi(this);

    sim::log::set_log_level(sim::log::level::Debug);
    sim::log::set_log_fuction( std::bind( &MainWindow::logFunc, this, std::placeholders::_1, std::placeholders::_2) );

    enable_BC(false);
    enable_MCS(false);
    enable_MCC(false);

    startTimer(100);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::enable_BC(bool bEnable)
{
    ui->spinBox_BC_Port->setEnabled(!bEnable);
    ui->textEdit_BC_Receive->setEnabled(bEnable);
    ui->textEdit_BC_Send->setEnabled(bEnable);
    ui->pushButton_BC_Send->setEnabled(bEnable);

    if(bEnable)
    {
        const unsigned int port = ui->spinBox_BC_Port->value();
        m_broadcast = sim::net::broadcast::create(port);
    }
    else
    {
        m_BC_receiveText.clear();
        m_broadcast.reset();
    }
}

void MainWindow::enable_MCS(bool bEnable)
{
    ui->lineEdit_MCS_Address->setEnabled(!bEnable);
    ui->spinBox_MCS_Port->setEnabled(!bEnable);

    ui->pushButton_MCS_Send->setEnabled(bEnable);
    ui->textEdit_MCS_Send->setEnabled(bEnable);
    ui->textEdit_MCS_Text->setEnabled(bEnable);

    if(bEnable)
    {
        const std::string address = qPrintable(ui->lineEdit_MCS_Address->text());
        const unsigned short port = ui->spinBox_MCS_Port->value();

        m_multicast_server = sim::net::multicast_server::create(address, port);
    }
    else
    {
        m_multicast_server.reset();

        m_MCS_sendedText.clear();
        ui->textEdit_MCS_Text->clear();
        ui->textEdit_MCS_Send->clear();
    }
}

void MainWindow::enable_MCC(bool bEnable)
{
    ui->lineEdit_MCC_Address->setEnabled(!bEnable);
    ui->spinBox_MCC_Port->setEnabled(!bEnable);
    ui->textEdit_MCC_Text->setEnabled(bEnable);

    if(bEnable)
    {
        const std::string address = qPrintable(ui->lineEdit_MCC_Address->text());
        const unsigned short port = ui->spinBox_MCC_Port->value();

        m_multicast_client = sim::net::multicast_client::create(address, port);
    }
    else
    {

        m_multicast_client.reset();
        m_MCC_receiveText.clear();
        ui->textEdit_MCC_Text->clear();
    }
}

void MainWindow::run_BC()
{
    if(m_broadcast.get())
    {
        std::string address;
        unsigned short port;

        while(m_broadcast->get_message(m_rawData, &address, &port))
        {
            char* ch = (char*)m_rawData.get_data_ptr();
            bool bInvalid(true);
            for(int i = m_rawData.get_data_size() - 1; i >= 0; --i)
            {
                if(ch[i] == '\n' || ch[i] == '\0')
                {
                    bInvalid = false;
                    break;
                }
            }

            if(!bInvalid)
            {
                m_BC_receiveText +=
                        QString::fromStdString(address) +
                        ":" +
                        QString::number(port) +
                        ": " +
                        QString::fromLocal8Bit((const char*)m_rawData.get_data_ptr(), (int)m_rawData.get_data_size()) +
                        "\n";
                ui->textEdit_BC_Receive->setText(m_BC_receiveText);
            }
            else
            {
                logFunc(sim::log::level::Warning, "Test | Broadcast | Receive - endline symbol not detected");
            }
        }
    }
}

void MainWindow::run_MCC()
{
    if(m_multicast_client.get())
    {
        std::string address;
        unsigned short port;

        while(m_multicast_client->get_message(m_rawData, &address, &port))
        {
            char* ch = (char*)m_rawData.get_data_ptr();
            bool bInvalid(true);
            for(int i = m_rawData.get_data_size() - 1; i >= 0; --i)
            {
                if(ch[i] == '\n' || ch[i] == '\0')
                {
                    bInvalid = false;
                    break;
                }
            }

            if(!bInvalid)
            {
                m_MCC_receiveText +=
                        QString::fromStdString(address) +
                        ":" +
                        QString::number(port) +
                        ": " +
                        QString::fromLocal8Bit((const char*)m_rawData.get_data_ptr(), (int)m_rawData.get_data_size()) +
                        "\n";
                ui->textEdit_MCC_Text->setText(m_MCC_receiveText);
            }
            else
            {
                logFunc(sim::log::level::Warning, "Test | Multicast client | Receive - endline symbol not detected");
            }
        }
    }
}

void MainWindow::on_pushButton_BC_Run_toggled(bool checked)
{
    enable_BC(checked);
}

void MainWindow::on_pushButton_BC_Send_clicked()
{
    QString text = ui->textEdit_BC_Send->toPlainText();

    m_rawData.set_data(text.toLocal8Bit().constData(), text.length() + 1);
    m_broadcast->send_message(m_rawData);

    ui->textEdit_BC_Send->clear();
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    run_BC();
    run_MCC();

    std::lock_guard<std::mutex> locker(m_mutex);
    if(m_bLogChange)
    {
        m_bLogChange = false;
        ui->textEdit_Log->setText(m_logText);
    }
}

void MainWindow::logFunc(sim::log::level level, const std::string &text)
{
    std::lock_guard<std::mutex> locker(m_mutex);
    QString str;

    str = QTime::currentTime().toString("hh:mm:ss") + " | ";

    switch (level)
    {
    case sim::log::level::Error: str += "E | "; break;
    case sim::log::level::Warning: str += "W | "; break;
    case sim::log::level::Debug: str += "D | "; break;
    default:    break;
    }

    str += QString::fromLocal8Bit(text.c_str()) + "\n";
    m_logText += str;

    m_bLogChange = true;
}

void MainWindow::on_pushButton_MCS_Run_toggled(bool checked)
{
    enable_MCS(checked);
}

void MainWindow::on_pushButton_MCS_Send_clicked()
{
    if(m_multicast_server.get())
    {
        QString text = ui->textEdit_MCS_Send->toPlainText();
        ui->textEdit_MCS_Send->clear();

        m_MCS_sendedText += text + "\n";
        ui->textEdit_MCS_Text->setText(m_MCS_sendedText);

        m_rawData.set_data(qPrintable(text), text.length() + 1);
        m_multicast_server->send_message(m_rawData);
    }
}

void MainWindow::on_pushButton_MCC_Run_clicked(bool checked)
{
    enable_MCC(checked);
}
