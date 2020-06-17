#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mythread.h"
#include <QMainWindow>
#include <QTimer>
#include <QTcpSocket>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    MyThread *mThread;

private slots:
    void makePlot();

    void displaySocketDataGraph(int);

    void displaySocketDataHistogram(int);

    void graphScroll();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QTimer dataTimer;
    QTcpSocket *socket;
};

#endif // MAINWINDOW_H
