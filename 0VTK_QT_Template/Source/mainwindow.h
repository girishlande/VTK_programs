#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkImagePlaneWidget.h"

class QVTKWidget;
class QVBoxLayout;
class vtkImageViewer2;
class QScrollBar;
class QGridLayout;
class vtkRenderer;

namespace Ui {
class MainWindow;
}

#define vtkNew(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

  void InitialiseView();
  void updateSlider(int value);

 private slots:
  void on_actionOpen_DICOM_file_triggered();
  void on_actionExit_triggered();
  void sliderChanged(int value);
  void UpdateViewForDICOM();
  void test1();
  void test2();
  

 private:
  Ui::MainWindow* ui;
  QScrollBar* m_slider;
  vtkImageViewer2* m_vtkImageViewer;
  QVBoxLayout* vboxLayout;
  QWidget* centralwidget;
  QVTKWidget* m_vtkView;
  QGridLayout* m_container_layout;
  QString m_dicom_dir_path = "";

};

#endif  // MAINWINDOW_H
