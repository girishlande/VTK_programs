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

 protected:
  void AddSphereActor(vtkRenderer* renderer,double position[3]);
  void setUpSlider(QScrollBar** slider);
  void cacheCameraPosition();
  void refreshView();
  void addCoordinateSystem();
  void AddLine(double pt1[3], double pt2[3], double color[3]);
  void AddCS1();
  void AddCS2();

 private slots:

  void on_actionOpen_DICOM_file_triggered();
  void on_actionExit_triggered();
  void sliderChanged(int value);
  void UpdateViewForDICOM();
  void test1();
  void test2();
  void sliderChangedX(int value);
  void sliderChangedY(int value);
  void sliderChangedZ(int value);
  void XView();
  void YView();
  void ZView();

  
private:
  Ui::MainWindow* ui;
  QScrollBar* m_slider;
  vtkImageViewer2* m_vtkImageViewer;
  QVBoxLayout* vboxLayout;
  QWidget* centralwidget;
  QVTKWidget* m_vtkView;
  QGridLayout* m_container_layout;

  QString m_dicom_dir_path = "";

  vtkRenderer* m_renderer;

  QScrollBar* m_sliderX;
  QScrollBar* m_sliderY;
  QScrollBar* m_sliderZ;

  double m_cameraPosition[3];
  double m_cameraFocalPoint[3];
  double m_distance;
  double m_roll;
  double m_viewAngle;
  double m_parallelScale;
  double m_zoom;

  double m_position[3] = {0, 0, 0};
  double m_sphereCounter = 0;
  double m_radius = 10;
};

#endif  // MAINWINDOW_H
