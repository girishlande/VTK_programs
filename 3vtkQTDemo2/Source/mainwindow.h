#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"

class QVTKWidget;
class QVBoxLayout;
class vtkImageViewer2;
class QScrollBar;
class QGridLayout;
class QVTKOpenGLNativeWidget;
class GPLView;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QVBoxLayout *vboxLayout;
    QWidget *centralwidget;
    GPLView* view1;
    GPLView* view2;
    GPLView* view3;
    GPLView* view4;
    QGridLayout *m_container_layout;

    QString dicom_dir_path;
    int current_view_index;

    void DoView_1();
    void DoView_2();
    void DoView_3();
    void DoView_4();

    void UpdateViewForDICOM();
    void FullScreen();
    void QuadScreen();

private slots:
    void on_actionOpen_DICOM_file_triggered();
    void on_actionImport_STL_file_triggered();
    void on_actionExit_triggered();

    void updateview_1();
    void updateview_2();
    void updateview_3();
    void updateview_4();

    void pelvic_plane();
    void orient_angle();

    void sliderChanged1(int value);
    void sliderChanged2(int value);

protected :
    void check_and_import_stl();
    void check_and_import_obj();

private:
    Ui::MainWindow *ui;
    QScrollBar* s2;
    QScrollBar* s1;
    vtkImageViewer2* s1_viewer;
    vtkImageViewer2* s2_viewer;

    QVTKOpenGLNativeWidget* vtk_widget_;
};

#endif // MAINWINDOW_H
