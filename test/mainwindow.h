#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <net/broadcast.h>
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

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    void logFunc(sim::log::level level, const std::string& text);
    void enable_BC(bool bEnable);

private:
    Ui::MainWindow *ui;

    std::mutex m_mutex;

    QString m_logText;

    QString m_BC_receiveText;

    sim::base::raw_data m_rawData;

    sim::net::broadcast_ptr m_broadcast;
};

#endif // MAINWINDOW_H
