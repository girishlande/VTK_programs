#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateProgress(int value);

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QThread* _thread;
};

#endif // MAINWINDOW_H
