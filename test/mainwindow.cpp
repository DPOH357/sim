#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>

#include <functional>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sim::log::set_log_level(sim::log::level::Debug);
    //sim::log::set_log_fuction( std::bind( &MainWindow::logFunc, this, std::placeholders::_1, std::placeholders::_2) );

    enable_BC(false);

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

    str += QString::fromStdString(text);
    m_logText += str;

    ui->textEdit_Log->setText(m_logText);
}
