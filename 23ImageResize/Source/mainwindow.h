#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"
#include "vtkImageData.h"
#include "vtkImagePlaneWidget.h"
#include "vtkSmartPointer.h"

class QVTKWidget;
class QVBoxLayout;
class vtkImageViewer2;
class QScrollBar;
class QGridLayout;
class vtkRenderer;
class vtkActor2D;
class vtkImageActor;

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

 protected:
  void initialiseWithDICOM();
  void prepareImageData(vtkSmartPointer<vtkImageData>& input, int direction,
                        vtkSmartPointer<vtkImageData>& output);
  vtkSmartPointer<vtkImageData> ResizeMe(vtkSmartPointer<vtkImageData> input);
  vtkSmartPointer<vtkImageData> resizeImage(
      vtkSmartPointer<vtkImageData>& input);

  void AddImageInLeftRenderer(vtkSmartPointer<vtkImageData> input);
  void AddImageInRightRenderer(vtkSmartPointer<vtkImageData> input);

  void PrintImageDetails(vtkSmartPointer<vtkImageData> input);
  
  void ResizeUsingInterpolation();
  void ReadPng();

 private slots:

  void on_actionOpen_DICOM_file_triggered();
  void on_actionExit_triggered();
  void sliderChanged(int value);
  void UpdateViewForDICOM();
  void test1();
  void test2();
  void MultipleViewports();

 private:
  Ui::MainWindow* ui;
  QScrollBar* m_slider;
  vtkImageViewer2* m_vtkImageViewer;
  QVBoxLayout* vboxLayout;
  QWidget* centralwidget;
  QVTKWidget* m_vtkView;
  QGridLayout* m_container_layout;

  QString m_dicom_dir_path = "";

  vtkSmartPointer<vtkImageData> m_imageData;
  vtkActor2D* m_left_actor = nullptr;
  vtkActor2D* m_right_actor = nullptr;
  vtkImageActor* m_leftImageActor = nullptr;
  vtkImageActor* m_rightImageActor = nullptr;

  vtkRenderer* m_leftrenderer;
  vtkRenderer* m_rightrenderer;
};

#endif  // MAINWINDOW_H
