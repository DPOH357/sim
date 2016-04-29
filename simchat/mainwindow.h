#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QTimer>
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

private:
    Ui::MainWindow *ui;

    QTimer          m_timer;
    QVector<ChatAgent*>
                    m_agentsList;
};

#endif // MAINWINDOW_H
