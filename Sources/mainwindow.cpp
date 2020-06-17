#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <random>
#include <stdio.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::makePlot();

    mThread = new MyThread(this);
    connect(mThread,SIGNAL(socketData(int)),this,SLOT(displaySocketDataGraph(int)));
    connect(mThread,SIGNAL(socketData(int)),this,SLOT(displaySocketDataHistogram(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::makePlot()
{
    // configure axis rect:
    ui->customPlot->plotLayout()->clear(); // clear default axis rect so we can start from scratch
    QCPAxisRect *wideAxisRect = new QCPAxisRect(ui->customPlot);
    wideAxisRect->setupFullAxesBox(true);
    wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabels(true);
    QCPLayoutGrid *subLayout = new QCPLayoutGrid;
    ui->customPlot->plotLayout()->addElement(0, 0, wideAxisRect); // insert axis rect in first row
    ui->customPlot->plotLayout()->addElement(1, 0, subLayout); // sub layout in second row (grid layout will grow accordingly)
    //customPlot->plotLayout()->setRowStretchFactor(1, 2);
    // prepare axis rects that will be placed in the sublayout:
    QCPAxisRect *subRectLeft = new QCPAxisRect(ui->customPlot, false); // false means to not setup default axes
    subLayout->addElement(0, 0, subRectLeft);
    // setup axes in sub layout axis rects:
    subRectLeft->addAxes(QCPAxis::atBottom | QCPAxis::atLeft);
    subRectLeft->axis(QCPAxis::atLeft)->ticker()->setTickCount(2);
    subRectLeft->axis(QCPAxis::atBottom)->grid()->setVisible(true);
    subRectLeft->axis(QCPAxis::atLeft)->setRangeLower(0);
    // synchronize the left and right margins of the top and bottom axis rects:
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->customPlot);
    subRectLeft->setMarginGroup(QCP::msLeft, marginGroup);
    wideAxisRect->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
    // move newly created axes on "axes" layer and grids on "grid" layer:
    foreach (QCPAxisRect *rect, ui->customPlot->axisRects())
    {
      foreach (QCPAxis *axis, rect->axes())
      {
        axis->setLayer("axes");
        axis->grid()->setLayer("grid");
      }
    }

    ui->customPlot->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft)); // blue line
    ui->customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsImpulse);
    ui->customPlot->graph(0)->rescaleAxes();


    ui->customPlot->addGraph(subRectLeft->axis(QCPAxis::atBottom), subRectLeft->axis(QCPAxis::atLeft));
    ui->customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));
    ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsImpulse);
    ui->customPlot->graph(1)->rescaleAxes();

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->customPlot->xAxis->setTicker(timeTicker);
    ui->customPlot->axisRect()->setupFullAxesBox();

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(graphScroll()));
    dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

void MainWindow::displaySocketDataGraph(int graphData)
{
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;
    if (key-lastPointKey > 0) // at most add point every 50 ms
    {
      // add data to lines:
      ui->customPlot->graph(0)->addData(key, graphData);
      // rescale value (vertical) axis to fit the current data:
      ui->customPlot->graph(0)->rescaleValueAxis();
      lastPointKey = key;
    }

    // calculate frames per second:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
      ui->statusBar->showMessage(
            QString("%1 FPS, Total Data points: %2")
            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
            .arg(ui->customPlot->graph(0)->data()->size())
            , 0);
      lastFpsKey = key;
      frameCount = 0;
    }
}



void MainWindow::displaySocketDataHistogram(int histoData)
{
    QVector<double> freq(1000000);
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;
    if (key-lastPointKey > 0) // at most add point every 50 ms
    {
        freq[histoData]++;
        // add data to lines:
        ui->customPlot->graph(1)->addData(histoData, freq[histoData]);
        ui->customPlot->yAxis->setRangeLower(0);
        ui->customPlot->graph(1)->rescaleValueAxis(true,true);
        ui->customPlot->graph(1)->rescaleKeyAxis();
        lastPointKey = key;
    }
}

void MainWindow::graphScroll()
{
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0;
    // make key axis range scroll with the data (at a constant range size of 8):]
    ui->customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->customPlot->replot();
}

void MainWindow::on_pushButton_clicked()
{
    mThread->start();
}

void MainWindow::on_pushButton_2_clicked()
{
    mThread->Stop = true;
}
