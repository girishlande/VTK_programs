#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkImagePlaneWidget.h"
#include "vtkDICOMImageReader.h"

class QVTKWidget;
class QVBoxLayout;
class vtkImageViewer2;
class QScrollBar;
class QGridLayout;
class vtkRenderer;
class vtkActor2D;
class vtkPoints;
class vtkImageActor;
class myVtkInteractorStyleImage;

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

  void updateSlider(int value);

 protected slots:
  void on_actionOpen_DICOM_file_triggered();
  void on_actionExit_triggered();
  void test1();
  void test2();
  void sliderChanged(int value);
  void readSingleImage();
  void readImage(int index);

 protected:
  void initialiseWithDICOM();
  void ViewportBorder();
  void drawAxialLine();
  void displayImageActorDetails(vtkImageActor* actor);
  void displyRendererDetails(vtkRenderer* renderer);
  void calculateViewportDetails(vtkImageActor* actor);
  void InitialiseCornerText();

 private:

  Ui::MainWindow* ui;
  vtkImageViewer2* m_vtkImageViewer;
  QVBoxLayout* vboxLayout;
  QWidget* centralwidget;
  QVTKWidget* m_vtkView;
  QGridLayout* m_container_layout;

  QString m_dicom_dir_path = "";

  vtkSmartPointer<vtkRenderer> m_renderer;
  vtkSmartPointer<vtkDICOMImageReader> m_dicom_reader;
  vtkImageData* m_dicom_image;
  bool m_planeVisible = false;

  vtkPoints* m_axialPoints = nullptr;
  vtkActor2D* m_axialLineActor = nullptr;

  QScrollBar* m_slider;
  int m_minSliceNumber;
  int m_maxSliceNumber;

  double m_topoViewWidth = 0.3;
  double m_topoViewHeight = 0.2;
  double m_topoMarginTop = 0.05;
  double m_topoMarginLeft = 0.03;

  std::vector<std::string> m_imageFiles;
  vtkImageActor* m_imageactor = nullptr;
  myVtkInteractorStyleImage* m_interaction = nullptr;
};

#endif  // MAINWINDOW_H
