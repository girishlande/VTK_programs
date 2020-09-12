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

 protected:
  void initialiseWithDICOM();
  void Create3DImagePlaneWidgets();
  void Show3DPlaneWidgets(bool flag);
  void ReadInputDICOM();
  void createMultipleViewports();
  void ViewportBorder(vtkSmartPointer<vtkRenderer>& renderer, double* color,
                      bool last);

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
  vtkSmartPointer<vtkImagePlaneWidget> m_plane;
  vtkSmartPointer<vtkImagePlaneWidget> m_3DPlaneWidget[3];
  bool m_planeVisible = false;
};

#endif  // MAINWINDOW_H
