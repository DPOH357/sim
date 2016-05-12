#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QTimer>
#include <QListWidgetItem>
#include <chatagent.h>

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

    void on_pushButton_LogIn_clicked();

    void slot_UserIn(QString addressStr, QString userName);
    void slot_UserOut(QString addressStr, QString userName);

    void slot_setDialog(const QStringList& dialog);

    void slot_messageReceived(QString addressStr, QString message);

    void on_listWidgetUsers_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_pushButtonSend_clicked();

signals:
    void send_getDialog(QString addressStr);
    void send_sendMessage(QString addressStr, QString message);

private:
    Ui::MainWindow *ui;

    QTimer          m_timer;
    QVector<ChatAgent*>
                    m_agentsList;
};

#endif // MAINWINDOW_H
