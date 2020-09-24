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
  void initialiseWithDICOM();
  void addDicomImageInViewport();
  void prepareImageData(vtkSmartPointer<vtkImageData>& input, int direction,
                        vtkSmartPointer<vtkImageData>& output);
  void fetchXYImage(vtkSmartPointer<vtkImageData>& input,
                    vtkSmartPointer<vtkImageData>& output);
  void fetchYZImage(vtkSmartPointer<vtkImageData>& input,
                    vtkSmartPointer<vtkImageData>& output);
  void fetchXZImage(vtkSmartPointer<vtkImageData>& input,
                    vtkSmartPointer<vtkImageData>& output);

  void AddLineActor(vtkRenderer* renderer);
  void AddSphereActor(vtkRenderer* renderer);

 private slots:

  void on_actionOpen_DICOM_file_triggered();
  void on_actionExit_triggered();
  void sliderChanged(int value);
  void UpdateViewForDICOM();
  void test1();
  void test2();
  void MultipleViewports();
  void createMultipleViewports();
  void ViewportBorder(vtkSmartPointer<vtkRenderer>& renderer,
                                  double* color, bool last);
  void printImageDetails(vtkSmartPointer<vtkImageData>& image);
  void createPlaneWidget();
  void calculateKeyPoints();

 private:
  Ui::MainWindow* ui;
  QScrollBar* m_slider;
  vtkImageViewer2* m_vtkImageViewer;
  QVBoxLayout* vboxLayout;
  QWidget* centralwidget;
  QVTKWidget* m_vtkView;
  QGridLayout* m_container_layout;

  QString m_dicom_dir_path = "";

   vtkSmartPointer<vtkImagePlaneWidget> m_plane;

  // Define center point of planer
  double m_plane_center[3];

  // Define Normal vectors of planes
  double m_normal_Z[3];
  double m_normal_X[3];
  double m_normal_Y[3];

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
