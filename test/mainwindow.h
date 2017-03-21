#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <net/broadcast.h>
#include <net/multicast_client.h>
#include <net/multicast_server.h>
#include <base/raw_data.h>
#include <mutex>

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
    void on_pushButton_BC_Run_toggled(bool checked);

    void on_pushButton_BC_Send_clicked();

    void on_pushButton_MCS_Run_toggled(bool checked);

    void on_pushButton_MCS_Send_clicked();

    void on_pushButton_MCC_Run_clicked(bool checked);

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    void logFunc(sim::log::level level, const std::string& text);

    void enable_BC(bool bEnable);

    void enable_MCS(bool bEnable);

    void enable_MCC(bool bEnable);

    void run_BC();

    void run_MCC();

private:
    Ui::MainWindow *ui;

    std::mutex m_mutex;

    QString m_logText;
    bool m_bLogChange;

    QString m_BC_receiveText;

    QString m_MCS_sendedText;

    QString m_MCC_receiveText;

    sim::base::raw_data m_rawData;

    sim::net::broadcast_ptr m_broadcast;

    sim::net::multicast_client_ptr m_multicast_client;

    sim::net::multicast_server_ptr m_multicast_server;
};

#endif // MAINWINDOW_H
