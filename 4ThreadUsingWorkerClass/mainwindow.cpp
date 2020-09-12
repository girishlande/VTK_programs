#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qthread.h"
#include "worker.h"
#include "qdebug.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _thread = new QThread();
    Worker* worker = new Worker();
    worker->moveToThread(_thread);
    connect(worker, &Worker::progressUpdate, this, &MainWindow::updateProgress);
    connect(_thread, SIGNAL (started()), worker, SLOT (process()));
    connect(worker, SIGNAL (finished()), _thread, SLOT (quit()));
    connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
    connect(_thread, SIGNAL (finished()), _thread, SLOT (deleteLater()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateProgress(int value)
{
    qDebug() << value;
    ui->progressBar->setValue(value);
}

void MainWindow::on_pushButton_clicked()
{
    _thread->start();
}
