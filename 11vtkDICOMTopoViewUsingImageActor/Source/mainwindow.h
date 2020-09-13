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

  void checkActiveRenderer(vtkRenderer* renderer);
  void updateSlider(int value);

 protected slots:
  void on_actionOpen_DICOM_file_triggered();
  void on_actionExit_triggered();
  void test1();
  void test2();
  void displayPlaneWidgets();
  void sliderChanged(int value);

 protected:
  void UpdateViewForDICOM();
  void initialiseWithDICOM();
  void ReadInputDICOM();
  void ViewportBorder();
  void fetchYZImage(vtkSmartPointer<vtkImageData>& input,
                                vtkSmartPointer<vtkImageData>& output);
  void drawAxialLine();
  void updateAxialLine();
  void displayImageActorDetails(vtkImageActor* actor);
  void displyRendererDetails(vtkRenderer* renderer);
  void calculateViewportDetails(vtkImageActor* actor);

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
};

#endif  // MAINWINDOW_H
