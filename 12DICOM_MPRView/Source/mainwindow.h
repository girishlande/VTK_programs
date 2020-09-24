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

  void hightlightActivePort(int mouseX, int mouseY);
  void setCurrentPosition(double pos[4]);

 protected slots:
  void on_actionOpen_DICOM_file_triggered();
  void on_actionExit_triggered();
  void test1();
  void test2();
  void displayPlaneWidgets();

 protected:
  void initialiseWithDICOM();
  void Create3DImagePlaneWidgets();
  void Create2DImagePlaneWidgets();
  void Show3DPlaneWidgets(bool flag);
  void ReadInputDICOM();
  void createMultipleViewports();
  void ViewportBorder(vtkSmartPointer<vtkRenderer>& renderer, double* color,
                      int index);
  int activeViewport(int mouseX, int mouseY);
  void resetViewportBorders(int exception);
  void set3DInteractionStyle();
  void set2DInteractionStyle();

 private:

  Ui::MainWindow* ui;
  vtkImageViewer2* m_vtkImageViewer;
  QVBoxLayout* vboxLayout;
  QWidget* centralwidget;
  QVTKWidget* m_vtkView;
  QGridLayout* m_container_layout;

  QString m_dicom_dir_path = "";

  vtkSmartPointer<vtkRenderer> m_renderer;
  vtkSmartPointer<vtkRenderer> m_renderers[4];
  vtkSmartPointer<vtkDICOMImageReader> m_dicom_reader;
  vtkImageData* m_dicom_image;
  vtkSmartPointer<vtkImagePlaneWidget> m_plane;
  vtkSmartPointer<vtkImagePlaneWidget> m_3DPlaneWidget[3];
  vtkSmartPointer<vtkImagePlaneWidget> m_2DPlaneWidget[3];
  vtkActor2D* m_viewBorders[4];
  int m_activeViewportIndex=0;

  bool m_planeVisible = false;
};

#endif  // MAINWINDOW_H
