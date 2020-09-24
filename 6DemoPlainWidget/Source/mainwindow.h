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

 protected slots:
  void on_actionOpen_DICOM_file_triggered();
  void on_actionExit_triggered();
  void test1();
  void test2();
  void displayPlaneWidgets();
  void Update3DPlaneWidgets();

 protected:
  void initialiseWithDICOM();
  void calculateKeyPoints();
  void Create3DImagePlaneWidgets();
  void Show3DPlaneWidgets(bool flag);
  void ReadInputDICOM();
  

 private:

  Ui::MainWindow* ui;
  QScrollBar* m_slider;
  vtkImageViewer2* m_vtkImageViewer;
  QVBoxLayout* vboxLayout;
  QWidget* centralwidget;
  QVTKWidget* m_vtkView;
  QGridLayout* m_container_layout;

  QString m_dicom_dir_path = "";

  vtkSmartPointer<vtkRenderer> m_renderer;
  vtkSmartPointer<vtkDICOMImageReader> m_dicom_reader;
  vtkImageData* m_dicom_image;
  vtkSmartPointer<vtkImagePlaneWidget> m_plane;
  vtkSmartPointer<vtkImagePlaneWidget> m_3DPlaneWidget[3];
  bool m_planeVisible = false;

  // Define center point of planer
  double m_plane_center[3] = {200, 200, 200};

  // Define Normal vectors of planes
  double m_normal_Z[3] = {0, 0, 1};
  double m_normal_X[3] = {1, 0, 0};
  double m_normal_Y[3] = {0, 1, 0};

  // Points away
  double m_XX_1[3];
  double m_YY_1[3];
  double m_ZZ_1[3];
  double m_XX_2[3];
  double m_YY_2[3];
  double m_ZZ_2[3];
  double m_XY_origin[3];
  double m_YZ_origin[3];
  double m_XZ_origin[3];
};

#endif  // MAINWINDOW_H
